
#include "fpaq0-c.h"
#include "fpaq0.hpp"



//////////////////////////// main ////////////////////////////

int fpaq0_Compress(unsigned char* encoded, unsigned char* decoded, int len){
	CHAR_STREAM* in = new CHAR_STREAM(decoded, len);
	CHAR_STREAM* out = new CHAR_STREAM(encoded, 0);
	EncodeFile(out,in);
	return out->Len;
}
int fpaq0_Uncompress(unsigned char* decoded, unsigned char* encoded, int len){
	CHAR_STREAM* in = new CHAR_STREAM(encoded, len);
	CHAR_STREAM* out = new CHAR_STREAM(decoded, 0);
	 DecodeFile(out,in);
	return out->Len;
}
