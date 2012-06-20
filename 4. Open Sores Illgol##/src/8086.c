/*
  8086.c
  8086 machine-code generating module including simple peephole opt.
  (c)2001 Cat's Eye Technologies.
  All rights reserved.  All liability disclaimed.
  See the file COPING for license information.
*/

byte s[16384];
byte * m;
#define GEN(x) *m = x; m++;
byte hi, lo;
#define GENW(x) lo=(byte)(x&0xff); hi=(byte)((x>>8)&0xff); GEN(lo); GEN(hi);
byte lbl = 0;
void g_label(void) { lbl = 1; }

#define DID(x)  lbl=0; return x;

int g_hlt(void)         { GEN(244); DID(1); }

int g_push_ax(void)     { GEN(80); DID(1); }
int g_push_bx(void)     { GEN(83); DID(1); }
int g_push_cx(void)     { GEN(81); DID(1); }
int g_push_dx(void)     { GEN(82); DID(1); }

int g_push_si(void)     { GEN(86); DID(1); }
int g_push_di(void)     { GEN(87); DID(1); }

int g_push_BX(void)     { GEN(255); GEN(55);  DID(2); }

int g_push_bp(void)	{ GEN(0x55); DID(1); }
int g_mov_ax_BP(sbyte s) { GEN(0x8B); GEN(0x46); GEN(s); DID(3); }
int g_mov_bp_sp(void)   { GEN(0x8B); GEN(0xEC); DID(2); }
int g_pop_bp(void)      { GEN(0x5D); DID(1); }

int g_lahf(void)      { GEN(159); DID(1); }

int g_int(byte i)     { GEN(205); GEN(i); DID(2); }

int g_es(void)         { GEN(38); return 1; }

int g_mov_bl_ah(void)    { GEN(0x88); GEN(0xe3); DID(2); }

int g_mov_ch_al(void)    { GEN(0x88); GEN(0xc5); DID(2); }
int g_mov_dl_al(void)    { GEN(0x88); GEN(0xc2); DID(2); }

int g_mov_ax_I(word i)   { GEN(161); GENW(i); g_label(); DID(3); }
int g_mov_I_ax(word i)   { GEN(163); GENW(i); g_label(); DID(3); }
int g_mov_es_I(word i)   { GEN(142); GEN(6); GENW(i); g_label(); DID(4); }

int g_mov_ax_sp(void)    { GEN(0x8B); GEN(0xC4); DID(2); }
int g_add_ax(sword i)    { GEN(5); GENW(i); DID(3); }
int g_mov_bp_ax(void)    { GEN(0x8B); GEN(0xE8); DID(2); }
int g_mov_dh_al(void)    { GEN(0x8A); GEN(0xF0); DID(2); }

int g_jmp(sword i)       { GEN(0xe9); GENW(i); DID(3); }

int g_jmp_ax(void)    { GEN(255); GEN(224); DID(2); }
int g_call_ax(void)   { GEN(255); GEN(208); DID(2); }

int g_mov_ax_BX(void) { GEN(139); GEN(7); DID(2); }
int g_mov_cl_BX(void) { GEN(138); GEN(15); DID(2); }
int g_mov_DI_al(void) { GEN(136); GEN(5); DID(2); }
int g_cmp_bx_ax(void)   { GEN(57);  GEN(195); DID(2); }
int g_cmp_al_DI(void)   { GEN(58); GEN(5); DID(2); }
int g_mov_al_SI(void)   { GEN(138); GEN(4); DID(2); }
int g_mov_ax_DI(void)   { GEN(139); GEN(5); DID(2); }
int g_cmp_dl(byte q)    { GEN(0x80); GEN(0xFA); GEN(q); DID(3); }
int g_cmp_al_dl(void)   { GEN(0x38); GEN(0xD0); DID(2); }

int g_add_BX_ax(void)   { GEN(1); GEN(7); DID(2); }
int g_sub_BX_ax(void)   { GEN(0x29); GEN(7); DID(2); }

int g_add_BX_al(void)   { GEN(0); GEN(7); DID(2); }
int g_sub_BX_al(void)   { GEN(0x28); GEN(7); DID(2); }

int g_shl_ax_1(void)    { GEN(209); GEN(224); DID(2); }
int g_shr_ax_1(void)    { GEN(209); GEN(232); DID(2); }

int g_shl_ax_cl(void)   { GEN(211); GEN(224); DID(2); }
int g_shr_ax_cl(void)   { GEN(211); GEN(232); DID(2); }

int g_cmp_ax(word i)    { GEN(0x3D); GENW(i); g_label(); DID(3); }
int g_cmp_bx(word i)    { GEN(0x81); GEN(0xFB); GENW(i); g_label(); DID(4); }

