#include <zephyr/drivers/gpio.h>
#include<zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "our_driver.h"

/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(app_led)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

namespace Driver::Test
{
    void ToggleLed()
    {
        const struct device* driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
        struct sensor_value value;

        our_driver_set_enabled(driver, true);                         // switch is ON
        sensor_sample_fetch(driver);                                  // LED turns ON 
        k_msleep(1000);
        sensor_channel_get(driver, SENSOR_CHAN_AMBIENT_TEMP, &value); // LED turns OFF
        k_msleep(1000);
        our_driver_set_enabled(driver, false);                        // switch is OFF
        sensor_sample_fetch(driver);                                  // LED stays OFF (fetch is ignored)
        k_msleep(1000);
    }

} // Driver::Test

int main(void)
{
    Driver::Test::ToggleLed();

    bool led_state = true;

    if (!gpio_is_ready_dt(&led)) return 0;

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    while (1) {
        if (gpio_pin_toggle_dt(&led) < 0) return 0;

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(CONFIG_APP_HEARBEAT_PERIOD_MS);
    }
    return 0;
}
