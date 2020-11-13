#include <mega8.h>
#include <alcd.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>

#define ADC_VREF_TYPE ((0<<REFS1) | (0<<REFS0) | (0<<ADLAR))
#define LED PORTD.1
#define LED_BUTTON PIND.0


    
    int counter = 0;
    char LCD_BUFER[10];
    char buf[10];
    float Voltage = 0;
     


unsigned int read_adc(unsigned char adc_input)
{
     ADMUX=adc_input | ADC_VREF_TYPE;
    // Delay needed for the stabilization of the ADC input voltage
    delay_us(10);
    // Start the AD conversion
    ADCSRA|=(1<<ADSC);
    // Wait for the AD conversion to complete
    while ((ADCSRA & (1<<ADIF))==0);
    ADCSRA|=(1<<ADIF);
    return ADCW;
}
void Read_and_Display_Voltage(void){
      lcd_gotoxy(0,1);
      lcd_puts("U= ");
      lcd_gotoxy(2,1);
      delay_ms(100);
      Voltage = (read_adc(0) / 51.1) * 3.1;
      ftoa(Voltage,1,buf);
      lcd_puts(buf);
}
void Read_and_Display_RPM(void){
  counter *= 60;
  lcd_gotoxy(0,0);
  lcd_puts("RPM ");
  lcd_gotoxy(4,0);
  itoa(counter,LCD_BUFER);
  lcd_puts(LCD_BUFER);
  counter = 0;
}
void Init_Timer(void)
{
    TCCR1A=0x00; 
    TCCR1B=0x05;
    TCNT1=0x00; 
    OCR1A=0x1E85; 
    TIMSK=0x10;
} 
void Init_ExtInterrupt(void)
{
 GICR|=(1<<INT1) | (0<<INT0);
 MCUCR=(1<<ISC11) | (1<<ISC10) | (0<<ISC01) | (0<<ISC00);
 GIFR=(1<<INTF1) | (0<<INTF0);
}
void Init_ADC(void)
{
    ADMUX=ADC_VREF_TYPE;
    ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADFR) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    SFIOR=(0<<ACME);
}



interrupt [TIM1_COMPA] void timer1_compa_isr(void)
{
  lcd_clear();   //Що разу чистимо буфер дсплею
  
  Read_and_Display_RPM(); 
  
  Read_and_Display_Voltage();    
      
  TCNT1=0; //обнуляем таймер 
  }


interrupt [EXT_INT1] void ext_int1_isr(void)
{
 counter++;
}
 
void main(void)
{
    Init_Timer();
    Init_ExtInterrupt();
    Init_ADC();        
    lcd_init(8);
    lcd_clear();

    #asm("sei") // Global Enable Interrupt
LED = 0;
 
while (1)
      {
       if(LED_BUTTON == 0)
       {
        LED = 1;
       }        
       
       else
       {
        LED = 0;
       }
            
      };
    }