int g_cmp_ax_dx(void)   { GEN(0x39); GEN(0xD0); DID(2); }
int g_jle(sbyte d)      { GEN(0x7E); GEN(d); DID(2); }
int g_jge(sbyte d)      { GEN(0x7D); GEN(d); DID(2); }
int g_jl(sbyte d)       { GEN(0x7C); GEN(d); DID(2); }
int g_jg(sbyte d)       { GEN(0x7F); GEN(d); DID(2); }

int g_add_bx_ax(void)   { GEN(1); GEN(0xC3); DID(2); }
int g_add_al(sbyte s)   { GEN(4); GEN(s); DID(2); }
int g_jmp_bx(void)      { GEN(0xff); GEN(0xe3); DID(2); }

int g_mov_al_ah(void)   { GEN(0x88); GEN(0xE0); DID(2); }
int g_mov_ah_dl(void)   { GEN(0x88); GEN(0xD4); DID(2); }
int g_mov_ah_al(void)   { GEN(0x88); GEN(0xC4); DID(2); }
int g_mov_dl_ah(void)   { GEN(0x88); GEN(0xE2); DID(2); }

int g_add_dx(sword w)   { GEN(0x81); GEN(0xC2); GENW(w); DID(4); }
int g_mov_BX_dl(void)   { GEN(0x88); GEN(0x17); DID(2); }

int g_cmp_al(sbyte b)   { GEN(0x3C); GEN(b); DID(2); }
int g_mov_bl_al(void)	{ GEN(0x88); GEN(0xC3); DID(2); }
int g_shl_bl_1(void)	{ GEN(0xD0); GEN(0xE3); DID(2); }
int g_add_bx(sword w)   { GEN(0x81); GEN(0xC3); GENW(w); g_label(); DID(4); }
int g_mov_cx_BX(void)   { GEN(0x8B); GEN(0x0F); DID(2); }
int g_sub_bx_ax(void)   { GEN(0x29); GEN(0xC3); DID(2); }
int g_sub_al(sbyte b)   { GEN(0x2C); GEN(b); DID(2); }
int g_neg_cx(void)      { GEN(0xF7); GEN(0xD9); DID(2); }
int g_xor_cx_dx(void)   { GEN(0x31); GEN(0xD1); DID(2); }
int g_call_bx(void)     { GEN(0xFF); GEN(0xD3); DID(2); }
int g_mov_es_ax(void)   { GEN(0x8E); GEN(0xC0); DID(2); }
int g_mov_ax_ds(void)   { GEN(0x8C); GEN(0xD8); DID(2); }
int g_and_ax(sword w)   { GEN(0x25); GENW(w); g_label(); DID(3); }
int g_neg_ax(void)      { GEN(0xF7); GEN(0xD8); DID(2); }
int g_add_ax_bx(void)	{ GEN(0x01); GEN(0xD8); DID(2); }
int g_mov_DI_cl(void)	{ GEN(0x88); GEN(0x0D); DID(2); }
int g_call_dx(void)     { GEN(0xFF); GEN(0xD2); DID(2); }
int g_mov_bx_I(sword w) { GEN(0x8B); GEN(0x1E); GENW(w); g_label(); DID(4); }
int g_mov_I_bx(sword w) { GEN(0x89); GEN(0x1E); GENW(w); g_label(); DID(4); }

int g_add_bx_cx(void)   { GEN(0x01); GEN(0xCB); DID(2); }

int g_xchg_dx_ax(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 146) { DID(-1); }
    m++;
  }
  GEN(146);
  DID(1);
}

int g_xchg_bx_ax(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 147) { DID(-1); }
    m++;
  }
  GEN(147);
  DID(1);
}

int g_xchg_cx_ax(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 145) { DID(-1); }
    m++;
  }
  GEN(145);
  DID(1);
}

int g_xchg_si_ax(void) { GEN(0x96); DID(1); }
int g_mov_ax_BP_SI(void) { GEN(0x8B); GEN(2); DID(2); }

int g_mov_bx_ax(void) { GEN(137); GEN(195); DID(2); }
int g_mov_cx_ax(void) { GEN(137); GEN(193); DID(2); }
int g_mov_dx_ax(void) { GEN(137); GEN(194); DID(2); }

int g_mov_ax_bx(void) { GEN(0x89); GEN(0xD8); DID(2); }
int g_mov_ax_cx(void) { GEN(0x89); GEN(0xC8); DID(2); }
int g_mov_ax_dx(void) { GEN(0x89); GEN(0xD0); DID(2); }

int g_mov_bx_cx(void) { GEN(0x89); GEN(0xCB); DID(2); }
int g_mov_bx_dx(void) { GEN(139); GEN(210); DID(2); }

int g_mov_dx_bx(void) { GEN(0x89); GEN(0xDA); DID(2); }
int g_mov_cx_bx(void) { GEN(0x89); GEN(0xD9); DID(2); }
int g_mov_dx_cx(void) { GEN(0x89); GEN(0xCA); DID(2); }
int g_mov_cx_dx(void) { GEN(0x89); GEN(0xD1); DID(2); }

