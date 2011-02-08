/* ersteuhr.c
Eine kleine Binäruhr.
Taste 1 schaltet auf Sekunden,
Taste 2 auf Minuten,
Taste 3 auf Stunden um.
Taste 4 invertiert die Anzeige.*/
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

volatile uint8_t sekunden, minuten, stunden;
volatile uint8_t modus, inverse;
volatile uint8_t anti_prell;
void timer0_on();
void timer1_on();


int main (void)
{
  modus = 1;
  inverse = 0;
  DDRB = 0; //Port B auf Input setzen
  PORTB = 0x0F; //Setze Pullup Wiederstände im Atmel für Taster (gibt 5V auf den Ausgang die die Schalter dann gegen Ground schalten)
  DDRC = 0xFF; //Port C auf Output setzen 
  srand(121); //randomseed initialisieren
  sekunden = rand() % 60; //sekunden auf einen Wert setzen
  minuten = rand() % 60; //minuten ebenso
  stunden = rand() % 24; //stunden eh
  timer0_on(); //schalte Timer0 an
  timer1_on(); //schalte Timer1 an
  sei(); //Schaltet Interrupts an
  
  while(42) {
   switch(inverse){
     case 0:
        switch(modus){
          case 1:
            PORTC= sekunden;
            break;
          case 2:
            PORTC= minuten;
            break;
          case 3:
            PORTC= stunden;
            break;
        }
        break;
    case 1:
        switch(modus){
          case 1:
            PORTC = (sekunden^0xFF);
            break;
          case 2:
            PORTC = (minuten^0xFF);
            break;
          case 3:
            PORTC = (stunden^0xFF);
            break;
        }
        break;
    }
  }
}

SIGNAL(SIG_OUTPUT_COMPARE1A){
  sekunden++;
  if (sekunden > 59){
    sekunden = 0;
    minuten++;
  }
  if (minuten > 59){
    minuten = 0;
    stunden++;
  }
  if (stunden > 23){
    stunden = 0;
  }
}

SIGNAL(SIG_OUTPUT_COMPARE0){  
  if((++anti_prell%12)==0){
    switch(PINB&0x0f){
      case 0x0E: //button 1 -> sekunden
        modus = 1;
        break;
      case 0x0D: //button 2 -> minuten
        modus = 2;
        break;
      case 0x07: //button 4 -> stunden
        modus = 3;
        break;
      case 0x0B: //button 3 -> invertieren
        inverse = (inverse+1)&1;
        break;
    }
  }
}

void timer0_on(){
  TCCR0 |= (1 << WGM01); //CTC Modus
  TCCR0 |= (1 << CS00); // Prescaler auf 1024
  TCCR0 |= (1 << CS02); // Prescaler auf 1024
  TIMSK |= (1 << OCIE0); // Interrupt auslösen wenn CTC-Match
  OCR0 = 150; //Schwellenwert für CTC
}

void timer1_on(){
  TCCR1B |= (1 << WGM12); //CTC Modus
  TCCR1B |= (1 << CS12); // Prescaler auf 1024
  TCCR1B |= (1 << CS10); // Prescaler auf 1024
  TIMSK |= (1 << OCIE1A); // Interrupt auslösen wenn CTC-Match
  OCR1A = 15624; //Schwellenwert für CTC
}