#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT our_driver // "our,driver"

// Creates a named log module this file
LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

struct Config
{
    struct gpio_dt_spec led;
};

static const struct Config ourConfig = 
{
    .led = GPIO_DT_SPEC_INST_GET(0, gpios),
};

static int channel_get_my_implementation(const struct device *dev,
                                         enum sensor_channel chan,
                                         struct sensor_value *val)
{
    LOG_INF("Hello from channel get, channel: %d", chan);
    const struct Config *cfg = dev->config;
    gpio_pin_set_dt(&cfg->led, 0);

    return 0;
}

static int sample_fetch_my_implementation(const struct device *dev,
				                          enum sensor_channel chan)
{
    LOG_INF("Hello from sample_fetch, channel: %d", chan);
    const struct Config *cfg = dev->config;
    gpio_pin_set_dt(&cfg->led, 1);

    return 0;
}

static DEVICE_API(sensor, api_kris_demo) = {
    .channel_get = channel_get_my_implementation,
	.sample_fetch = sample_fetch_my_implementation,
};

// Init function
static int init(const struct device* dev)
{
	const struct Config *cfg = dev->config;

	if (!gpio_is_ready_dt(&cfg->led)) {
		return -ENODEV;
	}
	LOG_INF("Device initialized!");
	return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_ACTIVE);
}

DEVICE_DT_INST_DEFINE(0, init, NULL, NULL, &ourConfig, POST_KERNEL, 80, &api_kris_demo);
