#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>

const struct device *dev = DEVICE_DT_GET_ANY(our_driver);

int main(void)
{
	if (!device_is_ready(dev)) {
		return -ENODEV;
	}

	/* Turn LED ON */
	sensor_sample_fetch(dev);
	k_msleep(1000);

	/* Turn LED OFF */
	sensor_channel_get(dev, SENSOR_CHAN_ALL, NULL);

	return 0;
}
