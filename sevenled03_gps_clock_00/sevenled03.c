
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );
void DOWFI ( void );

#define GPIOABASE   0x48000000
#define RCCBASE     0x40021000
#define RCC_CR      (RCCBASE+0x00)
#define RCC_CFGR    (RCCBASE+0x04)
#define RCC_CFGR2   (RCCBASE+0x2C)

#define FLASH_BASE  0x40022000
#define FLASH_ACR   (FLASH_BASE+0x00)

#define STK_CSR     0xE000E010
#define STK_RVR     0xE000E014
#define STK_CVR     0xE000E018
#define STK_MASK    0x00FFFFFF


#define ALLON ((1<<(7+16))|(1<<(6+16))|(1<<(5+16))|(1<<(4+0))|(1<<(3+0))|(1<<(2+0))|(1<<(1+0)))
#define ALLOFF ((1<<(7+0))|(1<<(6+0))|(1<<(5+0))|(1<<(4+16))|(1<<(3+16))|(1<<(2+16))|(1<<(1+16)))

#define LED2Y ((1<<(7+16))|(1<<(6+ 0))|(1<<(5+ 0)))
#define LED1Y ((1<<(7+ 0))|(1<<(6+16))|(1<<(5+ 0)))
#define LED0Y ((1<<(7+ 0))|(1<<(6+ 0))|(1<<(5+16)))

#define LED0H ((1<<(4+16))|(1<<(3+16))|(1<<(2+16))|(1<<(1+16)))
#define LED1H ((1<<(4+16))|(1<<(3+16))|(1<<(2+16))|(1<<(1+ 0)))
#define LED2H ((1<<(4+16))|(1<<(3+16))|(1<<(2+ 0))|(1<<(1+16)))
#define LED3H ((1<<(4+16))|(1<<(3+16))|(1<<(2+ 0))|(1<<(1+ 0)))
#define LED4H ((1<<(4+16))|(1<<(3+ 0))|(1<<(2+16))|(1<<(1+16)))
#define LED5H ((1<<(4+16))|(1<<(3+ 0))|(1<<(2+16))|(1<<(1+ 0)))
#define LED6H ((1<<(4+16))|(1<<(3+ 0))|(1<<(2+ 0))|(1<<(1+16)))
#define LED7H ((1<<(4+16))|(1<<(3+ 0))|(1<<(2+ 0))|(1<<(1+ 0)))
#define LED8H ((1<<(4+ 0))|(1<<(3+16))|(1<<(2+16))|(1<<(1+16)))
#define LED9H ((1<<(4+ 0))|(1<<(3+16))|(1<<(2+16))|(1<<(1+ 0)))
#define LEDAH ((1<<(4+ 0))|(1<<(3+16))|(1<<(2+ 0))|(1<<(1+16)))
#define LEDBH ((1<<(4+ 0))|(1<<(3+16))|(1<<(2+ 0))|(1<<(1+ 0)))
#define LEDCH ((1<<(4+ 0))|(1<<(3+ 0))|(1<<(2+16))|(1<<(1+16)))
#define LEDDH ((1<<(4+ 0))|(1<<(3+ 0))|(1<<(2+16))|(1<<(1+ 0)))
#define LEDEH ((1<<(4+ 0))|(1<<(3+ 0))|(1<<(2+ 0))|(1<<(1+16)))
#define LEDFH ((1<<(4+ 0))|(1<<(3+ 0))|(1<<(2+ 0))|(1<<(1+ 0)))

static const unsigned int ledrow[4]=
{
    LED0Y,
    LED1Y,
    LED2Y,
    0
};

static const unsigned int ledcol[16]=
{
    LED0H,
    LED1H,
    LED2H,
    LED3H,
    LED4H,
    LED5H,
    LED6H,
    LED7H,
    LED8H,
    LED9H,
    LEDAH,
    LEDBH,
    LEDCH,
    LEDDH,
    LEDEH,
    LEDFH,
};


#define USART1BASE 0x40013800

