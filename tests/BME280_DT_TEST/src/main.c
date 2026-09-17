#include <zephyr/devicetree.h>
#include <zephyr/ztest.h>
#include <stdint.h>

#define BME280_NODE DT_NODELABEL(bme280)
#define BME280_EXPECTED_ADDR 0x77
#define BME280_EXPECTED_CHIP_ID 0x60

struct mock_bme280_sample {
	uint8_t chip_id;
	int32_t temperature_centi_c;
};

static const struct mock_bme280_sample mock_sample = {
	.chip_id = BME280_EXPECTED_CHIP_ID,
	.temperature_centi_c = 2437,
};

static uint8_t mock_bme280_read_chip_id(void)
{
	return mock_sample.chip_id;
}

static int32_t mock_bme280_read_temperature_centi_c(void)
{
	return mock_sample.temperature_centi_c;
}

ZTEST(bme280_dt_tests, test_bme280_node_is_enabled)
{
	zassert_true(DT_NODE_HAS_STATUS(BME280_NODE, okay),
		     "bme280 devicetree node should be okay");
}

ZTEST(bme280_dt_tests, test_bme280_uses_i2c_device_binding)
{
	zassert_true(DT_NODE_HAS_COMPAT(BME280_NODE, i2c_device),
		     "bme280 node should use compatible = \"i2c-device\"");
}

ZTEST(bme280_dt_tests, test_bme280_i2c_address)
{
	zassert_equal(DT_REG_ADDR(BME280_NODE), BME280_EXPECTED_ADDR,
		      "bme280 address should be 0x77");
}

ZTEST(bme280_dt_tests, test_mock_chip_id)
{
	zassert_equal(mock_bme280_read_chip_id(), BME280_EXPECTED_CHIP_ID,
		      "mocked BME280 chip id should be 0x60");
}

ZTEST(bme280_dt_tests, test_mock_temperature_is_sane)
{
	int32_t temperature = mock_bme280_read_temperature_centi_c();

	zassert_between_inclusive(temperature, -4000, 8500,
				  "mocked temperature should be inside BME280 range");
}

ZTEST_SUITE(bme280_dt_tests, NULL, NULL, NULL, NULL, NULL);
