# Hello! This is i2c_relay program's page.

FAQ:

-How to get your program?

-Very easy:

0) git clone https://github.com/EthicalEarth/i2c_relay
1) cd i2c_relay
2) run ./configure
3) run make
4) cd src/ and you can see i2c_relay binary file.

-How to control relay module?

-Use sytax: ./i2c_relay --set [number of (gpio 1-8) relay] [status 0 or 1]

-You can reset all relays: ./i2c_relay --reset
(In my program first relay connect to fisrt GPIO of PCF8574). 

You can make adapter via PCB [https://github.com/EthicalEarth/i2c_relay_PCB_BOARD] (See [https://github.com/EthicalEarth/i2c_relay_PCB_BOARD/blob/master/PCB1.pdf])

-Address of my chip is not 0x38. Program not work.

0) Get address by cmdline: i2cdetect -y 1
1) Open file main.cc and change address.
2) Compile programm.
Same too if your i2c adapter is not /dev/i2c-1, just change it in function readwritebuffer.
