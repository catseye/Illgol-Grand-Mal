/*
  static.c for illgol.c
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
