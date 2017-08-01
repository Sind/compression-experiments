/*
 * PPMd.cpp
 *
 *  Created on: Feb 12, 2017
 *      Author: srodk
 */


#include "PPMs.h"
#include "PPMsType.hpp"
#include "Model.hpp"


int PPMs_Compress(unsigned char* encoded, unsigned char* decoded, int len, int order){
	CHAR_STREAM* in = new CHAR_STREAM(decoded, len);
	CHAR_STREAM* out = new CHAR_STREAM(encoded, 0);
	EncodeFile(out, in, order);
	return out->Len;
}
int PPMs_Uncompress(unsigned char* decoded, unsigned char* encoded, int len, int order){
	CHAR_STREAM* in = new CHAR_STREAM(encoded, len);
	CHAR_STREAM* out = new CHAR_STREAM(decoded, 0);

	DecodeFile(out, in, order);
	return out->Len;
}
