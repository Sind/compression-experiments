// fpaq0 - Stationary order 0 file compressor.
// (C) 2004, Matt Mahoney under GPL, http://www.gnu.org/licenses/gpl.txt
// To compile: g++ -O fpaq0.cpp

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "fpaq0.hpp"
namespace std {}  // for MARS compiler
using namespace std;

typedef unsigned long U32;  // 32 bit type



//////////////////////////// Predictor /////////////////////////

/* A Predictor estimates the probability that the next bit of
   uncompressed data is 1.  Methods:
   p() returns P(1) as a 12 bit number (0-4095).
   update(y) trains the predictor with the actual bit (0 or 1).
*/

class Predictor {
  int cxt;  // Context: last 0-8 bits with a leading 1
  int ct[512][2];  // 0 and 1 counts in context cxt
public:
  Predictor(): cxt(1) {
    memset(ct, 0, sizeof(ct));
  }

  // Assume a stationary order 0 stream of 9-bit symbols
  int p() const {
    return 4096*(ct[cxt][1]+1)/(ct[cxt][0]+ct[cxt][1]+2);
  }

  void update(int y) {
    if (++ct[cxt][y] > 65534) {
      ct[cxt][0] >>= 1;
      ct[cxt][1] >>= 1;
    }
    if ((cxt+=cxt+y) >= 512)
      cxt=1;
  }
};

//////////////////////////// Encoder ////////////////////////////

/* An Encoder does arithmetic encoding.  Methods:
   Encoder(COMPRESS, f) creates encoder for compression to archive f, which
     must be open past any header for writing in binary mode
   Encoder(DECOMPRESS, f) creates encoder for decompression from archive f,
     which must be open past any header for reading in binary mode
   encode(bit) in COMPRESS mode compresses bit to file f.
   decode() in DECOMPRESS mode returns the next decompressed bit from file f.
   flush() should be called when there is no more to compress.
*/

typedef enum {COMPRESS, DECOMPRESS} Mode;
class Encoder {
private:
  Predictor predictor;
  const Mode mode;       // Compress or decompress?
  CHAR_STREAM* archive;         // Compressed data file
  U32 x1, x2;            // Range, initially [0, 1), scaled by 2^32
  U32 x;                 // Last 4 input bytes of archive.
public:
  Encoder(Mode m, CHAR_STREAM* f);
  void encode(int y);    // Compress bit y
  int decode();          // Uncompress and return bit y
  void flush();          // Call when done compressing
};

// Constructor
Encoder::Encoder(Mode m, CHAR_STREAM* f): predictor(), mode(m), archive(f), x1(0),
                                   x2(0xffffffff), x(0) {

  // In DECOMPRESS mode, initialize x to the first 4 bytes of the archive
  if (mode==DECOMPRESS) {
    for (int i=0; i<4; ++i) {
      int c=getc(archive);
      if (c==EOF) c=0;
      x=(x<<8)+(c&0xff);
    }
  }
}

/* encode(y) -- Encode bit y by splitting the range [x1, x2] in proportion
to P(1) and P(0) as given by the predictor and narrowing to the appropriate
subrange.  Output leading bytes of the range as they become known. */

inline void Encoder::encode(int y) {

  // Update the range
  const U32 xmid = x1 + ((x2-x1) >> 12) * predictor.p();
  assert(xmid >= x1 && xmid < x2);
  if (y)
    x2=xmid;
  else
    x1=xmid+1;
  predictor.update(y);

  // Shift equal MSB's out
  while (((x1^x2)&0xff000000)==0) {
    putc(x2>>24, archive);
    x1<<=8;
    x2=(x2<<8)+255;
  }
}

/* Decode one bit from the archive, splitting [x1, x2] as in the encoder
and returning 1 or 0 depending on which subrange the archive point x is in.
*/
inline int Encoder::decode() {

  // Update the range
  const U32 xmid = x1 + ((x2-x1) >> 12) * predictor.p();
  assert(xmid >= x1 && xmid < x2);
  int y=0;
  if (x<=xmid) {
    y=1;
    x2=xmid;
  }
  else
    x1=xmid+1;
  predictor.update(y);

  // Shift equal MSB's out
  while (((x1^x2)&0xff000000)==0) {
    x1<<=8;
    x2=(x2<<8)+255;
    int c=getc(archive);
    if (c==EOF) c=0;
    x=(x<<8)+c;
  }
  return y;
}

// Should be called when there is no more to compress
void Encoder::flush() {

  // In COMPRESS mode, write out the remaining bytes of x, x1 < x < x2
  if (mode==COMPRESS) {
    while (((x1^x2)&0xff000000)==0) {
      putc(x2>>24, archive);
      x1<<=8;
      x2=(x2<<8)+255;
    }
    putc(x2>>24, archive);  // First unequal byte
  }
}

void  EncodeFile(CHAR_STREAM* EncodedFile,CHAR_STREAM* DecodedFile){
	int c;
	// Compress
	Encoder e(COMPRESS, EncodedFile);
	while ((c=getc(DecodedFile))!=EOF) {
	  e.encode(0);
	  for (int i=7; i>=0; --i)
		e.encode((c>>i)&1);
	}
	e.encode(1);  // EOF code
	e.flush();
}
void  DecodeFile(CHAR_STREAM* DecodedFile,CHAR_STREAM* EncodedFile){
	// Decompress
	Encoder e(DECOMPRESS, EncodedFile);
	while (!e.decode()) {
	  int c=1;
	  while (c<256)
		c+=c+e.decode();
	  putc(c-256, DecodedFile);
	}
}
