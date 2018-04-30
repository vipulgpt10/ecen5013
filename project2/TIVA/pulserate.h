/**
 * @file pulserate.h
 *
 * This header file provides the prototypes of the function for
 * pulserate sensor..
 *
 * @author Nagarjuna Reddy Kovuru & Vipul Gupta
 * @date 26 April 2018
 * Reference: Referred Sparkfun Example code for Pulse rate sensor.
 */

#ifndef __PULSERATE_H__
#define __PULSERATE_H__
//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c_wrapper.h"
#include "FreeRTOS.h"
#include "semphr.h"


//***********************************************************************************
// Defined Macros and Structure Types
//***********************************************************************************
/* I2C Slave address: 7bit*/
#define MAX30105_SLAVE_ADDR       0x57

/******** MAX30105 Register Address *******/
/* Status Registers*/
#define MAX30105_INTSTAT1         0x00
#define MAX30105_INTSTAT2         0x01
#define MAX30105_INTENABLE1       0x02
#define MAX30105_INTENABLE2       0x03

/* FIFO Registers */
#define MAX30105_FIFOWRITEPTR     0x04
#define MAX30105_FIFOOVERFLOW     0x05
#define MAX30105_FIFOREADPTR      0x06
#define MAX30105_FIFODATA         0x07

/* Configuration Registers */
#define MAX30105_FIFOCONFIG       0x08
#define MAX30105_MODECONFIG       0x09
#define MAX30105_PARTICLECONFIG   0x0A
#define MAX30105_LED1_PULSEAMP    0x0C
#define MAX30105_LED2_PULSEAMP    0x0D
#define MAX30105_LED3_PULSEAMP    0x0E
#define MAX30105_LED_PROX_AMP     0x10
#define MAX30105_MULTILEDCONFIG1  0x11
#define MAX30105_MULTILEDCONFIG2  0x12

/* Die Temperature Registers */
#define MAX30105_DIETEMPINT       0x1F
#define MAX30105_DIETEMPFRAC      0x20
#define MAX30105_DIETEMPCONFIG    0x21

/* Proximity Function Registers */
#define MAX30105_PROXINTTHRESH    0x30

/* Part ID Registers */
#define MAX30105_REVISIONID       0xFE
#define MAX30105_PARTID           0xFF // Should always be 0x15


/******** MAX30105 Command Registers *******/
/* FIFO configuration commands */
#define MAX30105_SAMPLEAVG_MASK     (uint8_t)~0b11100000
#define MAX30105_SAMPLEAVG_1        0x00
#define MAX30105_SAMPLEAVG_2        0x20
#define MAX30105_SAMPLEAVG_4        0x40
#define MAX30105_SAMPLEAVG_8        0x60
#define MAX30105_SAMPLEAVG_16       0x80
#define MAX30105_SAMPLEAVG_32       0xA0
#define MAX30105_ROLLOVER_MASK      0xEF
#define MAX30105_ROLLOVER_ENABLE    0x10
#define MAX30105_ROLLOVER_DISABLE   0x00
#define MAX30105_A_FULL_MASK        0xF0

/* Mode configuration commands */
#define MAX30105_SHUTDOWN_MASK      0x7F
#define MAX30105_SHUTDOWN           0x80
#define MAX30105_WAKEUP             0x00
#define MAX30105_RESET_MASK         0xBF
#define MAX30105_RESET              0x40
#define MAX30105_MODE_MASK          0xF8
#define MAX30105_MODE_REDONLY       0x02
#define MAX30105_MODE_REDIRONLY     0x03
#define MAX30105_MODE_MULTILED      0x07

/* Particle sensing configuration commands*/
#define MAX30105_ADCRANGE_MASK      0x9F
#define MAX30105_ADCRANGE_2048      0x00
#define MAX30105_ADCRANGE_4096      0x20
#define MAX30105_ADCRANGE_8192      0x40
#define MAX30105_ADCRANGE_16384     0x60

#define MAX30105_SAMPLERATE_MASK    0xE3
#define MAX30105_SAMPLERATE_50      0x00
#define MAX30105_SAMPLERATE_100     0x04
#define MAX30105_SAMPLERATE_200     0x08
#define MAX30105_SAMPLERATE_400     0x0C
#define MAX30105_SAMPLERATE_800     0x10
#define MAX30105_SAMPLERATE_1000    0x14
#define MAX30105_SAMPLERATE_1600    0x18
#define MAX30105_SAMPLERATE_3200    0x1C

