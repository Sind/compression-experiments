
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "snappy/snappy-c.h"
#include "fpaq0/fpaq0-c.h"
#include "PPMs/PPMs.h"
#include "LZW/lzw.h"
#include "LZW_MC/lzw_mc.h"
#include "LZ4/lz4.h"


#include "humidity_data.h"
#include "temperature_data.h"
#include "mixed_data.h"
#include "encode.h"

#define LENGTHS 2048
#define NUM_RUNS 4

#define NUM_DATASETS 3

#define VERIFY

void* datasets[NUM_DATASETS] = {
	humidity_data,
	temperature_data,
	mixed_data
};

char* dataset_names[NUM_DATASETS] = {
	"humidity",
	"temperature",
	"mixed"
};

int (*encoding_algorithms[NUM_DATASETS]) (void * input_orig, uint16_t input_length, int8_t* output) = {
	encode_uint8_t,
	encode_uint16_t,
	encode_uint8_t
};

int8_t encoded[3000];
uint8_t compressed[3000];


char* tex_encoding_names[2] = {
	"",
	"encoded"
};

#define NUM_ALGORITHMS 9
int (*compression_algorithms[NUM_ALGORITHMS]) (void* raw, int raw_len, void* compressed, int compressed_max_len) = {
	NULL,
	Snappy_Flex_Compress,
	FPAQ0_Flex_Compress,
	PPMs5_Flex_Compress,
	PPMs3_Flex_Compress,
	PPMs1_Flex_Compress,
	LZW_Flex_Compress,
	LZW_MC_Flex_Compress,
	LZ4_Flex_Compress
};

#ifdef VERIFY
uint8_t verification_decompressed[3000];
uint8_t verification_decoded[3000];

int (*decoding_algorithms[NUM_DATASETS]) (int8_t* input, int input_length, void* output_orig) = {
	decode_uint8_t,
	decode_uint16_t,
	decode_uint8_t
};

int (*decompression_algorithms[NUM_ALGORITHMS]) (void* compressed, int compressed_len, void* decompressed) = {
	NULL,
	Snappy_Flex_Decompress,
	FPAQ0_Flex_Decompress,
	PPMs5_Flex_Decompress,
	PPMs3_Flex_Decompress,
	PPMs1_Flex_Decompress,
	LZW_Flex_Decompress,
	LZW_MC_Flex_Decompress,
	LZ4_Flex_Decompress
};

#endif

char* algorithm_names[NUM_ALGORITHMS] = {
	"None",
	"Snappy",
	"FPAQ0",
	"PPMs(5)",
	"PPMs(3)",
	"PPMs(1)",
	"LZW",
	"LZW_MC",
	"LZ4"
};
char* tex_algorithm_names[NUM_ALGORITHMS] = {
	"none",
	"snappy",
	"fpaq",
	"ppmsf",
	"ppmst",
	"ppmso",
	"lzw",
	"lzwmc",
	"lzf"
};


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
	LZW_INIT();
	LZW_MC_INIT();

	// void *data_to_send = (void*)data;
	// volatile size_t len = DATA_LENGTH;
	FILE * output_file = fopen( "compression_test.tex", "w" );

	printf("test\n");
	for (int dataset = 0; dataset < NUM_DATASETS; dataset++)
	{
		for (int encode = 0; encode <= 1; encode++)
		{
			for(int compression = 0; compression < NUM_ALGORITHMS; compression++)
			{
				fprintf(output_file, "\\pgfplotstableread[row sep=\\\\, col sep=&]{\n" );
				fprintf(output_file, "\tpacketsize & compression\\\\\n" );

				for(int packet_size = 512; packet_size > 1; packet_size=packet_size/2)
				{
					if(packet_size < 32 && dataset == 2){
						continue;
					}
					int compression_total = 0;
					for(int run = 0; run < LENGTHS/packet_size; run++)
					{
						void * input_data = datasets[dataset] + run * packet_size;
						int len = packet_size;
						if(encode != 0){
							len = (*encoding_algorithms[dataset])(input_data, len, encoded);
							input_data = encoded;
						}
						if(compression != 0){
							len = (*compression_algorithms[compression])(input_data, len, compressed, packet_size*2);
							input_data = compressed;
						}
						compression_total += len;

#ifdef VERIFY
						void* verify_from = input_data;
						void* verify_to = datasets[dataset] + run * packet_size;
						if(compression != 0){
							len = (*decompression_algorithms[compression])(verify_from, len, verification_decompressed);
							verify_from = verification_decompressed;
						}
						if(encode != 0){
							len = (*decoding_algorithms[dataset])(verify_from, len, verification_decoded);
							verify_from = verification_decoded;
						}
						if(len != packet_size){
							printf("packet match failed with %d at %s %s %s %d %d\n", len, dataset_names[dataset],tex_encoding_names[encode],algorithm_names[compression], packet_size, run);
							return -1;
						}
						int verify_val = memcmp(verify_from,verify_to,len);
						if(verify_val != 0){
							printf("verification failed at %s %s %s %d %d\n",dataset_names[dataset],tex_encoding_names[encode],algorithm_names[compression], packet_size, run);
							return -1;
						}
#endif
					}
					fprintf(output_file, "\t%d & %f \\\\\n",packet_size,(double)LENGTHS/compression_total);
				}
				fprintf(output_file, "}\\compression%s%s%s\n", dataset_names[dataset],tex_algorithm_names[compression], tex_encoding_names[encode]);
				fprintf(output_file, "\\pgfplotstabletranspose[colnames from=packetsize]\\compression%s%s%strans{\\compression%s%s%s}\n", dataset_names[dataset],tex_algorithm_names[compression], tex_encoding_names[encode], dataset_names[dataset],tex_algorithm_names[compression], tex_encoding_names[encode]);
				fprintf(output_file, "\\pgfplotstablevertcat{\\compression%s%scombined}{\\compression%s%s%strans}\n\n", dataset_names[dataset],tex_encoding_names[encode], dataset_names[dataset],tex_algorithm_names[compression], tex_encoding_names[encode]);
			}
		}
	}
	fclose( output_file );

	return 0;

}

