
#include "lzw_mc.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>



uint16_t lzw_mc_dictionary_total;
uint8_t lzw_mc_dictionary[DICTIONARY_TOTAL_LENGTH];
uint16_t lzw_mc_dictionary_position;
uint8_t lzw_mc_dictionary_sizes[DICTIONARY_LENGTH]; // a dictionary entry can't be greater than 256 chars
uint16_t lzw_mc_dictionary_indexes[DICTIONARY_LENGTH];

////The cache acts as a fifo queue
uint16_t cache[CACHE_LENGTH];
uint16_t cache_end;
uint16_t cache_taken;

lzw_mc_lookup_node_t lzw_mc_lookup_nodes[DICTIONARY_LENGTH];
lzw_mc_lookup_node_t *lzw_mc_lookups[256];

/* Prototypes */
void LZW_MC_initialize_table();
int LZW_MC_find_longest_prefix(char* str, uint16_t max_length);
void LZW_MC_extend_dictionary_c(char* str, int len);
void LZW_MC_extend_dictionary_d(int previous_index, int current_index);


void LZW_MC_write_next_bits(uint8_t *dest, uint16_t *offset, uint8_t *current_shift, uint8_t num_bits, uint16_t value){
	dest = dest + *offset;
	while(num_bits > 0){
		uint8_t available_bits = 8-*current_shift;
		uint8_t bits_to_write = available_bits;
		if (num_bits < available_bits){
			bits_to_write = num_bits;
		}
		uint8_t mask = 0xFF >> (8-bits_to_write);
		uint8_t values_to_write = (value & mask) << *current_shift;
		*dest = *dest | values_to_write;
		uint8_t new_shift = (*current_shift + bits_to_write) % 8;
		if(*current_shift >= new_shift){
			*offset +=1;
			dest += 1;
		}
		*current_shift = new_shift;
		value >>= bits_to_write;
		num_bits -= bits_to_write;
	}
}
uint16_t LZW_MC_read_next_bits(uint8_t *source, uint16_t *offset, uint8_t *current_shift, uint8_t num_bits){
	source = source + *offset;
	uint16_t value = 0;
	uint8_t value_shift = 0;
	while(num_bits > 0){
		uint8_t available_bits = 8 - *current_shift;
		uint8_t bits_to_read = available_bits;
		if (num_bits < available_bits){
			bits_to_read = num_bits;
		}
		uint8_t mask = (0xFF >> (8-bits_to_read)) << *current_shift;
		uint8_t values_to_read = (*source & mask) >> *current_shift;
		value = value | ((uint16_t)values_to_read << value_shift);
		value_shift += bits_to_read;
		uint8_t new_shift = (*current_shift + bits_to_read) % 8;
		if(*current_shift >= new_shift){
			*offset +=1;
			source += 1;
		}
		*current_shift = new_shift;
		num_bits -= bits_to_read;
	}
	return value;
}

void LZW_MC_INIT(){
	int i;
	for(i = 0; i < 256; i++){
		lzw_mc_dictionary[i] = (char) i;
		lzw_mc_dictionary_indexes[i] = i;
		lzw_mc_dictionary_sizes[i] = 1;
		lzw_mc_lookup_nodes[i].lzw_mc_dictionary_position = i;
	}
}

void LZW_MC_initialize_table(){
	lzw_mc_dictionary_position = 256;
	lzw_mc_dictionary_total = 256;
	int i;
	for(i = 0; i < 256; i++){
		lzw_mc_lookups[i] = &(lzw_mc_lookup_nodes[i]);
	}
	cache_end = 0;
	cache_taken = 0;
}

int LZW_MC_find_longest_prefix(char* str, uint16_t max_length){
	int i;
	unsigned char c = *str;
	lzw_mc_lookup_node_t *cl = lzw_mc_lookups[c];
	while(true){
		uint16_t ci = cl->lzw_mc_dictionary_position;
		uint16_t cs = lzw_mc_dictionary_sizes[ci];
		if(cs > max_length){
			cl = cl->next;
			continue;
		}
		int cmp = memcmp(str, lzw_mc_dictionary+lzw_mc_dictionary_indexes[ci],cs);
		if(cmp == 0){
			return ci;
		}
		cl = cl->next;
	}
	return -1;
}

