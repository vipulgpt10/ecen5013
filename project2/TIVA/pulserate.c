/**
 * @file pulserate.c
 * @brief The implementation file for pulserate sensor read&write functions
 *
 * @author Kovuru Nagarjuna Reddy & Vipul Gupta
 * @date 26 April 2018
 *
 */

/*
 Optical Heart Rate Detection (PBA Algorithm)
 By: Nathan Seidle
 SparkFun Electronics
 Date: October 2nd, 2016
*/

/* Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*
*/

//***********************************************************************************
// Include files
//***********************************************************************************
#include "pulserate.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************
uint8_t activeLEDs;

extern uint32_t g_ui32SysClock;

extern xSemaphoreHandle i2c_lock;

int16_t IR_AC_Max = 20;
int16_t IR_AC_Min = -20;

int16_t IR_AC_Signal_Current = 0;
int16_t IR_AC_Signal_Previous;
int16_t IR_AC_Signal_min = 0;
int16_t IR_AC_Signal_max = 0;
int16_t IR_Average_Estimated;

int16_t positiveEdge = 0;
int16_t negativeEdge = 0;
int32_t ir_avg_reg = 0;

int16_t cbuf[32];
uint8_t offset = 0;

static const uint16_t FIRCoeffs[12] = {172, 321, 579, 927, 1360, 1858, 2390, 2916, 3391, 3768, 4012, 4096};


struct Record
{
  uint32_t red[STORAGE_SIZE];
  uint32_t IR[STORAGE_SIZE];
  uint32_t green[STORAGE_SIZE];
  uint8_t head;
  uint8_t tail;
} sense; //This is our circular buffer of readings from the sensor


void pulserate_write(uint8_t reg_address, uint8_t data)
{
    xSemaphoreTake(i2c_lock, portMAX_DELAY);
    /* write register to be written*/
    i2c_write(MAX30105_SLAVE_ADDR,reg_address);
    /* write data*/
    i2c_write_temp(MAX30105_SLAVE_ADDR,data);
    xSemaphoreGive(i2c_lock);

}

i2cStatus_t pulserate_read(uint8_t reg_address, uint8_t* data)
{
    xSemaphoreTake(i2c_lock, portMAX_DELAY);
    /* write register to be read*/
    i2c_write_single(MAX30105_SLAVE_ADDR,reg_address);
    /* read data*/
    i2c_read_byte(MAX30105_SLAVE_ADDR, data);
    xSemaphoreGive(i2c_lock);
    return SUCCESS;

}

i2cStatus_t pulserate_read_3bytes(uint8_t reg_address, uint32_t * data)
{
    xSemaphoreTake(i2c_lock, portMAX_DELAY);
    /* do a single write to gain access to a register */
    i2c_write_single(MAX30105_SLAVE_ADDR,reg_address);

    /* now just do a plain I2C read */
    i2cStatus_t i2c_read_byte_rc;
    i2c_read_byte_rc = i2c_read_3bytes(MAX30105_SLAVE_ADDR, data);

    xSemaphoreGive(i2c_lock);
    /* error handling */
    if(i2c_read_byte_rc != SUCCESS)
        return i2c_read_byte_rc;

    return SUCCESS;
}

uint8_t test_pulserate(void)
{
    uint8_t check = 0;

    pulserate_read(MAX30105_PARTID, &check);
    if(check != MAX_30105_EXPECTEDPARTID)     /* Default value */
        return MAX30105_FAILED;

    return MAX30105_SUCCESS;
}

