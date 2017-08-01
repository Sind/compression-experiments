/*
 * lzw.h
 *
 *  Created on: Oct 18, 2016
 *      Author: srodk
 */

#ifndef SRC_LZW_MC_H_
#define SRC_LZW_MC_H_

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
#define NUM_BITS_CACHE 5

#define DICTIONARY_LENGTH (1 << NUM_BITS_DICTIONARY)
#define CACHE_LENGTH (1 << NUM_BITS_CACHE)

#define DICTIONARY_TOTAL_LENGTH (DICTIONARY_LENGTH * 4)

//these need to be redone
#define DICTIONARY_SHIFT 16 - NUM_BITS_DICTIONARY
#define CACHE_SHIFT 16 - NUM_BITS_CACHE

#define DICTIONARY_MASK 0xFFFF >> (DICTIONARY_SHIFT)
#define CACHE_MASK 0xFFFF >> (CACHE_SHIFT)

typedef struct lzw_mc_lookup_node {
   uint16_t lzw_mc_dictionary_position;
   void *next;
} lzw_mc_lookup_node_t;


void LZW_MC_INIT();

int LZW_MC_Compress(uint16_t input_length, uint8_t* input, uint16_t max_output_length, uint8_t* output);

int LZW_MC_Decompress(uint16_t input_length, uint8_t* input, uint8_t*output);

inline int Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len){
	return LZW_MC_Compress(raw_len, raw, compressed_max_len,compressed);
}

inline int Flex_Decompress(void* compressed, int compressed_len, void* decompressed){
	return LZW_MC_Decompress(compressed_len, compressed, decompressed);
}
#endif /* SRC_LZW_MC_H_ */
