#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define CLOCK_DELAY 5
#define DEFAULT_BRIGHTNESS {1,0,0,0,1,0,0,1} //0x88+1
#define AUTO_MODE {0,1,0,0,0,0,0,0} //0x40  (Command: auto increment mode)
#define FIXED_MODE {0,1,0,0,0,1,0,0} //0x44 (Command: fixed position mode)
#define START_ADDR {1,1,0,0,0,0,0,0}  //0xC0 (Command to set the starting position at first digit)

int Digits[16][8] = {
    //H,G,F,E,D,C,B,A
     {0,0,1,1,1,1,1,1}, //0
     {0,0,0,0,0,1,1,0}, //1
     {0,1,0,1,1,0,1,1}, //2
     {0,1,0,0,1,1,1,1}, //3
     {0,1,1,0,0,1,1,0}, //4
     {0,1,1,0,1,1,0,1}, //5
     {0,1,1,1,1,1,0,1}, //6
     {0,0,0,0,0,1,1,1}, //7
     {0,1,1,1,1,1,1,1}, //8
     {0,1,1,0,1,1,1,1}, //9
    /*
     {0,1,1,1,0,1,1,1}, //A
     {0,1,1,1,1,1,0,0}, //b
     {0,0,1,1,1,0,0,1}, //C
     {0,1,0,1,1,1,1,0}, //d
     {0,1,1,1,1,0,0,1}, //E
     {0,1,1,1,0,0,0,1}  //F
     */
};

FILE* clk = NULL;
FILE* dio = NULL;

void writeDIO(int v) {
    /*Write the GPIO file*/
    dio = fopen("sys/class/gpio/gpio24/value", "w");
    fprintf(dio,"%d", v);
    fclose(dio);
}
void writeCLK(int v) {
    /*Write the GPIO file*/
    clk = fopen("/sys/class/gpio/gpio25/value", "w");
    fprintf(clk, "%d", v);
    fclose(clk);
}

void initialize(){  
    /*Setup GPIO files*/
    clk = fopen("/sys/class/gpio/export", "w");
    fprintf(clk, "%d", 25);
    fclose(clk);
    
    dio = fopen("sys/class/gpio/export", "w");
    fprintf(dio, "%d", 24);
    fclose(dio);
    
    clk = fopen("/sys/class/gpio/gpio25/direction", "w");
    fprintf(clk, "out");
    fclose(clk);
    
    dio = fopen("sys/class/gpio/gpio24/direction", "w");
    fprintf(dio, "out");
    fclose(dio);
    
    writeCLK(0);
    writeDIO(0);
    
}

void end() {
    /*Close GPIO files*/
    clk = fopen("/sys/class/gpio/unexport", "w");
    fprintf(clk, "%d", 25);
    fclose(clk);
    
    dio = fopen("sys/class/gpio/unexport", "w");
    fprintf(dio, "%d", 24);
    fclose(dio);
}

void startWrite(){
    //send start signal to TM1637
    writeCLK(1);    //CLK high
    writeDIO(1);    //DIO high
    usleep(CLOCK_DELAY);
    writeDIO(0);    //DIO low
}

void stopWrite() {
    //send stop signal to TM1637
    writeCLK(0);    //CLK low
    usleep(CLOCK_DELAY);
    writeDIO(0);    //DIO low
    usleep(CLOCK_DELAY);
    writeCLK(1);    //CLK high
    usleep(CLOCK_DELAY);
    writeDIO(1);    //DIO high
}

void writeByte(int Byte[8]) {
    for (int i = 7; i >= 0; i--) {     
    	 //CLK low   
        writeCLK(0);   
        
        //Set data bit                                 //H,G,F,E,D,C,B,A                    
        if (Byte[i] == 1)       //Example: 3: {0,1,0,0,1,1,1,1}
            writeDIO(1);
        else 
            writeDIO(0);
        usleep(CLOCK_DELAY);
        writeCLK(1);    //CLK high
        usleep(CLOCK_DELAY);
    }
    /*Waiting for ACK */
    writeCLK(0);    //CLK low
    usleep(CLOCK_DELAY);
    writeCLK(1);    //CLK high
    usleep(CLOCK_DELAY);
    writeCLK(0);    //CLK low
    usleep(CLOCK_DELAY);
}

void showNum(int num) {
    if (num >= 0 && num <= 9999) {
        int mode[8] = AUTO_MODE;
        int addr[8] = START_ADDR;
        int bright[8] = DEFAULT_BRIGHTNESS;

        startWrite(); 
        writeByte(mode); //mode = {0,1,0,0,0,0,0,0} (continuous enter)
        stopWrite();

        startWrite();
        writeByte(addr); //addr = {1,1,0,0,0,0,0,0} (0xC0 start at left)
        writeByte(Digits[num / 1000]);
        writeByte(Digits[num / 100 % 10]);
        writeByte(Digits[num / 10 % 10]);
        writeByte(Digits[num % 10]);
        stopWrite();

        startWrite();
        writeByte(bright); //bright = {1,0,0,0,1,0,0,1} (0x88+1 Set brightness)   
        stopWrite();
    }
    else {
        printf("Error: Exceed Limit\n");
    }
}

void clear() {
	int mode[8] = AUTO_MODE;
        int addr[8] = START_ADDR;
        int bright[8] = DEFAULT_BRIGHTNESS;

        startWrite(); 
        writeByte(mode); //mode = {0,1,0,0,0,0,0,0} (continuous enter)
        stopWrite();

        startWrite();
        writeByte(addr); //addr = {1,1,0,0,0,0,0,0} (0xC0 start at left)
        writeByte(Digits[0]);
        writeByte(Digits[0]);
        writeByte(Digits[0]);
        writeByte(Digits[0]);
        stopWrite();

        startWrite();
        writeByte(bright); //bright = {1,0,0,0,1,0,0,1} (0x88+1 Set brightness)   
        stopWrite();
}
int main(void)
{
    initialize();
    //showNum(9218);
    //sleep(5);
    
    int a = 1000;
    while(a>0){
    	showNum(a);
    	sleep(1);
    	a-=100;
    }

    clear();
   
    end();
    exit(0);
}