/* brings value at index to the front of the ringbuffer */
void LZW_MC_bring_to_front(int index){
	int i;
	int tmp = cache[index];
	if(cache_taken != CACHE_LENGTH){
		for(i = index; i < cache_end-1; i++){
			cache[i] = cache[i+1];
		}
		cache[cache_end-1] = tmp;
		return;
	}
	if(index == cache_end){
		cache_end = (cache_end + 1)%CACHE_LENGTH;
		return;
	}
	if(index < cache_end){
		for(i = index; i < cache_end-1; i++){
			cache[i] = cache[i+1];
		}
		cache[cache_end-1] = tmp;
		return;
	}
	for(i = index; i < CACHE_LENGTH-1; i++){
		cache[i] = cache[i+1];
	}
	if(cache_end == 0){
		cache[CACHE_LENGTH-1] = tmp;
		return;
	}
	cache[CACHE_LENGTH-1] = cache[0];
	for(i = 0; i < cache_end-1; i++){
		cache[i] = cache[i+1];
	}
	cache[cache_end-1] = tmp;
}


// -1 if not found
int LZW_MC_find_in_cache(int value){
	int index;
	for(index = 0; index < cache_taken; index++){
		if(index == cache_end){
			continue;
		}
		if(cache[index] == value){
			return index;
		}
	}
	return -1;
}


//adds a value to the buffer
void LZW_MC_extend_cache(int value){
	//check if it isn't already in the buffer
	int index = LZW_MC_find_in_cache(value);
	if (index != -1){
		LZW_MC_bring_to_front(index);
		return;
	}

	/* if cache isn't filled yet, just add to end of cache */
	if(cache_taken != CACHE_LENGTH){
		cache[cache_end] = value;
		cache_end = (cache_end + 1)%CACHE_LENGTH;
		cache_taken++;
		return;
	}

	/* if cache is filled, kick out oldest, and add newest */
	cache[cache_end] = value;
	cache_end = (cache_end + 1)%CACHE_LENGTH;
}


void LZW_MC_extend_dictionary_c(char* str, int len){
	if(lzw_mc_dictionary_position == DICTIONARY_LENGTH) return;
	memcpy(lzw_mc_dictionary+lzw_mc_dictionary_total,str,len);
	lzw_mc_dictionary_indexes[lzw_mc_dictionary_position] = lzw_mc_dictionary_total;
	lzw_mc_dictionary_sizes[lzw_mc_dictionary_position] = len;
	lzw_mc_lookup_nodes[lzw_mc_dictionary_position].lzw_mc_dictionary_position = lzw_mc_dictionary_position;
	unsigned char c = (unsigned char) *str;
	lzw_mc_lookup_nodes[lzw_mc_dictionary_position].next = lzw_mc_lookups[c];
	lzw_mc_lookups[c] = &(lzw_mc_lookup_nodes[lzw_mc_dictionary_position]);
	LZW_MC_extend_cache(lzw_mc_dictionary_position);
	lzw_mc_dictionary_position++;
	lzw_mc_dictionary_total += len;
}


void LZW_MC_extend_dictionary_d(int previous_index, int current_index){
	if(lzw_mc_dictionary_position == DICTIONARY_LENGTH) return;
	memcpy(lzw_mc_dictionary+lzw_mc_dictionary_total,lzw_mc_dictionary+lzw_mc_dictionary_indexes[previous_index],lzw_mc_dictionary_sizes[previous_index]);
	lzw_mc_dictionary[lzw_mc_dictionary_total+lzw_mc_dictionary_sizes[previous_index]] = lzw_mc_dictionary[lzw_mc_dictionary_indexes[current_index]];
	lzw_mc_dictionary_indexes[lzw_mc_dictionary_position] = lzw_mc_dictionary_total;
	lzw_mc_dictionary_sizes[lzw_mc_dictionary_position] = lzw_mc_dictionary_sizes[previous_index]+1;
	LZW_MC_extend_cache(lzw_mc_dictionary_position);
	unsigned char c = (unsigned char) *(lzw_mc_dictionary+lzw_mc_dictionary_indexes[previous_index]);
	lzw_mc_lookup_nodes[lzw_mc_dictionary_position].lzw_mc_dictionary_position = lzw_mc_dictionary_position;
	lzw_mc_lookup_nodes[lzw_mc_dictionary_position].next = lzw_mc_lookups[c];
	lzw_mc_lookups[c] = &(lzw_mc_lookup_nodes[lzw_mc_dictionary_position]);
	lzw_mc_dictionary_position++;
	lzw_mc_dictionary_total += lzw_mc_dictionary_sizes[previous_index]+1;
}


