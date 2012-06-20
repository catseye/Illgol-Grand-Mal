/*
  illgol##.c
  THIS PROGRAM MAY OR MAY NOT BE THE OPEN SORES ILLGOL## TO 8086 COMPILER
  (c)2001 Cat's Eye Technologies.
  All rights reserved.  All liability disclaimed.
  See the file COPING for license information.
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
  char cmd[128];

  printf("Open Sores ILLGOL##\n"
         "(c)2001 Cat's Eye Technologies. "
         "All rights reserved. All liability disclaimed.\n"
         "See the file COPING for license information.");

  if (argc < 3)
  {
    ofn = "out.com";
  } else
  {
    ofn = argv[2];
  }
  if(argv[1]==NULL)
  {
    argv[1] = argv[0];
  }
  sprintf(cmd, "perl i3pp.pl prj\\%s.ill >tmp.ill", argv[1]);
  system(cmd);
  inf = fopen("tmp.ill", "r");
  if (inf != NULL)
  {
    outf = fopen(ofn, "wb");
    if (outf != NULL)
    {
      scanfile = inf;
      m = s;
      dp = d;
      sbuffer = stalloc(16);   /* string buffer for str, stf, etc */
      eofflag = stalloc(2);
      pencolor= stalloc(2);
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
	fwrite(d, (dp - d), 1, outf);
	fwrite(s, c, 1, outf);
	printf("Success, %d bytes written to '%s'\n", (c+(dp-d)+6), ofn);
      } else
      {
	fprintf(stderr, "Failure: could not compile '%s'\n", argv[1]);
      }
      fclose(outf);
      fclose(inf);
      unlink("tmp.ill");
      return 0;
    }
    fclose(inf);
    unlink("tmp.ill");
    fprintf(stderr, "Failure: could not open output file '%s'\n", ofn);
    return 1;
  }
  fprintf(stderr, "Failure: could not open input file '%s'\n", argv[1]);
  return 1;
}

/* END of illgol##.c */

