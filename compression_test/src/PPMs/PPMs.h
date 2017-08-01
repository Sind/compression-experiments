/****************************************************************************
 *  This file is part of PPMs project                                       *
 *  Written and distributed to public domain by Dmitry Shkarin 2004, 2006   *
 *  Contents: interface to encoding/decoding routines                       *
 *  Comments: this file can be used as an interface to PPMs module          *
 *  (consisting of Model.cpp) from external program                         *
 ****************************************************************************/
#if !defined(_PPMS_H_)
#define _PPMS_H_

#define PPM_ORDER 1


#ifdef  __cplusplus
extern "C" {
#endif

int PPMs_Compress(unsigned char* encoded, unsigned char* decoded, int len, int order);
int PPMs_Uncompress(unsigned char* decoded, unsigned char* encoded, int len, int order);

int PPMs5_Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len);

int PPMs5_Flex_Decompress(void* compressed, int compressed_len, void* decompressed);
int PPMs3_Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len);

int PPMs3_Flex_Decompress(void* compressed, int compressed_len, void* decompressed);
int PPMs1_Flex_Compress(void* raw, int raw_len, void* compressed, int compressed_max_len);

int PPMs1_Flex_Decompress(void* compressed, int compressed_len, void* decompressed);
#ifdef  __cplusplus
}
#endif

#endif /* !defined(_PPMS_H_) */