int LZW_MC_Compress(uint16_t input_length, uint8_t* input, uint16_t max_output_length, uint8_t* output){
	LZW_MC_initialize_table();
	memset (output, 0, max_output_length);
	uint16_t output_length = 1;
	uint8_t current_shift = 0;
	while(input_length > 0 && output_length < max_output_length)
	{
		uint16_t i = LZW_MC_find_longest_prefix(input, input_length);
		int k = LZW_MC_find_in_cache(i);
		uint16_t e, num_bits;

		if(k != -1){
			num_bits = NUM_BITS_CACHE;
			e = k;
			LZW_MC_write_next_bits(output, &output_length, &current_shift,1,1);
		}else{
			num_bits = NUM_BITS_DICTIONARY;
			e = i;
			LZW_MC_write_next_bits(output, &output_length, &current_shift,1,0);
		}
		LZW_MC_write_next_bits(output, &output_length, &current_shift,num_bits,e);

		LZW_MC_extend_cache(i);
		LZW_MC_extend_dictionary_c(input, lzw_mc_dictionary_sizes[i]+1);
		input += lzw_mc_dictionary_sizes[i];//move input forwards
		input_length -= lzw_mc_dictionary_sizes[i];
	}
	if(current_shift != 0)
	{
		output_length++;
	}
	*output = current_shift;
	return output_length;
}

int LZW_MC_Decompress(uint16_t input_length, uint8_t* input, uint8_t* output){
	LZW_MC_initialize_table();
	uint8_t last_shift = *input;
	uint16_t input_index = 1;
	uint16_t output_length = 0;
	int previous_index = -1;
	uint16_t current_index;
	uint8_t current_shift = 0;
	while(input_index < input_length)
	{
		if (input_index == input_length-1){
			if(current_shift == last_shift){
				break;
			}
		}
//		read from table
		uint8_t cache_hit = LZW_MC_read_next_bits(input,&input_index,&current_shift,1);
		if (cache_hit == 1){
			uint8_t cache_index = LZW_MC_read_next_bits(input,&input_index,&current_shift,NUM_BITS_CACHE);
			if(cache_index == cache_end){
				current_index = lzw_mc_dictionary_position;
			}else{
				current_index = cache[cache_index];
			}

		}else{
			current_index = LZW_MC_read_next_bits(input,&input_index,&current_shift,NUM_BITS_DICTIONARY);
		}

		//add to dictionary
		if(previous_index != -1){
			if(current_index == lzw_mc_dictionary_position){
				LZW_MC_extend_dictionary_d(previous_index,previous_index);
			} else {
				LZW_MC_extend_dictionary_d(previous_index,current_index);
			}
		}
		LZW_MC_extend_cache(current_index);
		//print to output
		memcpy(output,lzw_mc_dictionary+lzw_mc_dictionary_indexes[current_index],lzw_mc_dictionary_sizes[current_index]);
		//move forwards
		output += lzw_mc_dictionary_sizes[current_index];
		output_length += lzw_mc_dictionary_sizes[current_index];
		previous_index = current_index;
	}

	return output_length;
}

int LZW_MC_Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len){
	return LZW_MC_Compress(raw_len, raw, compressed_max_len,compressed);
}

int LZW_MC_Flex_Decompress(void* compressed, int compressed_len, void* decompressed){
	return LZW_MC_Decompress(compressed_len, compressed, decompressed);
}
