/*
  runtime.c for illgol##.c
  THIS FILE MAY OR MAY NOT BE PART OF THE OPEN SORES ILLGOL## TO 8086 COMPILER
  (c)2001 Cat's Eye Technologies.
  All rights reserved.  All liability disclaimed.  
  See the file COPING for license information.  
*/

byte * trig_table  = NULL;
byte * sin_routine = NULL;
byte * cos_routine = NULL;
int trig_installed = 0;

int trigtab[] =
{   0,  24,  49,  73,  98, 122, 146, 170,
  195, 219, 242, 266, 290, 313, 336, 359,
  382, 405, 427, 449, 471, 492, 514, 534,
  555, 575, 595, 615, 634, 653, 671, 689,
  707, 724, 740, 757, 773, 788, 803, 817,
  831, 844, 857, 870, 881, 893, 903, 914,
  923, 932, 941, 949, 956, 963, 970, 975,
  980, 985, 989, 992, 995, 997, 998, 999 };

void runtime_trig(void)
{
  byte * l;

  if (trig_installed) return;

  trig_table = stalloc(128);
  memcpy(trig_table, trigtab, 128);

  l = g_lambda();

  g_xor_ah_ah();
  g_xor_bx_bx();
  g_xor_dx_dx();
  g_cmp_al(63);
  g_ja(+17);

  g_label(); g_mov_bl_al();
  g_label(); g_shl_bl_1();

  g_add_bx(caddr(trig_table));
  g_mov_cx_BX();
  g_or_dx_dx();
  g_jz(+2);
  g_neg_cx();
  g_ret();

  g_label();
  g_cmp_al(127);
  g_ja(+7);

  g_mov_bl(127);
  g_sub_bx_ax();

  g_jmp(-26);
  g_label();

  g_cmp_al(191);
  g_ja(+8);

  g_sub_al(128);
  g_mov_dx(0x8000);
  g_jmp(-40);

  g_label();

  g_mov_bl(255);
  g_sub_bx_ax();

  g_mov_dx(0x8000);
  g_jmp(-48);

  g_ret();
  sin_routine = g_close_lambda(l);

  l = g_lambda();

  g_cmp_al(192);
  g_jb(+7);
  g_sub_al(192);
  g_mov_bx(caddr(sin_routine));
  g_jmp_bx();
  g_label();
  g_add_al(64);
  g_mov_bx(caddr(sin_routine));
  g_jmp_bx();

  cos_routine = g_close_lambda(l);

  trig_installed = 1;
}

byte * hgr_routine = NULL;
byte * text_routine = NULL;
byte * hplot_routine = NULL;
byte * hline_routine = NULL;
byte * hindex_routine = NULL;
byte * hget_routine = NULL;
byte * hput_routine = NULL;
byte * hlay_routine = NULL;

int gfx_installed = 0;

void runtime_gfx(void)
{
  byte * l;

  if (gfx_installed) return;

  l = g_lambda();
  g_mov_ax(0x0013);
  g_int(0x10);
  g_mov_ax(0xA000);
  g_mov_es_ax();
  g_ret();
  hgr_routine = g_close_lambda(l);

  l = g_lambda();
  g_mov_ax(0x0003);
  g_int(0x10);
  g_mov_ax_ds();
  g_mov_es_ax();
  g_ret();
  text_routine = g_close_lambda(l);

  l = g_lambda();

  g_mov_ah_al();
  g_xor_al_al();
  g_mov_dx_ax();
  g_shr_ax_1();
  g_shr_ax_1();
  g_add_ax_dx();
  g_add_ax_bx();
  g_push_ax();
  g_pop_di();
  g_es(); g_mov_DI_cl();
  g_ret();
  hplot_routine = g_close_lambda(l);

  l = g_lambda();
  g_mov_ah_al();
  g_xor_al_al();
  g_mov_dx_ax();
  g_shr_ax_1();
  g_shr_ax_1();
  g_add_ax_dx();
  g_add_ax_bx();
  g_push_ax();
  g_pop_di();
  g_ret();
  hindex_routine = g_close_lambda(l);

  l = g_lambda();
  /*
    col = 0; row = 0; done = 0;
    while not done
    {
      copy es:di to cx
      inc es:di; inc col;
      if col == w
      {
	col = 0; es:di += (320 - w)
	inc row;
	if row == h done = 1;
      }
    }
  */
  g_ret();
  hget_routine = g_close_lambda(l);

  gfx_installed = 1;
}

/* END of runtime.c */

