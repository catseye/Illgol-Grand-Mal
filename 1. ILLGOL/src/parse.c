/*
  parse.c for illgol.c
*/

#define tokeq(x)  (!strcmp(token,x))
#define tokne(x)  (strcmp(token,x))

int lino = 0;
byte * seedaddr = NULL;
byte * sbuffer;

void error(char * s)
{
  fprintf(stderr, "Error (token '%s', line %d): %s.\n", token, lino, s);
}

int member(void)
{
  if (token[0] >= '0' && token[0] <= '9')
  {
    int v = atoi(token);
    scan();
    return v;
  }
  if (sym_exists(token))
  {
    int c = 0;
    int v = 0;
    sym_addr(token, &c, &v);
    scan();
    if (c) return v;
  }
  error("Unrecognized member");
  return 0;
}

void assignment(void);
void assignments(void);
void block(void);
void boolexpr(void);
void addexpr(void);

void dereference(void)    /* assume address has already been pushed */
{
  while(tokeq(".") || tokeq("[") || tokeq("^"))
  {
    if(tokeq("."))
    {
      scan();
      g_pop_bx();
      g_add_bx(member());
      g_push_bx();
    }
    else if(tokeq("["))
    {
      scan();
      addexpr();
      if(tokeq("]"))
      {
	scan();
	g_pop_ax();
	g_pop_bx();
	g_add_bx_ax();
	g_push_bx();
      } else
      {
	error("Missing ]");
      }
    }
    else if(tokeq("^"))
    {
      scan();
      g_pop_bx();
      g_mov_ax_BX();
      g_push_ax();
    }
  }
}

int name(int dcl)
{
  if(sym_exists(token) || dcl)
  {
    int c = 0;
    int v = 0;
    int l = 0;
    l = caddr(sym_addr(token, &c, &v));
    scan();
    g_mov_bx(l);
    g_push_bx();
    dereference();
    return l;
  } else
  {
    error("Expected identifier");
    return 0;
  }
}

