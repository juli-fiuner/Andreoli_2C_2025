/*! @mainpage Proyecto 1, ejercicios 4, 5 y 6
 *
 * @section genDesc General Description
 *
 * Este código convierte un número decimal a BCD para luego mostrarlo en un BCD 7 segmentos 

 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral 		|   ESP32   	|
 * |:------------------:|:--------------|
 * | 	PIN_SEL_3	 	| 	GPIO_9		|
 * | 	PIN_SEL_2	 	| 	GPIO_18		|
 * | 	PIN_SEL_1	 	| 	GPIO_19		|
 * | 	PIN_D1	 		| 	GPIO_20		|
 * | 	PIN_D2	 		| 	GPIO_21		|
 * | 	PIN_D3	 		| 	GPIO_22		|
 * | 	PIN_D4	 		| 	GPIO_23		|
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
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/


/*==================[internal data definition]===============================*/
/** @def struct gpioConf_t
 *  @brief configuración de un GPIO, este struct contiene los parámetros necesarios para inicializar un GPIO : pin y dirección (output o input)
 */
typedef struct{
		gpio_t pin; /// Nro de pin
		io_t dir; /// lo establece como entrada/input (0) o salida/output (1)
	}gpioConf_t;

/** @def resto  
 * @brief Variable que almacena el resto de una división por 10
 */
uint8_t resto;	

/** @def dato  
 * @brief Contiene el número que se quiere convertir a BCD y/o mostrarse en la pantalla del BCD 7 segmentos
 */
uint32_t dato;

/** @def nrodigitos  
 * @brief Indica la cantidad de dígitos que forman el dato
 */
uint8_t nrodigitos;

/** @def bcd_array  
 * @brief arreglo que almacena el dato en su formato BCD, es decir
 * separado en centenas, decenas, unidades (en ese orden). Por ejemplo: el número 123 se almacena como [1, 2, 3]
 */
uint8_t bcd_array;

/** @def vector_bcd  
 * @brief Vector que se utiliza para enviar información a los conversores BCD 7 segmentos.
 * Cada elemento está conectado a un GPIO de salida y se cambia el estado de cada uno, a ‘0’ o a ‘1’, 
 * según el estado del bit correspondiente en el BCD ingresado
 */
gpioConf_t vector_bcd;

/** @def vector_pulsos  
 * @brief Cada elemento de este vector está conectado a un GPIO, establecido como salida,
 * conectado 1 a cada conversor BCD 7 segmentos, para así unir el BCD con el LCD correspondiente (unidad, decena o centena)
 */
gpioConf_t vector_pulsos;


/*==================[internal functions declaration]=========================*/
/** @fn void conversor_binario_BCD(uint32_t dato, uint8_t nrodigitos, uint8_t * bcd_array)
 *  @brief Esta función toma el dato ingresado, lo convierte en BCD y guarda cada dígito en el un puntero de tipo arreglo. 
 *  Corresponde al ejercicio 4
 *  @param uint32_t dato, uint8_t nrodigitos, uint8_t * bcd_array
 *  @return
 */
void conversor_binario_BCD(uint32_t dato, uint8_t nrodigitos, uint8_t * bcd_array);

/** @fn void mostrar_nro_BCD(uint8_t bcd_array, gpioConf_t * vector_bcd)
 * @brief Esta función toma un arreglo (donde cada dato representa un número en bd) 
 * y configura el estado de los GPIO como '0' o '1' según el bit correspondiente. 
 * Corresponde al ejercicio 5
 * @param uint8_t bcd_array, gpioConf_t * vector_bcd
 * @return
 */
void mostrar_nro_BCD(uint8_t bcd_array, gpioConf_t * vector_bcd);

/** @fn void set_display(uint32_t dato, uint8_t nrodigitos, gpioConf_t * vector_bcd, gpioConf_t*vector_pulso)
 * @brief La función set_display toma el dato ingresado y lo muestra en pantalla, usa las funciones conversor_binario_BCD
 * y mostrar_nro_BCD para lograrlo. 
 * Corresponde al ejercicio 6.
 * @param uint32_t dato, uint8_t nrodigitos, gpioConf_t * vector_bcd, gpioConf_t*vector_pulso
 * @return
 */
void set_display(uint32_t dato, uint8_t nrodigitos, gpioConf_t * vector_bcd, gpioConf_t*vector_pulso);

/*==================[external functions definition]==========================*/

void conversor_binario_BCD(uint32_t dato, uint8_t nrodigitos, uint8_t * bcd_array){
	uint8_t i;
	/* pongo cada digito en el array, sigue como binario*/
	for (i=nrodigitos; i>0; i--){
		resto= dato%10;
		bcd_array[i-1]= resto;
		dato=dato/10;
	}
}

void mostrar_nro_BCD(uint8_t bcd_array, gpioConf_t * vector_bcd){
	uint8_t n;
	for (n=0; n<4; n++){
		if (bcd_array&1){
			GPIOOn(vector_bcd[n].pin);

		} else {
			GPIOOff(vector_bcd[n].pin);
		};
		bcd_array=bcd_array>>1;}
	}

void set_display(uint32_t dato, uint8_t nrodigitos, gpioConf_t * vector_bcd, gpioConf_t*vector_pulso){
	uint8_t bcd_array[nrodigitos],n;
	conversor_binario_BCD(dato,nrodigitos,bcd_array);
	for (n=nrodigitos;n>0;n--){
		mostrar_nro_BCD(bcd_array[n-1],vector_bcd);
		GPIOOn(vector_pulso[n-1].pin);
		GPIOOff(vector_pulso[n-1].pin);
	}
}

void app_main(void){
	gpioConf_t b0= {GPIO_20, 1};
	gpioConf_t b1={GPIO_21, 1};
	gpioConf_t b2= {GPIO_22,1};
	gpioConf_t b3= {GPIO_23,1};
	gpioConf_t vector_conversor[4];
	vector_conversor[0] = b0;
	vector_conversor[1] = b1;
	vector_conversor[2] = b2;
	vector_conversor[3] = b3;

	/* para vector de pulsos*/
	gpioConf_t centena= {GPIO_19, 1};
	gpioConf_t decena={GPIO_18, 1};
	gpioConf_t unidad= {GPIO_9,1};
	gpioConf_t vector_pulsos[3];
	vector_pulsos[0]=centena;
	vector_pulsos[1]=decena;
	vector_pulsos[2]=unidad;
	uint8_t i;
	for(i=0; i<4; i++)
		{GPIOInit(vector_conversor[i].pin,vector_conversor[i].dir);};
	for(i=0; i<3; i++)
		{GPIOInit(vector_pulsos[i].pin,vector_pulsos[i].dir);};

	set_display(222,3,vector_conversor,vector_pulsos);
	while (1)
	{
		/* code */
	}
	


}

/*==================[end of file]============================================*/