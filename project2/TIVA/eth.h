/* eth.h */

#ifndef INC_ETH_H_
#define INC_ETH_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <inc/hw_gpio.h>
#include <inc/hw_emac.h>

#include "driverlib/interrupt.h"
#include <driverlib/gpio.h>
#include <driverlib/flash.h>
#include <driverlib/sysctl.h>
#include <driverlib/udma.h>
#include <driverlib/pin_map.h>

//emac specific
#include <driverlib/emac.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"


/* Ethernet DMA descriptors */
#define NUM_TX_DESCRIPTORS 3
#define NUM_RX_DESCRIPTORS 3

tEMACDMADescriptor g_psRxDescriptor[NUM_TX_DESCRIPTORS];
tEMACDMADescriptor g_psTxDescriptor[NUM_RX_DESCRIPTORS];
uint32_t g_ui32RxDescIndex;
uint32_t g_ui32TxDescIndex;

/* Transmit and receive buffers */
#define RX_BUFFER_SIZE 1536
uint8_t g_ppui8RxBuffer[NUM_RX_DESCRIPTORS][RX_BUFFER_SIZE];

/* Functions */

void eth_init(uint32_t ui32Base);

int32_t eth_tx(uint8_t *pui8Buf, int32_t i32BufLen);

int32_t eth_rx(uint8_t * rx_buff, int32_t i32FrameLen);

int32_t eth_rx_framelen(void);

void EthernetIntHandler(void);


#endif /* INC_ETH_H_ */
