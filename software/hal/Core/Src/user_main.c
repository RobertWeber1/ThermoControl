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
			HAL_GPIO_WritePin(ENABLE_POWER_GPIO_Port, ENABLE_POWER_Pin, GPIO_PIN_RESET);
			break;

		case 1:

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


template<size_t PortAddr, uint16_t Pin, bool Inverted=false>
struct Output
{
	Output()
	: toggle_deadline_(0xffff)
	, blinking_(false)
	{}

	static void on()
	{
		HAL_GPIO_WritePin(port(), Pin, (Inverted?GPIO_PIN_RESET:GPIO_PIN_SET));
	}

	static void off()
	{
		HAL_GPIO_WritePin(port(), Pin, (Inverted?GPIO_PIN_SET:GPIO_PIN_RESET));
	}

	static void toogle()
	{
		HAL_GPIO_TogglePin(port(), Pin);
	}

	void process()
	{
		if(not blinking_)
		{
			return;
		}

		auto const now = HAL_GetTick();
		if(now > toggle_deadline_)
		{
			puts("toggle");
			toggle_deadline_ = now + 100;
			toogle();
		}
	}

	void blink_on()
	{
		if(not blinking_)
		{
			toggle_deadline_ = HAL_GetTick();
		}
		blinking_ = true;
	}

	void blink_off()
	{
		blinking_ = false;
	}

private:
	static GPIO_TypeDef* port()
	{
		return reinterpret_cast<GPIO_TypeDef*>(PortAddr);
	}

	uint32_t toggle_deadline_;
	bool blinking_;
};

Output<GPIOA_BASE, SPI1_SEL_NEG_Pin> spi1_sel;
Output<GPIOA_BASE, STATUS_LED_Pin, true> status_led;
Output<GPIOC_BASE, ACTIVITY_LED_Pin, true> activity_led;

void user_main()
{
	RetargetInit(&huart2);
	puts("-------------------Start----------------");

	spi1_sel.on();

	activity_led.on();
	status_led.on();
	HAL_Delay(100);
	activity_led.toogle();
	status_led.toogle();
	HAL_Delay(100);
	activity_led.toogle();
	status_led.toogle();
	HAL_Delay(100);
	activity_led.toogle();
	status_led.toogle();

	//HAL_TIM_OnePulse_Start_IT(&htim1, TIM_CHANNEL_1);
	thermo::Sensor sensor;

	auto const start = HAL_GetTick();
	spi1_sel.off();
	HAL_SPI_Receive(&hspi1, reinterpret_cast<uint8_t*>(txbuf), 2, 10000);
	spi1_sel.on();

	printf("transfer time: %ld ms\n", HAL_GetTick() - start);

	sensor.process(txbuf[0], txbuf[1]);
	if(sensor.has_error())
	{
		status_led.blink_on();
		activity_led.blink_on();
	}

	print_state(sensor);

	while (1)
	{
		printf("send: %04x %04x\n", txbuf[0], txbuf[1]);
		auto const ret = HAL_SPI_TransmitReceive(
			&hspi2,
			reinterpret_cast<uint8_t*>(txbuf),
			reinterpret_cast<uint8_t*>(rxbuf),
			2,
			1000);

		status_led.process();
		activity_led.process();

		if(ret == HAL_OK)
		{
			printf("recv: %04x %04x\n", rxbuf[0], rxbuf[1]);
			if(rxbuf[0] == 0x434d)
			{
				uint8_t hops = rxbuf[1] & 0x00ff;
				uint8_t cmd = rxbuf[1] >> 8;
				if(hops == 0)
				{
					process_command(cmd);

					spi1_sel.off();
					HAL_SPI_Receive(&hspi1, reinterpret_cast<uint8_t*>(txbuf), 2, 1000);
					spi1_sel.on();

					sensor.process(txbuf[0], txbuf[1]);
					if(sensor.has_error())
					{
						puts("blink_on");
						status_led.blink_on();
						activity_led.blink_on();
					}
					else
					{
						status_led.blink_off();
						activity_led.blink_off();
						if(cmd == 0)
						{
							HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
							HAL_GPIO_WritePin(ACTIVITY_LED_GPIO_Port, ACTIVITY_LED_Pin, GPIO_PIN_SET);
						}
						else
						{
							HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(ACTIVITY_LED_GPIO_Port, ACTIVITY_LED_Pin, GPIO_PIN_RESET);
						}
					}

					print_state(sensor);
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
		else
		{
			printf("SPI transfer error: %d\n", ret);
			process_command(0);
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
