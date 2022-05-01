// ConsoleCommands.c
// This is where you add commands:
//		1. Add a protoype
//			static eCommandResult_T ConsoleCommandVer(const char buffer[]);
//		2. Add the command to mConsoleCommandTable
//		    {"ver", &ConsoleCommandVer, HELP("Get the version string")},
//		3. Implement the function, using ConsoleReceiveParam<Type> to get the parameters from the buffer.
#include <stdio.h>
#include <string.h>
#include "consoleCommands.h"
#include "console.h"
#include "consoleIo.h"
#include "version.h"
#include "../../Drivers/STM32F429I-Discovery/stm32f429i_discovery_gyroscope.h"

#define IGNORE_UNUSED_VARIABLE(x)     if ( &x == &x ) {}
#define ABS(x)         (x < 0) ? (-x) : x

static eCommandResult_T ConsoleCommandComment(const char buffer[]);
static eCommandResult_T ConsoleCommandVer(const char buffer[]);
static eCommandResult_T ConsoleCommandHelp(const char buffer[]);
static eCommandResult_T ConsoleCommandParamExampleInt16(const char buffer[]);
static eCommandResult_T ConsoleCommandParamExampleHexUint16(const char buffer[]);
static eCommandResult_T ConsoleCommandGyroPresent(const char buffer[]);
static eCommandResult_T ConsoleCommandGyroTest(const char buffer[]);

static const sConsoleCommandTable_T mConsoleCommandTable[] =
{
    {";", &ConsoleCommandComment, HELP("Comment! You do need a space after the semicolon. ")},
    {"help", &ConsoleCommandHelp, HELP("Lists the commands available")},
    {"ver", &ConsoleCommandVer, HELP("Get the version string")},
    {"int", &ConsoleCommandParamExampleInt16, HELP("How to get a signed int16 from params list: int -321")},
    {"u16h", &ConsoleCommandParamExampleHexUint16, HELP("How to get a hex u16 from the params list: u16h aB12")},
	{"gyrop", &ConsoleCommandGyroPresent, HELP("Check is gyro present and responding")},
	{"gyrotest", &ConsoleCommandGyroTest, HELP("Test gyro: params tcount - number of times to test")},

	CONSOLE_COMMAND_TABLE_END // must be LAST
};

static eCommandResult_T ConsoleCommandGyroTest(const char buffer[]){
	float Buffer[3];
	float Xval, Yval, Zval = 0x00;
	uint16_t tcount = 0;
    char strbuf[100];
    eCommandResult_T result;

    result = ConsoleReceiveParamInt16(buffer, 1, &tcount);
    if ( COMMAND_SUCCESS == result ){
		if (BSP_GYRO_Init() == GYRO_OK){
			while(tcount > 0){
				/* Read Gyro Angular data */
				BSP_GYRO_GetXYZ(Buffer);

				/* Update autoreload and capture compare registers value */
				Xval = ABS((Buffer[0]));
				Yval = ABS((Buffer[1]));
				Zval = ABS((Buffer[2]));

				//Reset the string buffer
				memset(strbuf, 0x00, 100);

				sprintf(strbuf, "%f | %f | %f\n", Xval, Yval, Zval);

				ConsoleIoSendString(strbuf);

				// Delay for 50 ms
				HAL_Delay(50);
				tcount--;
			}
			ConsoleIoSendString("Test completed");
		}
    } else {
    	return COMMAND_ERROR;
    }
	return COMMAND_SUCCESS;
}


/**
 * Testing is the gyro present or not
 * In case that the gyro is present the device will return Gyro OK else Gyro error
 */
static eCommandResult_T ConsoleCommandGyroPresent(const char buffer[]){
	if (BSP_GYRO_Init() == GYRO_OK){
		ConsoleIoSendString("Gyro OK\n");
	} else {
		ConsoleIoSendString("Gyro Error\n");
	}
	return COMMAND_SUCCESS;
}

static eCommandResult_T ConsoleCommandComment(const char buffer[])
{
	// do nothing
	IGNORE_UNUSED_VARIABLE(buffer);
	return COMMAND_SUCCESS;
}

static eCommandResult_T ConsoleCommandHelp(const char buffer[])
{
	uint32_t i;
	uint32_t tableLength;
	eCommandResult_T result = COMMAND_SUCCESS;

    IGNORE_UNUSED_VARIABLE(buffer);

	tableLength = sizeof(mConsoleCommandTable) / sizeof(mConsoleCommandTable[0]);
	for ( i = 0u ; i < tableLength - 1u ; i++ )
	{
		ConsoleIoSendString(mConsoleCommandTable[i].name);
#if CONSOLE_COMMAND_MAX_HELP_LENGTH > 0
		ConsoleIoSendString(" : ");
		ConsoleIoSendString(mConsoleCommandTable[i].help);
#endif // CONSOLE_COMMAND_MAX_HELP_LENGTH > 0
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}

static eCommandResult_T ConsoleCommandParamExampleInt16(const char buffer[])
{
	int16_t parameterInt;
	eCommandResult_T result;
	result = ConsoleReceiveParamInt16(buffer, 1, &parameterInt);
	if ( COMMAND_SUCCESS == result )
	{
		ConsoleIoSendString("Parameter is ");
		ConsoleSendParamInt16(parameterInt);
		ConsoleIoSendString(" (0x");
		ConsoleSendParamHexUint16((uint16_t)parameterInt);
		ConsoleIoSendString(")");
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}
static eCommandResult_T ConsoleCommandParamExampleHexUint16(const char buffer[])
{
	uint16_t parameterUint16;
	eCommandResult_T result;
	result = ConsoleReceiveParamHexUint16(buffer, 1, &parameterUint16);
	if ( COMMAND_SUCCESS == result )
	{
		ConsoleIoSendString("Parameter is 0x");
		ConsoleSendParamHexUint16(parameterUint16);
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}

static eCommandResult_T ConsoleCommandVer(const char buffer[])
{
	eCommandResult_T result = COMMAND_SUCCESS;

    IGNORE_UNUSED_VARIABLE(buffer);

	ConsoleIoSendString(VERSION_STRING);
	ConsoleIoSendString(STR_ENDLINE);
	return result;
}


const sConsoleCommandTable_T* ConsoleCommandsGetTable(void)
{
	return (mConsoleCommandTable);
}