void primitive(void)
{
  int ptr = 0; /* pointer-to-variable such as in: x = ^y; */

  if(tokeq("call"))
  {
    byte * b = m; int l; int c; int v; int a = 0; int i;
    scan();
    boolexpr();
    if(tokeq("("))
    {
      scan();
      a++;
      boolexpr();
      g_pop_ax();
      g_pop_bx();
      g_push_ax();
      g_push_bx();
      while (tokeq(","))
      {
	scan();
	a++;
	boolexpr();
	g_pop_ax();
	g_pop_bx();
	g_push_ax();
	g_push_bx();
      }
      if(tokeq(")"))
      {
	scan();
      } else
      {
	error("Missing )");
      }
    }
    g_pop_bx();
    g_push_bp();
    g_mov_ax_sp();
    g_add_ax(a << 1);
    g_mov_bp_ax();
    g_call_bx();
    g_pop_dx();   /* hang on to returnval */
    g_pop_bp();
    for(i=0;i<a;i++)
    {
      g_pop_ax();  /* clean up stack arguments */
    }
    g_push_dx();
    return;
  }

  if(tokeq("sin"))
  {
    runtime_trig();
    scan();
    boolexpr();
    g_pop_ax();
    g_mov_bx(caddr(sin_routine));
    g_call_bx();
    g_push_cx();
    return;
  }

  if(tokeq("cos"))
  {
    runtime_trig();
    scan();
    boolexpr();
    g_pop_ax();
    g_mov_bx(caddr(cos_routine));
    g_call_bx();
    g_push_cx();
    return;
  }

  if(tokeq("abs"))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_and_ax(0x7fff);
    g_push_ax();
    return;
  }

  if(tokeq("asc"))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_and_ax(0x007f);
    g_push_ax();
    return;
  }

  if(tokeq("fix"))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_mov_ah_al();
    g_xor_al_al();
    g_push_ax();
    return;
  }

  if(tokeq("int"))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_mov_al_ah();
    g_xor_ah_ah();
    g_push_ax();
    return;
  }

  if(tokeq("fre"))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_mov_ax_I(caddr(d)-2);
    g_neg_ax();
    g_push_ax();
    return;
  }

  if(tokeq("rnd"))
  {
    scan();
    if(tokeq("("))
    {
      scan();
      name(0);
      if(tokeq(")"))
      {
	scan();
      } else error("Missing )");
    } else
    {
      error("Need identifier in rnd()");
    }
    g_pop_bx();
    g_mov_ax_BX();
    g_mov_dx(58653U);
    g_imul_dx();
    g_mov_dx(13849U);
    g_add_ax_dx();
    g_mov_BX_ax();
    g_push_ax();
    return;
  }

  if(tokeq("stu"))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_mov_bx(caddr(sbuffer + 14));
    g_label(); g_xor_dx_dx();
    g_mov_cx(10);
    g_idiv_cx();
    g_add_dx('0');
    g_mov_BX_dl();
    g_dec_bx();
    g_or_ax_ax();
    g_jnz(-18);
    g_inc_bx();
    g_push_bx();
    return;
  }

  if(tokeq("str"))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_push_ax();
    g_cmp_ax(0);
    g_jg(+2);
    g_neg_ax();
    g_label(); g_mov_bx(caddr(sbuffer + 14));
    g_label(); g_xor_dx_dx();
    g_mov_cx(10);
    g_idiv_cx();
    g_add_dx('0');
    g_mov_BX_dl();
    g_dec_bx();
    g_or_ax_ax();
    g_jnz(-18);
    g_pop_ax();
    g_cmp_ax(0);
    g_jge(+5);
    g_mov_al('-');
    g_mov_BX_al();
    g_dec_bx();
    g_label(); g_inc_bx();
    g_push_bx();
    return;
  }

  if(tokeq("sif"))
  {
    scan();
    boolexpr();

    g_pop_ax();
    g_push_ax();
    g_xor_ah_ah();
    g_mov_dx(39);
    g_imul_dx();
    /* g_mov_al_ah();
    g_mov_ah_dl(); */

    g_mov_bx(caddr(sbuffer + 14));
    g_mov_cx(10);
    g_label(); g_xor_dx_dx();
    g_idiv_cx();
    g_add_dx('0');
    g_mov_BX_dl();
    g_dec_bx();
    g_cmp_bx(caddr(sbuffer + 10));
    g_jnz(-17);

    g_mov_dx('.');
    g_mov_BX_dl();
    g_dec_bx();

    g_pop_ax();
    g_mov_al_ah();
    g_xor_ah_ah();

    g_label(); g_xor_dx_dx();
    g_idiv_cx();
    g_add_dx('0');
    g_mov_BX_dl();
    g_dec_bx();
    g_or_ax_ax();
    g_jnz(-15);
    g_inc_bx();

    g_push_bx();

    return;
  }

  if(tokeq("^"))
  {
    scan(); ptr = 1;
  }
  if(tokeq("#"))
  {
    scan();
    if (tokeq("("))
    {
      scan();
      boolexpr();
      if (tokeq(")"))
      {
	scan();
	g_pop_ax();
	g_shl_ax_1();
	g_neg_ax();
	g_xchg_si_ax();
	g_mov_ax_BP_SI();
	g_push_ax();
	return;
      }
      error("Missing )");
      return;
    }
    g_mov_ax_BP(-2 * member());
    g_push_ax();
    return;
  }
  if(sym_exists(token))
  {
    name(0);
    if (!ptr)
    {
      g_pop_bx();
      g_push_BX();
      if (tokeq("++"))
      {
	scan();
	g_mov_ax_BX();
	g_inc_ax();
	g_mov_BX_ax();
      }
      if (tokeq("--"))
      {
	scan();
	g_mov_ax_BX();
	g_dec_ax();
	g_mov_BX_ax();
      }
    }
    return;
  }
  if (ptr)
  {
    error("Identifier should follow ^");
  }
  if(token[0] >= '0' && token[0] <= '9')
  {
    int v = atoi(token);
    g_mov_ax(v);
    g_push_ax();
    scan();
    if (tokeq("."))
    {
      scan();
      while(strlen(token) < 4)
      {
	strcat(token, "0");
      }
      v = atoi(token) / 39;
      scan();
      if (v > 255) v = 255;
      g_pop_ax();
      g_mov_ah_al();
      g_mov_al(v & 0xff);
      g_push_ax();
    }
    return;
  }
  if(token[0] == '"')
  {
    char * q = (token+1);
    byte * b = stalloc(strlen(q)+1);
    strcpy((char *)b, q);
    g_mov_ax(caddr(b));
    g_push_ax();
    scan();
    while(token[0] == '"')
    {
      char * q = (token+1);
      byte * b;
      stretract();
      b = stalloc(strlen(q)+1);
      strcpy((char *)b, q);
      scan();
    }
    return;
  }
  if(tokeq("{"))
  {
    byte * l; byte * q; int c=0; int v=0; int lc;
    scan();
    l = m;    /* this is our lambda function */
    g_label();
    g_nop();
    assignments();
    if (tokeq("}"))
    {
      scan();
      g_ret();
      /* move it to the data area... */
      q = stalloc(m - l);
      memcpy(q, l, m - l);
      m = l;
      g_mov_ax(caddr(q));
      g_push_ax();
      return;
    }
    error("Missing }");
    return;
  }
  if(tokeq("("))
  {
    scan();
    boolexpr();
    if (tokeq(")"))
    {
      scan();
      return;
    }
    error("Missing )");
    return;
  }
  if(tokeq("*"))
  {
    scan();
    boolexpr();
    /* gen code to allocate that many bytes */
    g_mov_bx(260);
    g_mov_ax_BX();
    g_pop_dx();
    g_push_ax();
    g_add_ax_dx();
    g_mov_BX_ax();
    return;
  }
  error("Unrecognized primitive");
}

