#ifndef OUR_DRIVER_H_
#define OUR_DRIVER_H_

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Custom extension function to update dynamic driver data parameter.
 *
 * @param dev Pointer to the device structure.
 * @param value New parameter value to set.
 * @return 0 on success, negative errno on failure.
 */
int our_driver_set_param(const struct device *dev, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif /* OUR_DRIVER_H_ */
