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


#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>
#include <ctype.h>
#include <type_traits>

using namespace std;
using namespace libconfig;


int I2C_ADDR;
int INVERT;
int DEVICE;
int LENGTH;
int START;

//std::string DIRECTION;
std::string  HUB;


//#define I2C_ADDR 0x38              //Адрес устройства
//#define INVERT 1                   //Необходимость инвертирования результата

                                     //Для PCF8574, нужно инвертировать при работе с релейным модулем.
int readwritebuffer(int mode, std::bitset<8> buffer){
char i2cbuffer[1];                   //Выделяем буфер для чтения
int i2cfd;                           //Создаем индификатор файла
char* c = &*HUB.begin();

i2cfd = open(c, O_RDWR);             //Открываем файл
if (i2cfd < 0) {                     //Проверка возможности открытия
	printf("Error opening device file: %s\n", strerror(errno));
    exit(0);// !!!                   //Выход из программы, если устройство не определено.
}
if (ioctl(i2cfd, I2C_SLAVE, I2C_ADDR) < 0) {       //Проверяем устройство на наличие
	printf("ioctl error: %s\n", strerror(errno));
    exit(0); //!!!                                 //Выход из программы, если устройство не определено.
}


	//Читаем значение
if(mode==0){
	read(i2cfd, i2cbuffer, 1);                                //Читаем состояние реле
	close (i2cfd);
	printf("DATA in buffer: 0x%02X\n", i2cbuffer[0]);         //Печатаем на экран
return i2cbuffer[0];                                          //Возвращаем значение
}
if(mode==1){
	i2cbuffer[0]=(int)(buffer.to_ulong());
	write(i2cfd,i2cbuffer, 1);                                //Пишем состояние реле
	close (i2cfd);
	printf("Writed data in buffer: 0x%02X\n", i2cbuffer[0]);  //Печатаем на экран	
	}	
}




