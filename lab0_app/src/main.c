#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED5180_NODE DT_ALIAS(led5180)
#define BUTTON_NODE DT_ALIAS(sw0)

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(LED5180_NODE, gpios);
static const struct gpio_dt_spec button =
    GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

int main(void)
{
    if (!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&button)) {
        return 0;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0 ||
        gpio_pin_configure_dt(&button, GPIO_INPUT) < 0) {
        return 0;
    }

    int previous = 0;
    int stable = 0;
    int64_t changed_at = k_uptime_get();

    while (1) {
        int pressed = gpio_pin_get_dt(&button);
        if (pressed < 0) {
            return 0;
        }

        int64_t now = k_uptime_get();

        if (pressed != previous) {
            previous = pressed;
            changed_at = now;
        }

        /* Accept a change after the input stays stable for 30 ms. */
        if (pressed != stable && now - changed_at >= 30) {
            stable = pressed;
            if (stable == 1) {
                if (gpio_pin_toggle_dt(&led) < 0) {
                    return 0;
                }
            }
        }

        k_msleep(5);
    }
}