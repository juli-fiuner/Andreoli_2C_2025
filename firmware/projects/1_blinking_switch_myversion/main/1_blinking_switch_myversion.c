/*! @mainpage Proyecto 1, ej 2. Versión modificada de 'blinking_switch'
 *
 * \section genDesc General Description
 *
 * Este código hace titilar los LED 1 y 2 al presionar las teclas 1 y 2 correspondientemente; 
 * si se presionan las teclas 1 y 2 juntas, titila el LED 3.
 *
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 15/09/2025 | Document creation		                         |
 *
 * @author Andreoli Aguilar, Julieta
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
/** @def CONFIG_BLINK_PERIOD 
 * @brief constante usada para marcar el ritmo del blinking
 */
#define CONFIG_BLINK_PERIOD 500
/*==================[internal data definition]===============================*/
/** @def teclas  
 * @brief Variable que almacena qué tecla es presionada
 */
uint8_t teclas;
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){

	LedsInit();
	SwitchesInit();
	LedOff(LED_1);
	LedOff(LED_2);
	LedOff(LED_3);
    while(1)    {
    	teclas  = SwitchesRead();
			switch(teclas){
    		case SWITCH_1:
				LedToggle(LED_1);	
    		break;
    		case SWITCH_2:
    			LedToggle(LED_2);
    		break;
			case (SWITCH_1 | SWITCH_2):
				LedToggle(LED_3);
			break;
			vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS); //probar eliminando esta linea, creo que es innecesaria y no hace nada :P

    	}
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}
