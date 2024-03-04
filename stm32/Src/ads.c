/**
  ******************************************************************************
  * @file    ads.c
  * @author  Stephen Taylor
  * @brief   Soil Power Sensor ADS12 library
  *          This file provides a function to read from the onboard ADC (ADS1219).
  * @date    11/27/2023
  *
  ******************************************************************************
  **/

/* Includes ------------------------------------------------------------------*/
#include "ads.h"

int HAL_status(HAL_StatusTypeDef ret) {
  int status;
  if (ret == HAL_OK){
    status = 0;
  } else if (ret == HAL_ERROR){
    status = 1;
  } else if (ret == HAL_BUSY){
    status = 2;
  } else {
    status = 3;
  }
  return status;
}

HAL_StatusTypeDef ADC_init(void){
  uint8_t code = ADS12_RESET_CODE;
  uint8_t register_data[2] = {0x40, 0x03};
  HAL_StatusTypeDef ret;

  // Control register breakdown.
  //  7:5 MUX (default)
  //  4   Gain (default)
  //  3:2 Data rate (default)
  //  1   Conversion mode (default)
  //  0   VREF (External reference 3.3V)

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET); // Power down pin has to be set to high before any of the analog circuitry can function
  //HAL_Delay(1000);
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, &code, 1, HAL_MAX_DELAY);  // Send the reset code
  if (ret != HAL_OK){
    return ret;
  } 

  // Set the control register, leaving everything at default except for the VREF, which will be set to external reference mode
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, register_data, 2, HAL_MAX_DELAY);
  if (ret != HAL_OK){
    return ret;
  }
    
  code = ADS12_START_CODE;
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, &code, 1, HAL_MAX_DELAY); // Send a start code
  if (ret != HAL_OK){
    return ret;
  }
  return ret;
}

HAL_StatusTypeDef ADC_configure(uint8_t reg_data) {
  uint8_t code = ADS12_RESET_CODE;
  uint8_t register_data[2] = {0x40, reg_data};
  HAL_StatusTypeDef ret;
  uint8_t recx_reg;
  char reg_string[40];

  // Set the control register, leaving everything at default except for the VREF, which will be set to external reference mode
  //ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, &code, 1, HAL_MAX_DELAY);  // Send the reset code
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, register_data, 2, HAL_MAX_DELAY);
  
  code = ADS12_START_CODE;
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, &code, 1, HAL_MAX_DELAY); // Send a start code
  HAL_Delay(10); // Delay between reconfiguration and measurment
  return ret;
}

double ADC_readVoltage(void){
  uint8_t code;
  double reading;
  HAL_StatusTypeDef ret;
  uint8_t rx_data[3] = {0x00, 0x00, 0x00}; // Why is this only 3 bytes?

  ret = ADC_configure(0x03);
    
  while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))); // Wait for the DRDY pin on the ADS12 to go low, this means data is ready
  code = ADS12_READ_DATA_CODE;
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, &code, 1, HAL_MAX_DELAY);
  ret = HAL_I2C_Master_Receive(&hi2c2, ADS12_READ, rx_data, 3, 1000);

  uint64_t temp = ((uint64_t)rx_data[0] << 16) | ((uint64_t)rx_data[1] << 8) | ((uint64_t)rx_data[2]); // Chop the last byte, as it seems to be mostly noise
  reading = (double) temp;

  // Uncomment these lines if you wish to see the raw and shifted values from the ADC for calibration purpouses
  // You will have to use these lines to get the raw x values to plug into the linear_regression.py file
  // char raw[45];
  // sprintf(raw, "Raw: %x %x %x Shifted: %f \r\n\r\n",rx_data[0], rx_data[1], rx_data[2], reading);
  // HAL_UART_Transmit(&huart1, (const uint8_t *) raw, 36, 19);

  //reading =  (VOLTAGE_SLOPE * reading) + VOLTAGE_B; // Calculated from linear regression
  return reading;
}

double ADC_readCurrent(void){
  uint8_t code;
  double reading;
  HAL_StatusTypeDef ret;
  uint8_t rx_data[3] = {0x00, 0x00, 0x00}; // Why is this only 3 bytes?

  ret = ADC_configure(0x23);
    
  while((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))); // Wait for the DRDY pin on the ADS12 to go low, this means data is ready
  code = ADS12_READ_DATA_CODE;
  ret = HAL_I2C_Master_Transmit(&hi2c2, ADS12_WRITE, &code, 1, HAL_MAX_DELAY);
  ret = HAL_I2C_Master_Receive(&hi2c2, ADS12_READ, rx_data, 3, 1000);

  uint64_t temp = ((uint64_t)rx_data[0] << 16) | ((uint64_t)rx_data[1] << 8) | ((uint64_t)rx_data[2]); // Chop the last byte, as it seems to be mostly noise
  reading = (double) temp;

  // Uncomment these lines if you wish to see the raw and shifted values from the ADC for calibration purpouses
  // You will have to use these lines to get the raw x values to plug into the linear_regression.py file
  // char raw[45];
  // sprintf(raw, "Raw: %x %x %x Shifted: %f \r\n\r\n",rx_data[0], rx_data[1], rx_data[2], reading);
  // HAL_UART_Transmit(&huart1, (const uint8_t *) raw, 36, 19);

  //reading =  (CURRENT_SLOPE * reading) + CURRENT_B; // Calculated from linear regression
  return reading;
}

HAL_StatusTypeDef probeADS12(void){
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_IsDeviceReady(&hi2c2, ADS12_WRITE, 10, 20);
  return ret;
}
