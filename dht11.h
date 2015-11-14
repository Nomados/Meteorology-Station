/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DHT11_H
#define __DHT11_H

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include "hal.h"

/* Exported constants --------------------------------------------------------*/
//#define DHT11_OK 0
//#define DHT11_NO_CONN 1
//#define DHT11_CS_ERROR 2

/* Return codes */
typedef enum
{   DHT11_OK = 0,
    DHT11_TIMEOUT,
    DHT11_WRONG_CHCKSUM
} DHT11_ERROR_CODE_t;

#define TIMEOUT_VALUE       100000
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
DHT11_ERROR_CODE_t read_DHT11(uint8_t * pData);


#endif /* __DHT11_H */
