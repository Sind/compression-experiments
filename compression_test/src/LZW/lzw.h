/*
 * lzw.h
 *
 *  Created on: Oct 18, 2016
 *      Author: srodk
 */

#ifndef SRC_LZW_H_
#define SRC_LZW_H_

#include <stdint.h>
/*
 * The number of bits per dictionary entry decides the size of the dictionay,
 * and how many bytes we can compress.
 * With 10 bits, we can end up with (1024-256)*10/8=960 bytes(compressed),
 * without prematurely running out of dictionary entries.
 * With 12 bits, we can go up to (4096-256)*12/8=5760 bytes(compressed).
 * It is best to go with as low number of bits as possible,
 * without running out of dictionary entries too soon.
 * If we want to compress data in 2-page chunks(1024 bytes),
 * we should use 10 bits. For 4-page chunks, 11 bits.
 * And for 8-page chunks, 12 bits.
 */
#define NUM_BITS_DICTIONARY 9

#define DICTIONARY_LENGTH (1 << NUM_BITS_DICTIONARY)

#define DICTIONARY_TOTAL_LENGTH (DICTIONARY_LENGTH * 4)

typedef struct lookup_node {
   uint16_t dictionary_position;
   void *next;
} lookup_node_t;


void LZW_INIT();

int LZW_Compress(uint16_t input_length, uint8_t* input, uint16_t max_output_length, uint8_t* output);

int LZW_Decompress(uint16_t input_length, uint8_t* input, uint8_t*output);

int LZW_Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len);

int LZW_Flex_Decompress(void* compressed, int compressed_len, void* decompressed);

#endif /* SRC_LZW_H_ */
