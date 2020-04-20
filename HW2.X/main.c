#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include"spi.h"
#include<math.h>

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

#define PI 3.1415
#define SINEPOINTS 50
#define TRIANGLEPOINTS 100

static volatile unsigned short Sinewaveform[SINEPOINTS];
static volatile unsigned short Trianglewaveform[TRIANGLEPOINTS];

void makesine();
void maketriangle();

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
    TRISBbits.TRISB4 = 1;
    
    
    initSPI();

    __builtin_enable_interrupts();

    unsigned char iA = 0;
    
    unsigned short vA = 0; // voltage value
    unsigned char cA = 0; // channel A
    unsigned short pA = 0;// printed message
    
    unsigned char iB = 0;
    
    unsigned short vB = 0; // voltage value
    unsigned char cB = 1; // channel B
    unsigned short pB = 0;// printed message
//    
    makesine();
    maketriangle();
    
    while (1) {
        // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the sysclk   24 megahertz
        vA = Sinewaveform[iA];
        pA = (cA << 15);
        pA = pA|(0b111<<12);
        pA = pA|vA;
        
        vB = Trianglewaveform[iB];
        pB = (cB << 15);
        pB = pB|(0b111<<12);
        pB = pB|vB;
//        
        
        LATAbits.LATA0 = 0;
        spi_io(pA>>8);
        spi_io(pA);
        LATAbits.LATA0 = 1;
        
        LATAbits.LATA0 = 0;
        spi_io(pB>>8);
        spi_io(pB);
        LATAbits.LATA0 = 1;
        
        iA++;
        if (iA == 50) {
            iA = 0;
        }
        iB++;
        if (iB == 100) {
            iB = 0;
        }
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 24000000/100){//100Hz
        }
    }
}  

void makesine(){
    int i = 0, center = 2048, A = 2048;
    for (i = 0; i < 50; ++i){
        Sinewaveform[i] = A*sin((2*PI/50)*i) + center;
    }
}

void maketriangle(){
    int i = 0, low = 0, high = 4096;
    for (i = 0; i < 100; ++i){
        if (i < 50){
        Trianglewaveform[i] = (high - low)*i/50;
        } else {
            Trianglewaveform[i] = 4096 - (high - low)*(i-49)/50;
        }
    }
}
