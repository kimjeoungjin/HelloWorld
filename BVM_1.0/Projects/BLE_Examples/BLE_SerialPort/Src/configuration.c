#include "configuration.h"



void Gpio_Configuration( void )
{
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_15, LL_GPIO_MODE_OUTPUT); //LED0
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_15, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_15, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_15, LL_GPIO_PULL_UP);
	
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_15);
//	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_15);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT); //LED0
	LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);
	
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
	
	
	
}

void ALL_LED_OFF ( void ) 
{
	BVM_LED_OFF(GPIOA,LED0);
	BVM_LED_OFF(GPIOB,LED1 | LED2 | LED3 | LED4 | LED5);
}

void ALL_LED_ON ( void ) 
{
	BVM_LED_ON(GPIOA,LED0);
	BVM_LED_ON(GPIOB,LED1 | LED2 | LED3 | LED4 | LED5);
}



void BAT_Cheak ( uint32_t bat_adc ) 
{
	if ( bat_adc <= BAT_LEVEL_0 )
	{
		BVM_LED_ON(GPIOB,LED2 | LED3);
		Clock_Wait(2000);
		ALL_LED_OFF( );
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);
	}
	else 
	{
		if ( bat_adc >= BAT_LEVEL_0 ) 	BVM_LED_ON(GPIOA,LED0);
		Clock_Wait(50);
		if ( bat_adc >= BAT_LEVEL_20 ) 	BVM_LED_ON(GPIOB,LED1);
		Clock_Wait(50);
		if ( bat_adc >= BAT_LEVEL_40 ) 	BVM_LED_ON(GPIOB,LED2);
		Clock_Wait(50);
		if ( bat_adc >= BAT_LEVEL_60 ) 	BVM_LED_ON(GPIOB,LED3);
		Clock_Wait(50);
		if ( bat_adc >= BAT_LEVEL_80 ) 	BVM_LED_ON(GPIOB,LED4);
		Clock_Wait(50);
		if ( bat_adc >= BAT_LEVEL_100 ) 	BVM_LED_ON(GPIOB,LED5);
		Clock_Wait(2000);
		ALL_LED_OFF( );
	}
}

void Adc_Configuration (void)
{	
	LL_APB1_EnableClock(LL_APB1_PERIPH_ADCDIG | LL_APB1_PERIPH_ADCANA);
	
	LL_ADC_InvertOutputBitDiffModeDisable(ADC);//
	LL_ADC_InvertOutputBitSingleNegModeEnable(ADC);//
	
	LL_ADC_SetOverrunDF(ADC,ADC_NEW_DATA_IS_KEPT);//
	
	LL_ADC_SetSampleRate(ADC, LL_ADC_SAMPLE_RATE_28);//

	LL_ADC_ConfigureDSDataOutput(ADC, USER_DATAWIDTH, USER_RATIO);//

	LL_ADC_SetADCMode(ADC, LL_ADC_OP_MODE_ADC);//

	LL_ADC_SetSequenceLength(ADC, LL_ADC_SEQ_LEN_01);//
	
	LL_ADC_LDOEnable(ADC);//
		
	LL_ADC_InputSamplingMode(ADC, LL_ADC_SAMPLING_AT_START);
	
	LL_ADC_Enable(ADC);
	

	LL_ADC_SetVoltageRangeSingleVinm1(ADC, ADC_VIN_RANGE_3V6);	
//	LL_ADC_InputSamplingMode(ADC,ADC_SAMPLING_AT_START);


//	LL_ADC_InputSamplingMode(ADC, LL_ADC_SAMPLING_AT_END);
	LL_ADC_SetChannelSeq0(ADC, LL_ADC_CH_VINM1_TO_SINGLE_NEGATIVE_INPUT);
	
	LL_ADC_ContinuousModeDisable(ADC);
	
	LL_ADC_StartConversion(ADC);
	
	Clock_Wait(50);
}

static uint32_t tim_prescaler = 0;
static uint32_t tim_period = 0;

