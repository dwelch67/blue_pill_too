
void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

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
    unsigned int rx;

    clock_init();

    ra=GET32(RCCBASE+0x14);
    ra|=1<<17; //enable port a
    PUT32(RCCBASE+0x14,ra);

    //moder
    ra=GET32(GPIOABASE+0x00);
    ra&=~(3<<(4<<1));
    ra|= (1<<(4<<1));
    PUT32(GPIOABASE+0x00,ra);
    //OTYPER
    ra=GET32(GPIOABASE+0x04);
    ra&=~(1<<4); //PA4
    PUT32(GPIOABASE+0x04,ra);

    PUT32(STK_CSR,4);
    PUT32(STK_RVR,1000000-1);
    PUT32(STK_CVR,0x00000000);
    PUT32(STK_CSR,5);

    for(rx=0;;rx++)
    {
        PUT32(GPIOABASE+0x18, (1<<(4+ 0)) );
        delay(96);
        PUT32(GPIOABASE+0x18, (1<<(4+16)) );
        delay(96);
    }
    return(0);
}
