/*
 * program_config.h
 *
 *  Created on: Mar 3, 2017
 *      Author: srodk
 */

#ifndef SRC_PROGRAM_CONFIG_H_
#define SRC_PROGRAM_CONFIG_H_

#include <stdbool.h>

/********************************************************************************
 * GLOBAL DEFINES
 * */

#define SPLIT_SIZE 512

#define SPLIT_NUM 4

/* Whether to be in send mode or receive mode */
#define SENDING

/* Compression algorithm to use
 * for compression and decompression.
 * Select at most one.
 * If none are selected, that data is not compressed/decompressed
 */
//#define COMPRESS_SNAPPY
//#define COMPRESS_FPAQ0
//#define COMPRESS_PPMS5
//#define COMPRESS_PPMS3
//#define COMPRESS_PPMS1
//#define COMPRESS_LZW
//#define COMPRESS_LZW_MC
//#define COMPRESS_LZ4

/* whether or not transform the data into a more compressable domain */
//#define ENCODE

/* Time in milliseconds to wait before starting the core program */
#define STARTUP_WAIT_MS 1000

/* Time in milliseconds between periodic sending/receiving. */
#define UPDATE_MS      600

/* Radio channel top communicate on */
#define RADIO_CHANNEL 0

#ifdef SENDING
/********************************************************************************
 * SEND MODE DEFINES
 * Settings specific to send mode
 */


/* Radio transfer power gain. */
#define RADIO_TX_DBM_1 10
#define RADIO_TX_DBM_2 20

/* Time to wait in milliseconds between sending packets, to let the receiver catch up */
#define TX_WAIT_MS 1

#else
/********************************************************************************
 * READ MODE DEFINES
 * settings specific to receive mode
 */

/* Whether or not to verify the decompressed data.
 * If set, the program compares decompressed value
 * to original 50 ms after receive, and
 * lights a led if the value is wrong.
 */
//#define VERIFY

/* The receiver needs to wake up and listen for transmition,
 * before the transmitter begins transitting.
 * Time to preempt the transmitter by, in milliseconds
 */
#define RADIO_RX_TIMER_PREEMPT 2

#endif


/*******************************************************************************
 * STATIC DEFINES AND VARIABLES
 * Do not modify these
 */

/* Memory manager configuration */
#define MAX_BUFFER_SIZE  76
/* General application memory sizes */
#define APP_MAX_PACKET_LENGTH  (MAX_BUFFER_SIZE - sizeof(RAIL_RxPacketInfo_t))
#define RAIL_RF_XTAL_FREQ 38400000

unsigned char messageBuffer[MAX_BUFFER_SIZE];

#if defined(COMPRESS_PPMS5) ||defined(COMPRESS_PPMS3) || defined(COMPRESS_PPMS1)
#define COMPRESS_PPMS
#endif
#ifdef SENDING

#define RADIO_TX_DDBM_1 (RADIO_TX_DBM_1 * 10)
#define RADIO_TX_DDBM_2 (RADIO_TX_DBM_2 * 10)

//#define COMPRESS_WAIT_MS 900

#if defined(COMPRESS_SNAPPY) || defined(COMPRESS_FPAQ0) || defined(COMPRESS_PPMS) || defined(COMPRESS_LZW) || defined(COMPRESS_LZW_MC)|| defined(COMPRESS_LZ4)
#define COMPRESS
#endif

volatile bool packageSent;

#else
#if defined(COMPRESS_SNAPPY) || defined(COMPRESS_FPAQ0) || defined(COMPRESS_PPMS) || defined(COMPRESS_LZW) || defined(COMPRESS_LZW_MC)|| defined(COMPRESS_LZ4)
#define DECOMPRESS
#endif

volatile bool packageReceived;
int rxDataLen;
void *rxData;

#endif

#endif /* SRC_PROGRAM_CONFIG_H_ */


