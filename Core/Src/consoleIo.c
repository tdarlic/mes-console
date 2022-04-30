// Console IO is a wrapper between the actual in and output and the console code
// In an embedded system, this might interface to a UART driver.
#include "stm32f4xx_hal.h"
#include "consoleIo.h"
#include <stdio.h>
#include <string.h>


UART_HandleTypeDef *consoleUartHandle;

uint8_t consoleByteBuffer = 0;

// This buffer will hold command
uint8_t consoleCommandBuffer[20];

uint8_t consoleRxCompleted = 0;

uint8_t consoleBufferCount = 0;

eConsoleError ConsoleReset(void);
eConsoleError ConsoleByteReset(void);

eConsoleError ConsoleIoInit(UART_HandleTypeDef *huart)
{
	consoleUartHandle = huart;
	ConsoleReset();
	HAL_UART_Receive_IT(consoleUartHandle, &consoleByteBuffer, 1);
	return CONSOLE_SUCCESS;
}

/**
 * Resets the console variabes
 */
eConsoleError ConsoleReset(void)
{
	consoleByteBuffer = 0;
	consoleRxCompleted = 0;
	consoleBufferCount = 0;
	memset(consoleCommandBuffer, 0, 20);
	return CONSOLE_SUCCESS;
}

/**
 * Resets the console variabes
 */
eConsoleError ConsoleByteReset(void)
{
	consoleRxCompleted = 0;
	consoleBufferCount = 0;
}


// This is modified for the Wokwi RPi Pico simulator. It works fine 
// but that's partially because the serial terminal sends all of the 
// characters at a time without losing any of them. What if this function
// wasn't called fast enough?
eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
	uint32_t i = 0;
	char ch;

	while (consoleRxCompleted)
	{
		while (i < consoleBufferCount + 1){
			buffer[i] = consoleCommandBuffer[i];
		}
		*readLength = consoleBufferCount;
		printf("%s", consoleCommandBuffer);
		ConsoleByteReset();
	}

	*readLength = i;
	return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoSendString(const char *buffer)
{
	printf("%s", buffer);
	return CONSOLE_SUCCESS;
}

/*
 * This function gets called acter completion of RX cycle on UART
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	consoleCommandBuffer[consoleBufferCount++] = consoleByteBuffer;
	if ('\n' == consoleByteBuffer){
		consoleRxCompleted = 1;
	}
	HAL_UART_Receive_IT(consoleUartHandle, &consoleByteBuffer, 1);
}

