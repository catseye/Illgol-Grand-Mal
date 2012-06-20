/*
  illgol.c - an ILLGOL to 8086 compiler
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned int  word;
typedef signed   char sbyte;
typedef signed   int  sword;

#include "static.c"
#include "8086.c"
#include "symbol.c"
#include "runtime.c"
#include "scan.c"
#include "parse.c"

int main(int argc, char ** argv)
{
  int c = 0;
  FILE * outf;
  FILE * inf;
  char * ofn;

  printf("ILLGOL to 8086 compiler (c)1999 Cat's-Eye Technologies.\n");

  if (argc < 3)
  {
    ofn = "out.com";
  } else
  {
    ofn = argv[2];
  }
  inf = fopen(argv[1], "r");
  if (inf == NULL)
  {
    char s[128];
    sprintf(s, "prj\\%s.ill", argv[1]);
    inf = fopen(s, "r");
    ofn = (char *)malloc(128);
    sprintf(ofn, "%s.com", argv[1]);
  }
  if (inf != NULL)
  {
    outf = fopen(ofn, "wb");
    if (outf != NULL)
    {
      scanfile = inf;
      m = s;
      dp = d;
      sbuffer = stalloc(16);   /* string buffer for str, stf, etc */
      scan();
      if(illgol())
      {
	/* write jump */
	char q[16]; int soc = 0; int soh = 0;
	c = (m - s);
	q[0] = 0xe9;
	soc = (dp - d) + 3;
	soh = soc + 259 + c;
	q[1] = soc & 0xff;          /* low word of start of code */
	q[2] = (soc >> 8) & 0xff;   /* high word of start of code */
	q[3] = 144;                 /* nop to line things up again */
	q[4] = soh & 0xff;          /* low word of start of heap */
	q[5] = (soh >> 8) & 0xff;   /* high word of start of heap */
	fwrite(q, 6, 1, outf);
	/* write data */
	/* fprintf(stderr, "Data length: %d\n", (dp - d)); */
	fwrite(d, (dp - d), 1, outf);
	/* write c bytes to f */
	fwrite(s, c, 1, outf);
	printf("Success, %d bytes written to '%s'\n", (c+(dp-d)+6), ofn);
      } else
      {
	fprintf(stderr, "Failure: could not compile '%s'\n", argv[1]);
      }
      fclose(outf);
      fclose(inf);
      return 0;
    }
    fclose(inf);
    fprintf(stderr, "Failure: could not open output file '%s'\n", ofn);
    return 1;
  }
  fprintf(stderr, "Failure: could not open input file '%s'\n", argv[1]);
  return 1;
}
