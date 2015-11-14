/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"
#include "BarometerData.h"
#include "dht11.h"
#include <string.h>
#include "chprintf.h"




BaroData bd; // baro data structure
static uint8_t dht_buffer[5]; // DHT buffer


I2CConfig i2cconfig =
{
		0x00901D2B,
		0,
		0
};



SerialConfig sc =
{
		19200,
		0,
		0,
		0

};



/* itoa: */
void itoa(int n, char s[])
{

	/* reverse the string */
	void reverse(char s[])
	{
		int i, j;
		char c;

		for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
			c = s[i];
			s[i] = s[j];
			s[j] = c;
		}
	}

	int i, sign;

	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}





/*
 * OLED thread, times are in milliseconds.
 */
static WORKING_AREA(waOLED, 512);
static msg_t Oled(void *arg)
{
	(void)arg;
	chRegSetThreadName("oled");


	gfxInit();
	font_t font = gdispOpenFont("UI1");
	font_t font1 = gdispOpenFont("UI2");
	gdispSetClip(0, 0, gdispGetWidth(), gdispGetHeight());
	uint8_t x = 7, y = 15;

	while (true)
	{
		char ch[10];
		int temperature = 0;
		int pressure = 0;
		readBarometerData(&bd, 1);
		temperature = (int)bd.TEMP;
		pressure = (int)(bd.PRESSURE * 0.0075);
		int alt = (int)getAltitude(&bd);

		gdispClear(Black);


		gdispDrawString(x, y - 12, "Weather ", font1, White);

		gdispDrawString(x, y, "temperature: ", font, White);
		gdispDrawString(x, y + 12, "pressure: ", font, White);
		gdispDrawString(x, y + 24, "altitude: ", font, White);
		gdispDrawString(x, y + 36, "humidity: ", font, White);



		itoa(temperature, ch);
		gdispDrawString(x+80, y, ch, font, White);

		itoa(pressure, ch);
		gdispDrawString(x+80, y+12, ch, font, White);

		itoa(alt, ch);
		gdispDrawString(x+80, y+24, ch, font, White);



		if (read_DHT11(&dht_buffer[0]) == DHT11_OK)
		{
			itoa(dht_buffer[0], ch);
			gdispDrawString(x+80, y+36, ch, font, White);
			chprintf((BaseSequentialStream*)&SD2, "Temperature: %02dC  pressure: %d kPA  humidity: %02d %%\n\r", temperature, pressure/100, dht_buffer[0]);
		}
		else
		{
			gdispDrawString(x+80, y+36, "Err", font, White);
			chprintf((BaseSequentialStream*)&SD2, "Temperature: %02dC  pressure=%dkPA can't read DHT11 sensor\n\r", temperature, pressure/100);
		}




		gdispGFlush(gdispGetDisplay(0));
		chThdSleepMilliseconds(1000);
	}

	return 0;

}



/*
 * Application entry point.
 */
int main(void)
{
	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();


	/*
	 * Starting the I2C driver 2 for bmp180.
	 */
	i2cStart(&I2CD2, &i2cconfig);
	initialize_bmp180(&bd, 3);
	/*
	 * Activates the serial driver 1 using the driver default configuration.
	 * PA9 and PA10 are routed to USART1 and PA2 and PA3 to USART2
	 */
	sdStart(&SD1, &sc);
	sdStart(&SD2, &sc);

	/*
	 * Creates the threads.
	 */
	//chThdCreateStatic(waDHT11, sizeof(waDHT11), NORMALPRIO - 4, DHT11_Reader, NULL);
	chThdCreateStatic(waOLED, sizeof(waOLED), NORMALPRIO + 1, Oled, NULL);


	while (TRUE)
	{
		chThdSleepMilliseconds(1000);
	}
}
