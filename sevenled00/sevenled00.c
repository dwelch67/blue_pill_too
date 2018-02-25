
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

volatile unsigned int counter;
volatile unsigned int icount;
void systick_handler ( void )
{
    switch(counter++)
    {
        //PUT32(GPIOABASE+0x18,   (counter&0xF) | (((~counter)&0xF)<<16) );
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
    PUT32(STK_RVR,1000000-1);
    PUT32(STK_CVR,0x00000000);
    PUT32(STK_CSR,5);

    while(1)
    {
        PUT32(GPIOABASE+0x18,ALLON);
        delay(8);
        PUT32(GPIOABASE+0x18,ALLOFF);
        delay(16);
    }

    return(0);
}

/*

halt
flash write_image erase sevenled00/sevenled00.elf
reset


*/