static int uart_init ( void )
{
    unsigned int ra;

    ra=GET32(RCCBASE+0x14);
    ra|=1<<17; //enable port a
    PUT32(RCCBASE+0x14,ra);

    ra=GET32(RCCBASE+0x18);
    ra|=1<<14; //enable USART1
    PUT32(RCCBASE+0x18,ra);

    //moder 10
    ra=GET32(GPIOABASE+0x00);
    ra&=~(3<<18); //PA9
    ra|=2<<18; //PA9
    ra&=~(3<<20); //PA10
    ra|=2<<20; //PA10
    PUT32(GPIOABASE+0x00,ra);
    //OTYPER 0
    ra=GET32(GPIOABASE+0x04);
    ra&=~(1<<9); //PA9
    ra&=~(1<<10); //PA10
    PUT32(GPIOABASE+0x04,ra);
    //ospeedr 11
    ra=GET32(GPIOABASE+0x08);
    ra|=3<<18; //PA9
    ra|=3<<20; //PA10
    PUT32(GPIOABASE+0x08,ra);
    //pupdr 00
    ra=GET32(GPIOABASE+0x0C);
    ra&=~(3<<18); //PA9
    ra&=~(3<<20); //PA10
    PUT32(GPIOABASE+0x0C,ra);
    //afr 0001
    ra=GET32(GPIOABASE+0x24);
    ra&=~(0xF<<4); //PA9
    ra|=0x1<<4; //PA9
    ra&=~(0xF<<8); //PA10
    ra|=0x1<<8; //PA10
    PUT32(GPIOABASE+0x24,ra);

    ra=GET32(RCCBASE+0x0C);
    ra|=1<<14; //reset USART1
    PUT32(RCCBASE+0x0C,ra);
    ra&=~(1<<14);
    PUT32(RCCBASE+0x0C,ra);

    //8mhz  8000000/115200 = 69.444
    //48mhz  48000000/115200 = 416.666
    //48mhz  48000000/4800 = 10000

    //PUT32(USART1BASE+0x0C,69); //8MHz
    //PUT32(USART1BASE+0x0C,417); //48MHz
    PUT32(USART1BASE+0x0C,10000); //48MHz
    PUT32(USART1BASE+0x08,1<<12);
    PUT32(USART1BASE+0x00,(1<<3)|(1<<2)|1);

    return(0);
}

static void uart_send ( unsigned int x )
{
    while(1) if(GET32(USART1BASE+0x1C)&(1<<7)) break;
    PUT32(USART1BASE+0x28,x);
}

static
unsigned int uart_recv ( void )
{
    while(1) if((GET32(USART1BASE+0x1C))&(1<<5)) break;
    return(GET32(USART1BASE+0x24));
}

//static void hexstrings ( unsigned int d )
//{
    ////unsigned int ra;
    //unsigned int rb;
    //unsigned int rc;

    //rb=32;
    //while(1)
    //{
        //rb-=4;
        //rc=(d>>rb)&0xF;
        //if(rc>9) rc+=0x37; else rc+=0x30;
        //uart_send(rc);
        //if(rb==0) break;
    //}
    //uart_send(0x20);
//}

//static void hexstring ( unsigned int d )
//{
    //hexstrings(d);
    //uart_send(0x0D);
    //uart_send(0x0A);
//}

static unsigned char xstring[32];
static unsigned int tim[4];
#define ZMASK 0xFF
//static unsigned char zbuff[ZMASK+1];
static unsigned int zhead;
static unsigned int ztail;




volatile unsigned int lasttim[4];

volatile unsigned int counter;
volatile unsigned int ledx;
volatile unsigned int ledy;
void systick_handler ( void )
{

    switch(counter++)
    {
        case 0:
        {
            PUT32(GPIOABASE+0x18,ledrow[ledy]|ledcol[0xF]);
            break;
        }
        case 1:
        {
            unsigned int rb;

            switch(ledy)
            {
                case 0:
                {
                    rb=lasttim[1];
                    if(lasttim[0]) rb+=10;
                    break;
                }
                case 1:
                {
                    rb=lasttim[2];
                    break;
                }
                case 2:
                {
                    rb=lasttim[3];
                    break;
                }
                default:
                {
                    rb=0;
                    break;
                }
            }
            PUT32(GPIOABASE+0x18,ledrow[ledy]|ledcol[rb&0xF]);
            break;
        }
        case 10:
        {
            PUT32(GPIOABASE+0x18,ALLOFF);
            break;
        }
        case 20:
        {
            //PUT32(GPIOABASE+0x18,ALLOFF);
            ledx++;
            if(ledx>3)
            {
                ledx=0;
                ledy++;
                if(ledy>2) ledy=0;
            }
            counter=0;
            break;
        }
    }
}

