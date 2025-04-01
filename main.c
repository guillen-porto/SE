#include "MKL46Z4.h"
#include "lcd.h"

extern unsigned int reverse_int(unsigned int in);

void irclk_ini() {
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0 = 32 kHz, 1 = 4 MHz
}

//Custom delay function. Delay(3) would be equivalent to 3 normal calls to delay()
void delay(int mult)
{
  volatile int i;
  volatile int waitTime = 1000000 * mult;

  for (i = 0; i < waitTime; i++);
}


int main(void){

    //Displ is the number of bits that this is displaced.
    //With displ = 16, number 0xn would be 0xn0000
    //This is to test if it works in different possible positions
    for(int displ = 0; displ <= 16; displ += 4){

        //Original and reverse numbers
        unsigned int original = 0x9F06 << displ; 
        unsigned int reversed;

        //Numbers that will be displayed by the lcd
        unsigned int displayed_orig;
        unsigned int displayed_rev;

        irclk_ini();
        lcd_ini();

        reversed = reverse_int(original);

        //The lcd should display the same thing in all iterations -> displace numbers based on displ
        displayed_orig = original >> displ;
        displayed_rev = reversed >> (16 - displ);

        lcd_display_hex(displayed_orig);
        delay(4); //Quadruple delay (could make this with a button press to have more control)
        lcd_display_hex(displayed_rev);
        delay(4);
    }
}