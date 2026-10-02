#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <our_drivers/our_driver.h>

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

/* 4. Subcommand handler: set <value> */
static int cmd_sensor_set(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	if (!device_is_ready(dev)) {
		shell_error(sh, "Device not ready");
		return -ENODEV;
	}

	int err = 0;
	/* Parse integer argument from argv[1] */
	long val = shell_strtol(argv[1], 10, &err);

	if (err != 0) {
		shell_error(sh, "Invalid argument: %s is not a valid integer", argv[1]);
		return -EINVAL;
	}

	/* Range validation (e.g., allowed range: 0 to 10000) */
	if (val < 0 || val > 10000) {
		shell_error(sh, "Argument out of range: %ld (Allowed: 0 - 10000)", val);
		return -ERANGE;
	}

	/* Call the custom driver extension API */
	int ret = our_driver_set_param(dev, (uint32_t)val);
	if (ret == 0) {
		shell_print(sh, "Driver parameter successfully updated to: %ld", val);
	} else {
		shell_error(sh, "Failed to set parameter: %d", ret);
	}

	return ret;
}

/* Define static subcommand set */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
	SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch()", cmd_sensor_fetch),
	SHELL_CMD(read,  NULL, "Call sensor_channel_get() and print result", cmd_sensor_read),
	SHELL_CMD(info,  NULL, "Print device name and ready state", cmd_sensor_info),

	/* Enforce argument count: 2 mandatory ("set", "<value>"), 0 optional */
	SHELL_CMD_ARG(set, NULL, "Set custom driver parameter <value>", cmd_sensor_set, 2, 0),

	SHELL_SUBCMD_SET_END
);

/* Register root command 'sensor' */
SHELL_CMD_REGISTER(sensor, &sub_sensor, "Sensor driver shell commands", NULL);

int main(void)
{
	return 0;
}
