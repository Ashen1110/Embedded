#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void setGPIO(){
	FILE *p = fopen("/sys/class/gpio/export","w");
	fprintf(p,"%d",4);
	fclose(p);
}

void GPIO_IO(){
	FILE *p = fopen("/sys/class/gpio/gpio4/direction","w");
	fprintf(p,"out");
	fclose(p);

}

void GPIO_voltage(){
	FILE *p = fopen("/sys/class/gpio/gpio4/value","w");
	for (int i = 0; i < 100; i++) {
		if (fprintf(p,"%d",1) < 0) {
		    perror("Error writing to /sys/class/gpio/gpio4/value");
		    exit(1);
		}
		usleep(50000);
		fclose(p); //寫入閃的電壓
		FILE *p = fopen("/sys/class/gpio/gpio4/value","w");
		if (fprintf(p,"%d", 0) < 0) {
		    perror("Error writing to /sys/class/gpio/gpio4/value");
		    exit(1);
		}
		usleep(50000);
		fclose(p); //寫入滅的電壓
    }
    //fclose(p); 不可以最後才 close，會寫不進去
}

void GPIO_close(){
	FILE *p = fopen("/sys/class/gpio/unexport","w");
	fprintf(p,"%d",4);
	fclose(p);
}


int main(){
	setGPIO();
	GPIO_IO();
	GPIO_voltage();
	GPIO_close();
	return 0;
}
