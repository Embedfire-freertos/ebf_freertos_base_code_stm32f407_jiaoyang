/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 ����ģ��
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32ȫϵ�п����� 
  * ��̳    :http://www.embedfire.com
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/* ������Ӳ��bspͷ�ļ� */
#include "board_init.h"

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle;
static TaskHandle_t LowPriority_Task_Handle = NULL;/* LowPriority_Task������ */
static TaskHandle_t MidPriority_Task_Handle = NULL;/* MidPriority_Task������ */
static TaskHandle_t HighPriority_Task_Handle = NULL;/* HighPriority_Task������ */
/********************************** �ں˶����� *********************************/
/*
 * ����������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
SemaphoreHandle_t MuxSem_Handle =NULL;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
 
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void LowPriority_Task(void* pvParameters);/* LowPriority_Task����ʵ�� */
static void MidPriority_Task(void* pvParameters);/* MidPriority_Task����ʵ�� */
static void HighPriority_Task(void* pvParameters);/* MidPriority_Task����ʵ�� */

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

/******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */


/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  /* ������Ӳ����ʼ�� */
  BSP_Init();
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS������ʵ�飡\n");
   /* ���� AppTaskCreate ���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
															
	if(pdFAIL != xReturn)/* �����ɹ� */
    vTaskStartScheduler();   /* �������񣬿������� */
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  taskENTER_CRITICAL();           //�����ٽ���

  /* ������ֵ������ */
  MuxSem_Handle = xSemaphoreCreateMutex();	 
  if(NULL != MuxSem_Handle)
    printf("MuxSem_Handle�����������ɹ�!\r\n");
	xSemaphoreGive( MuxSem_Handle );//������ֵ������
  /* ����LowPriority_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LowPriority_Task, /* ������ں��� */
                        (const char*    )"LowPriority_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LowPriority_Task_Handle);/* ������ƿ�ָ�� */
	
	if(pdFAIL != xReturn)/* �����ɹ� */
		printf("���� LowPriority_Task ����ɹ�!\n");
	else
		printf("���� LowPriority_Task ����ʧ��!\n");

  /* ����MidPriority_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )MidPriority_Task,  /* ������ں��� */
                        (const char*    )"MidPriority_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&MidPriority_Task_Handle);/* ������ƿ�ָ�� */ 

	if(pdFAIL != xReturn)/* �����ɹ� */
		printf("���� MidPriority_Task ����ɹ�!\n");
	else
		printf("���� MidPriority_Task ����ʧ��!\n");

  /* ����HighPriority_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )HighPriority_Task,  /* ������ں��� */
                        (const char*    )"HighPriority_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )4, /* ��������ȼ� */
                        (TaskHandle_t*  )&HighPriority_Task_Handle);/* ������ƿ�ָ�� */ 

	if(pdFAIL != xReturn)/* �����ɹ� */
		printf("���� HighPriority_Task ����ɹ�!\n");
	else
		printf("���� HighPriority_Task ����ʧ��!\n");

  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



/**********************************************************************
  * @ ������  �� LowPriority_Task
  * @ ����˵���� LowPriority_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LowPriority_Task(void* parameter)
{	
  static uint32_t i;
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  while (1)
  {
    printf("LowPriority_Task ��ȡ������\n");
    //��ȡ������ MuxSem,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(MuxSem_Handle,/* ��������� */
                              portMAX_DELAY); /* �ȴ�ʱ�� */
    if( xReturn == pdTRUE )
      printf("LowPriority_Task Runing\n\n");
    
    for(i=0;i<4000000;i++)//ģ������ȼ�����ռ�û�����
		{
			taskYIELD();//�����������
		}
    
    printf("LowPriority_Task �ͷŻ�����!\r\n");
    xReturn = xSemaphoreGive( MuxSem_Handle );//������ֵ������
    
		LED1_TOGGLE;
    
    vTaskDelay(1000);
  }
}

/**********************************************************************
  * @ ������  �� MidPriority_Task
  * @ ����˵���� MidPriority_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void MidPriority_Task(void* parameter)
{	 
  while (1)
  {
   printf("MidPriority_Task Runing\n");
   vTaskDelay(1000);
  }
}

/**********************************************************************
  * @ ������  �� HighPriority_Task
  * @ ����˵���� HighPriority_Task ��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void HighPriority_Task(void* parameter)
{	
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  while (1)
  {
    printf("HighPriority_Task ��ȡ������\n");
    //��ȡ������ MuxSem,û��ȡ����һֱ�ȴ�
		xReturn = xSemaphoreTake(MuxSem_Handle,/* ��ֵ��������� */
                              portMAX_DELAY); /* �ȴ�ʱ�� */
    if(pdTRUE == xReturn)
      printf("HighPriority_Task Runing\n");
		
		LED1_TOGGLE;
		
		printf("HighPriority_Task �ͷŻ�����!\r\n");
		
    xReturn = xSemaphoreGive( MuxSem_Handle );//������ֵ������

    vTaskDelay(1000);
  }
}

/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	
	/* ��ʼ��ϵͳʱ�� */
	SystemClock_Config();
	
	/* �������ȼ�����Ϊ4 */
	HAL_NVIC_SetPriorityGrouping( NVIC_PRIORITYGROUP_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();
	
	/* KEY ��ʼ�� */
	Key_GPIO_Config();
	
	/* ���ڳ�ʼ��	*/
	DEBUG_USART_Config();
  
}

/********************************END OF FILE****************************/
