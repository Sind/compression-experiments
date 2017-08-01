/*
 * Model.hpp
 *
 *  Created on: Feb 12, 2017
 *      Author: srodk
 */

#ifndef SRC_MODEL_HPP_
#define SRC_MODEL_HPP_


#include "PPMsType.hpp"


void _STDCALL EncodeFile(_PPMD_FILE* EncodedFile,_PPMD_FILE* DecodedFile,int MaxOrder);
void _STDCALL DecodeFile(_PPMD_FILE* DecodedFile,_PPMD_FILE* EncodedFile,int MaxOrder);

#endif /* SRC_MODEL_HPP_ */
