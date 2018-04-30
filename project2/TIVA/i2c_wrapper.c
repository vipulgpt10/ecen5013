/**
 * @file i2c_wrapper.c
 * @brief The implementation file for i2c wrapper functions
 *
 * This implementation file provides the function definitions for
 * various i2c wrapper operations.
 *
 * @author Kovuru Nagarjuna Reddy
 * @date 26 April 2018
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c_wrapper.h"

//***********************************************************************************
// Global variables/structures and Macros
//***********************************************************************************


//***********************************************************************************
//Function Definitions
//***********************************************************************************
/******************************************************************//**********
 * @brief i2c_read_byte()
 * This function reads single byte.
 * @address: address of the register to be read
 * @data: Pointer to store the received data
 * @return: status
 *****************************************************************************/
i2cStatus_t i2c_read_byte(uint8_t address, uint8_t* data)
{
    /* Check for NULL pointer */
    if(data==NULL)
    {
        return I2C_READ_NULL_PTR;
    }
    /* Set slave address, true=read*/
    ROM_I2CMasterSlaveAddrSet(I2C2_BASE,address,true);
    /* send command to receive single command*/
    ROM_I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);
    /* Wait until the transmit is complete */
    while(ROM_I2CMasterBusy(I2C2_BASE));
    /* read data */
    *(data) = ROM_I2CMasterDataGet(I2C2_BASE);

    while(ROM_I2CMasterBusy(I2C2_BASE));

    return SUCCESS;
}

/******************************************************************//**********
 * @brief i2c_read_word()
 * This function reads word.
 * @address: address of the register to be read
 * @data: Pointer to store the received data
 * @return: status
 *****************************************************************************/
i2cStatus_t i2c_read_word(uint8_t address, uint16_t* data)
{
    if(data==NULL)
    {
        return I2C_READ_NULL_PTR;
    }
    /* Set slave address, true=read*/
    ROM_I2CMasterSlaveAddrSet(I2C2_BASE,address,true);
    /* Send command to start burst receive*/
    ROM_I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_RECEIVE_START);
    /* Wait until the transmit is complete */
    while(ROM_I2CMasterBusy(I2C2_BASE));
    /* read 1st byte*/
    *((uint8_t*)data) = ROM_I2CMasterDataGet(I2C2_BASE);
    /* set slave address to read 2nd byte*/
    ROM_I2CMasterSlaveAddrSet(I2C2_BASE,address,true);
    /* send command to receive last byte(finish)*/
    ROM_I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    /* Wait until the transmit is complete */
    while(ROM_I2CMasterBusy(I2C2_BASE));
    /* read data */
    *((uint8_t*)data + 1) = ROM_I2CMasterDataGet(I2C2_BASE);

    return SUCCESS;
}

/******************************************************************//**********
 * @brief i2c_write_single()
 * This function writes single byte.
 * @address: address of the register to be written
 * @data: Data to be written
 * @return: status
 *****************************************************************************/
void i2c_write_single(uint8_t address, uint8_t data)
{
    /* Set slave address, false=write*/
    ROM_I2CMasterSlaveAddrSet(I2C2_BASE,address,false);
    /* put data */
    ROM_I2CMasterDataPut(I2C2_BASE, data);
    /* write command to start sending data  */
    ROM_I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_SINGLE_SEND);
    /* wait until the transmit is complete */
    while(ROM_I2CMasterBusy(I2C2_BASE));
}

/******************************************************************//**********
 * @brief i2c_write()
 * This function writes start byte in burst writing.
 * @address: address of the register to be written
 * @data: Data to be written
 * @return: status
 *****************************************************************************/
void i2c_write(uint8_t address, uint8_t data)
{
    /* Set slave address, false=write*/
    ROM_I2CMasterSlaveAddrSet(I2C2_BASE,address,false);
    /* put data */
    ROM_I2CMasterDataPut(I2C2_BASE, data);
    /* write command to start sending data  */
    ROM_I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_SEND_START);
    /* wait until the transmit is complete */
    while(ROM_I2CMasterBusy(I2C2_BASE));
}

/******************************************************************//**********
 * @brief i2c_write_temp()
 * This function writes last(finish) byte in burst writing.
 * @address: address of the register to be written
 * @data: Data to be written
 * @return: status
 *****************************************************************************/
void i2c_write_temp(uint8_t address, uint8_t data)
{
    /* Set slave address, false=write*/
    ROM_I2CMasterSlaveAddrSet(I2C2_BASE,address,false);
    /* put data */
    ROM_I2CMasterDataPut(I2C2_BASE, data);
    /* write command to finish sending data  */
    ROM_I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);
    /* wait until the transmit is complete */
    while(ROM_I2CMasterBusy(I2C2_BASE));
    return;
}

/******************************************************************//**********
 * @brief i2c_init()
 * This function initializes I2C peripheral and GPIO pins(SCL&SDA)
 *****************************************************************************/
void i2c_init(void)
{
    /* Enable GPIO peripheral used for I2C*/
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    /* Wait for pheripheral to be ready */
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL));

    /* Configure GPIO MUX as I2C Pins */
    ROM_GPIOPinConfigure(GPIO_PL1_I2C2SCL);
    ROM_GPIOPinConfigure(GPIO_PL0_I2C2SDA);
    ROM_GPIOPinTypeI2C(GPIO_PORTL_BASE, GPIO_PIN_0);
    GPIOPinTypeI2CSCL(GPIO_PORTL_BASE, GPIO_PIN_1);

    /* Enable I2C Peripheral */
    ROM_SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C2);
    ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
    /* wait for the peripheral to be ready */
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_I2C2));
    /* Enable clock */
    ROM_I2CMasterInitExpClk(I2C2_BASE, 40000000, true);
}

/******************************************************************//**********
 * @brief i2c_read_3bytes()
 * This function reads 3 bytes.
 * @address: address of the register to be read
 * @data: Pointer to store the received data
 * @return: status
 *****************************************************************************/
i2cStatus_t i2c_read_3bytes(uint8_t address, uint32_t* data)
{
    if(data==NULL)
        return I2C_READ_NULL_PTR;

    *((uint8_t*)data+3) = 0;

    ROM_I2CMasterSlaveAddrSet(I2C2_BASE,address,true);
    ROM_I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(ROM_I2CMasterBusy(I2C2_BASE));
    *((uint8_t*)data+2) = ROM_I2CMasterDataGet(I2C2_BASE);
    while(ROM_I2CMasterBusy(I2C2_BASE));

    ROM_I2CMasterSlaveAddrSet(I2C2_BASE,address,true);
    ROM_I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_RECEIVE_CONT);
    while(ROM_I2CMasterBusy(I2C2_BASE));
    *((uint8_t*)data + 1) = ROM_I2CMasterDataGet(I2C2_BASE);
    while(ROM_I2CMasterBusy(I2C2_BASE));

    ROM_I2CMasterSlaveAddrSet(I2C2_BASE,address,true);
    ROM_I2CMasterControl(I2C2_BASE,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(ROM_I2CMasterBusy(I2C2_BASE));
    *((uint8_t*)data + 0) = ROM_I2CMasterDataGet(I2C2_BASE);
    while(ROM_I2CMasterBusy(I2C2_BASE));

    return SUCCESS;
}