void factor(void)
{
  primitive();
  while(tokeq(">>") || tokeq("<<"))
  {
    char x = token[0];
    scan();
    primitive();
    if (x == '>')
    {
      g_pop_cx();
      g_pop_ax();
      g_shr_ax_cl();
      g_push_ax();
    }
    if (x == '<')
    {
      g_pop_cx();
      g_pop_ax();
      g_shr_ax_cl();
      g_push_ax();
    }
  }
}

void term(void)
{
  int fixed = 0;
  factor();
  while(tokeq("*") || tokeq("/"))
  {
    char x = token[0];
    scan();
    if (tokeq("."))
    {
      fixed = 1;
      scan();
    }
    factor();
    if(x == '*')
    {
      g_pop_ax();
      g_pop_dx();
      g_imul_dx();
      if (fixed)
      {
	g_mov_al_ah();
	g_mov_ah_dl();
      }
      g_push_ax();
    }
    if(x == '/')
    {
      g_pop_ax();
      g_pop_cx();
      g_xor_dx_dx();
      if (fixed)
      {
	g_mov_dl_ah();
	g_mov_ah_al();
	g_xor_al_al();
      }
      g_idiv_cx();
      g_push_ax();
    }
  }
}

void addexpr(void)
{
  term();
  while(tokeq("+") || tokeq("-"))
  {
    char x = token[0];
    scan();
    term();
    if(x == '+')
    {
      g_pop_ax();
      g_pop_dx();
      g_add_ax_dx();
      g_push_ax();
    }
    if(x == '-')
    {
      g_pop_dx();
      g_pop_ax();
      g_sub_ax_dx();
      g_push_ax();
    }
  }
}

void compexpr(void)
{
  char x, y;
  addexpr();
  while(tokeq("=") || tokeq("<>") || tokeq(">") ||
	tokeq(">=") || tokeq("<") || tokeq("<="))
  {
    x = token[0]; y = token[1];
    scan();
    addexpr();
    if(x == '=')
    {
      g_pop_ax();
      g_pop_dx();
      g_sub_ax_dx();
      g_jz(+3);
      g_xor_ax_ax();
      g_dec_ax();
      g_label(); g_inc_ax();
      g_push_ax();
    }
    if(x == '<' && y == '>')
    {
      g_pop_ax();
      g_pop_dx();
      g_xor_cx_cx();
      g_sub_ax_dx();
      g_jz(+1);
      g_inc_cx();
      g_label(); g_push_cx();
    }
    if(x == '<' && y == 0)
    {
      g_pop_dx();
      g_pop_ax();
      g_xor_cx_cx();
      g_cmp_ax_dx();
      g_jge(+1);
      g_inc_cx();
      g_label(); g_push_cx();
    }
    if(x == '>' && y == 0)
    {
      g_pop_dx();
      g_pop_ax();
      g_xor_cx_cx();
      g_cmp_ax_dx();
      g_jle(+1);
      g_inc_cx();
      g_label(); g_push_cx();
    }
    if(x == '<' && y == '=')
    {
      g_pop_dx();
      g_pop_ax();
      g_xor_cx_cx();
      g_cmp_ax_dx();
      g_jg(+1);
      g_inc_cx();
      g_label(); g_push_cx();
    }
    if(x == '>' && y == '=')
    {
      g_pop_dx();
      g_pop_ax();
      g_xor_cx_cx();
      g_cmp_ax_dx();
      g_jl(+1);
      g_inc_cx();
      g_label(); g_push_cx();
    }
  }
}

