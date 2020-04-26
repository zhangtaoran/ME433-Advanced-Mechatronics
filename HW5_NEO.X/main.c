#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ws2812b.h"
#include "ssd1306.h"
#include <stdio.h>

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 1;
    i2c_master_setup();
    ssd1306_setup();
    
    ws2812b_setup();

    __builtin_enable_interrupts();
    
    wsColor color1[5];
    color1[0] = HSBtoRGB(60, 1, 1);
    color1[1] = HSBtoRGB(120, 1, 1);
    color1[2] = HSBtoRGB(180, 1, 1);
    color1[3] = HSBtoRGB(240, 1, 1);
    color1[4] = HSBtoRGB(300, 1, 1);
    
    wsColor color2[5];
    color2[0] = HSBtoRGB(120, 1, 1);
    color2[1] = HSBtoRGB(180, 1, 1);
    color2[2] = HSBtoRGB(240, 1, 1);
    color2[3] = HSBtoRGB(300, 1, 1);
    color2[4] = HSBtoRGB(360, 1, 1);
    
    wsColor color3[5];
    color3[0] = HSBtoRGB(180, 1, 1);
    color3[1] = HSBtoRGB(240, 1, 1);
    color3[2] = HSBtoRGB(300, 1, 1);
    color3[3] = HSBtoRGB(360, 1, 1);
    color3[4] = HSBtoRGB(60, 1, 1);
    
    wsColor color4[5];
    color4[0] = HSBtoRGB(240, 1, 1);
    color4[1] = HSBtoRGB(300, 1, 1);
    color4[2] = HSBtoRGB(360, 1, 1);
    color4[3] = HSBtoRGB(60, 1, 1);
    color4[4] = HSBtoRGB(120, 1, 1);
    
    wsColor color5[5];
    color5[0] = HSBtoRGB(300, 1, 1);
    color5[1] = HSBtoRGB(360, 1, 1);
    color5[2] = HSBtoRGB(60, 1, 1);
    color5[3] = HSBtoRGB(120, 1, 1);
    color5[4] = HSBtoRGB(180, 1, 1);
    
    wsColor color6[5];
    color6[0] = HSBtoRGB(360, 1, 1);
    color6[1] = HSBtoRGB(60, 1, 1);
    color6[2] = HSBtoRGB(120, 1, 1);
    color6[3] = HSBtoRGB(180, 1, 1);
    color6[4] = HSBtoRGB(240, 1, 1);
    
//    color[5] = HSBtoRGB(360, 1, 0.1);
    
//    wsColor color[1];
//    color[0] = HSBtoRGB(120, 1, 0);
    
    char m[50];
    char n[50];
    char o[50];
    int i = 0; int j = 0; int k = 0;
    i = color1[0].r;
    j = color1[0].g;
    k = color1[0].b;
    sprintf(m,"r=%d",i);
    sprintf(n,"g=%d",j);
    sprintf(o,"b=%d",k);
    drawsentence(15,0,m);
    drawsentence(15,8,n);
    drawsentence(15,16,o);
    ssd1306_update();

    while (1) {
        // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the sysclk   24 megahertz

        ws2812b_setColor(color1,5);
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 24000000/20) {}
        ws2812b_setColor(color2,5);
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 24000000/20) {}
        ws2812b_setColor(color3,5);
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 24000000/20) {}
        ws2812b_setColor(color4,5);
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 24000000/20) {}
        ws2812b_setColor(color5,5);
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 24000000/20) {}
        ws2812b_setColor(color6,5);
    }
}  