//static void uart_check ( void )
//{
    //if((GET32(USART1BASE+0x1C))&(1<<5))
    //{
        //zhead=(zhead+1)&ZMASK;
        //zbuff[zhead]=GET32(USART1BASE+0x24)&0xFF;
    //}
//}
static void show_clock ( void )
{
    unsigned int ra;

    ra=0;
    if(tim[0]!=lasttim[0]) ra++;
    if(tim[1]!=lasttim[1]) ra++;
    if(tim[2]!=lasttim[2]) ra++;
    if(tim[3]!=lasttim[3]) ra++;
    if(ra==0) return;

    lasttim[0]=tim[0];
    lasttim[1]=tim[1];
    lasttim[2]=tim[2];
    lasttim[3]=tim[3];

    uart_send(0x30+lasttim[0]);
    uart_send(0x30+lasttim[1]);
    uart_send(0x30+lasttim[2]);
    uart_send(0x30+lasttim[3]);
    uart_send(0x0D);
    uart_send(0x0A);

}

static void clock_init ( void )
{
    unsigned int ra;

    ra=GET32(RCC_CR);
    ra|=(1<<16); //HSEON
    PUT32(RCC_CR,ra);
    //wait for HSERDY
    while(1)
    {
        ra=GET32(RCC_CR);
        if(ra&(1<<17)) break;
    }
    //switch to HSE
    ra=GET32(RCC_CFGR);
    ra&=~3;
    ra|=1;
    PUT32(RCC_CFGR,ra);
    //wait for it
    while(1)
    {
        ra=GET32(RCC_CFGR);
        if((ra&3)==1) break;
    }
    //now using external clock.
    //----------


    if(1)
    {
        //should already be off
        ra=GET32(RCC_CR);
        ra&=(~(1<<24)); //PLLON
        PUT32(RCC_CR,ra);
        //wait for PLLRDY off
        while(1)
        {
            ra=GET32(RCC_CR);
            if((ra&(1<<25))==0) break;
        }
        //should already be zero
        PUT32(RCC_CFGR2,0x00000000);
    }

    //external clock is 8MHz 8*6 = 48MHz.
    //0100: PLL input clock x 6
    ra=GET32(RCC_CFGR);
    ra&=(~(0xF<<18));
    ra|=(0x4<<18);
    ra|=(1<<16); //PLLSRC HSE/PREDIV
    PUT32(RCC_CFGR,ra);
    ra=GET32(RCC_CR);
    ra|=(1<<24); //PLLON
    PUT32(RCC_CR,ra);
    //wait for PLLRDY
    while(1)
    {
        ra=GET32(RCC_CR);
        if((ra&(1<<25))!=0) break;
    }

    //if we overclock the flash we can crash
    ra=GET32(FLASH_ACR);
    ra|=0x00000001;
    PUT32(FLASH_ACR,ra);

    //switch to PLL
    ra=GET32(RCC_CFGR);
    ra&=~3;
    ra|=2;
    PUT32(RCC_CFGR,ra);
    //wait for it
    while(1)
    {
        ra=GET32(RCC_CFGR);
        if((ra&3)==2) break;
    }
}

int delay ( unsigned int n )
{
    unsigned int ra;

    while(n--)
    {
        while(1)
        {
            ra=GET32(STK_CSR);
            if(ra&(1<<16)) break;
        }
    }
    return(0);
}


//$GPRMC,000143.00,A,4030.97866,N,07955.13947,W,0.419,,020416,,,A*6E

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
static unsigned int timezone;
//------------------------------------------------------------------------
static int do_nmea ( void )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;
    unsigned int state;
    unsigned int off;
    //unsigned char toggle_seconds;

    //toggle_seconds=0;
    state=0;
    off=0;
