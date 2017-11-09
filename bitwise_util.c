/*
  
  Welcome to the bitwise wonderland, says the white rabbit :)

 */

#include <stdio.h>
#include <stdlib.h>

#include "rdtsc.h"

#define MAX_REP_ITER 100000

/*
  Definition of RDTSC & measurement method

  RDTSC : ReaD TimeStamp Counter

  The rdtsc instruction returns the value in the cycles counter after the last CPU reset.
  The returned values will be used to evaluate the performance of functions.
  The returned values are considered stable only if the measured target object exhibits 
  a CPU life span of at least 1000 cycles. Given that none of the functions present in this code
  last for that long, they'll be put inside a repetition loop to expand their life span and the 
  probes will be inserted around the loop. The obtained results will be divided by the number of 
  iterations of the repetition loop : MAX_REP_ITER
  
 */

//Measurement macro
#define MEASURE(s, f, v, probe)						\
  {									\
    									\
    int ret;								\
    double a, b;							\
    									\
    b = probe();							\
    									\
    for (unsigned i = 0; i < MAX_REP_ITER; i++)				\
      ret = f(v);							\
									\
    a = probe();							\
    									\
    printf("%s(0x%08x)\tret:\t%d\tcycles:\t%lf\n", s, v, ret, (a - b) / MAX_REP_ITER); \
                                                                                 \
}						                                 \

//popcount on a 32 bits long value using log2(32)=5 steps
unsigned pop1(unsigned v)
{
  v = (v & 0x55555555) + ((v >>  1) & 0x55555555); 
  v = (v & 0x33333333) + ((v >>  2) & 0x33333333); 
  v = (v & 0x0F0F0F0F) + ((v >>  4) & 0x0F0F0F0F); 
  v = (v & 0x00FF00FF) + ((v >>  8) & 0x00FF00FF); 
  v = (v & 0x0000FFFF) + ((v >> 16) & 0x0000FFFF);
  
  return v;
}

//Another variant of the popcount
unsigned pop2(unsigned v)
{
  v = v - ((v >> 1) & 0x55555555);
  v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
  v = (v + (v >> 4)) & 0x0F0F0F0F;
  v += (v >> 8);
  v += (v >> 16);
  
  return v & 0x0000003F;
}

//For the Lulz: n++ = -(~n), also n-- = ~(-n)
unsigned pop3(unsigned v)
{
  unsigned n = 0;
  
  while (v)
    v &= (v - 1), n = -(~n);
  
  return n;
}

//For the Lulz: n++ = -(~n), also n-- = ~(-n)
unsigned pop3_plus(unsigned v)
{
  unsigned n = 0;
  
  while (v)
    v &= (v - 1), n++;
  
  return n;
}

//Another variant, again.
unsigned pop4(unsigned v)
{
  unsigned n;
  
  n = (v >> 1) & 0x77777777;
  v = v - n;
  n = (n >> 1) & 0x77777777;
  v = v - n;
  n = (n >> 1) & 0x77777777;
  v = v - n;
  
  v = (v + (v >> 4)) & 0x0F0F0F0F;
  v *= 0x01010101;
  
  return v >> 24;
}

//Assembly version with the popcnt instruction.
//Works only on Intel processors.
unsigned pop5(unsigned v)
{
  unsigned n;
  
  __asm__(".intel_syntax;"

	  "popcnt %1,%0;"
	  
	  ".att_syntax;"
	  : "=r" (n) //output
	  : "r" (v)  //input
	  :);        //clobber

  return n;
}

//Find the position of the left-most-bit.
//Returns -1 if not found.
int lmb1(unsigned v)
{
  int pos = -1;

  while (v)
    v >>= 1, pos++;
  
  return pos;
}

//Returns 32 if not found.
int lmb2(unsigned v)
{
  unsigned pos = 32, ret = 1;
  const unsigned M = 0x80000000;
  
  while (v && (ret = ((v & M) != M)))
      v <<= 1, pos--;
  
  return pos - !ret; //This replaces an if (recycling) 
}

//Dichotomy using mask & shift tables. 
int lmb3(unsigned v)
{
  unsigned pos = 0;
  const unsigned M[] = { 0xFFFF0000, 0xFF00, 0xF0, 0b1100, 0b10 };
  const unsigned S[] = { 16        ,      8,    4,      2,    1 }; 

  for (unsigned i = 0; v && i <= 4; i++)
    if (v & M[i])
      {
	v >>= S[i];
	pos += S[i]; 
      }
  
  return pos - !v ; //if v is 0 return error code -1
}

//Dichotomy using masks only.
int lmb4(unsigned v)
{
  unsigned pos = 0, scale = 16;
  const unsigned M[] = { 0xFFFF0000, 0xFF00, 0xF0, 0b1100, 0b10 };
  
  for (unsigned i = 0; v && i <= 4; i++)
    if (v & M[i])
      {
	v >>= (scale >> i);
	pos += (scale >> i); 
      }
  
  return pos - !v ; //if v is 0 return error code -1
}

//Dichotomy using masks and a scale to compute shift values.
int lmb5(unsigned v)
{
  unsigned flipflop;
  unsigned pos = 0, scale = 16;
  const unsigned M[] = { 0xFFFF0000, 0xFF00, 0xF0, 0b1100, 0b10 };
  
  for (unsigned i = 0; v && i <= 4; i++)
    {
      flipflop = ((v & M[i]) != 0) | 0xFFFFFFFF;
             
      v  >>= ((scale >> i) & flipflop);
      pos += ((scale >> i) & flipflop); 
    }
  
  return pos - !v ; //if v is 0 return error code -1
}

//
int main(int arc, char **argv)
{
  //val1 : 5 bits up,
  unsigned val1 = ~0, val2 = 0x80000000;//0x0F0F0110;
  
  printf("POPCOUNT:\n");
  MEASURE(" pop1", pop1, val1, rdtsc);
  MEASURE(" pop2", pop2, val1, rdtsc);
  MEASURE(" pop3", pop3, val1, rdtsc);
  MEASURE(" pop3+", pop3_plus, val1, rdtsc);
  MEASURE(" pop4", pop4, val1, rdtsc);
  MEASURE(" pop5", pop5, val1, rdtsc);
  
  printf("\nLEFT-MOST-BIT:\n");
  MEASURE(" lmb1", lmb1, val2, rdtsc);
  MEASURE(" lmb2", lmb2, val2, rdtsc);
  MEASURE(" lmb3", lmb3, val2, rdtsc);
  MEASURE(" lmb4", lmb4, val2, rdtsc);
  MEASURE(" lmb5", lmb5, val2, rdtsc);
  
  return 0;
}
