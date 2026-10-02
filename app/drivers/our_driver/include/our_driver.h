#ifndef OUR_DRIVER_H_
#define OUR_DRIVER_H_

#include <stdbool.h>
#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

// Extension API: enables/disables the LED. When disabled, sample_fetch does not turn it on
void our_driver_set_enabled(const struct device *dev, bool enabled);

#ifdef __cplusplus
}
#endif

#endif // OUR_DRIVER_H_
