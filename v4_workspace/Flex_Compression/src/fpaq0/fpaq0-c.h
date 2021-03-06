/****************************************************************************
 *  This file is part of PPMs project                                       *
 *  Written and distributed to public domain by Dmitry Shkarin 2004, 2006   *
 *  Contents: interface to encoding/decoding routines                       *
 *  Comments: this file can be used as an interface to PPMs module          *
 *  (consisting of Model.cpp) from external program                         *
 ****************************************************************************/
#if !defined(_FPAQ0_H_)
#define _FPAQ0_H_


#ifdef  __cplusplus
extern "C" {
#endif

int fpaq0_Compress(unsigned char* encoded, unsigned char* decoded, int len);
int fpaq0_Uncompress(unsigned char* decoded, unsigned char* encoded, int len);

inline int Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len){
	return fpaq0_Compress((unsigned char*)compressed, (unsigned char*)raw, raw_len);
}

inline int Flex_Decompress(void* compressed, int compressed_len, void* decompressed){
	return fpaq0_Uncompress((unsigned char*)decompressed, (unsigned char*)compressed, compressed_len);
}
#ifdef  __cplusplus
}
#endif

#endif /* !defined(_PPMD_H_) */
