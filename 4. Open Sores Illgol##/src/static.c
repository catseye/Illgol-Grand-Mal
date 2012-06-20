/*
  static.c for illgol##.c
  THIS FILE MAY OR MAY NOT BE PART OF THE OPEN SORES ILLGOL## TO 8086 COMPILER  
  (c)2001 Cat's Eye Technologies.  
  All rights reserved.  All liability disclaimed.    
  See the file COPING for license information.    
*/

byte d[16384];    /* dataspace */
byte *dp;

byte * stalloc(int l)
{
  byte * f = dp;
  memset(dp, 0, l);
  dp += l;
  return f;
}

word caddr(byte * b)  /* return eventual machine address of static data */
{
  return (word)((b - d) + 262);
}

void stretract(void) /* only on multiple successive string literals */
{
  dp--;
}

/* END of static.c */

