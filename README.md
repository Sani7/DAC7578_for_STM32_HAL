# DAC7578_for_STM32_HAL
## Description
This code is a HAL library for the stm32 platform to communicate with the DAC7578 12-bit DAC over I2C.
## Example
See example folder
## Usage
### Handle
```c
typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint16_t address;
    GPIO_TypeDef *ldac_port;
    uint16_t ldac_pin;
} DAC7578_HandleTypeDef;
```
### Enum
```c
typedef enum {
    power_on = 0x00,
    power_off_1k = 0x01,
    power_off_100k = 0x02,
    power_off_high_impedance = 0x03
} DAC7578_power_t;
```
### Functions
```c
/**
 * @brief This function initializes the DAC handle
 * @param hdac The DAC handle
 * @param hi2c The I2C handle the DAC is connected to
 * @param address The I2C address of the DAC (via hardware jumpers)
 */
void DAC7578_Init(DAC7578_HandleTypeDef *hdac, I2C_HandleTypeDef *hi2c, uint8_t address, GPIO_TypeDef *ldac_port, uint16_t ldac_pin);

/**
 * @brief Set the output of a DAC
 * @param hdac The DAC handle
 * @param chan The DAC number (0-7)
 * @param value The 10-bit value to set the DAC to
 * @param update True to update the DAC output, false to only set the input register
 */
void DAC7578_set_dac_output(DAC7578_HandleTypeDef *hdac, uint8_t chan, uint16_t value, bool update);

/**
 * @brief Update one DAC output
 * @param hdac The DAC handle
 * @param chan The DAC number (0-7)
 */
void DAC7578_update_dac_output(DAC7578_HandleTypeDef *hdac, uint8_t chan);

/**
 * @brief Set all dac outputs globally
 * @param hdac The DAC handle
 * @param value The 10-bit value to set the DAC to
 * @param update True to update the DAC output, false to only set the input register
 */
void DAC7578_set_dac_output_all(DAC7578_HandleTypeDef *hdac, uint16_t value, bool update);

/**
 * @brief Update all DAC ouputs
 * @param hdac The DAC handle
 */
void DAC7578_update_dac_output_all(DAC7578_HandleTypeDef *hdac);

/**
 * @brief Read one dac output register
 * @param hdac The DAC handle
 * @param chan The DAC number (0-7)
 * @param value The pointer to the output value
 */
void DAC7578_read_dac_output(DAC7578_HandleTypeDef *hdac, uint8_t chan, uint16_t *value);

/**
 * @brief Read one dac input register
 *
 * @param hdac The DAC handle
 * @param chan The DAC number (0-7)
 * @param value The pointer to the output value
 */
void DAC7578_read_dac_input(DAC7578_HandleTypeDef *hdac, uint8_t chan, uint16_t *value);

/**
 * @brief Set the power mode of a dac
 * @param hdac The DAC handle
 * @param chan The DAC number (0-7)
 * @param power The power mode to set the dac to
 */
void DAC7578_write_dac_power(DAC7578_HandleTypeDef* hdac, uint8_t chan, DAC7578_power_t power);

/**
 * @brief Set the power mode of all dacs
 * @param hdac The DAC handle
 * @param power The power mode to set all dacs to
 */
void DAC7578_write_dac_power_all(DAC7578_HandleTypeDef* hdac, DAC7578_power_t power);

/**
 * @brief This function makes the provided dac channels ignore the ldac pin
 *
 * @param hdac This function makes all dac channels ignore the ldac pin
 * @param chan The channel(s) that have to ignore the ldac pin
 */
void DAC7578_dac_ignore_ldac_pin(DAC7578_HandleTypeDef* hdac, uint8_t chan);

/**
 * @brief This function makes all dac channels ignore the ldac pin
 *
 * @param hdac
 */
void DAC7578_all_dac_ignore_ldac_pin(DAC7578_HandleTypeDef* hdac);
```
