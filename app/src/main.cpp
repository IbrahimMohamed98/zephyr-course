#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <our_drivers/our_driver.h>

const struct device *dev = DEVICE_DT_GET_ANY(our_driver);

int main(void)
{
	if (!device_is_ready(dev)) {
		return -ENODEV;
	}

	/* Call standard sensor functions */
	sensor_sample_fetch(dev);

	/* Call custom extension function to modify dynamic data parameter */
	our_driver_set_param(dev, 500);

	sensor_channel_get(dev, SENSOR_CHAN_ALL, NULL);

	return 0;
}
