/*
 * fpaq0.hpp
 *
 *  Created on: Feb 16, 2017
 *      Author: srodk
 */

#ifndef SRC_FPAQ0_HPP_
#define SRC_FPAQ0_HPP_

#include <cstdio>

class CHAR_STREAM{
public:
	CHAR_STREAM(unsigned char* str, int len): Len(len), Pos(0), p(str){}
	int get() {return (Pos < Len)? p[Pos++]: EOF;}
	int put(unsigned char c){Len++; return (p[Pos++] = c);}
	int Len, Pos;
private:
	unsigned char* p;
};

#define getc(pps)   ((pps)->get())
#define putc(c,pps) ((pps)->put(c))


void  EncodeFile(CHAR_STREAM* EncodedFile,CHAR_STREAM* DecodedFile);
void  DecodeFile(CHAR_STREAM* DecodedFile,CHAR_STREAM* EncodedFile);


#endif /* SRC_FPAQ0_HPP_ */
