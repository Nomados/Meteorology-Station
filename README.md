# Meteorology-Station
This device is based on stm32F051 MCU and features measurements of 
temperature, humidity and pressure. It uses sh1106 1.3 OLED display to 
show data and is capable to transfer data via two UARTs if you want to 
organize radio communication. It also has Eagle PCB layout and ready to 
use gerbers (SeedStudio format).

This application uses Chibios/RT RTOS (2.6.x) and uGFX graphics engine.
You should download this software. Place Chibios and uGFX in ../ folder 
of makefile's location. 

For more details please visit:
http://mcudu.blogspot.kr/2015/11/meteorology-station.html
