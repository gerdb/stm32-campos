
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PRINTF_H
#define __PRINTF_H


/* Includes ------------------------------------------------------------------*/
#include "usartl1.h"


/* Function prototypes -------------------------------------------------------*/
int my_printf(const char *format, ...);
int sprintf(char *out, const char *format, ...);


#endif /* __PRINTF_H */
