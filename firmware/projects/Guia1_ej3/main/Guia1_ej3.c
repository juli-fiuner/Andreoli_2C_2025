/*! @mainpage Proyecto 1, ej 3
 *
 * @section genDesc General Description
 *
 * Este programa enciende, apaga, o hace parpadear el LED 1, 2 o 3, según se elija.
 * En el caso de que se haga parpadear, también se puede elegir cuantas veces y la duración temporal de cada ciclo
 * 
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
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

/*==================[internal data definition]===============================*/
/** @def struct LED
 *  @brief estructura LED, que contiene el modo de funcionamiento (ON, OFF, TOGGLE), el número del led afectado y,
 *  en el caso de que el modo sea TOGGLE, la cantidad de ciclos y el periodo que tendrán los mismos
 */
struct LED
{
	uint8_t mode; 	///modo de funcionamiento -> ON, OFF, TOGGLE
	uint8_t n_led;	///indica el número de led a controlar 
	uint8_t n_ciclos; ///indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo; ///indica el tiempo de cada ciclo
};
/*==================[internal functions declaration]=========================*/
/** @fn void ActivarLED(struct LED *led)
 *  @brief enciende, apaga, o hace parpadear el LED 1, 2 o 3, según se elija. 
 *  En el caso de que se haga parpadear, también se puede elegir cuantas veces y la duración temporal de cada ciclo
 *  @param struct LED *led
 *  @return
 */
void ActivarLED(struct LED *led);
/*==================[external functions definition]==========================*/
enum{ON, OFF, TOGGLE};



void ActivarLED(struct LED *led){
	switch (led->mode){
		case ON:
			switch(led->n_led){
				case LED_1:
					LedOn(LED_1);
				break;
				case LED_2:
					LedOn(LED_2);
				break;
				case LED_3:
					LedOn(LED_3);
				break;
			}		
		break;
		case OFF:
			switch(led->n_led){
				case LED_1:
					LedOff(LED_1);
				break;
				case LED_2:
					LedOff(LED_2);
				break;
				case LED_3:
					LedOff(LED_3);
				break;
			}
		break;


case TOGGLE:
				int i;
				for (i=1; i<2*(led->n_ciclos); i++){
				LedToggle(led->n_led);
				vTaskDelay(led->periodo / portTICK_PERIOD_MS);
				}
				LedOff(led->n_led);
		
	};


};
void app_main(void){
	LedsInit();
	struct LED led={TOGGLE,LED_1,10,500};
	ActivarLED(&led);

}


/*==================[end of file]============================================*/