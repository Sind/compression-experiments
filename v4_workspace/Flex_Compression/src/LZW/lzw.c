
#include "lzw.h"
#include "em_device.h"
#include "em_chip.h"
#include <stdint.h>
#include <string.h>



uint16_t dictionary_total;
uint8_t dictionary[DICTIONARY_TOTAL_LENGTH];
uint16_t dictionary_position;
uint8_t dictionary_sizes[DICTIONARY_LENGTH]; // a dictionary entry can't be greater than 256 chars
uint16_t dictionary_indexes[DICTIONARY_LENGTH];

lookup_node_t lookup_nodes[DICTIONARY_LENGTH];
lookup_node_t *lookups[256];

/* Prototypes */
void LZW_initialize_table();
int LZW_find_longest_prefix(char* str, uint16_t max_length);
void LZW_extend_dictionary_c(char* str, int len);
void LZW_extend_dictionary_d(int previous_index, int current_index);

void LZW_write_next_bits(uint8_t *dest, uint16_t *offset, uint8_t *current_shift, uint8_t num_bits, uint16_t value){
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
uint16_t LZW_read_next_bits(uint8_t *source, uint16_t *offset, uint8_t *current_shift, uint8_t num_bits){
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

void LZW_INIT(){
	int i;
	for(i = 0; i < 256; i++){
		dictionary[i] = (char) i;
		dictionary_indexes[i] = i;
		dictionary_sizes[i] = 1;
		lookup_nodes[i].dictionary_position = i;
	}
}

void LZW_initialize_table(){
	dictionary_position = 256;
	dictionary_total = 256;
	int i;
	for(i = 0; i < 256; i++){
		lookups[i] = &(lookup_nodes[i]);
	}
}

int LZW_find_longest_prefix(char* str, uint16_t max_length){
	int i;
	unsigned char c = *str;
	lookup_node_t *cl = lookups[c];
	while(true){
		uint16_t ci = cl->dictionary_position;
		uint16_t cs = dictionary_sizes[ci];
		if(cs > max_length){
			cl = cl->next;
			continue;
		}
		int cmp = memcmp(str, dictionary+dictionary_indexes[ci],cs);
		if(cmp == 0){
			return ci;
		}
		cl = cl->next;
	}
	return -1;
//	for(i = dictionary_position-1; i >= 0; i--){
//		if(dictionary_sizes[i] > max_length){
//			continue;
//		}
//		int cmp = memcmp(str, dictionary+dictionary_indexes[i],dictionary_sizes[i]);
//		if(cmp == 0){
//			return i;
//		}
//	}
//	return -1;
}

void LZW_extend_dictionary_c(char* str, int len){
	if(dictionary_position == DICTIONARY_LENGTH) return;
	memcpy(dictionary+dictionary_total,str,len);
	dictionary_indexes[dictionary_position] = dictionary_total;
	dictionary_sizes[dictionary_position] = len;
	lookup_nodes[dictionary_position].dictionary_position = dictionary_position;
	unsigned char c = (unsigned char) *str;
	lookup_nodes[dictionary_position].next = lookups[c];
	lookups[c] = &(lookup_nodes[dictionary_position]);
	dictionary_position++;
	dictionary_total += len;
}


void LZW_extend_dictionary_d(int previous_index, int current_index){
	if(dictionary_position == DICTIONARY_LENGTH) return;
	memcpy(dictionary+dictionary_total,dictionary+dictionary_indexes[previous_index],dictionary_sizes[previous_index]);
	dictionary[dictionary_total+dictionary_sizes[previous_index]] = dictionary[dictionary_indexes[current_index]];
	dictionary_indexes[dictionary_position] = dictionary_total;
	dictionary_sizes[dictionary_position] = dictionary_sizes[previous_index]+1;
	unsigned char c = (unsigned char) *(dictionary+dictionary_indexes[previous_index]);
	lookup_nodes[dictionary_position].dictionary_position = dictionary_position;
	lookup_nodes[dictionary_position].next = lookups[c];
	lookups[c] = &(lookup_nodes[dictionary_position]);
	dictionary_position++;
	dictionary_total += dictionary_sizes[previous_index]+1;
}


int LZW_Compress(uint16_t input_length, uint8_t* input, uint16_t max_output_length, uint8_t* output){
	LZW_initialize_table();
	memset (output, 0, max_output_length);
	uint16_t output_length = 1;
	uint8_t current_shift = 0;
	while(input_length > 0 && output_length < max_output_length)
	{
		uint16_t i = LZW_find_longest_prefix(input, input_length);

		LZW_write_next_bits(output, &output_length, &current_shift,NUM_BITS_DICTIONARY,i);

		LZW_extend_dictionary_c(input, dictionary_sizes[i]+1);
		input += dictionary_sizes[i];//move input forwards
		input_length -= dictionary_sizes[i];
	}
	if(current_shift != 0)
	{
		output_length++;
	}
	*output = current_shift;
	return output_length;
}

int LZW_Decompress(uint16_t input_length, uint8_t* input, uint8_t* output){
	LZW_initialize_table();
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
		current_index = LZW_read_next_bits(input,&input_index,&current_shift,NUM_BITS_DICTIONARY);

		if(previous_index != -1){
			if(current_index == dictionary_position){
				LZW_extend_dictionary_d(previous_index,previous_index);
			} else {
				LZW_extend_dictionary_d(previous_index,current_index);
			}
		}

		//print to output
		memcpy(output,dictionary+dictionary_indexes[current_index],dictionary_sizes[current_index]);
		//move forwards
		output += dictionary_sizes[current_index];
		output_length += dictionary_sizes[current_index];
		previous_index = current_index;
	}

	return output_length;
}
