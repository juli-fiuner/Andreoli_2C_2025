/*! @mainpage Proyecto 2, ejercicio 1 y 2
 *
 * @section genDesc General Description
 *
 *      Esta aplicación mide la distancia a la que se encuentra un objeto situado frente al sensor HC-SR04 y da evidencia de esta medida de 2 manerass:
 * - LEDs: enciende cierta cantidad de LEDs según la distancia medida. Si es menor a 10 cm, se apagan todos los LEDs; si está entre 10 y
 * 20 cm, enciende el 1er LED; si está entre 20 y 30cm, el 1er y 2ndo LED; y si es mayor a 30cm se prenden los 3 LEDs.
 * - Pantalla LCD, donde muestra el valor númerico medido.
 *      Además, con la tecla 1 se puede activar y desactivar la medición y con la tecla 2 la pantalla 'congela' el valor.
 * 
 * 
 * 
 * @section hardConn Hardware Connection
 *
 * |    Peripheral      |   ESP32   	|
 * |:------------------:|:--------------|
 * | 	ECHO	    	| 	GPIO_3		|
 * | 	TRIGGER	    	| 	GPIO_2		|
 * | 	+5V 	    	| 	+5V 		|
 * | 	GND 	 	    | 	GND 		|
 * | 	PIN_SEL_3	 	| 	GPIO_9		|
 * | 	PIN_SEL_2	 	| 	GPIO_18		|
 * | 	PIN_SEL_1	 	| 	GPIO_19		|
 * | 	PIN_D1	 		| 	GPIO_20		|
 * | 	PIN_D2	 		| 	GPIO_21		|
 * | 	PIN_D3	 		| 	GPIO_22		|
 * | 	PIN_D4	 		| 	GPIO_23		|
 * 
 * 
 * 
 * @section changelog Changelog
 *
 * |   Date	    | Description                                        |
 * |:----------:|:----------------.----------------------------------|
 * | 24/10/2025 | Entrega de proyecto		                         |
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
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
/** @def TaskHandle_t led1_task_handle
 *  @brief Handle de la tarea que controla los LEDs y la pantalla LCD.
 */
TaskHandle_t led1_task_handle = NULL;
/** @def TaskHandle_t tecla_task_handle
 * @brief Handle de la tarea que maneja la lectura de las teclas (junto con las variables de control).
 */
TaskHandle_t tecla_task_handle = NULL;
/** @def uint8_t tecla
 * @brief variable que almacena la tecla leída.
 */
uint8_t tecla;
/** @def uint8_t variable_control_1
 * @brief se encarga de detener o retomar la medición.
 * si es 0 --> medicion activa; si es 1 --> medición pausada.
 */
uint8_t variable_control_1=0;
/** @def uint8_t variable_control_1
 * @brief controla la actualización del valor en la pantalla LCD.
 * si es 0 --> mantiene el valor actualizado; si es 1 --> congela el valor .
 */
uint8_t variable_control_2=0;
/** @def uint16_t medida
 * @brief variable que almacena la última distancia medida.
 */
uint16_t medida =0;
/*==================[internal functions declaration]=========================*/
/** @fn static void controlar_by_medida(void *pvParameter)
 *  @brief Controla los LEDs y la pantalla LCD según la distancia medida por el sensor.
 * @return
 */
static void controlar_by_medida(void *pvParameter);

 /** @fn static void leer_teclas(void *pvParameter)
 *  @brief Lee las teclas y actualiza las variables de control.
 * @return
 */

/*==================[external functions definition]==========================*/


static void controlar_by_medida(void *pvParameter){
    while(true){
        if (!variable_control_1){ //variable = 0 --> mide, =1 --> no mide
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

        if (!variable_control_2){ // variable = 0 --> muestra medidas nuevas, =1 --> se mantiene
            LcdItsE0803Write(medida);
        }

        }
}

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