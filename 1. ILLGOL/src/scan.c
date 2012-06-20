/*
  scan.c for illgol.c
*/

FILE * scanfile;
char   token[256];

void scan_trace(char * s)
{
  /* fprintf(stderr, "%s: %s\n", s, token); */
}

void scan(void)
{
  char x;
  int i = 0;
RESTART_SCAN:    /* a regrettable label */
  if (feof(scanfile)) { token[0] = 0; return; }
  x = (char)getc(scanfile);
  if (feof(scanfile)) { token[0] = 0; return; }
  while (x <= ' ')
  {
    x = (char)getc(scanfile);
    if (feof(scanfile)) { token[0] = 0; return; }
  }

  if ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z'))
  {
    while ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') ||
	   (x >= '0' && x <= '9') || (x == '\''))
    {
      token[i++] = x;
      x = (char)getc(scanfile);
      if (feof(scanfile)) { token[0] = 0; return; }
    }
    ungetc(x, scanfile);
    token[i] = 0;
    if(!strcmp(token, "NB"))
    {
      x = (char)getc(scanfile);
      if (feof(scanfile)) { token[0] = 0; return; }
      while (x != '\n')
      {
	x = (char)getc(scanfile);
	if (feof(scanfile)) { token[0] = 0; return; }
      }
      i = 0;
      goto RESTART_SCAN;
    }
    scan_trace("Alphanumeric");
    return;
  }
  if (x >= '0' && x <= '9')
  {
    while (x >= '0' && x <= '9')
    {
      token[i++] = x;
      x = (char)getc(scanfile);
      if (feof(scanfile)) { token[0] = 0; return; }
    }
    ungetc(x, scanfile);
    token[i] = 0;
    scan_trace("Digit");
    return;
  }
  if (x == '"')
  {
    token[i++] = x;
    x = (char)getc(scanfile);
    if (feof(scanfile)) { token[0] = 0; return; }
    while (x != '"')
    {
      token[i++] = x;
      x = (char)getc(scanfile);
      if (feof(scanfile)) { token[0] = 0; return; }
    }
    token[i] = 0;
    scan_trace("String");
    return;
  }
  if (x == '+' || x == '-' || x == '>' || x == '<')
  {
    while (x == '+' || x == '-' || x == '>' || x == '<' || x == '=')
    {
      token[i++] = x;
      x = (char)getc(scanfile);
      if (feof(scanfile)) { token[0] = 0; return; }
    }
    ungetc(x, scanfile);
    token[i] = 0;
    scan_trace("Compound Symbol");
    return;
  }
  if (x == '(' || x == ')' || x == '=' || x == ';' || x == '*' || x == '/'
	       || x == '|' || x == '&' || x == '^' || x == '.' || x == '!'
	       || x == '{' || x == '}' || x == ',' || x == '[' || x == ']'
	       || x == '~' || x == '?' || x == '#')
  {
    token[0] = x;
    token[1] = 0;
    scan_trace("Discrete Symbol");
    return;
  }
  token[0] = 0;
}
