#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>

#define BME280_NODE DT_NODELABEL(bme280)
#define BME280_CHIP_ID_REG 0xD0
#define BME280_CTRL_MEAS_REG 0xF4
#define BME280_TEMP_MSB_REG 0xFA
#define BME280_CALIB00_REG 0x88

static const struct i2c_dt_spec bme280 = I2C_DT_SPEC_GET(BME280_NODE);

struct bme280_calib {
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
};

static uint16_t u16_le(const uint8_t *buf)
{
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

static int16_t s16_le(const uint8_t *buf)
{
    return (int16_t)u16_le(buf);
}

static int bme280_read_calibration(struct bme280_calib *calib)
{
    uint8_t buf[6];
    int ret = i2c_burst_read_dt(&bme280, BME280_CALIB00_REG, buf, sizeof(buf));

    if (ret < 0) {
        return ret;
    }

    calib->dig_t1 = u16_le(&buf[0]);
    calib->dig_t2 = s16_le(&buf[2]);
    calib->dig_t3 = s16_le(&buf[4]);

    return 0;
}

static int bme280_start_temperature_measurement(void)
{
    /* osrs_t=x1, osrs_p=skipped, mode=normal */
    return i2c_reg_write_byte_dt(&bme280, BME280_CTRL_MEAS_REG, 0x23);
}

static int bme280_read_raw_temperature(int32_t *raw_temp)
{
    uint8_t buf[3];
    int ret = i2c_burst_read_dt(&bme280, BME280_TEMP_MSB_REG, buf, sizeof(buf));

    if (ret < 0) {
        return ret;
    }

    *raw_temp = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);
    return 0;
}

static int32_t bme280_compensate_temperature_centi_c(const struct bme280_calib *calib,
                                                     int32_t adc_t)
{
    int32_t var1 = ((((adc_t >> 3) - ((int32_t)calib->dig_t1 << 1))) *
                    ((int32_t)calib->dig_t2)) >> 11;
    int32_t var2 = (((((adc_t >> 4) - ((int32_t)calib->dig_t1)) *
                      ((adc_t >> 4) - ((int32_t)calib->dig_t1))) >> 12) *
                    ((int32_t)calib->dig_t3)) >> 14;
    int32_t t_fine = var1 + var2;

    return (t_fine * 5 + 128) >> 8;
}

int main(void)
{
    struct bme280_calib calib;
    int ret;

    printk("BME280 test starting\n");
    printk("BME280 bus: %s\n", bme280.bus->name);
    printk("BME280 addr: 0x%02x\n", bme280.addr);

    if (!device_is_ready(bme280.bus)) {
        printk("I2C bus is not ready\n");
        return 0;
    }

    uint8_t chip_id = 0;
    ret = i2c_reg_read_byte_dt(&bme280, BME280_CHIP_ID_REG, &chip_id);
    if (ret < 0) {
        printk("Failed to read BME280 chip id: %d\n", ret);
        return 0;
    }
    printk("BME280 chip id: 0x%02x\n", chip_id);

    ret = bme280_read_calibration(&calib);
    if (ret < 0) {
        printk("Failed to read BME280 calibration: %d\n", ret);
        return 0;
    }
    printk("BME280 calib T1=%u T2=%d T3=%d\n",
           calib.dig_t1, calib.dig_t2, calib.dig_t3);

    ret = bme280_start_temperature_measurement();
    if (ret < 0) {
        printk("Failed to start BME280 measurement: %d\n", ret);
        return 0;
    }

    while (1) {
        int32_t raw_temp = 0;

        ret = bme280_read_raw_temperature(&raw_temp);
        if (ret < 0) {
            printk("Failed to read BME280 temperature: %d\n", ret);
        } else {
            int32_t temp_centi_c =
                bme280_compensate_temperature_centi_c(&calib, raw_temp);

            printk("BME280 raw temp: %d\n", raw_temp);
            printk("BME280 temperature: %d.%02d C\n",
                   temp_centi_c / 100, temp_centi_c % 100);
        }

        k_sleep(K_SECONDS(2));
    }

    return 0;
}