#define MAX30105_PULSEWIDTH_MASK    0xFC
#define MAX30105_PULSEWIDTH_69      0x00
#define MAX30105_PULSEWIDTH_118     0x01
#define MAX30105_PULSEWIDTH_215     0x02
#define MAX30105_PULSEWIDTH_411     0x03

/*Multi-LED Mode configuration */
#define MAX30105_SLOT1_MASK         0xF8
#define MAX30105_SLOT2_MASK         0x8F
#define MAX30105_SLOT3_MASK         0xF8
#define MAX30105_SLOT4_MASK         0x8F

#define SLOT_NONE                   0x00
#define SLOT_RED_LED                0x01
#define SLOT_IR_LED                 0x02
#define SLOT_GREEN_LED              0x03
#define SLOT_NONE_PILOT             0x04
#define SLOT_RED_PILOT              0x05
#define SLOT_IR_PILOT               0x06
#define SLOT_GREEN_PILOT            0x07

/* MAX30105 Expected Part ID: Only Readable */
#define MAX_30105_EXPECTEDPARTID    0x15

#define MAX30105_SUCCESS            (1)
#define MAX30105_FAILED             (0)

#define STORAGE_SIZE                (4)

//***********************************************************************************
// Global Variables
//***********************************************************************************


//***********************************************************************************
// Function Prototypes
//***********************************************************************************
/******************************************************************//**********
 * @brief pulserate_write()
 * This function writes data to register.
 * @reg_address: Address of the register to be written
 * @data: Data to be written
 * @return: void
 *****************************************************************************/
void pulserate_write(uint8_t reg_address, uint8_t data);

/******************************************************************//**********
 * @brief pulserate_read()
 * This function reads data from register.
 * @reg_address: Address of the register to be written
 * @data: Data to be written
 * @return: void
 *****************************************************************************/
i2cStatus_t pulserate_read(uint8_t reg_address, uint8_t* data);

/******************************************************************//**********
 * @brief pulserate_read_3bytes()
 * This function reads data from register.
 * @reg_address: Address of the register to be written
 * @data: Data to be written
 * @return: void
 *****************************************************************************/
i2cStatus_t pulserate_read_3bytes(uint8_t reg_address, uint32_t * data);

/**
 * @brief Start-up test for the sensor
 *
 * After enabling the sensor, read Device ID for testing.
 *
 * @return 1 = SUCCESS, 0 = FAILED
 */
uint8_t test_pulserate(void);

/**
 * @brief Initialize the particle sensor
 *
 * Initialize the particle sensor for heart rate.
 *
 * @return void
 */
void init_pulserate(void);

/**
 * @brief Updates the IR LED value read by the sensor
 *
 * It updates the IR LED value read by the sensor after new
 * data has been fetched.
 *
 * @return IR value read
 */
uint32_t getIR(void);

/**
 * @brief Checks for new data and updates the value read by the sensor
 *
 * It checks and updates the data fields read by the sensor. If no
 * data is received, it returns 0.
 *
 * @return Number of samples
 */
int32_t check(void);

/**
 * @brief Detects a heart pulse
 *
 * It takes a sample value and the sample number and returns
 * true if a beat is detected
 *
 * @return True id heart beat detected
 */
bool checkForBeat(int32_t sample);

/**
 * @brief Average DC Estimator
 *
 * It estimates the DC value and return the average value.
 *
 * @return Average value
 */
int16_t averageDCEstimator(int32_t *p, uint16_t x);

/**
 * @brief Low Pass Filter
 *
 * It filters the value using a low pass filter implementation.
 *
 * @return Filtered value
 */
int16_t lowPassFIRFilter(int16_t din);

/**
 * @brief Integer multiplier
 *
 * It multiplies the values and returns it.
 *
 * @return Multiplied value
 */
int32_t mul16(int16_t x, int16_t y);

/**
 * @brief Reads and updates the register values
 *
 * It reads and updates the register values based on the register mask.
 *
 * @return void
 */
void bitMask(uint8_t reg, uint8_t mask, uint8_t thing);


#endif /* __PULSERATE_H__ */
