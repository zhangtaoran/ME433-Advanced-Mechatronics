#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ssd1306.h"
#include "font.h"
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

void drawletter(int x, int y, char letter);
void drawsentence(int x, int y, char* s);

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
    LATAbits.LATA4 = 0;
    
    i2c_master_setup();
    ssd1306_setup();

    __builtin_enable_interrupts();
    
    char m[50];
    char n[50];
    char h[50];
    char c[50];
    int i = 0;
    sprintf(m,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    sprintf(n,"01234567890123456789012345");
    
    
//    drawletter(20,10,'3');
//    ssd1306_update();
    
    drawsentence(0,0,m);
    ssd1306_update();
    drawsentence(0,8,n);
    ssd1306_update();

    float t;
    
    while (1) {
        // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the sysclk   24 megahertz
        
        sprintf(h,"Hello World! i = %d", i);
        drawsentence(10,16,h);
        ssd1306_update();
        i++;
        
        _CP0_SET_COUNT(0);
        ssd1306_update();
        t = _CP0_GET_COUNT();
        t = 24000000/t;
        sprintf(c,"FPS=%.2f", t);
        drawsentence(20,24,c);
        ssd1306_update();
        
        
    }
}  

void drawletter(int x, int y, char letter){
    int i,j;
    for(i=0; i < 5; i++){
        for(j=0; j < 8; j++){
           if (((ASCII[letter-0x20][i] >> j) & 1 )== 1){
                ssd1306_drawPixel(i+x, j+y, 1);
           } else {
                ssd1306_drawPixel(i+x, j+y, 0); 
           }
        }
    }
}

void drawsentence(int x, int y, char* m){
    int s=0;
    while (m[s]!=0) {
        drawletter(x+5*s,y,m[s]);
        s++;
    }
}