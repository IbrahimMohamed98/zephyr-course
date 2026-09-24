#define DT_DRV_COMPAT our_driver

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <our_drivers/our_driver.h>

LOG_MODULE_REGISTER(our_driver, CONFIG_SENSOR_LOG_LEVEL);

struct our_driver_config {
	struct gpio_dt_spec led;
};

struct our_driver_data {
	uint32_t custom_param; /* Runtime mutable parameter */
};

int our_driver_set_param(const struct device *dev, uint32_t value)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	/* Access dynamic runtime data struct */
	struct our_driver_data *data = dev->data;

	/* Update parameter in dynamic data struct */
	data->custom_param = value;

	return 0;
}

static int our_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	ARG_UNUSED(chan);
	const struct our_driver_config *cfg = dev->config;

	return gpio_pin_set_dt(&cfg->led, 1);
}

static int our_driver_channel_get(const struct device *dev,
				  enum sensor_channel chan,
				  struct sensor_value *val)
{
	ARG_UNUSED(chan);
	ARG_UNUSED(val);
	const struct our_driver_config *cfg = dev->config;

	return gpio_pin_set_dt(&cfg->led, 0);
}

static int our_driver_init(const struct device *dev)
{
	const struct our_driver_config *cfg = dev->config;

	if (!gpio_is_ready_dt(&cfg->led)) {
		LOG_ERR("GPIO device %s not ready", cfg->led.port->name);
		return -ENODEV;
	}

	int ret = gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		LOG_ERR("Failed to configure GPIO (%d)", ret);
		return ret;
	}

	return 0;
}

static const struct sensor_driver_api our_driver_api = {
	.sample_fetch = our_driver_sample_fetch,
	.channel_get = our_driver_channel_get,
};

#define OUR_DRIVER_DEFINE(inst)                                                \
	static struct our_driver_data our_driver_data_##inst;                      \
	static const struct our_driver_config our_driver_config_##inst = {         \
		.led = GPIO_DT_SPEC_INST_GET(inst, led_gpios),                        \
	};                                                                         \
                                                                               \
	DEVICE_DT_INST_DEFINE(inst,                                                \
			      our_driver_init,                                         \
			      NULL,                                                    \
			      &our_driver_data_##inst,                                 \
			      &our_driver_config_##inst,                               \
			      POST_KERNEL,                                             \
			      CONFIG_SENSOR_INIT_PRIORITY,                             \
			      &our_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_DEFINE)
