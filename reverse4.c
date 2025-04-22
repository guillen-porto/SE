unsigned int reverse_int(unsigned int in)
{ // Devolve o enteiro invertido bit a bit
    // Paso 1: Invertir bits pares e impares
    in = ((in & 0x55555555) << 1) | ((in >> 1) & 0x55555555);
    // Paso 2: Invertir grupos de 2 bits, para obter cuartetos xa invertidos
    in = ((in & 0x33333333) << 2) | ((in >> 2) & 0x33333333);
    // Paso 3: Invertir cuartetos, obtendo 4 bytes xa completamente invertidos
    in = ((in & 0x0F0F0F0F) << 4) | ((in >> 4) & 0x0F0F0F0F);
    // Paso 4: Invertir a orde dos 4 bytes
    in = __builtin_bswap32(in);
    // Función propia de GCC para invertir bytes nunha palabra de 32 bits
    // https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
    return in;
} 