void MX_TIMx_Init(void)
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

	/* Set the pre-scaler value to have TIMx counter clock equal to 10 kHz      */
	tim_prescaler = __LL_TIM_CALC_PSC(LL_TIM_GetPeriphClock(TIMx), 10000);

	/* - Set the auto-reload value to have a counter frequency of 1000 Hz        */
	tim_period = __LL_TIM_CALC_ARR(LL_TIM_GetPeriphClock(TIMx), tim_prescaler, 1000);

	/* Peripheral clock enable */
	LL_EnableClock(LL_PERIPH_TIMx);

	TIM_InitStruct.Prescaler = tim_prescaler;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = tim_period;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	TIM_InitStruct.RepetitionCounter = 0;
	LL_TIM_Init(TIMx, &TIM_InitStruct);

	/* TIMx interrupt Init */
	NVIC_SetPriority(TIMx_IRQn, IRQ_MED_PRIORITY);
	NVIC_EnableIRQ(TIMx_IRQn);
	
	Clock_Wait(10);
	
	/* Clear the update flag */
	LL_TIM_ClearFlag_UPDATE(TIMx);

	/* Enable the update interrupt */
	LL_TIM_EnableIT_UPDATE(TIMx);

	/* Enable counter */
	LL_TIM_EnableCounter(TIMx);

	/* Force update generation */
	LL_TIM_GenerateEvent_UPDATE(TIMx);
	
}

void LPUART1_Configure(void)
{
	/* (1) Enable GPIO clock and configures the LPUART1 pins *******************/

	/* Enable the peripheral clock of GPIOB */
	LL_EnableClock_LPUART();
	LL_EnableClock_LPUART_TX_PORT();
	LL_EnableClock_LPUART_RX_PORT();

	/* Configure TX Pin as : Alternate function, High Speed, PushPull, No-Pull */
	LL_GPIO_SetPinMode(LPUART_TX_PORT, LPUART_TX_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_LPUART_TX();
	LL_GPIO_SetPinSpeed(LPUART_TX_PORT, LPUART_TX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(LPUART_TX_PORT, LPUART_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(LPUART_TX_PORT, LPUART_TX_PIN, LL_GPIO_PULL_NO);

	/* Configure RX Pin as : Alternate function, High Speed, PushPull, No-Pull */
	LL_GPIO_SetPinMode(LPUART_RX_PORT, LPUART_RX_PIN, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_LPUART_RX();
	LL_GPIO_SetPinSpeed(LPUART_RX_PORT, LPUART_RX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(LPUART_RX_PORT, LPUART_RX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(LPUART_RX_PORT, LPUART_RX_PIN, LL_GPIO_PULL_NO);

	/* (2) NVIC Configuration for LPUART1 interrupts */
	/*  - Set priority for LPUART1_IRQn */
	/*  - Enable LPUART1_IRQn           */
	NVIC_SetPriority(LPUART1_IRQn, IRQ_MED_PRIORITY);
	NVIC_EnableIRQ(LPUART1_IRQn);


	/* (4) Configure LPUART1 functional parameters ********************************/


	/* TX/RX direction */
	LL_LPUART_SetTransferDirection(LPUART1, LL_LPUART_DIRECTION_TX_RX);

	/* 8 data bit, 1 start bit, 1 stop bit, no parity */
	LL_LPUART_ConfigCharacter(LPUART1, LL_LPUART_DATAWIDTH_8B, LL_LPUART_PARITY_NONE, LL_LPUART_STOPBITS_1);

	/* No Hardware Flow control */
	/* Reset value is LL_USART_HWCONTROL_NONE */
	// LL_USART_SetHWFlowCtrl(LPUART1, LL_USART_HWCONTROL_NONE);

	/* Set Baudrate to 9600 using HSI frequency set to HSI_VALUE */
	LL_LPUART_SetBaudRate(LPUART1, LL_LPUART_PRESCALER_DIV1, 9600); 

	LL_LPUART_EnableIT_RXNE_RXFNE(LPUART1);  

	/* (5) Enable LPUART1 **********************************************************/
	LL_LPUART_Enable(LPUART1);
}




