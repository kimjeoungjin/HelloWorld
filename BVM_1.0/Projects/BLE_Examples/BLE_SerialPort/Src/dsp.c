#include "uart.h"
#include "configuration.h"


#define BUFFER_SIZE	5
#define SCALE 		50

float adcData_In[BUFFER_SIZE] = {0};
float adcData_Out[BUFFER_SIZE] = {0};
float adc_Filtered = 0.0;
float adc_Temp = 0.0;

// IIR, Bandpass, 1-10hz, 200hz, Order 2, Scalse = 205
const float ar[] = {1,	0.782095198023337,	0.679978526916300,	0.182675697753032,	0.030118875043169};
const float ma[] = {0.167179268608490,	0.668717074433960,	1.003075611650940,	0.668717074433960,	0.167179268608490};


float IirFilter(float* input, float* output, const float* ma, const float* ar, uint8_t fsize)
{
	uint8_t i, j;
	float sum;
	
	sum = ma[0] * input[0];
	for ( i = 1; i < fsize; i++ ) {
		sum += ( (ma[i] * input[i]) - (ar[i] * output[i]) );
	}
	
	output[0] = sum;
	for ( j = fsize-1; j >= 1; j-- ) {
		input[j] = input[j-1];
		output[j] = output[j-1];
	}
	
	return sum;
}



void ADC_DataProcess( float adc_data )
{
	adcData_In[0] = adc_data;
	
	adc_Temp =  IirFilter(adcData_In, adcData_Out, ma, ar, BUFFER_SIZE); 
	
	if ( adc_Temp > 0 )	adc_Filtered = adc_Temp;
	else 				adc_Filtered = (-1) * adc_Temp;
}