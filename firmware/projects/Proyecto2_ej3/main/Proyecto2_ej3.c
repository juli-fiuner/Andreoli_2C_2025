/*! @mainpage Proyecto 2 ejercicio 3. Interrupciones y puerto serie
 *
 * @section genDesc General Description
 * 
 *      Esta aplicación mide la distancia a la que se encuentra un objeto situado frente al sensor HC-SR04 y da evidencia de esta medida de 2 manerass:
 * - LEDs: enciende cierta cantidad de LEDs según la distancia medida. Si es menor a 10 cm, se apagan todos los LEDs; si está entre 10 y
 * 20 cm, enciende el 1er LED; si está entre 20 y 30cm, el 1er y 2ndo LED; y si es mayor a 30cm se prenden los 3 LEDs.
 * - Pantalla LCD, donde muestra el valor númerico medido.
 * - A través de una terminal en la PC (enviando los datos por un puerto serie).
 *      Además, con la tecla 1/letra 'O' se puede activar y desactivar la medición y con la tecla 2/letra 'H' la pantalla 'congela' el valor.
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
 * |:----------:|:---------------------------------------------------|
 * | 24/10/2025 | Entrega de proyecto		                         |
 *
 * @author Andreoli Aguilar, Julieta
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "switch.h"
#include "timer_mcu.h"
#include "uart_mcu.h"

/*==================[macros and definitions]=================================*/
/** @brief Período de interrupción del temporizador A en microsegundos (= 1 seg)
 */
#define CONFIG_PERIOD_TECLA1_US 1000000

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

/** @fn void FuncTimerA(void* param)
*  @brief función de atención de interrupción, envía una notificación a la tarea asociada al LED_1 que caducó el timer
* @return 
*/
void FuncTimerA(void* param);

/** @fn void Leertecla_UART(void *param)
*  @brief función de atención de interrupción de la UART, modifica las variables de control 1 y 2
* @return
*/
void Leertecla_UART(void *param);

/** @fn void leer_tecla1()
*  @brief Callback de interrupción de Switch_1, invierte la variable de control 1
* @return
*/
void leer_tecla1();


/** @fn void leer_tecla2()
*  @brief Callback de interrupción de Switch_2, invierte la variable de control 2
* @return
*/
void leer_tecla2();


void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(led1_task_handle, pdFALSE);
}

void Leertecla_UART(void *param){
    uint8_t tecla;
    UartReadByte(UART_PC,&tecla);
    if (tecla == 'o'){
        variable_control_1=!variable_control_1;
    }else if (tecla =='h'){
    	variable_control_2=!variable_control_2;
    }
}

/*==================[external functions definition]==========================*/
void leer_tecla1(){
	variable_control_1=!variable_control_1;       
}

void leer_tecla2(){
	variable_control_2=!variable_control_2;
}


static void controlar_by_medida(void *pvParameter){
    while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    /* La tarea espera en este punto hasta recibir una notificación */
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
            
        }else{
            medida=0;
        }

        if (!variable_control_2){ // variable = 0 --> muestra medidas nuevas, =1 --> se mantiene
            LcdItsE0803Write(medida);
            uint8_t *msj =UartItoa(medida,10);
		    UartSendString(UART_PC,(char *)msj);
		    UartSendString(UART_PC," cm\r\n");
        }


        }
}



void app_main(void){
    HcSr04Init(GPIO_3, GPIO_2);
    LedsInit();
    LcdItsE0803Init();
    SwitchesInit();
	SwitchActivInt(SWITCH_1, leer_tecla1, NULL);
	SwitchActivInt(SWITCH_2, leer_tecla2, NULL);


    timer_config_t timer= {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD_TECLA1_US,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer);

	serial_config_t my_uart={
		.port=UART_PC,
		.baud_rate = 9600,
		.func_p= Leertecla_UART,
		.param_p=NULL	
	};
	UartInit(&my_uart);

    xTaskCreate(&controlar_by_medida, "Control de leds y lcd", 2048, NULL, 5, &led1_task_handle);
	TimerStart(timer.timer);
}
/*==================[end of file]============================================*/