#include "shell.h"
#include "shell_port.h"
#include "cdos.h"

#include "gd32f30x.h"

#define LOG_TRUE (1)

//extern UART_HandleTypeDef huart4;

//Shell shell;
//Log shell_log;

//static char shellBuffer[512];
//static uint8_t data;

//short userShellWrite(char *data, unsigned short len)
//{
//	HAL_UART_Transmit(&huart4, (uint8_t *)data, len, 0xFFFF);

//    return len;
//}

//void uartLogWrite(char *buffer, short len)
//{
//    if(shell_log.shell != NULL)
//    {
//        shellWriteEndLine(shell_log.shell, buffer, len);
//    }
//}

//void shell_init(void)
//{
//    shell.write = userShellWrite;
//    shellInit(&shell, shellBuffer, 512);
//	HAL_UART_Receive_IT(&huart4, &data, 1);

//	shell_log.active = LOG_TRUE;
//	shell_log.write = uartLogWrite;
//	logRegister(&shell_log, &shell);
//	logSetLevel(&shell_log, LOG_INFO);	
//}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if (huart->Instance == USART4)
//	{
//		shellHandler(&shell, data);
//		HAL_UART_Receive_IT(&huart4, &data, 1);
//	}
//}

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
//	
//}
