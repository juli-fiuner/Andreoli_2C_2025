/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
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
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
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

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
enum{ON, OFF, TOGGLE};
struct LED
{
	uint8_t mode; 	/*ON, OFF, TOGGLE*/
	uint8_t n_led;	/* indica el número de led a controlar*/ 
	uint8_t n_ciclos; /*indica la cantidad de ciclos de encendido/apagado*/
	uint16_t periodo; /*indica el tiempo de cada ciclo*/
};


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