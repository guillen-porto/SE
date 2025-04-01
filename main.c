#include "MKL46Z4.h"
#include "lcd.h"

extern unsigned int reverse_int(unsigned int in);



int main(void){

    lcd_ini();

    unsigned int original = 3579;
    unsigned int reversed;

    reversed = reverse_int(original);

    printf("Original: %u\nReversed: %u", original, reversed);
}