/**
 * @file DAC7578.c
 * @brief 
 * @author sanderspeetjens
 */

#include "DAC7578.h"

#define DAC7578_TIMEOUT 100

void DAC7578_Init(DAC7578_HandleTypeDef *hdac, I2C_HandleTypeDef *hi2c, uint8_t address, GPIO_TypeDef *ldac_port, uint16_t ldac_pin) {
    hdac->hi2c = hi2c;
    address &= 0b1110;
    hdac->address = (0b1001 << 4) | address;
    hdac->ldac_port = ldac_port;
    hdac->ldac_pin = ldac_pin;
}

/**
 * @brief Set the byte in the designated register to value.
 * @param regAddr Register address to write.
 * @param val Value to set, 0 to 255.
 */
void DAC7578_SetRegByte(DAC7578_HandleTypeDef *hdac, uint8_t regAddr, uint16_t val) {
    uint8_t bytes[3] = { regAddr, val >> 8, val & 0xff };
    HAL_I2C_Master_Transmit(hdac->hi2c, hdac->address, bytes, 3, DAC7578_TIMEOUT);
}

/**
 * @brief Gets the byte in the designated register.
 * @param regAddr Register address to read.
 * @return Value stored in the register, 0 to 255.
 */
uint16_t DAC7578_GetRegByte(DAC7578_HandleTypeDef *hdac, uint8_t regAddr) {
    uint8_t val[2];
    HAL_I2C_Master_Transmit(hdac->hi2c, hdac->address, &regAddr, 1, DAC7578_TIMEOUT);
    HAL_I2C_Master_Receive(hdac->hi2c, hdac->address, val, 2, DAC7578_TIMEOUT);
    return val[1] + (val[0] << 8);
}

/**
 * @brief This function sets the output of a DAC
 * @param hdac The DAC handle
 * @param chan The DAC number (0-7)
 * @param value The 10-bit value to set the DAC to
 * @return HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR or HAL_BUSY otherwise
 */
void DAC7578_set_dac_output(DAC7578_HandleTypeDef *hdac, uint8_t chan, uint16_t value, bool update) {
    uint8_t regAddr = 0x00;
    // limit value to 10 bits
    value &= 0xFFF;
    value = (value << 4);

    // set input register of dac n_dac to value
    // register is 0x00 + n_dac
    regAddr = 0b0111 & chan;
    if (update) {
        regAddr |= 0x30;
    }
    DAC7578_SetRegByte(hdac, regAddr, value);
}

void DAC7578_update_dac_output(DAC7578_HandleTypeDef *hdac, uint8_t chan) {
    uint8_t regAddr = 0x00;
    // update dac chan
    // register is 0x00 + n_dac
    regAddr = 0b1111 & chan;
    regAddr |= 0x10;
    HAL_GPIO_WritePin(hdac->ldac_port, hdac->ldac_pin, GPIO_PIN_RESET);
    DAC7578_SetRegByte(hdac, regAddr, 0xFFFF);
    HAL_GPIO_WritePin(hdac->ldac_port, hdac->ldac_pin, GPIO_PIN_SET);
}

void DAC7578_set_dac_output_all(DAC7578_HandleTypeDef *hdac, uint16_t value, bool update) {
    // set input register of all dacs to value
    // register is 0x00 and broadcast address is 0x0F
    uint8_t regAddr = 0x0F;
    // limit value to 12 bits
    value &= 0xFFF;
    value = (value << 4);

    if (update) {
        regAddr |= 0x30;
    }
    DAC7578_SetRegByte(hdac, regAddr, value);
}

void DAC7578_update_dac_output_all(DAC7578_HandleTypeDef *hdac) {
    uint8_t regAddr = 0x00;
    // update all dacs
    // register is 0x00 and broadcast address is 0x0F
    regAddr = 0x0F;
    regAddr |= 0x10;
    HAL_GPIO_WritePin(hdac->ldac_port, hdac->ldac_pin, GPIO_PIN_RESET);
    DAC7578_SetRegByte(hdac, regAddr, 0);
    HAL_GPIO_WritePin(hdac->ldac_port, hdac->ldac_pin, GPIO_PIN_SET);
}

void DAC7578_read_dac_output(DAC7578_HandleTypeDef *hdac, uint8_t chan, uint16_t *value) {
    uint8_t regAddr = 0x00;
    uint16_t ret;
    // read input register of dac chan
    // register is 0x10 + chan
    regAddr = 0b1111 & chan;
    regAddr |= 0x10;
    ret = DAC7578_GetRegByte(hdac, regAddr);
    ret >>= 4;
    if (value != NULL)
    	*value = ret;
}

void DAC7578_read_dac_input(DAC7578_HandleTypeDef *hdac, uint8_t chan, uint16_t *value) {
    uint8_t regAddr = 0x00;
    uint16_t ret;
    // read input register of dac chan
    // register is 0x10 + chan
    regAddr = 0b1111 & chan;
    ret = DAC7578_GetRegByte(hdac, regAddr);
    ret >>= 4;
    if (value != NULL)
    	*value = ret;
}

void DAC7578_write_dac_power(DAC7578_HandleTypeDef* hdac, uint8_t chan, DAC7578_power_t power) {
    uint8_t regAddr = 0x40;

    uint16_t pd_config = ((uint16_t) power) << 13;       // PD1, PD0
    uint16_t chan_config = ((uint16_t) chan) << 5; // DACH ~ DACA

    DAC7578_SetRegByte(hdac, regAddr, pd_config | chan_config);
}



void DAC7578_write_dac_power_all(DAC7578_HandleTypeDef* hdac, DAC7578_power_t power) {
    uint8_t regAddr = 0x40;

    uint16_t val = 0x1FE;
    val |= (power & 0x3) << 1;
    DAC7578_SetRegByte(hdac, regAddr, val);
}

void DAC7578_dac_ignore_ldac_pin(DAC7578_HandleTypeDef* hdac, uint8_t chan)
{
	uint8_t regAddr = 0x60;
	uint16_t val = chan << 8;

		DAC7578_SetRegByte(hdac, regAddr, val);
}

void DAC7578_all_dac_ignore_ldac_pin(DAC7578_HandleTypeDef* hdac) {
	uint8_t regAddr = 0x60;
	uint16_t val = 0xFF00;

	DAC7578_SetRegByte(hdac, regAddr, val);
}
//
//void DAC7578_read_dac_power(DAC7578_HandleTypeDef* hdac, uint8_t n_dac, DAC7578_power_t *power) {
//    uint8_t regAddr = 0x40;
//    uint16_t val = DAC7578_GetRegByte(hdac, regAddr);
//    val >>= (n_dac + 5);
//    *power = val & 0x03;
//}

//HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
//                                          uint16_t Size, uint32_t Timeout);
//HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
//                                         uint16_t Size, uint32_t Timeout);
