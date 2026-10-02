#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

/* Retrieve the device handle */
static const struct device *dev = DEVICE_DT_GET_ANY(our_driver);

/* 1. Subcommand handler: fetch */
static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(dev)) {
		shell_error(sh, "Device not ready");
		return -ENODEV;
	}

	int ret = sensor_sample_fetch(dev);
	if (ret == 0) {
		shell_print(sh, "Sample fetched successfully (LED ON)");
	} else {
		shell_error(sh, "Failed to fetch sample: %d", ret);
	}
	return ret;
}

/* 2. Subcommand handler: read */
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(dev)) {
		shell_error(sh, "Device not ready");
		return -ENODEV;
	}

	struct sensor_value val;
	int ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);
	if (ret == 0) {
		shell_print(sh, "Sensor value read successfully (LED OFF)");
	} else {
		shell_error(sh, "Failed to read channel: %d", ret);
	}
	return ret;
}

/* 3. Subcommand handler: info */
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	bool ready = device_is_ready(dev);
	shell_print(sh, "Device Name: %s", dev ? dev->name : "Unknown");
	shell_print(sh, "Ready State: %s", ready ? "Ready" : "Not Ready");

	return 0;
}

/* Define static subcommand set */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
	SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch()", cmd_sensor_fetch),
	SHELL_CMD(read,  NULL, "Call sensor_channel_get() and print result", cmd_sensor_read),
	SHELL_CMD(info,  NULL, "Print device name and ready state", cmd_sensor_info),
	SHELL_SUBCMD_SET_END
);

/* Register root command 'sensor' */
SHELL_CMD_REGISTER(sensor, &sub_sensor, "Sensor driver shell commands", NULL);

int main(void)
{
	return 0;
}
