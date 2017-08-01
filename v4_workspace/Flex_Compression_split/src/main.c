#include "em_device.h"
#include "em_chip.h"

#include "em_cmu.h"
#include "em_emu.h"

#include "rtcdriver.h"
#include <string.h>

#include "rail.h"
#include "rail_types.h"

#include "rail_config.h"

#include "program_config.h"


#if defined COMPRESS_SNAPPY
#include "snappy/snappy-c.h"
#elif defined COMPRESS_FPAQ0
#include "fpaq0/fpaq0-c.h"
#elif defined COMPRESS_PPMS
#include "PPMs/PPMs.h"
#if defined COMPRESS_PPMS5
#define Flex_Compress(raw,raw_len,compressed,compressed_max_len) PPMs5_Flex_Compress(raw,raw_len,compressed,compressed_max_len);
#define Flex_Decompress(compressed,recvlen,encoded) PPMs5_Flex_Decompress(compressed,recvlen,encoded);;
#elif defined  COMPRESS_PPM3
#define Flex_Compress(raw,raw_len,compressed,compressed_max_len) PPMs3_Flex_Compress(raw,raw_len,compressed,compressed_max_len);
#define Flex_Decompress(compressed,recvlen,encoded) PPMs3_Flex_Decompress(compressed,recvlen,encoded);;
#else
#define Flex_Compress(raw,raw_len,compressed,compressed_max_len) PPMs1_Flex_Compress(raw,raw_len,compressed,compressed_max_len);
#define Flex_Decompress(compressed,recvlen,encoded) PPMs1_Flex_Decompress(compressed,recvlen,encoded);;
#endif
#elif defined COMPRESS_LZW
#include "LZW/lzw.h"
#elif defined COMPRESS_LZW_MC
#include "LZW_MC/lzw_mc.h"
#elif defined COMPRESS_LZ4
#include "LZ4/lz4.h"
#endif



int currentConfig = 0;

/* load data to transfer/compress */
#include "data.h"


#ifdef SENDING
RAIL_TxData_t transmitPayload;
volatile int radio_tx_dbm = RADIO_TX_DDBM_1;
#else

DATA_TYPE decompressed[DATA_SAMPLES];

#ifdef VERIFY
#include "bsp.h"
#endif

#endif

#ifdef ENCODE

ENCODED_DATA_TYPE encoded[ENCODED_DATA_LENGTH];

#endif

uint8_t compressed[MAX_COMPRESSED_LENGTH];

static const RAIL_Init_t railInitParams = {
  APP_MAX_PACKET_LENGTH,
  RAIL_RF_XTAL_FREQ,
};

void changeRadioConfig(int newConfig)
{
  // Turn off the radio before reconfiguring it
  RAIL_RfIdle();

  // Reconfigure the radio parameters
  RAIL_PacketLengthConfigFrameType(frameTypeConfigList[newConfig]);
  if (RAIL_RadioConfig((void*)configList[newConfig])) { while(1); }

  // Set us to a valid channel for this config and force an update in the main
  // loop to restart whatever action was going on
  RAIL_ChannelConfig(channelConfigs[newConfig]);
  currentConfig = newConfig;
}




/**************************************************************************//**
 * Local variables
 *****************************************************************************/
/* RTC callback parameters. */
static void (*rtcCallback)(void*) = 0;
static void * rtcCallbackArg = 0;

/** Timer used for periodic send/receive. */
RTCDRV_TimerID_t UpdateTimerId;

/** Timer used for periodic compression */
RTCDRV_TimerID_t CompressTimerId;

static void UpdateCallback(RTCDRV_TimerID_t id, void *user);
static void CompressCallback(RTCDRV_TimerID_t id, void *user);

volatile int perform = 0;
volatile int compress = 0;
/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
	/* Chip errata */
	CHIP_Init();


	/* Initialize the system clocks and other HAL components */
	halInit();

	/* Initialize hardware */
	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
	EMU_DCDCInit(&dcdcInit);

	//NVIC_EnableIRQ(USART1_TX_IRQn);

	/* Enable High Frequency Peripherals */
	CMU_ClockEnable(cmuClock_HFPER, true);

	/* Initialize Radio */
	RAIL_RfInit(&railInitParams);

	/* Configure modem, packet handler */
	changeRadioConfig(currentConfig);

	/* Configure RAIL callbacks with no appended info*/

	RAIL_RxConfig((  RAIL_RX_CONFIG_INVALID_CRC
				 | RAIL_RX_CONFIG_SYNC1_DETECT
				 | RAIL_RX_CONFIG_ADDRESS_FILTERED
				 | RAIL_RX_CONFIG_BUFFER_OVERFLOW),
				false);

	RTCDRV_Init();
	RTCDRV_AllocateTimer(&UpdateTimerId);
	RTCDRV_AllocateTimer(&CompressTimerId);

	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

#if defined(COMPRESS_LZW)
	LZW_INIT();
#elif defined(COMPRESS_LZW_MC)
	LZW_MC_INIT();
