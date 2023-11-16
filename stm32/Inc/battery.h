/**
 * @file battery.h
 * @brief Provides declarations for battery monitoring interface.
 * 
 * Assumes the HAL layer is already initialized.
 * 
 * @author John Madden <jmadden173@pm.me>
*/

#ifndef __BATTERY_H__
#define __BATTERY_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the battery monitoring module.
 * 
 * Starts continuous sampling on ADC.
 * 
 * @return Return code
*/
unsigned char battery_init(void);

/**
 * @brief Gets the most recent battery voltage level
 * 
 * The battery voltage level is in mV.
 * 
 * @return Battery voltage
*/
unsigned int battery_voltage(void);

#ifdef __cplusplus
}
#endif

#endif /* __BATTERY_H__*/