void init_pulserate(void)
{
    uint8_t readData;

    /* Soft reset */
    bitMask(MAX30105_MODECONFIG, MAX30105_RESET_MASK, MAX30105_RESET);
    /* proceed if reset is complete */
    pulserate_read(MAX30105_MODECONFIG, &readData);
    while(readData & MAX30105_RESET)
    {
        pulserate_read(MAX30105_MODECONFIG, &readData);
    }

    /* FIFO Config | Sample Average = 4 */
    bitMask(MAX30105_FIFOCONFIG, MAX30105_SAMPLEAVG_MASK, MAX30105_SAMPLEAVG_4);
    /* Enable roll over if FIFO over flows */
    bitMask(MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_ENABLE);

    /* LED Mode Config | Multi Mode 2 */
    bitMask(MAX30105_MODECONFIG, MAX30105_MODE_MASK, MAX30105_MODE_MULTILED);
    activeLEDs = 3;

    /* Particle Sensing Configuration | ADC Range 4096 | Sample rate 400 */
    bitMask(MAX30105_PARTICLECONFIG, MAX30105_ADCRANGE_MASK, MAX30105_ADCRANGE_4096);

    bitMask(MAX30105_PARTICLECONFIG, MAX30105_SAMPLERATE_MASK, MAX30105_SAMPLERATE_400);

    /* pulse width */
    bitMask(MAX30105_PARTICLECONFIG, MAX30105_PULSEWIDTH_MASK, MAX30105_PULSEWIDTH_411);

    /* LED Pulse Amplitude Configuration | power level - 0x1F */
    pulserate_write(MAX30105_LED1_PULSEAMP, 0x1F); // Turn Red LED to low to indicate sensor is On
    pulserate_write(MAX30105_LED2_PULSEAMP, 0x1F); // IR
    pulserate_write(MAX30105_LED3_PULSEAMP, 0x1F); // Turn off Green
    pulserate_write(MAX30105_LED_PROX_AMP, 0x1F); // Presence detection of ~8 inch

    /* Multi-LED Mode Configuration, Enable the reading of the three LEDs */
    bitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT1_MASK, SLOT_RED_LED);
    bitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT2_MASK, SLOT_IR_LED << 4);
    bitMask(MAX30105_MULTILEDCONFIG2, MAX30105_SLOT3_MASK, SLOT_GREEN_LED);

    /* Clear FIFO */
    pulserate_write(MAX30105_FIFOWRITEPTR, 0);
    pulserate_write(MAX30105_FIFOOVERFLOW, 0);
    pulserate_write(MAX30105_FIFOREADPTR, 0);

    pulserate_write(MAX30105_LED1_PULSEAMP, 0x1D); // Turn Red LED to low to indicate sensor is On
    pulserate_write(MAX30105_LED3_PULSEAMP, 0); // Turn off Green

}

uint32_t getIR(void)
{
    /* Check for new data */
    while(!(check()))
    {
        SysCtlDelay(g_ui32SysClock/3000);
    }

    return (sense.IR[sense.head]);
}



int32_t check(void)
{
    //Read register FIDO_DATA in (3-byte * number of active LED) chunks
    //Until FIFO_RD_PTR = FIFO_WR_PTR

    uint8_t readPointer, writePointer;
    int32_t numberOfSamples = 0;
    uint32_t bytesLeftToRead, toGet;

    pulserate_read(MAX30105_FIFOREADPTR, &readPointer);
    pulserate_read(MAX30105_FIFOWRITEPTR, &writePointer);

    //Do we have new data?
    if (readPointer != writePointer)
    {
        //Calculate the number of readings we need to get from sensor
        numberOfSamples = writePointer - readPointer;

        if (numberOfSamples < 0)
            numberOfSamples += 32; //Wrap condition

        //We now have the number of readings, now calc bytes to read
        //For this example we are just doing Red and IR (3 bytes each)
        bytesLeftToRead = numberOfSamples * activeLEDs * 3;

        /* read a burst of data from the FIFO register | I2C Buffer length = 32 */
        while (bytesLeftToRead > 0)
        {
            toGet = bytesLeftToRead;
            if (toGet > I2C_BUFFER_LENGTH)
            {
                //Trim toGet to be a multiple of the samples we need to read
                toGet = I2C_BUFFER_LENGTH - (I2C_BUFFER_LENGTH % (activeLEDs * 3));
            }

            bytesLeftToRead -= toGet;

            while (toGet > 0)
            {
                sense.head++; //Advance the head of the storage struct
                sense.head %= STORAGE_SIZE; //Wrap condition

                uint32_t tempLong;

                //Burst read three bytes - RED
                pulserate_read_3bytes(MAX30105_FIFODATA, &tempLong);

                tempLong &= 0x3FFFF; //Zero out all but 18 bits

                //Store this reading into the sense array
                sense.red[sense.head] = tempLong;

                //Burst read three more bytes - IR
                pulserate_read_3bytes(MAX30105_FIFODATA, &tempLong);

                tempLong &= 0x3FFFF; //Zero out all but 18 bits

                sense.IR[sense.head] = tempLong;

                //Burst read three more bytes - Green
                pulserate_read_3bytes(MAX30105_FIFODATA, &tempLong);

                tempLong &= 0x3FFFF; //Zero out all but 18 bits

                sense.green[sense.head] = tempLong;

                toGet -= activeLEDs * 3;
            }
        }
    }
    return (numberOfSamples);
}

