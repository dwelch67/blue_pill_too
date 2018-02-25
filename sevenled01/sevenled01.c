
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

#define LED23 ((1<<(7+16))|(1<<(6+0))|(1<<(5+0))|(1<<(4+16))|(1<<(3+16))|(1<<(2+16))|(1<<(1+0)))
#define LED22 ((1<<(7+16))|(1<<(6+0))|(1<<(5+0))|(1<<(4+16))|(1<<(3+16))|(1<<(2+0))|(1<<(1+16)))
#define LED21 ((1<<(7+16))|(1<<(6+0))|(1<<(5+0))|(1<<(4+16))|(1<<(3+0))|(1<<(2+16))|(1<<(1+16)))
#define LED20 ((1<<(7+16))|(1<<(6+0))|(1<<(5+0))|(1<<(4+0))|(1<<(3+16))|(1<<(2+16))|(1<<(1+16)))

#define LED13 ((1<<(7+0))|(1<<(6+16))|(1<<(5+0))|(1<<(4+16))|(1<<(3+16))|(1<<(2+16))|(1<<(1+0)))
#define LED12 ((1<<(7+0))|(1<<(6+16))|(1<<(5+0))|(1<<(4+16))|(1<<(3+16))|(1<<(2+0))|(1<<(1+16)))
#define LED11 ((1<<(7+0))|(1<<(6+16))|(1<<(5+0))|(1<<(4+16))|(1<<(3+0))|(1<<(2+16))|(1<<(1+16)))
#define LED10 ((1<<(7+0))|(1<<(6+16))|(1<<(5+0))|(1<<(4+0))|(1<<(3+16))|(1<<(2+16))|(1<<(1+16)))

#define LED03 ((1<<(7+0))|(1<<(6+0))|(1<<(5+16))|(1<<(4+16))|(1<<(3+16))|(1<<(2+16))|(1<<(1+0)))
#define LED02 ((1<<(7+0))|(1<<(6+0))|(1<<(5+16))|(1<<(4+16))|(1<<(3+16))|(1<<(2+0))|(1<<(1+16)))
#define LED01 ((1<<(7+0))|(1<<(6+0))|(1<<(5+16))|(1<<(4+16))|(1<<(3+0))|(1<<(2+16))|(1<<(1+16)))
#define LED00 ((1<<(7+0))|(1<<(6+0))|(1<<(5+16))|(1<<(4+0))|(1<<(3+16))|(1<<(2+16))|(1<<(1+16)))

//[y][x]
static const unsigned int ledout[3][4]=
{
    {LED03,LED02,LED01,LED00},
    {LED13,LED12,LED11,LED10},
    {LED23,LED22,LED21,LED20},
};

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
            PUT32(GPIOABASE+0x18,ledout[ledy][ledx]);
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
            if(rb&(1<<ledx)) break;
            PUT32(GPIOABASE+0x18,ALLOFF);
            break;
        }
        case 20:
        {
            PUT32(GPIOABASE+0x18,ALLOFF);
            break;
        }
        case 100:
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


int notmain ( void )
{
    unsigned int ra;

    clock_init();

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

    lasttim[0]=1;
    lasttim[1]=1;
    lasttim[2]=2;
    lasttim[3]=3;

    PUT32(STK_CSR,7);

    for(ra=0;;ra++)
    {
        DOWFI();
        if(ra==10000)
        {
            ra=0;
            lasttim[3]++;
            if(lasttim[3]>9)
            {
                lasttim[3]=0;
                lasttim[2]++;
                if(lasttim[2]>5)
                {
                    lasttim[2]=0;
                    lasttim[1]++;
                    if(lasttim[0])
                    {
                        if(lasttim[1]>2)
                        {
                            lasttim[0]=0;
                            lasttim[1]=1;
                        }
                    }
                    else
                    {
                        if(lasttim[1]>9)
                        {
                            lasttim[0]=1;
                            lasttim[1]=0;
                        }
                    }
                }
            }
        }
    }
    return(0);
}

/*

halt
flash write_image erase sevenled01/sevenled01.elf
reset


*/
