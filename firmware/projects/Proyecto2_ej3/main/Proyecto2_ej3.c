/*! @mainpage Proyecto 2 ej 3. Interrupciones y puerto serie
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
#define CONFIG_PERIOD_TECLA2_US 1000000 /*1seg*/
#define CONFIG_PERIOD_TECLA1_US 1000000

/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL;
uint8_t tecla, variable_control_1=0, variable_control_2=0;
uint16_t medida =0;

/*==================[internal functions declaration]=========================*/

void FuncTimerA(void* param){ /*funcion de atencion de interrupcion*/
    vTaskNotifyGiveFromISR(led1_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada al LED_1 que caducó el timer */
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