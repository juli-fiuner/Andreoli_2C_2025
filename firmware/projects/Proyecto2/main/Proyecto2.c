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
#include "switch.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL;
TaskHandle_t tecla_task_handle = NULL;
uint8_t tecla, variable_control_1=0, variable_control_2=0;
uint16_t medida =0;
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/


static void controlar_by_medida(void *pvParameter){
    while(true){
        if (!variable_control_1){
        medida = HcSr04ReadDistanceInCentimeters();
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

            vTaskDelay(1000 / portTICK_PERIOD_MS);         
            
        }else{
            medida=0;
        }

        if (!variable_control_2){
            LcdItsE0803Write(medida);
        }

        }
}

/** tarea de leer teclas 

 * si tecla=switch_1 --> variable=0 --> se prende
 * 
 * si again tecla=switch_1 --> variable= 1 --> se apaga
*/
static void leer_teclas(void *pvParameter){
    while(1){
        tecla = SwitchesRead();
        switch (tecla){
        case SWITCH_1:  
            variable_control_1=!variable_control_1;
            vTaskDelay(100/portTICK_PERIOD_MS);
            break;
        case SWITCH_2:
            variable_control_2=!variable_control_2;       
            vTaskDelay(100/portTICK_PERIOD_MS); 
            break;
        vTaskDelay(50/portTICK_PERIOD_MS);
        }
    }
}


void app_main(void){
    HcSr04Init(GPIO_3, GPIO_2);
    LedsInit();
    LcdItsE0803Init();
    SwitchesInit();
    xTaskCreate(&controlar_by_medida, "Control de leds y lcd", 2048, NULL, 5, &led1_task_handle);
    xTaskCreate(&leer_teclas, "Lee teclas", 2048, NULL, 5, &tecla_task_handle);
}
/*==================[end of file]============================================*/