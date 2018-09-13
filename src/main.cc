/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cc
 * Copyright (C) 2018 Andrey Ivanov <EthicalTerminal@gmail.com>
 * 
 * I2C_Relay is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * I2C_Relay is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <bitset>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define I2C_ADDR 0x38           //Адрес устройства
#define INVERT 1                //Необходимость инвертирования результата
                                //Для PCF8574, нужно инвертировать при работе с релейным модулем.
int readwritebuffer(int mode, std::bitset<8> buffer){
char i2cbuffer[1];          //Выделяем буфер для чтения
int i2cfd;                  //Создаем индификатор файла

i2cfd = open("/dev/i2c-1", O_RDWR); //Открываем файл
if (i2cfd < 0) {                    //Проверка возможности открытия
	printf("Error opening file: %s\n", strerror(errno));
		//return 1;
}
if (ioctl(i2cfd, I2C_SLAVE, I2C_ADDR) < 0) {  //Проверяем устройство на наличие
	printf("ioctl error: %s\n", strerror(errno));
		//return 1;
}
	//Читаем значение
if(mode==0){
	read(i2cfd, i2cbuffer, 1);  //Читаем состояние реле
	close (i2cfd);
	printf("DATA in buffer: 0x%02X\n", i2cbuffer[0]);         //Печатаем на экран
return i2cbuffer[0];                      //Возвращаем значение
}
if(mode==1){
	i2cbuffer[0]=(int)(buffer.to_ulong());
	write(i2cfd,i2cbuffer, 1);  //Пишем состояние реле
	close (i2cfd);
	printf("Writed data in buffer: 0x%02X\n", i2cbuffer[0]);         //Печатаем на экран	
	}	
}




void set(std::bitset<8> buffer,int relay, bool status){	   //Функция установки бита (состояние реле)
if (INVERT){	
	buffer.flip();                     //Необходимо проинвертировать результат.
	   							   // 0хFF -  все реле отключены, 0x00 - все реле включены.
	                               // После инвертирования получаем реальный статус реле 0x00 - отключено, 
	                               //0xFF - все включено
}	
buffer.set(relay,status);

if (INVERT){	
	buffer.flip();                  //Необходимо проинвертировать результат.
	   							   // 0хFF -  все реле отключены, 0x00 - все реле включены.
	                               // После инвертирования получаем реальный статус реле 0x00 - отключено, 
	                               //0xFF - все включено
}

readwritebuffer(1,buffer);	
}

void reset(std::bitset<8> buffer){
if (INVERT){	
	buffer.flip();                 //Необходимо проинвертировать результат.
	   							   // 0хFF -  все реле отключены, 0x00 - все реле включены.
	                               // После инвертирования получаем реальный статус реле 0x00 - отключено, 
	                               //0xFF - все включено
}		
buffer.reset();//Функция сброса всех битов релейного модуля
if (INVERT){	
	buffer.flip();                 //Необходимо проинвертировать результат.
	   							   // 0хFF -  все реле отключены, 0x00 - все реле включены.
	                               // После инвертирования получаем реальный статус реле 0x00 - отключено, 
	                               //0xFF - все включено
}	

readwritebuffer(1,buffer);	
}


int main(int argc, char **argv){

int pcfbuf = readwritebuffer(0,0);      //Выделяем локальную переменную под прочитанный буфер микросхемы
std::bitset<8> buffer = pcfbuf; //Выделение слова для чипа.
	   
int relay=1,status=0;           //Порядковый номер реле, его статус
	                               //Начальные данные.
if(argc <= 1) {

	printf("\n");
	printf("syntax : \n\t%s --help\n", argv[0]);
	printf("\t%s --set [relay] [status]\n ", argv[0]);
	printf("\t%s --reset\n ", argv[0]);	   
return 0;
   }

if(!strcmp(argv[1], "--help")) {
    printf("You requested help message.\n");
};// else 

if(!strcmp(argv[1], "--set")) {
    if(argc <=3|argc >=5) {
         printf("'--set' operation requires two parameters.\n");
    } else {
            if(sscanf(argv[2], "%d", &relay) != 1 || sscanf(argv[3], "%d", &status) != 1) {
            	printf("'--set' operation requires two integer parameters.\n");
            } else {
				if (relay<1|relay>8) {printf("Wrong value. Index of relay can be only 1-8.\n");return 0;}
				               
				set(buffer,relay-1,status);//запрос
								
            }
        }
};


if(!strcmp(argv[1], "--reset")) {
      if(argc >= 3) {
         printf("'--reset' operation not requires parameters.\n"); 
         } else {
         reset(buffer); //запрос  
         }
};
	
if(not(!strcmp(argv[1], "--set")|!strcmp(argv[1], "--reset")|!strcmp(argv[1], "--help"))){
    printf("Unknown parameter: '%s'. Type %s --help for help.\n", argv[1], argv[0]);
   	};
}



