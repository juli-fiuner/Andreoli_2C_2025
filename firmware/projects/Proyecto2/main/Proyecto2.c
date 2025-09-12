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
#include "hc_sr04.h"
#include "lcditse0803.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/


static void controlar_led_by_medida(void *pvParameter){
    while(true){
        uint16_t medida = HcSr04ReadDistanceInCentimeters();
        if (medida<10){
            LedOff(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);

        } else if (medida<20){
            LedOn(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
        }else if(medida<30){
            LedOn(LED_1);
            LedOn(LED_2);
            LedOff(LED_3);

        }else{
            LedOn(LED_1);
            LedOn(LED_2);
            LedOn(LED_3);
            
        }
        LcdItsE0803Write(medida);
        vTaskDelay(1000 / portTICK_PERIOD_MS);         
        }
}

void app_main(void){
    HcSr04Init(GPIO_3, GPIO_2);
    LedsInit();
    LcdItsE0803Init();
    xTaskCreate(&controlar_led_by_medida, "Control de leds", 2048, NULL, 5, &led1_task_handle);
    
}
/*==================[end of file]============================================*/