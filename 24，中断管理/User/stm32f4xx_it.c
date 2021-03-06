/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "board_init.h"
/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup GPIO_EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	printf("HardFault_Handler\n");
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{
	#if (INCLUDE_xTaskGetSchedulerState  == 1 )
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
	#endif  /* INCLUDE_xTaskGetSchedulerState */  
		xPortSysTickHandler();
	#if (INCLUDE_xTaskGetSchedulerState  == 1 )
		}
	#endif  /* INCLUDE_xTaskGetSchedulerState */
  HAL_IncTick();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/* 声明引用外部队列 & 二值信号量 */
extern QueueHandle_t Test_Queue;
extern SemaphoreHandle_t BinarySem_Handle;

static uint32_t send_data1 = 1;
static uint32_t send_data2 = 2;


/*********************************************************************************
  * @ 函数名  ： KEY1_IRQHandler
  * @ 功能说明： 中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void KEY1_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;

  uint32_t ulReturn;
  /* 进入临界段，临界段可以嵌套 */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
	//确保是否产生了EXTI Line中断
	if(__HAL_GPIO_EXTI_GET_IT(KEY1_INT_GPIO_PIN) != RESET) 
	{
    /* 将数据写入（发送）到队列中，等待时间为 0  */
		xQueueSendFromISR(Test_Queue, /* 消息队列的句柄 */
											&send_data1,/* 发送的消息内容 */
											&pxHigherPriorityTaskWoken);
		
		//如果需要的话进行一次任务切换
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
		
		//清除中断标志位
		__HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);     
	}
  
  /* 退出临界段 */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

/*********************************************************************************
  * @ 函数名  ： KEY1_IRQHandler
  * @ 功能说明： 中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void KEY2_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
  uint32_t ulReturn;
  /* 进入临界段，临界段可以嵌套 */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
  //确保是否产生了EXTI Line中断
	if(__HAL_GPIO_EXTI_GET_IT(KEY2_INT_GPIO_PIN) != RESET) 
	{
    /* 将数据写入（发送）到队列中，等待时间为 0  */
		xQueueSendFromISR(Test_Queue, /* 消息队列的句柄 */
											&send_data2,/* 发送的消息内容 */
											&pxHigherPriorityTaskWoken);
		
		//如果需要的话进行一次任务切换
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
		
		//清除中断标志位
		__HAL_GPIO_EXTI_CLEAR_IT(KEY2_INT_GPIO_PIN);  
	}  
  
  /* 退出临界段 */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

/*********************************************************************************
  * @ 函数名  ： DEBUG_USART_IRQHandler
  * @ 功能说明： 串口中断服务函数
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  ********************************************************************************/
void DEBUG_USART_IRQHandler(void)
{
	uint32_t ulReturn;
	/* 进入临界段，临界段可以嵌套 */
	ulReturn = taskENTER_CRITICAL_FROM_ISR();

	if(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_IDLE)!=RESET)
	{
		//空闲中断产生，说明接收完成，停止串口接收，此函数会停止DMA的接收，并调用HAL_UART_AbortReceiveCpltCallback
		HAL_UART_AbortReceive_IT(&UartHandle);
		//清除空闲中断标志位
		__HAL_UART_CLEAR_IDLEFLAG(&UartHandle);
	}
	
	/* 退出临界段 */
	taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

void KEY3_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(KEY3_INT_GPIO_PIN);
}
void KEY4_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(KEY4_INT_GPIO_PIN);
}
void KEY5_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(KEY5_INT_GPIO_PIN);
}

void DMA2_Stream5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UartHandle.hdmarx);
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
