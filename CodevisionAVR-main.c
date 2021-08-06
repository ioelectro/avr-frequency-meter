#include <mega16a.h>
#include <delay.h>
#include <stdint.h>

#define DATA_PORT             PORTB
#define SEG_CONTROL_PORT      PORTC

#define SEG_SWITCH_TIME_MS    10
#define DISPLAY_XTIME         10

const uint8_t To7Seg[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40};

float freq=-1;
unsigned long timerOVFCounter=0;
unsigned long timerCap;

void control_seg(uint8_t segNumberFrom0To7,uint8_t OnOff)
{
    if(OnOff)SEG_CONTROL_PORT|=(1<<segNumberFrom0To7);
    else SEG_CONTROL_PORT&=~(1<<segNumberFrom0To7);
}

void print_one_seg(uint8_t number0To9,uint8_t dotPoint)
{
    uint8_t mix=To7Seg[number0To9];
    mix|=(dotPoint<<7);
    DATA_PORT=~mix;
}

void print_null()
{
    print_one_seg(10,0);

    control_seg(0,1);
    control_seg(1,1);
    control_seg(2,1);
    control_seg(3,1);

    delay_ms(500);
}

void print_float(float fnum)
{
    int i,j;
    long temp;
    long num=fnum*100;
    uint8_t a,b,c,d;

    a=num/1000;
    temp=num%1000;

    b=temp/100;
    temp=temp%100;

    c=temp/10;
    d=temp%10;


    for(i=0;i<DISPLAY_XTIME;i++)
    {
        for(j=0;j<4;j++)
        {
            switch(j)
            {
                case 0:print_one_seg(a,0);break;
                case 1:print_one_seg(b,1);break;
                case 2:print_one_seg(c,0);break;
                case 3:print_one_seg(d,0);break;
            }
            control_seg(j,1);
            delay_ms(SEG_SWITCH_TIME_MS);
            control_seg(j,0);
        }
    }
}

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
    timerCap=TCNT1;
    TCNT1=0;
    freq=1000000UL/(float)((long)timerCap+(timerOVFCounter*0xffffUL));
    timerOVFCounter=0;
}

// Timer1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
    timerOVFCounter++;
}

void main(void)
{
    //GPIO Init
    DDRC=0x0F;
    DDRB=0xFF;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 1000/000 kHz
    // Mode: Normal top=0xFFFF
    // OC1A output: Disconnected
    // OC1B output: Disconnected
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer Period: 65/536 ms
    // Timer1 Overflow Interrupt: On
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (1<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

    // External Interrupt(s) initialization
    // INT0: On
    // INT0 Mode: Rising Edge
    // INT1: Off
    // INT2: Off
    GICR|=(0<<INT1) | (1<<INT0) | (0<<INT2);
    MCUCR=(0<<ISC11) | (0<<ISC10) | (1<<ISC01) | (1<<ISC00);
    MCUCSR=(0<<ISC2);
    GIFR=(0<<INTF1) | (1<<INTF0) | (0<<INTF2);

    // Global enable interrupts
    #asm("sei")

    while (1)
    {
        if(freq>=0 && freq <=99.99)
            print_float(freq);
        else print_null();
    }
}