void negexpr(void)
{
  if(tokeq("!"))
  {
    scan(); /* generate boolean not */
    compexpr();
    g_pop_ax();
    g_or_ax_ax();
    g_jz(+3);
    g_xor_ax_ax();
    g_dec_ax();
    g_inc_ax();
    g_push_ax();
  } else
  if(tokeq("?"))
  {
    scan(); /* generate boolean buffer */
    compexpr();
    g_pop_ax();
    g_xor_bx_bx();
    g_or_ax_ax();
    g_jz(+1);
    g_inc_bx();
    g_push_bx();
  } else
  {
    compexpr();
  }
}

void boolexpr(void)
{
  negexpr();
  while(tokeq("&") || tokeq("|") || tokeq("~"))
  {
    char x = token[0];
    scan();
    negexpr();
    if(x == '&')
    {
      g_pop_ax();
      g_pop_dx();
      g_and_ax_dx();
      g_push_dx();
    }
    if(x == '|')
    {
      g_pop_ax();
      g_pop_dx();
      g_or_ax_dx();
      g_push_dx();
    }
    if(x == '~')
    {
      g_pop_ax();
      g_pop_dx();
      g_xor_ax_dx();
      g_push_dx();
    }
  }
}

void fullmoonfever(void)
{
  int ok = 1;
  while(ok)
  {
    /* error("FMF"); */
    if(tokeq("GO"))
    {
      scan();
      boolexpr();
      boolexpr();
      g_pop_ax();
      g_pop_dx();
      g_mov_dh_al();
      g_mov_ah(2);
      g_xor_bx_bx();
      g_int(0x10);
    }
    else if(tokeq("PRINT"))
    {
      scan();
      boolexpr();
      g_pop_si();
      g_label(); g_mov_al_SI();
      g_or_al_al();
      g_jz(10);
      g_mov_ah(14);
      g_mov_bl(15);
      g_int(16);
      g_inc_si();
      g_jmp(-16);
      g_label();
    }
    else if(tokeq("PAUSE"))
    {
      scan();
      boolexpr();
      g_pop_cx();
      g_xor_dx_dx();
      g_clc();
      g_rcr_cx_1();
      g_rcr_dx_1();
      g_mov_ah(0x86);
      g_int(0x15);
    }
    else if(tokeq("CLREOL"))
    {
      scan(); /* gen code to clear to end of line */
      g_mov_bh(0);
      g_mov_ah(0x03);
      g_int(0x10);
      g_mov_cx_dx();
      g_xor_cl_cl();
      g_mov_ax(0x0700);
      g_mov_bh(0x07);
      g_mov_dl(0x4f);
      g_int(0x10);
    }
    else if(tokeq("CLREOD"))
    {
      scan(); /* gen code to clear to end of display */
      g_mov_bh(0);
      g_mov_ah(0x03);
      g_int(0x10);
      g_mov_cx_dx();
      g_xor_cl_cl();
      g_mov_ax(0x0700);
      g_mov_bh(0x07);
      g_mov_dx(0x184f);
      g_int(0x10);
    }
    else if(tokeq("CLRSCR"))
    {
      scan();	/* gen code to clear screen */
      g_mov_ax(0x0700);
      g_mov_bh(0x07);
      g_xor_cx_cx();
      g_mov_dx(0x184f);
      g_int(0x10);
    }
    else if(tokeq("LF"))
    {
      scan(); /* gen code to move cursor left one column */
      g_mov_bh(0);
      g_mov_ah(0x03);
      g_int(0x10);
      g_dec_dl();
      g_mov_ah(0x02);
      g_int(0x10);
    }
    else if(tokeq("RT"))
    {
      scan(); /* gen code to move cursor right one column */
      g_mov_bh(0);
      g_mov_ah(0x03);
      g_int(0x10);
      g_inc_dl();
      g_mov_ah(0x02);
      g_int(0x10);
    }
    else if(tokeq("UP"))
    {
      scan(); /* gen code to move cursor up one row */
      g_mov_bh(0);
      g_mov_ah(0x03);
      g_int(0x10);
      g_dec_dh();
      g_mov_ah(0x02);
      g_int(0x10);
    }
    else if(tokeq("DN"))
    {
      scan(); /* gen code to move cursor right one column */
      g_mov_bh(0);
      g_mov_ah(0x03);
      g_int(0x10);
      g_inc_dh();
      g_mov_ah(0x02);
      g_int(0x10);
    }
    else if(tokeq("INSLIN"))
    {
      scan(); /* gen code to insert one row */
      g_mov_bh(0);
      g_mov_ah(0x03);
      g_int(0x10);
      g_mov_cx_dx();
      g_mov_ax(0x0701);
      g_mov_bh(0x07);
      g_mov_dx(0x184f);
      g_int(0x10);
    }
    else if(tokeq("DELLIN"))
    {
      scan(); /* gen code to delete one row */
      g_mov_bh(0);
      g_mov_ah(0x03);
      g_int(0x10);
      g_inc_dh();
      g_mov_cx_dx();
      g_mov_ax(0x0601);
      g_mov_bh(0x07);
      g_mov_dx(0x184f);
      g_int(0x10);
    }
    else if(tokeq("INS"))
    {
      scan(); /* gen code to insert one char */
      error("FMF INS instruction not supported");
    }
    else if(tokeq("DEL"))
    {
      scan();
      error("FMF DEL instruction not supported");
      /* gen code to delete one char */
    }
    else if(tokeq("DO"))
    {
      byte * backward = NULL;

      scan();
      boolexpr();

      g_label();
      backward = m; /* this is where we jump back to */

      fullmoonfever();
      if (tokeq(";"))
      {
	scan();
	g_pop_ax();
	g_dec_ax();
	g_push_ax();
	g_or_ax_ax();
	g_jz(+3);
	g_jmp(-3 - (m - backward));
      } else
      {
	error("(FMF) DO block should end with ;");
      }
    }
    else ok = 0;
  }
}

