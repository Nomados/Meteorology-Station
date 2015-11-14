#include "dht11.h"



void DELAY_US(int delay) // delay for MCU running on 48 MHz
{
	int i;
	for (i = 0; i < delay*8; i++)
	{
		asm("NOP");
	}
}


DHT11_ERROR_CODE_t read_DHT11(uint8_t * pData)
{
	//reset DHT11
	palSetPadMode(GPIOB, GPIOB_PIN2, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(GPIOB, GPIOB_PIN2);
	chThdSleepMilliseconds(20);

	palSetPad(GPIOB, GPIOB_PIN2);
	palSetPadMode(GPIOB, GPIOB_PIN2, PAL_MODE_INPUT);

	DHT11_ERROR_CODE_t errorCode = DHT11_OK;
	int i = 0;
	int j = 0;

	int timeout = TIMEOUT_VALUE;

	while(palReadPad(GPIOB, GPIOB_PIN2) == 1) /* hold on '1' */
	{
		if ((timeout)-- <= 0)
		{
			errorCode = DHT11_TIMEOUT;
			break;
		}
	};
	while(palReadPad(GPIOB, GPIOB_PIN2) == 0) /* hold on '0' */
	{
		if ((timeout)-- <= 0)
		{
			errorCode = DHT11_TIMEOUT;
			break;
		}
	};
	while(palReadPad(GPIOB, GPIOB_PIN2) == 1) /* hold on '1' */
	{
		if ((timeout)-- <= 0)
		{
			errorCode = DHT11_TIMEOUT;
			break;
		}
	};
	/* start sequence - end */
	chSysLock();
	/* read sequence */
	if (errorCode == DHT11_OK)
	{
		for(j=0; j<5; j++) // bytes
		{
			for(i=0; i < 8; i++)
			{
				timeout = TIMEOUT_VALUE;
				while(palReadPad(GPIOB, GPIOB_PIN2) == 0)
				{
					if ((timeout)-- <= 0)
					{
						errorCode = DHT11_TIMEOUT;
						break;
					}
				}; /* 50 us on 0 */

				if (palReadPad(GPIOB, GPIOB_PIN2) == 1)
				{
					DELAY_US(30);
				}

				pData[j] <<= 1;

				if(palReadPad(GPIOB, GPIOB_PIN2) == 1)
				{
					DELAY_US(40); /* wait 'till 70us */
					//gptPolledDelay(&GPTD3, US2ST(40));
					pData[j] |= 1;
				}
				else
				{
					pData[j] &= 0xfe;
				}
			}
		}
	}
	/* read sequence - end */
	chSysUnlock();
	/* checksum check */
	if (errorCode == DHT11_OK )
	{
		if ((pData[0] + pData[2]) != pData[4])
		{
			errorCode = DHT11_WRONG_CHCKSUM;
		}
		else
		{
			errorCode = DHT11_OK;
		}
	}

	return errorCode;
}

