# Hello! This is i2c_relay program's page.

﻿# NEW

    Now program support configuration file.

﻿# FAQ:

- How to get your program?

- Very easy:

0) git clone -b v2.0 https://github.com/EthicalEarth/i2c_relay
1) cd i2c_relay
2) mkdir build / cd build
2) run cmake ..
3) run make
4) cd build/ and you can see i2c_relay binary file.

- How to control relay module?

- Use sytax: ./i2c_relay --device [device number] --set [number of (gpio Start-Length (min 0, max 7)) relay] [status 0 or 1]

- You can reset all relays: ./i2c_relay --device [device number] --reset
(In my program first relay connect to GPIO0  of PCF8574). 

You can make adapter via PCB [https://github.com/EthicalEarth/i2c_relay_PCB_BOARD] (See [https://github.com/EthicalEarth/i2c_relay_PCB_BOARD/blob/master/PDF_NEXT_RE_FOR_PRINT.pdf])

- Address of my chip is not 0x38. Program not work.

0) Get address by cmdline: i2cdetect -y 1
1) Change configuration file.
2) Check bus of your i2c adapter (/dev/i2c-X).




﻿# NEW

    Now program support configuration file.
Configuration file stored in /etc/i2c_relay.cfg
    The program allows you to configure the i2с data bus through
the configuration file.
    This means that if you want to use devices with the same address,
you can connect them to different buses and write them to
the configuration file.
    All the program needs to know is the device number,
it retrieves the rest of the data from the configuration file.
    You do not have to compile the program with a strictly writed address
and access path to the bus /dev/i2c. 
    You also do not have to give the program "unforgettable" addresses
to control a specific device.

﻿# IMPORTANT

The configuration file contains the following required fields:

devices = (   { device  = 01;
                address = 0x38;
                start = 0;
                length  = 4;
                inversion=1;
                hub = "/dev/i2c-1"; },
              { device  = 02;
                address = 0x38;
                start = 0;
                length  = 8;
                inversion=1;
                hub = "/dev/i2c-1"; }
          );
          
0) devices - Mandatory field denoting a group of devices.
1) device - Unique device number. This is where you turn to a specific port expander.
2) address - The unique address of the port expander device. You can find out a unique number by connecting the device to the bus and typing the command: i2cdetect -y 1 (1 - number of bus, /dev/i2c-1).
3) start - The starting GPIO number that is allowed to be controlled by this program.
4) length - The maximum number of exits allowed for program management.
Since the program works with PCF8574 and this device is a 3-bit port extender, the maximum number is 8-1=7.
You can specify from 0 to 7. The program will prohibit working with outputs, the ordinal number of which is greater than that recorded in the configuration file.
5) inversion - Allows you to invert the value of the buffer programmatically for the user in order to be compatible with various relay modules that are activated 1 or 0.
6) hub - path to the i2c bus file of the Linux device pseudo filesystem.
