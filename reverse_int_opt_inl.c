unsigned int reverse_int(unsigned int in)
{
  unsigned int out = 0;

  asm(
    ".syntax unified\n\t"
    "movs   r2, #31\n\t"
    "L2:\n\t"
    "movs r1, #1\n\t"
    "ands r1, %[ORIGINAL]\n\t"
    "subs r2, r2, 1\n\t"
    "orrs %[REVERSED], r1\n\t"
    "lsls %[REVERSED], %[REVERSED], #1\n\t"
    "lsrs %[ORIGINAL], %[ORIGINAL], #1\n\t"
    "cmp r2, #0\n\t"
    "bne L2\n\t"
    "movs r1, #1\n\t"
    "ands r1, %[ORIGINAL]\n\t"
    "orrs %[REVERSED], r1"
    : [ORIGINAL] "+l" (in), [REVERSED] "+l" (out)
  );

  return out;
}
