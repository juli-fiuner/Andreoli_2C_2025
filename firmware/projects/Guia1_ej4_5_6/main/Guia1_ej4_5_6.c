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
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct{
		gpio_t pin;
		io_t dir;
	}gpioConf_t;
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

/* act 4: función que convierta un nro a BCD y guardar cada digito en un puntero de tipo arreglo */


void conversor_binario_BCD(uint32_t dato, uint8_t nrodigitos, uint8_t * bcd){
	uint8_t i, resto;
	/* pongo cada digito en el array, sigue como binario*/
	for (i=nrodigitos; i>0; i--){
		resto= dato%10;
		bcd[i-1]= resto;
		dato=dato/10;
	}
}

/* act 5: recibe bcd_array luego de pasar por el conversor y el vector de GPIO */

void mostrar_nro_BCD(uint8_t bcd, gpioConf_t * vector){
	uint8_t n;
	for (n=0; n<4; n++)
		if (bcd&1){
			GPIOOn(vector[n].pin);

		} else {
			GPIOOff(vector[n].pin);
		};
		bcd=bcd>>1;
	}

void set_display(uint32_t dato, uint8_t digitos, gpioConf_t * vector_bcd, gpioConf_t*vector_pulso){
	uint8_t bcd_array[digitos],n;
	conversor_binario_BCD(dato,digitos,bcd_array);
	for (n=digitos;n>0;n--){
		mostrar_nro_BCD(bcd_array[n-1],vector_bcd);
		GPIOOn(vector_pulso[n-1].pin);
	}
}

void app_main(void){
	uint8_t bcd_array[3];
	conversor_binario_BCD(10,2,bcd_array);
	gpioConf_t b0= {GPIO_20, 1};
	gpioConf_t b1={GPIO_21, 1};
	gpioConf_t b2= {GPIO_22,1};
	gpioConf_t b3= {GPIO_23,1};
	gpioConf_t vector_conversor[4];
	vector_conversor[0] = b0;
	vector_conversor[1] = b1;
	vector_conversor[2] = b2;
	vector_conversor[3] = b3;


}

/*==================[end of file]============================================*/