void assignment(void)
{
  int bc = 0;
  int cc = 0;
  int cv = 0;

  if(tokeq(";") || tokeq("}") || tokeq("FIN"))
  {
    return;
  }

  if(token[0] >= '0' && token[0] <= '9')
  {
    lino = atoi(token);  /* line number */
    scan();
  }

  if(tokeq("for"))
  {
    byte * p;
    int l;
    g_label();
    scan();
    l = name(0);          /* todo: name(0,0) to generate no 'push' */
    g_pop_bx();           /* a little wasteful, but what the hell */
    if (tokeq("="))
    {
      scan();
      boolexpr();
      g_pop_ax();
      g_mov_bx(l);
      g_mov_BX_ax();
      if (tokeq("to"))
      {
	scan();
	boolexpr();             /* leaves value on stack until... */
	p = m;
	g_label();
	block();

	g_pop_ax();         /* here, then we */
	g_push_ax();        /* recycle it until... */
	g_mov_bx_I(l);
	g_inc_bx();
	g_mov_I_bx(l);
	g_cmp_bx_ax();
	g_jg(+3);
	g_jmp(-3 - (m - p));
	g_pop_ax();         /* we finally trash it */
	return;
      } else
      if (tokeq("link"))
      {
	int b = 0;
	scan();
	b = member();
	p = m;
	g_label();
	block();

	g_mov_bx_I(l);
	g_add_bx(b);
	g_mov_ax_BX();
	g_mov_I_ax(l);
	g_or_ax_ax();
	g_jz(+3);
	g_jmp(-3 - (m - p));

	return;
      } else
      {
	error("Expected 'to' or 'link'");
      }
    }
    error("Screwed-up 'for' syntax");
    return;
  }

  if(tokeq("while"))
  {
    byte * p = m;    /* this is where we loop back to */
    byte * backward = NULL;
    byte * t = NULL;
    g_label();
    scan();
    boolexpr();
    g_pop_ax();
    g_or_ax_ax();
    g_jnz(+3);
    backward = m; /* this is where we backpatch to */
    g_jmp(0);
    g_label();
    block();

    g_jmp(-3 - (m - p));
    t = m;
    m = backward;
    g_jmp((t - m) - 3);  /* here we make the actual backpatch */
    m = t;
    g_label();

    return;
  }

  if(tokeq("if"))
  {
    byte * backward = NULL; byte * b2 = NULL;
    byte * t = NULL;
    scan();
    boolexpr();
    g_pop_ax();
    g_or_ax_ax();
    g_jnz(+3);
    backward = m; /* this is where we backpatch to */
    g_jmp(0);
    g_label();
    block();

    if(tokeq("else"))
    {
      b2 = m;
      g_jmp(0);  /* a second backpatch */
      t = m;
      m = backward;
      g_jmp((t - backward) - 3);  /* here we make the first backpatch */
      m = t;
      g_label();
      scan();
      block();

      t = m;
      m = b2;
      g_jmp((t - b2) - 3);  /* here we make the 2nd backpatch */
      m = t;
      g_label();

    } else /* no 'else' clause in source */
    {
      t = m;
      m = backward;
      g_jmp((t - backward) - 3);  /* here we make the first backpatch */
      m = t;
      g_label();
    }
    return;
  }

  if(tokeq("out"))
  {
    scan();
    if(tokeq("tty"))
    {
      scan();
    }
    boolexpr();
    g_pop_ax();
    g_mov_ah(14);
    g_mov_bl(15);
    g_int(16);
    return;
  }

  if(tokeq("in"))
  {
    scan();
    if(tokeq("tty"))
    {
      scan();
    }
    name(0);
    g_pop_bx();
    g_xor_ah_ah();
    g_int(22);
    g_mov_BX_al();
    return;
  }

  if(tokeq("print"))
  {
    do
    {
      scan();
      if (tokeq("EoL"))
      {
	scan();
	g_mov_ah(14);
	g_mov_bl(15);
	g_mov_al(13);
	g_int(16);
	g_mov_al(10);
	g_int(16);
      } else
      {
	boolexpr();
	g_pop_si();
	g_label(); g_mov_al_SI();
	g_or_al_al();
	g_jz(10);
	g_mov_ah(14);
	g_mov_bl(15);
	g_int(16);
	g_inc_si();
	g_jmp(-16);
	g_label();
      }
    } while (!tokne(","));
    return;
  }

  if(tokeq("FMF"))    /* oh my... Full Moon Fever compatibility mode! */
  {
    scan();
    if(tokeq("{"))
    {
      scan();
      fullmoonfever();
      if(tokeq("}"))
      {
	scan();
      } else
      {
	error("Missing }");
      }
    } else
    {
      error("Missing {");
    }
    return;
  }
  if(tokeq("border"))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_mov_ah_al();
    g_xchg_bx_ax();
    g_mov_ax(0x1001);
    g_int(0x10);
    return;
  }

  if(tokeq("hgr"))
  {
    runtime_gfx();
    scan();
    /*boolexpr();
    g_pop_ax();*/
    g_mov_ax(caddr(hgr_routine));
    g_call_ax();
    return;
  }

  if(tokeq("hplot"))
  {
    runtime_gfx();
    scan();
    boolexpr();
    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();
    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();

    g_pop_cx();
    g_pop_ax();
    g_pop_bx();

    g_mov_dx(caddr(hplot_routine));
    g_call_dx();
    return;
  }

  if(tokeq("hget"))
  {
    runtime_gfx();
    scan();
    boolexpr();
    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();
    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();

    g_pop_ax();
    g_pop_bx();
    g_mov_dx(caddr(hindex_routine));
    g_call_dx();
    g_push_di();

    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();
    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();

    g_pop_ax();
    g_pop_bx();
    g_pop_di();
    g_pop_cx();  /* pointer to destination */
    g_mov_dx(caddr(hget_routine));
    g_call_dx();

    return;
  }

  if(tokeq("hput"))
  {
    runtime_gfx();
    scan();
    boolexpr();
    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();
    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();

    g_pop_ax();
    g_pop_bx();
    g_mov_dx(caddr(hindex_routine));
    g_call_dx();
    g_push_di();

    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();
    if (tokne(",")) { error("Missing ,"); } else { scan(); }
    boolexpr();

    g_pop_bx();
    g_pop_ax();
    g_pop_di();
    g_pop_cx();  /* pointer to source */
    g_mov_dx(caddr(hput_routine));
    g_call_dx();

    return;
  }

  if(tokeq("text"))
  {
    scan();
    runtime_gfx();
    /*boolexpr();
    g_pop_ax();*/
    g_mov_ax(caddr(text_routine));
    g_call_ax();
    return;
  }

  if(tokeq("don't"))
  {
    scan();
    boolexpr();
    g_pop_bx();
    g_mov_al(0xc3); /* ret */
    g_mov_BX_al();
    return;
  }

  if(tokeq("reinstate"))
  {
    scan();
    boolexpr();
    g_pop_bx();
    g_mov_al(144); /* nop */
    g_mov_BX_al();
    return;
  }

  /* todo someday... "detour" */

  if(tokeq("do"))
  {
    byte * b = m; int l; int c; int v; int a = 0; int i;
    g_label();
    scan();
    boolexpr();
    if(tokeq("("))
    {
      scan();
      a++;
      boolexpr();
      g_pop_ax();
      g_pop_bx();
      g_push_ax();
      g_push_bx();
      while (tokeq(","))
      {
	scan();
	a++;
	boolexpr();
	g_pop_ax();
	g_pop_bx();
	g_push_ax();
	g_push_bx();
      }
      if(tokeq(")"))
      {
	scan();
      } else
      {
	error("Missing )");
      }
    }

    g_pop_bx();
    g_push_bp();
    g_mov_ax_sp();
    g_add_ax(a << 1);
    g_mov_bp_ax();
    g_call_bx();

    if(tokeq("DISCARD"))
    {
      scan();
      g_pop_ax();   /* discard returnval */
    }
    g_pop_bp();
    for(i=0;i<a;i++)
    {
      g_pop_ax();  /* clean up stack arguments */
    }
    if(tokeq("until"))
    {
      scan();
      boolexpr();
      g_pop_ax();
      g_or_ax_ax();
      g_jnz(+3);
      g_jmp(-3 - (m - b));
      g_label();
    }
    return;
  }

  if (tokeq("yield"))  /* todo: restrict usage outside of function */
  {
    scan();
    boolexpr();
    g_pop_dx();
    g_pop_ax();
    g_push_dx();
    g_push_ax();
    g_ret();
    return;
  }

  if (tokeq("CONST"))
  {
    char q[128];
    scan();
    cc = 1;
    strcpy(q,token);
    scan();
    if (tokeq("="))
    {
      scan();
      cv = atoi(token);
      scan();
      sym_addr(q, &cc, &cv);
      while (tokeq(","))
      {
	scan();
	cc = 1;
	strcpy(q,token);
	scan();
	if (tokeq("="))
	{
	  scan();
	  cv = atoi(token);
	  scan();
	  sym_addr(q, &cc, &cv);
	}
      }
    } else
    {
      error("Malformed CONST");
    }
    return;
  }
  if(tokeq("*"))
  {
    scan();
    name(1);  /* declaring instance */
  } else
  {
    name(0);  /* application */
  }
  if(tokeq("*"))
  {
    scan();
    if(token[0] >= '0' && token[0] <= '9')
    {
      int v = atoi(token) - 2;
      stalloc(v);
      scan();
    } else
    {
      error("Need literal integer for array dimension");
    }
  } else
  {
    dereference();
  }
  if(tokeq("BYTE"))
  {
    bc = 1;
    scan();
  }
  if(tokeq("="))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_pop_bx();
    if(bc)
    {
      g_mov_BX_al();
    } else
    {
      g_mov_BX_ax();
    }
  }
  if(tokeq("+="))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_pop_bx();
    if(bc)
    {
      g_add_BX_al();
    } else
    {
      g_add_BX_ax();
    }
  }
  if(tokeq("-="))
  {
    scan();
    boolexpr();
    g_pop_ax();
    g_pop_bx();
    if(bc)
    {
      g_sub_BX_al();
    } else
    {
      g_sub_BX_ax();
    }
  }
}

void assignments(void)
{
  assignment();
  while (tokeq(";"))
  {
    scan();
    if (tokne("FIN") && tokne("") && tokne("}"))
    {
      assignment();
    } else break;
  }
}

void block(void)
{
  if (tokeq("{"))
  {
    scan();
    assignments();
    if (tokeq("}"))
    {
      scan();
    } else
    {
      error("Missing }");
    }
  } else
  {
    assignment();
  }
}

int illgol(void)
{
  assignments();
  if (tokeq("FIN"))
  {
    g_mov_al(0x00);
    g_mov_ah(0x4c);
    g_int(0x21);
    return 1;
  }
  error("Could not find FIN"); return 0;
}
