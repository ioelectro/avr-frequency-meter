PROJECT		=	project
DEVICE 		=	atmega8
FREQ   		=	8000000UL

CC     		=	avr-gcc
OC     		=	avr-objcopy
SIZE    	=	avr-size

FLAGS   	=	-Wall -Os
CCC	    	=	$(CC) $(FLAGS) -mmcu=$(DEVICE) -DF_CPU=$(FREQ)


default : compile

compile :
	$(CCC) -c main.c -o main.o
	$(CCC) -o $(PROJECT).elf main.o
	$(OC) -j .text -j .data -O ihex $(PROJECT).elf $(PROJECT).hex
	$(SIZE) --format=avr --mcu=$(DEVICE) $(PROJECT).elf


clean:
	rm *.o *.elf *.hex