//$GPRMC,054033.00,V,
    while(1)
    {
        ra=uart_recv();
        //uart_send(ra);
        //uart_send(0x30+state);
        switch(state)
        {
            case 0:
            {
                if(ra=='$') state++;
                else state=0;
                break;
            }
            case 1:
            {
                if(ra=='G') state++;
                else state=0;
                break;
            }
            case 2:
            {
                if(ra=='P') state++;
                else state=0;
                break;
            }
            case 3:
            {
                if(ra=='R') state++;
                else state=0;
                break;
            }
            case 4:
            {
                if(ra=='M') state++;
                else state=0;
                break;
            }
            case 5:
            {
                if(ra=='C') state++;
                else state=0;
                break;
            }
            case 6:
            {
                off=0;
                if(ra==',') state++;
                else state=0;
                break;
            }
            case 7:
            {
                if(ra==',')
                {
                    if(off>7)
                    {
                        rb=xstring[0]&0xF;
                        rc=xstring[1]&0xF;
                        //1010
                        rb=/*rb*10*/(rb<<3)+(rb<<1); //times 10
                        rb+=rc;
                        if(rb>12) rb-=12;
                        //ra=5; //time zone adjustment winter
                        //ra=4; //time zone adjustment summer
                        ra=timezone;
                        if(rb<=ra) rb+=12;
                        rb-=ra;
                        if(rb>9)
                        {
                            xstring[0]='1';
                            rb-=10;
                        }
                        else
                        {
                            xstring[0]='0';
                        }
                        rb&=0xF;
                        xstring[1]=0x30+rb;
                        rb=0;
                        //zstring[rb++]=0x77;
                        //toggle_seconds^=0x10;
                        //zstring[rb++]=toggle_seconds;
                        //zstring[rb++]=xstring[0];
                        //zstring[rb++]=xstring[1];
                        //zstring[rb++]=xstring[2];
                        //zstring[rb++]=xstring[3];
                        //xstring[rb++]=0x0D;
                        //zstring[rb++]=0;
                        tim[0]=xstring[0]&0xF;
                        tim[1]=xstring[1]&0xF;
                        tim[2]=xstring[2]&0xF;
                        tim[3]=xstring[3]&0xF;
                    }
                    else
                    {
                        //zstring[0]=0x33;
                        //zstring[1]=0x33;
                        //zstring[2]=0x33;
                        //zstring[3]=0x33;
                        //xstring[4]=0x0D;
                        //zstring[5]=0;
                        tim[0]=0;
                        tim[1]=0;
                        tim[2]=0;
                        tim[3]=0;
                    }
                    off=0;
                    state++;
                }
                else
                {
                    if(off<16)
                    {
                        xstring[off++]=ra;
                    }
                }
                break;
            }
            case 8:
            {
                //if(zstring[off]==0)
                //{
                    //state=0;
                //}
                //else
                //{
                    //uart_send(zstring[off++]);
                //}
                //show_time();
                show_clock();
                state=0;
                break;
            }
        }
    }
    return(0);
}



int notmain ( void )
{
    unsigned int ra;

    clock_init();
    uart_init();

    ra=GET32(RCCBASE+0x14);
    ra|=1<<17; //enable port a
    PUT32(RCCBASE+0x14,ra);

    //moder
    ra=GET32(GPIOABASE+0x00);
    ra&=~(3<<(1<<1));
    ra&=~(3<<(2<<1));
    ra&=~(3<<(3<<1));
    ra&=~(3<<(4<<1));
    ra&=~(3<<(5<<1));
    ra&=~(3<<(6<<1));
    ra&=~(3<<(7<<1));
    ra|= (1<<(1<<1));
    ra|= (1<<(2<<1));
    ra|= (1<<(3<<1));
    ra|= (1<<(4<<1));
    ra|= (1<<(5<<1));
    ra|= (1<<(6<<1));
    ra|= (1<<(7<<1));
    PUT32(GPIOABASE+0x00,ra);
    //otyper
    ra=GET32(GPIOABASE+0x04);
    ra&=~(1<<1);
    ra&=~(1<<2);
    ra&=~(1<<3);
    ra&=~(1<<4);
    ra&=~(1<<5);
    ra&=~(1<<6);
    ra&=~(1<<7);
    PUT32(GPIOABASE+0x04,ra);

    PUT32(STK_CSR,4);
    PUT32(STK_RVR,1000-1);
    PUT32(STK_CVR,0x00000000);

    counter=0;
    ledx=0;
    ledy=0;

    timezone=4;
    if(GET32(0x20000D00)==0x12341234) timezone=5;
    PUT32(0x20000D00,0x12341234);

    lasttim[0]=1;
    lasttim[1]=1;
    lasttim[2]=2;
    lasttim[3]=3;

    zhead=0;
    ztail=0;

    PUT32(STK_CSR,7);

    do_nmea();

    return(0);
}

/*

halt
flash write_image erase sevenled03_gps_clock_00/sevenled03.elf
reset

*/
