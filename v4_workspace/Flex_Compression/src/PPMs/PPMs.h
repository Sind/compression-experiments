/****************************************************************************
 *  This file is part of PPMs project                                       *
 *  Written and distributed to public domain by Dmitry Shkarin 2004, 2006   *
 *  Contents: interface to encoding/decoding routines                       *
 *  Comments: this file can be used as an interface to PPMs module          *
 *  (consisting of Model.cpp) from external program                         *
 ****************************************************************************/
#if !defined(_PPMS_H_)
#define _PPMS_H_

#ifdef  __cplusplus
extern "C" {
#endif

int PPMs_Compress(unsigned char* encoded, unsigned char* decoded, int len, int order);
int PPMs_Uncompress(unsigned char* decoded, unsigned char* encoded, int len, int order);


inline int PPMs5_Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len){
	return PPMs_Compress((unsigned char*)compressed, (unsigned char*)raw, raw_len, 5);
}
inline int PPMs5_Flex_Decompress(void* compressed, int compressed_len, void* decompressed){
	return PPMs_Uncompress((unsigned char*)decompressed, (unsigned char*)compressed, compressed_len, 5);
}
inline int PPMs3_Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len){
	return PPMs_Compress((unsigned char*)compressed, (unsigned char*)raw, raw_len, 3);
}
inline int PPMs3_Flex_Decompress(void* compressed, int compressed_len, void* decompressed){
	return PPMs_Uncompress((unsigned char*)decompressed, (unsigned char*)compressed, compressed_len, 3);
}
inline int PPMs1_Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len){
	return PPMs_Compress((unsigned char*)compressed, (unsigned char*)raw, raw_len, 1);
}
inline int PPMs1_Flex_Decompress(void* compressed, int compressed_len, void* decompressed){
	return PPMs_Uncompress((unsigned char*)decompressed, (unsigned char*)compressed, compressed_len, 1);
}

#ifdef  __cplusplus
}
#endif

#endif /* !defined(_PPMS_H_) */
