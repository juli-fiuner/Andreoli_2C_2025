/*! @mainpage Proyecto 2, ejercicio 4
 *
 * @section genDesc General Description
 * 
 * Esta aplicación cumple con 2 objetivos:
 * 1- Digitaliza una señal analógica (conectada por CH1) y la transmite por UART para ser visualizada en una terminal de la PC
 * 
 * 2- Convierte una señal digital (ECG brindado por la cátedra) en una señal analógica. Luego, esta es visualizada utilizando las funciones de 1-.
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral 			|   ESP32   	|
 * |:----------------------:|:--------------|
 * | 	Potenciómetro	 	| 	CH1			|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description           	                         |
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
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
/** @brief Período de interrupción del temporizador AD en microsegundos (= 20 ms)
 */
#define CONFIG_PERIOD_TIMER_AD_US 20000

/** @brief Período de interrupción del temporizador DA en microsegundos (= 20 ms)
 */
#define CONFIG_PERIOD_TIMER_DA_US 20000 /*deberían ser 4ms pero no anduvo, volver a probar*/


/*==================[internal data definition]===============================*/
/** @def TaskHandle_t task_handle_ADC
 *  @brief Handle de la tarea de conversión AD
 */
TaskHandle_t taskhandle_ADC = NULL;

/** @def TaskHandle_t task_handle_DAC
 *  @brief Handle de la tarea de conversión DA
 */
TaskHandle_t taskhandle_DAC = NULL;

/** @brief iterador/índice que recorre los valores del ECG.
 */
uint16_t i =0;

/** @brief señal de ECG del profesor Filomena, provista por la cátedra.
 * 
 */
unsigned char ECG[] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
17,17,17} ;


/*==================[internal functions declaration]=========================*/

/** @def void Func_timer_AD(void *param)
 * @brief función de atención de interrupción del timer AD.
 * @return
 */ 

/** @def void Func_timer_DA(void *param)
* @brief función de atención de interrupción del timer DA.
* @return
*/ 

/** @def void medir_ADC(void *pvParameter)
* @brief Tarea que realiza la conversión analógica-digital. Lee los datos del ch1 y los envía por UART.
* @return
*/ 

/** @def void conversor_DAC(void *pvParameter)
* @brief Tarea que realiza la conversión digital-analógica. Envía los datos del ECG (de manera periódica) al conversor para generar una señal analógica.
* @return
*/ 
/*==================[external functions definition]==========================*/

void Func_timer_AD(void* param){ 
    vTaskNotifyGiveFromISR(taskhandle_ADC, pdFALSE);  
}

void Func_timer_DA(void* param){ 
    vTaskNotifyGiveFromISR(taskhandle_DAC, pdFALSE);  
}

void medir_ADC(void *pvParameter){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		uint16_t salida_conversor;
		AnalogInputReadSingle(CH1, &salida_conversor);

		UartSendString(UART_PC, ">volts: ");
		UartSendString(UART_PC, (char *)UartItoa(salida_conversor, 10));
		UartSendString(UART_PC, "\r\n");
	}		
}

void conversor_DAC(void *pvParameter){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogOutputWrite(ECG[i]);
		i++;
		if(i==(sizeof(ECG))){
		i=0;}

	}
}


void app_main(void){
	timer_config_t timer_AD= {
        .timer = TIMER_A,
        .period = CONFIG_PERIOD_TIMER_AD_US,
        .func_p = Func_timer_AD,
        .param_p = NULL
    };
    TimerInit(&timer_AD);

	timer_config_t timer_DA= {
        .timer = TIMER_B,
        .period = CONFIG_PERIOD_TIMER_DA_US,
        .func_p = Func_timer_DA,
        .param_p = NULL
    };
    TimerInit(&timer_DA);

	analog_input_config_t config={
		.input= CH1,
		.mode=ADC_SINGLE,
	};
	AnalogInputInit(&config);
	AnalogOutputInit();

	serial_config_t uart_ADC={
		.port=UART_PC,
		.baud_rate = 115200,
		.func_p= NULL,
		.param_p=NULL	
	};
	UartInit(&uart_ADC);

    xTaskCreate(&medir_ADC, "", 2048, NULL, 5, &taskhandle_ADC);
    xTaskCreate(&conversor_DAC, "", 2048, NULL, 5, &taskhandle_DAC);

	TimerStart(timer_AD.timer);
	TimerStart(timer_DA.timer);



}
/*==================[end of file]============================================*/