bool checkForBeat(int32_t sample)
{
    bool beatDetected = false;

    //  Save current state
    IR_AC_Signal_Previous = IR_AC_Signal_Current;

    //  Process next data sample
    IR_Average_Estimated = averageDCEstimator(&ir_avg_reg, sample);
    IR_AC_Signal_Current = lowPassFIRFilter(sample - IR_Average_Estimated);

    //  Detect positive zero crossing (rising edge)
    if ((IR_AC_Signal_Previous < 0) & (IR_AC_Signal_Current >= 0))
    {
        IR_AC_Max = IR_AC_Signal_max; //Adjust our AC max and min
        IR_AC_Min = IR_AC_Signal_min;

        positiveEdge = 1;
        negativeEdge = 0;
        IR_AC_Signal_max = 0;

        if ((IR_AC_Max - IR_AC_Min) > 20 & (IR_AC_Max - IR_AC_Min) < 1000)
        {
            //Heart beat!!!
            beatDetected = true;
        }
    }

    //  Detect negative zero crossing (falling edge)
    if ((IR_AC_Signal_Previous > 0) & (IR_AC_Signal_Current <= 0))
    {
        positiveEdge = 0;
        negativeEdge = 1;
        IR_AC_Signal_min = 0;
    }

    //  Find Maximum value in positive cycle
    if (positiveEdge & (IR_AC_Signal_Current > IR_AC_Signal_Previous))
    {
        IR_AC_Signal_max = IR_AC_Signal_Current;
    }

    //  Find Minimum value in negative cycle
    if (negativeEdge & (IR_AC_Signal_Current < IR_AC_Signal_Previous))
    {
        IR_AC_Signal_min = IR_AC_Signal_Current;
    }

    return(beatDetected);
}

int16_t averageDCEstimator(int32_t *p, uint16_t x)
{
    *p += ((((long) x << 15) - *p) >> 4);
    return (*p >> 15);
}


int16_t lowPassFIRFilter(int16_t din)
{
    cbuf[offset] = din;
    uint8_t i;

    int32_t z = mul16(FIRCoeffs[11], cbuf[(offset - 11) & 0x1F]);

    for(i = 0 ; i < 11 ; i++)
    {
        z += mul16(FIRCoeffs[i], cbuf[(offset - i) & 0x1F] + cbuf[(offset - 22 + i) & 0x1F]);
    }

    offset++;
    offset %= 32; //Wrap condition

    return(z >> 15);
}

int32_t mul16(int16_t x, int16_t y)
{
    return((long)x * (long)y);
}

void bitMask(uint8_t reg, uint8_t mask, uint8_t thing)
{
    // Grab current register context
    uint8_t readData;
    pulserate_read(reg, &readData);

    // Zero-out the portions of the register we're interested in
    readData = readData & mask;

    // Update the contents
    pulserate_write(reg, readData | thing);
}