int g_pop_ax(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 80) { DID(-1); }
    if (*m == 83)
    {
      return g_mov_ax_bx() - 1;
    }
    if (*m == 81)
    {
      return g_mov_ax_cx() - 1;
    }
    if (*m == 82)
    {
      return g_mov_ax_dx() - 1;
    }
    m++;
  }
  GEN(88);
  DID(1);
}

int g_pop_bx(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 83) { DID(-1); }
    if (*m == 80)
    {
      return g_mov_bx_ax() - 1;
    }
    if (*m == 81)
    {
      return g_mov_bx_cx() - 1;
    }
    if (*m == 82)
    {
      return g_mov_bx_dx() - 1;
    }
    m++;
  }
  GEN(91);
  DID(1);
}

int g_pop_cx(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 81) { DID(-1); }
    if (*m == 80)
    {
      return g_mov_cx_ax() - 1;
    }
    if (*m == 83)
    {
      return g_mov_cx_bx() - 1;
    }
    if (*m == 82)
    {
      return g_mov_cx_dx() - 1;
    }
    m++;
  }
  GEN(89);
  DID(1);
}

int g_pop_dx(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 82) { DID(-1); }
    if (*m == 80)
    {
      return g_mov_dx_ax() - 1;
    }
    if (*m == 83)
    {
      return g_mov_dx_bx() - 1;
    }
    if (*m == 81)
    {
      return g_mov_dx_cx() - 1;
    }
    m++;
  }
  GEN(90);
  DID(1);
}

int g_pop_di(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 87) { DID(-1) };
    m++;
  }
  GEN(95);
  DID(1);
}

int g_pop_si(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 86) { DID(-1); }
    m++;
  }
  GEN(94);
  DID(1);
}

int g_ret(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 208)
    {
      m--;
      if (*m == 255) /* call ax */
      {
        return g_jmp_ax() - 2;
      }
      m++;
    }
    m++;
  }
  GEN(0xc3);
  DID(1);
}

int g_xor_ax_ax(void) { GEN(51); GEN(192); DID(2); }
int g_xor_bx_bx(void) { GEN(0x31); GEN(0xDB); DID(2); }
int g_xor_cx_cx(void) { GEN(49); GEN(201); DID(2); }
int g_xor_dx_dx(void) { GEN(49); GEN(210); DID(2); }

int g_mov_dl_dh(void) { GEN(0x88); GEN(0xf2); DID(2); }
int g_mov_cl_ch(void) { GEN(0x88); GEN(0xe9); DID(2); }

int g_xor_al_al(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 228)
    {
      m--;
      if (*m == 50) /* xor ah, ah */
      {
        return g_xor_ax_ax() - 2;
      }
      m++;
    }
    m++;
  }
  GEN(50);
  GEN(192);
  DID(2);
}

int g_xor_bl_bl(void) { GEN(0x30); GEN(0xDB); DID(2); }
int g_xor_cl_cl(void) { GEN(0x30); GEN(0xC9); DID(2); }
int g_xor_dl_dl(void) { GEN(0x30); GEN(0xD2); DID(2); }

int g_xor_ah_ah(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 192)
    {
      m--;
      if (*m == 50) /* xor al, al */
      {
        return g_xor_ax_ax() - 2;
      }
      m++;
    }
    m++;
  }
  GEN(50); GEN(228); DID(2);
}

int g_xor_bh_bh(void) { GEN(48); GEN(255); DID(2); }
int g_xor_ch_ch(void) { GEN(0x30); GEN(0xED); DID(2); }
int g_xor_dh_dh(void) { GEN(0x30); GEN(0xF6); DID(2); }

int g_mov_ah_cl(void) { GEN(136); GEN(204); DID(2); }
int g_mov_dh_cl(void) { GEN(136); GEN(206); DID(2); }

int g_mov_BX_al(void) { GEN(136); GEN(7); DID(2); }
int g_mov_BX_ax(void) { GEN(137); GEN(7); DID(2); }
int g_mov_BX_cl(void) { GEN(136); GEN(15); DID(2); }
int g_mov_BX_cx(void) { GEN(137); GEN(15); DID(2); }

int g_or_ax_ax(void)  { GEN(11); GEN(192); DID(2); }
int g_or_al_al(void)  { GEN(10); GEN(192); DID(2); }
int g_or_dx_dx(void)  { GEN(9); GEN(210); DID(2); }
int g_not_ax(void)    { GEN(247); GEN(208); DID(2); }

int g_inc_si(void)    { GEN(70); DID(1); }
int g_inc_di(void)    { GEN(71); DID(1); }

