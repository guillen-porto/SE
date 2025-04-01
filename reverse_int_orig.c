unsigned int reverse_int(unsigned int in)
{
  unsigned int out = 0;
  // Devolve o enteiro invertido bit a bit

  for (unsigned int i=0; i<32; i++) {
    out = out << 1;
    out |= in & 1;
    in = in >> 1;
  }

  return out;
}