void set(std::bitset<8> buffer,int relay, bool status){	   //Функция установки бита (состояние реле)
if (INVERT){	
	buffer.flip();                 //Необходимо проинвертировать результат.
	   							   // 0хFF -  все реле отключены, 0x00 - все реле включены.
	                               // После инвертирования получаем реальный статус реле 0x00 - отключено, 
	                               //0xFF - все включено
}	
buffer.set(relay,status);

if (INVERT){	
	buffer.flip();                 //Необходимо проинвертировать результат.
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

int readconfig(void){
    
  Config cfg;

  // Read the file. If there is an error, report it and exit.
  try
  {
    cfg.readFile("/etc/i2c_relay.cfg");
  }
  catch(const FileIOException &fioex)
  {
    std::cerr << "I/O error while reading file." << std::endl;
    return(EXIT_FAILURE);
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return(EXIT_FAILURE);
  }

  // Get the store name.
  try
  {
    string name = cfg.lookup("name");
    cout << "Store name: " << name << endl << endl;
  }
  catch(const SettingNotFoundException &nfex)
  {
    cerr << "No 'name' setting in configuration file." << endl;
  }

  const Setting& root = cfg.getRoot();

  // Output a list of all devices in the tree.
  try
  {
    const Setting &devices = root["devices"];
    int count = devices.getLength();
    
    cout << "____________________________CONFIGURATION MAP____________________________"<<endl;
    
    cout << setw(7) << left << "Device" << "  "
         << setw(10) << left << "Address" << "   "
         << setw(15) << left << "Start" << "  "
         << setw(15) << left << "Lenght" << "  "
         << setw(10) << left << "Inversion" << "  "
         //<< setw(10) << left << "Direction" << "  "
         << setw(10) << left << "Hub" << "  "         
         << endl;
    
    for(int i = 0; i < count; ++i)
    {
      const Setting &output = devices[i];

      // Only output the record if all of the expected fields are present.
      int device, address;
      int length;
      //string direction;
      int inversion;
      string hub;
      int start;
    
      if(!(output.lookupValue("device", device)
           && output.lookupValue("address", address)
           && output.lookupValue("start", start)
           && output.lookupValue("length", length)
           //&& output.lookupValue("direction", direction)           
           && output.lookupValue("inversion", inversion)
           && output.lookupValue("hub", hub) 
      ))
        continue;
    
      cout << setw(7) << left << device << "  "
           << setw(11) << left << address << "  "
           << setw(15) << left << start << "  "            
           << setw(15) << left << length << "  "         
           << setw(10) << left << inversion << "  "
           //<< setw(10) << left << direction << "  "
           << setw(10) << left << hub << "  "
           << endl;
    }  
    
    cout << "__________________________END CONFIGURATION MAP___________________________"<<endl;
    
    cout << endl;
    

  }
  catch(const SettingNotFoundException &nfex)
  {
    // Ignore.
  }    
    

    try
    {
    const Setting &devices = root["devices"];
    int count = devices.getLength();



    for(int i = 0; i < count; ++i)
    {
        const Setting &output = devices[i];

      // Only output the record if all of the expected fields are present.
        int device, address;
        int length;
        //string direction;
        int inversion;
        string hub;
        int start;
    if(!(output.lookupValue("device", device)
           && output.lookupValue("address", address)
           && output.lookupValue("start", start)           
           && output.lookupValue("length", length)
           //&& output.lookupValue("direction", direction)           
           && output.lookupValue("inversion", inversion)
           && output.lookupValue("hub", hub) 
    ))
        continue;

    cout << endl;

	if (device == DEVICE){
        DEVICE=device;
        I2C_ADDR=address;
        LENGTH=length;
        //DIRECTION=direction;
        INVERT=inversion;
        HUB=hub;
        START=start;
        cout << "______________________________DEVICE FOUND_______________________________" <<endl;
        cout <<  setw(10) << "N:" << i << endl;
        cout <<  setw(10) << "Device: " << DEVICE << endl;
        cout <<  setw(10) << "Adress:"<<I2C_ADDR<<endl;  
        cout <<  setw(10) << "Start:"<<START<<endl;        
        cout <<  setw(10) << "Lenght:"<<LENGTH<<endl;
        cout <<  setw(10) << "Invert:"<<INVERT<<endl;        
        cout <<  setw(10) << "Hub:"<< HUB<<endl;            
        cout << "_________________________________________________________________________"<<endl;
    break;
    }
 
    }    
  
    cout << endl;

    }

    catch(const SettingNotFoundException &nfex)
    {
    // Ignore.
    }  
    
    
    
    
    
   
}
									/****Main****/
int main(int argc, char **argv){


	//int pcfbuf;
	//int pcfbuf = readwritebuffer(0,0);        //Выделяем локальную переменную под прочитанный буфер микросхемы
	//std::bitset<8> buffer = pcfbuf;           //Выделение слова для чипа.
	int relay=1,status=0;                          //Порядковый номер реле, его статус
	                                          //Начальные данные.
    
    


 
    

    
 
if(argc <= 1) {

	printf("\n");
	printf("syntax : \n\t%s --help\n", argv[0]);
	printf("\t%s --device [number] --set [relay] [status]\n ", argv[0]);
	printf("\t%s --device [number] --reset\n ", argv[0]);	   
return 0;
};

if(!strcmp(argv[1], "--help")) {
    printf("You requested help message.\n");
    
    printf("\n");
	printf("syntax : \n\t%s --help\n", argv[0]);
	printf("\t%s --device [number] --set [relay] [status]\n ", argv[0]);
	printf("\t%s --device [number] --reset\n ", argv[0]);	
    
};


if(!strcmp(argv[1], "--device")){

if (argc <= 3 ){
printf("'--device' operation requires parameter [device number] and operation [--set] or [--reset].\n");
exit;
} 
};

if (((argc > 6)|| (argc <= 5))&& (argc > 4)){
 if  ((!strcmp(argv[1], "--device"))&&(!strcmp(argv[3], "--set")))  {  
    printf("'--set' operation requires two integer parameters.\n");
    exit;
}; 
};

if (argc == 6){
 if  ((!strcmp(argv[1], "--device"))&&(!strcmp(argv[3], "--set")))  {  
    if(sscanf(argv[4], "%d", &relay) != 1 || sscanf(argv[5], "%d", &status) != 1) {
    printf("'--set' operation requires two integer parameters.\n");
    exit;
    } else {
    cout << "accept command"<<endl;
    if(sscanf(argv[2], "%d", &DEVICE) != 1 ){
            printf("'--set' operation requires 1 integer parameter.\n");
            exit;
    }

    readconfig();
    
    if (relay<START|relay>START+LENGTH) {printf("Wrong value. Index of relay can be only %d-%d\n",START, START+LENGTH,"."); return 0;}
    int pcfbuf = readwritebuffer(0,0);
    std::bitset<8> buffer = pcfbuf;   
    
	set(buffer,relay-1,status);             //запрос
    
    } 
    
}; 
};


if ((argc >= 4) ){
    if(!strcmp(argv[3], "--reset")){
        if(argc > 4){
        printf("'--reset' operation no requires parameters.\n");
        exit;
    } 
    else {
        if(sscanf(argv[2], "%d", &DEVICE) != 1 ){
            printf("'--set' operation requires 1 integer parameter.\n");
            exit;
    } else {
        
    
    readconfig();

    int pcfbuf = readwritebuffer(0,0);
    std::bitset<8> buffer = pcfbuf;     
    reset(buffer);  
    } 

    
    }
    } 
}; 



if(not(!strcmp(argv[1], "--device")||!strcmp(argv[1], "--help"))){
	printf("Unknown parameter: '%s'. Type %s --help for help.\n", argv[1], argv[0]);
    exit;
};
if (argc>3){
if(not(!strcmp(argv[3], "--reset")||!strcmp(argv[3], "--set"))){
	printf("Unknown parameter: '%s'. Type %s --help for help.\n", argv[3], argv[0]);
    exit;
}
};


return 0;    
}