int g_inc_bx(void)    { GEN(67); DID(1); }
int g_inc_cx(void)    { GEN(0x41); DID(1); }
int g_inc_dx(void)    { GEN(0x42); DID(1); }

int g_dec_bx(void)    { GEN(0x4B); DID(1); }
int g_dec_cx(void)    { GEN(0x49); DID(1); }
int g_dec_dx(void)    { GEN(74); DID(1); }

int g_dec_dh(void)    { GEN(0xFE); GEN(0xCE); DID(2); }
int g_inc_dh(void)    { GEN(0xFE); GEN(0xC6); DID(2); }
int g_dec_dl(void)    { GEN(0xFE); GEN(0xCA); DID(2); }
int g_inc_dl(void)    { GEN(0xFE); GEN(0xC2); DID(2); }

int g_clc(void)       { GEN(0xF8); DID(1); }
int g_rcr_cx_1(void)  { GEN(0xD1); GEN(0xD9); DID(2); }
int g_rcr_dx_1(void)  { GEN(0xD1); GEN(0xDA); DID(2); }

int g_and_ax_dx(void) { GEN(35); GEN(194); DID(2); }
int g_or_ax_dx(void)  { GEN(1); GEN(194); DID(2); }
int g_xor_ax_dx(void) { GEN(51); GEN(194); DID(2); }

int g_add_ax_dx(void) { GEN(1); GEN(208); DID(2); }
int g_sub_ax_dx(void) { GEN(41); GEN(208); DID(2); }
int g_sub_dx_ax(void) { GEN(41); GEN(194); DID(2); }

int g_imul_dx(void)   { GEN(247); GEN(234); DID(2); }
int g_idiv_cx(void)   { GEN(247); GEN(249); DID(2); }

int g_inc_ax(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 72) { DID(-1); }
    m++;
  }
  GEN(64);
  DID(1);
}

int g_dec_ax(void)
{
  if (!lbl)
  {
    m--;
    if (*m == 64) { DID(-1); }
    m++;
  }
  GEN(72);
  DID(1);
}

int g_nop(void)
{
  if (!lbl) { DID(0); }
  GEN(144); DID(1);
}

int g_mov_ah(byte i)
{
  if (i == 0)
  {
    return g_xor_ah_ah();
  }
  GEN(180); GEN(i); DID(2);
}

int g_mov_bh(byte i)
{
  if (i == 0)
  {
    return g_xor_bh_bh();
  }
  GEN(0xB7); GEN(i); DID(2);
}

int g_mov_ch(byte i)
{
  if (i == 0)
  {
    return g_xor_ch_ch();
  }
  GEN(0xB5); GEN(i); DID(2);
}

int g_mov_dh(byte i)
{
  if (i == 0)
  {
    return g_xor_dh_dh();
  }
  GEN(0xB6); GEN(i); DID(2);
}

int g_mov_al(byte i)
{
  if (i == 0)
  {
    return g_xor_al_al();
  }
  GEN(176); GEN(i); DID(2);
}

int g_mov_bl(byte i)
{
  if (i == 0)
  {
    return g_xor_bl_bl();
  }
  GEN(179); GEN(i); DID(2);
}

int g_mov_cl(byte i)
{
  if (i == 0)
  {
    return g_xor_cl_cl();
  }
  GEN(177); GEN(i); DID(2);
}

int g_mov_dl(byte i)
{
  if (i == 0)
  {
    return g_xor_dl_dl();
  }
  GEN(0xB2); GEN(i); DID(2);
}

int g_jz(sbyte i)      { GEN(116); GEN(i); DID(2); }
int g_jnz(sbyte i)     { GEN(117); GEN(i); DID(2); }
int g_ja(sbyte i)      { GEN(119); GEN(i); DID(2); }
int g_jb(sbyte i)      { GEN(114); GEN(i); DID(2); }

int g_and_ah(byte i)     { GEN(128); GEN(228); GEN(i); DID(3); }

int g_mov_ax(word i)
{
  if (i == 0)
  {
    return g_xor_ax_ax();
  }
  GEN(0xB8);
  GENW(i);
  DID(3);
}

int g_mov_bx(word i)
{
  if (i == 0)
  {
    return g_xor_bx_bx();
  }
  GEN(187); GENW(i); DID(3);
}

int g_mov_cx(word i)
{
  if (i == 0)
  {
    return g_xor_cx_cx();
  }
  GEN(185); GENW(i); DID(3);
}

int g_mov_dx(word i)
{
  if (i == 0)
  {
    return g_xor_dx_dx();
  }
  GEN(0xBA); GENW(i); DID(3);
}

byte * g_lambda(void)
{
  g_label();
  return m;
}

byte * g_close_lambda(byte * l)
{
  byte * q = stalloc(m - l);
  memcpy(q, l, m - l);
  m = l;
  return q;
}

/* END of 8086.c */
