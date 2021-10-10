#include <stdio.h>
#include <thermo/sensor.h>
#include <syscalls.h>
#include <main.h>

extern "C" {
	void EXTI4_15_IRQHandler(void);
	void ADC1_IRQHandler(void);
	void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
	void TIM1_CC_IRQHandler(void);
	void TIM3_IRQHandler(void);
	void SPI1_IRQHandler(void);
	void SPI2_IRQHandler(void);
	void SysTick_Handler(void);
}

extern ADC_HandleTypeDef hadc1;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart2;

/*
TIMx_CLK = 16000000 Hz
duration = 0.01s

Duration = (ARR+1)/(TIMx_CLK/(PSC+1)).

ARR = (Duration * TIMx_CLK / (PSC + 1)) -1
	= (0.01 * ) -1
	= 31999


Delay = CCRy/(TIMx_CLK/(PSC + 1))

pulse = Duration - Delay

Duraton = pulse + Delay

CCRy = Delay * (TIMx_CLK/(PSC + 1))
CCRy = 16000
pulse = 15999


0 < phi < PI

A = cos(x)

acos(A) = x

A = PI * alpha / 360

alpha = A * 360 / PI


delay = 0,005s, pulse = 0,0001s --> duration = 0,0051s
ARR = (0,0051 * 16000000 / 5) -1

CCR = 0,005 *
*/

uint16_t min_current = 2048;
uint16_t max_current = 2048;

uint16_t last_min_current = 2048;
uint16_t last_max_current = 2048;
uint16_t rxbuf[2] = {0,0};
uint16_t txbuf[2] = {0xabcd,0x1234};
uint16_t buffer[2] = {0};

constexpr uint16_t cmd_identifier = 'C' << 8 | 'M';

inline void process_command(uint8_t cmd)
{
	//printf("process_command cmd: %d\n", cmd);
	switch(cmd)
	{
		case 0:
			HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(ENABLE_POWER_GPIO_Port, ENABLE_POWER_Pin, GPIO_PIN_RESET);
			break;

		case 1:
			HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(ENABLE_POWER_GPIO_Port, ENABLE_POWER_Pin, GPIO_PIN_SET);
			break;

		default:
			break;
	}
}
					;
void print_state(thermo::Sensor const& sensor)
{
	if(sensor.has_error())
	{
		printf(
			"Sensor: %s (intern: %d C)\n",
			sensor.error_str(),
			static_cast<int>(sensor.internal_temperatur()));
	}
	else
	{
		printf(
			"Sensor: %d C (intern: %d C)\n",
			static_cast<int>(sensor.hot_end_temperature()),
			static_cast<int>(sensor.internal_temperatur()));
	}
}


void user_main()
{
	RetargetInit(&huart2);
	puts("-------------------Start----------------");

	HAL_GPIO_WritePin(SPI1_SEL_NEG_GPIO_Port, SPI1_SEL_NEG_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
	HAL_Delay(100);



	//HAL_TIM_OnePulse_Start_IT(&htim1, TIM_CHANNEL_1);
	thermo::Sensor sensor;

	auto const start = HAL_GetTick();
	HAL_GPIO_WritePin(SPI1_SEL_NEG_GPIO_Port, SPI1_SEL_NEG_Pin, GPIO_PIN_RESET);
	HAL_SPI_Receive(&hspi1, reinterpret_cast<uint8_t*>(txbuf), 2, 10000);
	HAL_GPIO_WritePin(SPI1_SEL_NEG_GPIO_Port, SPI1_SEL_NEG_Pin, GPIO_PIN_SET);

	printf("transfer time: %ld ms", HAL_GetTick() - start);

	sensor.process(txbuf[0], txbuf[1]);

	print_state(sensor);

	while (1)
	{
//		printf("send: %04x %04x\n", txbuf[0], txbuf[1]);
		if(HAL_SPI_TransmitReceive(
			&hspi2,
			reinterpret_cast<uint8_t*>(txbuf),
			reinterpret_cast<uint8_t*>(rxbuf),
			2,
			0xffff) == HAL_OK)
		{
		//	printf("recv: %04x %04x\n", rxbuf[0], rxbuf[1]);
			if(rxbuf[0] == 0x434d)
			{
				uint8_t hops = rxbuf[1] & 0x00ff;
				uint8_t cmd = rxbuf[1] >> 8;
				if(hops == 0)
				{
					process_command(cmd);
					HAL_GPIO_WritePin(SPI1_SEL_NEG_GPIO_Port, SPI1_SEL_NEG_Pin, GPIO_PIN_RESET);
					HAL_SPI_Receive(&hspi1, reinterpret_cast<uint8_t*>(txbuf), 2, 10000);
					HAL_GPIO_WritePin(SPI1_SEL_NEG_GPIO_Port, SPI1_SEL_NEG_Pin, GPIO_PIN_SET);

					//sensor.process(txbuf[0], txbuf[1]);
					//print_state(sensor);
				}
				else
				{
					txbuf[0] = cmd_identifier;
					txbuf[1] = (rxbuf[1] & 0xff00) | (hops - 1);
				}
			}
			else
			{
				txbuf[0] = rxbuf[0];
				txbuf[1] = rxbuf[1];
			}
		}
	}
}


void EXTI4_15_IRQHandler(void)
{
	puts(__FUNCTION__);
	HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
}


void ADC1_IRQHandler(void)
{
	// puts(__FUNCTION__);
	HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
	HAL_ADC_IRQHandler(&hadc1);
}


void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim1);

	last_min_current = min_current;
	last_max_current = max_current;

	min_current = 2048;
	max_current = 2048;

	printf("timer1 end of periode\n");
}


void TIM1_CC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim1);
}


void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim3);
}

int byte_count = 0;
void SPI2_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&hspi2);

	// byte_count++;
	// if(byte_count == 2)
	// {
	// 	printf("%s: send: %04x, %04x\n",__FUNCTION__, txbuf[0], txbuf[1]);
	// 	printf("%s: cmd: %04x, %04x\n",__FUNCTION__, rxbuf[0], rxbuf[1]);

	// 	HAL_SPI_TransmitReceive_IT(
	// 		&hspi2,
	// 		reinterpret_cast<uint8_t*>(txbuf),
	// 		reinterpret_cast<uint8_t*>(rxbuf),
	// 		2);
	// }
}

void SPI1_IRQHandler(void)
{

  HAL_SPI_IRQHandler(&hspi1);
}
