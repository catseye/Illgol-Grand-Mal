/*
  symbol.c for illgol.c
*/

typedef struct Symbol
{
  char * id;
  byte * addr;
  struct Symbol * next;
  int constant;
  int value;
} symbol;

symbol * head = NULL;

byte * sym_addr(char * s, int * c, int * v)
{
  symbol * h = head;
  symbol * n;
  /* fprintf(stderr, "fetching %s\n", s); */
  while(h != NULL)
  {
    if(!strcmp(s, h->id))
    {
      *c = h->constant;
      *v = h->value;
      return h->addr;
    }
    h = h->next;
  }
  /* fprintf(stderr, "Creating %s\n", s); */
  n = (symbol *)malloc(sizeof(symbol));
  /* fprintf(stderr, "Malloc'ed n\n"); */
  if (n == NULL)
  {
    fprintf(stderr, "Could not allocate symbol table record\n");
  } else
  {
    n->id = (char *)malloc(strlen(s)+1);
    if (n->id == NULL)
    {
      fprintf(stderr, "Could not allocate lexeme\n");
    } else
    {
      /* fprintf(stderr, "Copying %s to n->id\n", s); */
      strcpy(n->id, s);
      n->addr = stalloc(2);
      n->constant = *c;
      n->value = *v;
      n->next = head;
      head = n;
      return n->addr;
    }
  }
  return NULL;
}

int sym_exists(char * s)
{
  symbol * h = head;
  /* fprintf(stderr, "Looking for %s\n", s); */
  while(h != NULL)
  {
    /* fprintf(stderr, "..checking against %s\n", h->id); */
    if(!strcmp(s, h->id))
    {
      /* fprintf(stderr, "A match\n"); */
      return 1;
    }
    h = h->next;
  }
  /* fprintf(stderr, "*NO* match\n"); */
  return 0;
}
