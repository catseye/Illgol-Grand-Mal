/*
  symbol.c for illgol##.c
  THIS FILE MAY OR MAY NOT BE PART OF THE OPEN SORES ILLGOL## TO 8086 COMPILER  
  (c)2001 Cat's Eye Technologies.  
  All rights reserved.  All liability disclaimed.    
  See the file COPING for license information.    
*/

typedef struct Symbol
{
  char * id;
  byte * addr;
  byte * hook_addr;
  struct Symbol * next;
  int constant;
  int value;
  int type;
} symbol;

symbol * head = NULL;
symbol * shouldsym = NULL;

symbol * whatever = NULL;

byte * sym_addr(char * s, int * c, int * v, symbol ** whatever)
{
  symbol * h = head;
  symbol * n;
  while(h != NULL)
  {
    if(!strcmp(s, h->id))
    {
      *c = h->constant;
      *v = h->value;
      *whatever = h;
      return h->addr;
    }
    h = h->next;
  }
  n = (symbol *)malloc(sizeof(symbol));
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
      strcpy(n->id, s);
      n->addr = stalloc(2);
      n->hook_addr = NULL;
      n->constant = *c;
      n->value = *v;
      n->next = head;
      n->type = 0;
      head = n;
      *whatever = n;
      return n->addr;
    }
  }
  return NULL;
}

int sym_exists(char * s)
{
  symbol * h = head;
  while(h != NULL)
  {
    if(!strcmp(s, h->id))
    {
      return 1;
    }
    h = h->next;
  }
  return 0;
}

/* END of symbol.c */