#endif

	perform = 0;
	compress = 0;
	RTCDRV_StartTimer(UpdateTimerId, rtcdrvTimerTypeOneshot,
	                  STARTUP_WAIT_MS+UPDATE_MS, UpdateCallback, 0);

#ifdef SENDING
	RAIL_TxPowerSet(radio_tx_dbm);
	volatile int32_t dBm = RAIL_TxPowerGet();
	volatile int totalcompressedlen = 0;

	RTCDRV_StartTimer(CompressTimerId, rtcdrvTimerTypeOneshot,
	                  STARTUP_WAIT_MS+UPDATE_MS/2, CompressCallback, 0);

	void *data_to_send = (void*)data;
	volatile size_t len = DATA_LENGTH;

#else
#ifdef VERIFY

	 BSP_Init( BSP_INIT_BCC );
	 BSP_LedsInit();
#endif
#endif

	 int splitnum = 0;
	 volatile int totals_split = 0;
	 int recvlen = 0;


	while(1)
	{
		/* wait for asynchronous ping */
		EMU_EnterEM2(true);
		if(compress == 1){
			compress = 0;
#ifdef SENDING
			data_to_send = ((void*)data) + splitnum*SPLIT_SIZE;
			len = SPLIT_SIZE;
#ifdef ENCODE
			len = preprocess((DATA_TYPE*)data_to_send,len/sizeof(DATA_TYPE),encoded);
			data_to_send = (void*)encoded;
#endif
#ifdef COMPRESS
			len = Flex_Compress(data_to_send,len,compressed,MAX_COMPRESSED_LENGTH);
			data_to_send = compressed;
#endif
#else
			void* position = (void*)decompressed + (splitnum*SPLIT_SIZE);
#ifdef DECOMPRESS
#ifdef ENCODE
			recvlen = Flex_Decompress(compressed,recvlen,encoded);
			recvlen = postprocess(encoded,recvlen,position);
#else
			recvlen = Flex_Decompress(compressed,recvlen,position);
#endif
#elif defined(ENCODE)
			recvlen = postprocess(compressed,recvlen,position);
#else
			memcpy(position,compressed,recvlen);
#endif
			splitnum++;
			if(splitnum == SPLIT_NUM) splitnum = 0;
#endif
		}
		if (perform == 1){

#ifdef SENDING
			totalcompressedlen += len;
			transmitPayload.dataPtr = messageBuffer;
			transmitPayload.dataLength = len+2;
			messageBuffer[0] = len+1;
			messageBuffer[1] = splitnum;
			memcpy(&(messageBuffer[2]), data_to_send, len);
			totals_split += len;

			packageSent = false;
			RAIL_RfIdle();
			while(RAIL_TxDataLoad(&transmitPayload) != 0);
			RAIL_TxStart(RADIO_CHANNEL, NULL, NULL);

			/* wait until package sent */
			while(packageSent != true);
			splitnum++;
			if(splitnum == SPLIT_NUM){
				splitnum = 0;
				totals_split = 0;
				if(radio_tx_dbm == RADIO_TX_DDBM_1){
					radio_tx_dbm = RADIO_TX_DDBM_2;
				}else{
					radio_tx_dbm = RADIO_TX_DDBM_1;
				}

				RAIL_TxPowerSet(radio_tx_dbm);
			}

			RAIL_RfIdle();
			perform = 0;
			RTCDRV_StartTimer(CompressTimerId, rtcdrvTimerTypeOneshot,
			                  UPDATE_MS/2, CompressCallback, 0);
			RTCDRV_StartTimer(UpdateTimerId, rtcdrvTimerTypeOneshot,
			                  UPDATE_MS, UpdateCallback, 0);
#else
			volatile void* address_to_receive = compressed;

			packageReceived = false;
			RAIL_RfIdle();
			RAIL_RxStart(RADIO_CHANNEL);
			while(!packageReceived) EMU_EnterEM2(true);
			recvlen = ((uint8_t*) rxData)[0]-1;
			uint8_t thissplit =((uint8_t*) rxData)[1];
			if(thissplit != splitnum){
				RAIL_RfIdle();
				return -1;
			}
			void* address_to_copy;
			address_to_copy = rxData+2;
			memcpy(address_to_receive,address_to_copy,recvlen);

			RAIL_RfIdle();
			perform = 0;
			RTCDRV_StartTimer(UpdateTimerId, rtcdrvTimerTypeOneshot,
			                  UPDATE_MS-RADIO_RX_TIMER_PREEMPT, UpdateCallback, 0);
			RTCDRV_StartTimer(CompressTimerId, rtcdrvTimerTypeOneshot,
			                  UPDATE_MS/2, CompressCallback, 0);

#endif

		}
	}
}


/**************************************************************************//**
 * @brief Callback used to count between wake ups
 *****************************************************************************/
static void UpdateCallback(RTCDRV_TimerID_t id, void *user)
{
	perform = 1;
}

static void CompressCallback(RTCDRV_TimerID_t id, void *user){
	compress = 1;
}
