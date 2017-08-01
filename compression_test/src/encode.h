#ifndef ENCODE
#define ENCODE

int encode_uint8_t(void* input_orig, uint16_t input_length, int8_t* output)
{
	uint8_t* r_output = (uint8_t*) output;
	uint8_t* input = (uint8_t*) input_orig;
	r_output[0] = input[0];
	for(int i = 1; i < input_length; i++){
		r_output[i] = input[i] - input[i-1];
	}
	return input_length;
}
/* transform data back from easier to compress format */
int decode_uint8_t(int8_t* input, int input_length, void* output_orig)
{
	uint8_t* r_input = (uint8_t*) input;
	uint8_t* output = (uint8_t*) output_orig;
	output[0] = r_input[0];
	for(int i = 1; i < input_length; i++){
		output[i] = output[i-1] + r_input[i];
	}
	return input_length;
}
int encode_uint16_t(void * input_orig, uint16_t input_bytes, int8_t* output)
{
	uint16_t input_length = input_bytes / sizeof(uint16_t);
	uint16_t *input = input_orig;
	int input_index = 0;
	int output_index = 0;
	int unknown = 1;
	int current = 0;
	while(input_index < input_length){
		if(unknown == 1){
			unknown = 0;
			current = input[input_index];
			*((uint16_t*)&output[output_index]) = (uint16_t)current;
			output_index += sizeof(uint16_t);
			input_index++;
		}else {
			int next = input[input_index];
			if(next-current < -127 || next-current > 127){
				//if the next number can't be represented using +-127
				output[output_index] = -128;
				output_index++;
				unknown = 1;
			} else {
				int8_t v = (int8_t) (next - current);
				current = next;
				output[output_index] = v;
				output_index++;
				input_index++;
			}
		}
	}
	return output_index;
}


int decode_uint16_t(int8_t* input, int input_length, void* output_orig)
{
	uint16_t* output = (uint16_t*) output_orig;
	uint16_t current = *((uint16_t*)input);
	output[0] = current;
	int output_index = 1;
	int input_index = sizeof(uint16_t);
	
	while(input_index < input_length){
		int8_t change = input[input_index];
		input_index++;
		if(change == -128){
			//if the char is -128(reserved) the following two int8s are parsed as one int16_t
			current = *((uint16_t*)&input[input_index]);
			input_index += sizeof(uint16_t);
		} else {
			current = current + change;
		}
		output[output_index] = current;
		output_index++;
	}
	return output_index * sizeof(uint16_t);
}

#endif