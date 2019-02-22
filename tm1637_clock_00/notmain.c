
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
    //48mhz  48000000/9600 = 5000

    //PUT32(USART1BASE+0x0C,69); //8MHz
    //PUT32(USART1BASE+0x0C,417); //48MHz
    //PUT32(USART1BASE+0x0C,5000); //48MHz
    PUT32(USART1BASE+0x0C,833); //48MHz
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




#define CLK 1
#define DIO 0

static const unsigned int led_table[16]=
{
    0x3F, //0011 1111  0
    0x06, //0000 0110  1
    0x5B, //0101 1011  2
    0x4F, //0100 1111  3
    0x66, //0110 0110  4
    0x6D, //0110 1101  5
    0x7D, //0111 1101  6
    0x07, //0000 0111  7
    0x7F, //0111 1111  8
    0x6F, //0110 1111  9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};


static void cd_delay ( void )
{
    //unsigned int ra;
    //for(ra=0;ra<100;ra++) dummy(ra);
}

static void clk_high ( void )
{
    PUT32(GPIOABASE+0x18,1<<(CLK+ 0));
}
static void clk_low ( void )
{
    PUT32(GPIOABASE+0x18,1<<(CLK+16));
}
static void dio_high ( void )
{
    PUT32(GPIOABASE+0x18,1<<(DIO+ 0));
}
static void dio_low ( void )
{
    PUT32(GPIOABASE+0x18,1<<(DIO+16));
}
static unsigned int dio_read ( void )
{
    return(GET32(GPIOABASE+0x10)&1<<DIO);
}
static void dio_input ( void )
{
    unsigned int ra;

    ra=GET32(GPIOABASE+0x00);
    ra&=~(3<<(DIO<<1));
    ra|= (0<<(DIO<<1));
    PUT32(GPIOABASE+0x00,ra);
}
static void dio_output ( void )
{
    unsigned int ra;

    ra=GET32(GPIOABASE+0x00);
    ra&=~(3<<(DIO<<1));
    ra|= (1<<(DIO<<1));
    PUT32(GPIOABASE+0x00,ra);
}

static void cd_start ( void )
{
    dio_low();
    cd_delay();
}

static void cd_stop ( void )
{
    clk_high();
    cd_delay();
    dio_high();
    cd_delay();
}

static unsigned int cd_byte ( unsigned int data )
{
    unsigned int rm;

    for(rm=0x01;rm<0x100;rm<<=1)
    {
        clk_low();
        cd_delay();
        if(data&rm) dio_high();
        else        dio_low();
        cd_delay();
        clk_high();
        cd_delay();
    }

    clk_low();
    cd_delay();
    dio_input();
    cd_delay();
    clk_high();
    cd_delay();
    rm=dio_read();
    clk_low();
    cd_delay();
    dio_output();
    return(rm);
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


    if(lasttim[0]==0) lasttim[0]=10;


    cd_start();
    cd_byte(0x40);
    cd_stop();

    cd_start();
    cd_byte(0xC0);
    cd_byte(led_table[lasttim[0]]);
//    cd_byte(led_table[lasttim[1]]|((colon&1)<<7));
    cd_byte(led_table[lasttim[1]]);
    cd_byte(led_table[lasttim[2]]);
    cd_byte(led_table[lasttim[3]]);
    cd_stop();
}

//static void clock_init ( void )
//{
    //unsigned int ra;

    //ra=GET32(RCC_CR);
    //ra|=(1<<16); //HSEON
    //PUT32(RCC_CR,ra);
    ////wait for HSERDY
    //while(1)
    //{
        //ra=GET32(RCC_CR);
        //if(ra&(1<<17)) break;
    //}
    ////switch to HSE
    //ra=GET32(RCC_CFGR);
    //ra&=~3;
    //ra|=1;
    //PUT32(RCC_CFGR,ra);
    ////wait for it
    //while(1)
    //{
        //ra=GET32(RCC_CFGR);
        //if((ra&3)==1) break;
    //}
    ////now using external clock.
    ////----------


    //if(1)
    //{
        ////should already be off
        //ra=GET32(RCC_CR);
        //ra&=(~(1<<24)); //PLLON
        //PUT32(RCC_CR,ra);
        ////wait for PLLRDY off
        //while(1)
        //{
            //ra=GET32(RCC_CR);
            //if((ra&(1<<25))==0) break;
        //}
        ////should already be zero
        //PUT32(RCC_CFGR2,0x00000000);
    //}

    ////external clock is 8MHz 8*6 = 48MHz.
    ////0100: PLL input clock x 6
    //ra=GET32(RCC_CFGR);
    //ra&=(~(0xF<<18));
    //ra|=(0x4<<18);
    //ra|=(1<<16); //PLLSRC HSE/PREDIV
    //PUT32(RCC_CFGR,ra);
    //ra=GET32(RCC_CR);
    //ra|=(1<<24); //PLLON
    //PUT32(RCC_CR,ra);
    ////wait for PLLRDY
    //while(1)
    //{
        //ra=GET32(RCC_CR);
        //if((ra&(1<<25))!=0) break;
    //}

    ////if we overclock the flash we can crash
    //ra=GET32(FLASH_ACR);
    //ra|=0x00000001;
    //PUT32(FLASH_ACR,ra);

    ////switch to PLL
    //ra=GET32(RCC_CFGR);
    //ra&=~3;
    //ra|=2;
    //PUT32(RCC_CFGR,ra);
    ////wait for it
    //while(1)
    //{
        //ra=GET32(RCC_CFGR);
        //if((ra&3)==2) break;
    //}
//}

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

    //clock_init();
    uart_init();

    ra=GET32(RCCBASE+0x14);
    ra|=1<<17; //enable port a
    PUT32(RCCBASE+0x14,ra);

    //moder
    ra=GET32(GPIOABASE+0x00);
    ra&=~(3<<(DIO<<1));
    ra|= (1<<(DIO<<1));
    ra&=~(3<<(CLK<<1));
    ra|= (1<<(CLK<<1));
    PUT32(GPIOABASE+0x00,ra);

    //PA0 DATA PA1 CLOCK
    //otyper
    ra=GET32(GPIOABASE+0x04);
    ra&=~(1<<DIO);
    ra&=~(1<<CLK);
    PUT32(GPIOABASE+0x04,ra);

    //pupdr
    ra=GET32(GPIOABASE+0x0C);
    ra&=~(3<<(DIO<<1));
    ra|= (1<<(DIO<<1)); //pull up
    PUT32(GPIOABASE+0x0C,ra);

    clk_high();
    cd_delay();
    dio_high();
    cd_delay();
    dio_low();
    cd_delay();
    dio_high();
    cd_delay();
    //known state?

        cd_start();
        cd_byte(0x8f); //bright
        //cd_byte(0x8e); 
        //cd_byte(0x8c); 
        //cd_byte(0x8b);
        //...
        //cd_byte(0x88); //dim
        cd_stop();





    timezone=4;
    if(GET32(0x20000D00)==0x12341234) timezone=5;
    PUT32(0x20000D00,0x12341234);

    lasttim[0]=1;
    lasttim[1]=1;
    lasttim[2]=2;
    lasttim[3]=3;



    cd_start();
    cd_byte(0x40);
    cd_stop();

    cd_start();
    cd_byte(0xC0);
    cd_byte(led_table[lasttim[0]]);
//    cd_byte(led_table[lasttim[1]]|((colon&1)<<7));
    cd_byte(led_table[lasttim[1]]);
    cd_byte(led_table[lasttim[2]]);
    cd_byte(led_table[lasttim[3]]);
    cd_stop();


    zhead=0;
    ztail=0;

    do_nmea();

    return(0);
}

/*

halt
flash write_image erase tm1637_clock_00/notmain.elf
reset

*/
