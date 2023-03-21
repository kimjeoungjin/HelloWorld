/******************** (C) COPYRIGHT 2020 STMicroelectronics ********************
* File Name          : serial_port.c
* Author             : AMS - VMA RF  Application team
* Version            : V3.0.0
* Date               : 23-January-2020
* Description        : This file handles bytes received from USB and the init
*                      function.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "gp_timer.h" 
#include "ble_const.h" 
#include "bluenrg_lp_stack.h"
#include "app_state.h"
#include "osal.h"
#include "gatt_db.h"
#include "serial_port.h"
#include "gap_profile.h"
#if defined (CONFIG_OTA_LOWER) || defined (CONFIG_OTA_HIGHER)
#include "OTA_btl.h" 
#endif
#include "bluenrg_lp_evb_com.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#define CMD_BUFF_SIZE 512

#define ADV_INTERVAL_MIN    ((uint32_t)(100/0.625))     // 100 ms
#define ADV_INTERVAL_MAX    ((uint32_t)(100/0.625))     // 100 ms
#define SCAN_INTERVAL       ((uint16_t)(400/0.625))     // 400 ms
#define SCAN_WINDOW         ((uint16_t)(100/0.625))     // 100 ms
#define CONN_INTERVAL_MIN   ((uint16_t)(20/1.25))       // 20 ms
#define CONN_INTERVAL_MAX   ((uint16_t)(20/1.25))       // 20 ms
#define SUPERVISION_TIMEOUT ((uint16_t)(1000/10))       // 1000 ms
#define CE_LENGTH           ((uint16_t)(20/0.625))      // 20 ms

#define BD_ADDR_MASTER      0xa1, 0x00, 0x00, 0xE1, 0x80, 0x02
#define BD_ADDR_SLAVE       0xa2, 0x00, 0x00, 0xE1, 0x80, 0x02

#define LOCAL_NAME  'B','V','M','_','V','0','0','1'					// 8characters


#define MANUF_DATA_SIZE (30)


/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

uint8_t	from_app[];
uint8_t	command_received;

extern BOOL Connection_Flag;

uint8_t connInfo[20];
volatile int app_flags = SET_CONNECTABLE;
volatile uint16_t connection_handle = 0;
#ifndef CLIENT
static Advertising_Set_Parameters_t Advertising_Set_Parameters[1];
#endif

/* UUIDs */
UUID_t UUID_Tx;
UUID_t UUID_Rx;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Serial_port_DeviceInit.
* Description    : Init the Serial_port device.
* Input          : none.
* Return         : Status.
*******************************************************************************/
uint8_t Serial_port_DeviceInit(void)
{
	uint8_t ret;
	uint16_t service_handle;
	uint16_t dev_name_char_handle;
	uint16_t appearance_char_handle;
	uint8_t name[] = {'B', 'V', 'M', '-', 'T', 'E', 'S', 'T','1'};

	uint8_t role = GAP_PERIPHERAL_ROLE;
	uint8_t bdaddr[] = {BD_ADDR_SLAVE};

	ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
	if(ret != BLE_STATUS_SUCCESS)
	{
		return ret;
	}
	
	aci_hal_set_tx_power_level(1, 25);				//+8db up
	
	ret = aci_gatt_srv_init();
	if (ret != BLE_STATUS_SUCCESS) 
	{
		return ret;
	}
  
	ret = aci_gap_init(role, 0x00, 0x09, PUBLIC_ADDR, &service_handle, &dev_name_char_handle, &appearance_char_handle);
	if (ret != BLE_STATUS_SUCCESS) 
	{
		return ret;
	} 

	ret = Gap_profile_set_dev_name(0, sizeof(name), name);
	if (ret != BLE_STATUS_SUCCESS) 
	{
		return ret;
	}
	
	ret = Add_Bvm_Service();
	if (ret != BLE_STATUS_SUCCESS) 
	{
		return ret;
	}
  
	ret = aci_gap_set_advertising_configuration(0, 
									GAP_MODE_GENERAL_DISCOVERABLE,
									ADV_PROP_CONNECTABLE|ADV_PROP_SCANNABLE|ADV_PROP_LEGACY,
									ADV_INTERVAL_MIN, ADV_INTERVAL_MAX,
									ADV_CH_ALL,
									0,
									NULL,
									ADV_NO_WHITE_LIST_USE,
									0, /* 0 dBm */
									LE_1M_PHY, /* Primary advertising PHY */
									0, /* 0 skips */
									LE_1M_PHY, /* Secondary advertising PHY. Not used with legacy advertising. */
									0, /* SID */
									0 /* No scan request notifications */
	);


	static uint8_t manuf_data[MANUF_DATA_SIZE] =
	{ 
		0x02,
		AD_TYPE_FLAGS,
		FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE|FLAG_BIT_BR_EDR_NOT_SUPPORTED,
		2,                      /* Length of AD type Transmission Power */
		0x0A, 0x00,             /* Transmission Power = 0 dBm */ 
		9,                      /* Length of AD type Complete Local Name */
		0x09,                   /* AD type Complete Local Name */ 
		LOCAL_NAME,             /* Local Name */            
		13,                     /* Length of AD type Manufacturer info */
		0xFF,                   /* AD type Manufacturer info */
		0x01,                   /* Protocol version */
		0x05,		      /* Device ID: 0x05 = STEVAL-BCN002V1 Board */
		0x00,                   /* Feature Mask byte#1 */
		0x00,                   /* Feature Mask byte#2 */
		0x00,                   /* Feature Mask byte#3 */
		0x00,                   /* Feature Mask byte#4 */
		0x00,                   /* BLE MAC start */
		0x00, 
		0x00, 
		0x00, 
		0x00, 
		0x00                    /* BLE MAC stop */
	};

	for (int var = 0; var < 6; ++var) 
	{
		manuf_data[MANUF_DATA_SIZE - 1 - var] = bdaddr[var];
	}

	ret = aci_gap_set_advertising_data(0, ADV_COMPLETE_DATA, sizeof(manuf_data), manuf_data);
	
	return BLE_STATUS_SUCCESS;
	
}

void Send_Data_Over_BLE(void)
{
	if(!APP_FLAG(SEND_DATA) || APP_FLAG(TX_BUFFER_FULL))
	{  
		return;
	}

	APP_FLAG_CLEAR(SEND_DATA);

}

void Make_Connection(void)
{  
	tBleStatus ret;

	Advertising_Set_Parameters[0].Advertising_Handle = 0;
	Advertising_Set_Parameters[0].Duration = 0;
	Advertising_Set_Parameters[0].Max_Extended_Advertising_Events = 0;

	ret = aci_gap_set_advertising_enable(ENABLE, 1, Advertising_Set_Parameters); 
	if (ret != BLE_STATUS_SUCCESS)
	{

	}
}

void APP_Tick(void)
{
  
	if(APP_FLAG(SET_CONNECTABLE))
	{
		Make_Connection();
		
		APP_FLAG_CLEAR(SET_CONNECTABLE);
	}

	Send_Data_Over_BLE();
 
}


/* ***************** BlueNRG-1 Stack Callbacks ********************************/

/*******************************************************************************
 * Function Name  : hci_le_connection_complete_event.
 * Description    : This event indicates that a new connection has been created.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)
{
	LL_GPIO_TogglePin( GPIOA, LL_GPIO_PIN_1 );
	

	if(Status != BLE_STATUS_SUCCESS)
	return;

	connection_handle = Connection_Handle;

	APP_FLAG_SET(CONNECTED);

}/* end hci_le_connection_complete_event() */

/*******************************************************************************
 * Function Name  : hci_le_enhanced_connection_complete_event.
 * Description    : This event indicates that a new connection has been created
 * Input          : See file bluenrg_lp_events.h
 * Output         : See file bluenrg_lp_events.h
 * Return         : See file bluenrg_lp_events.h
 *******************************************************************************/
void hci_le_enhanced_connection_complete_event(uint8_t Status,
                                               uint16_t Connection_Handle,
                                               uint8_t Role,
                                               uint8_t Peer_Address_Type,
                                               uint8_t Peer_Address[6],
                                               uint8_t Local_Resolvable_Private_Address[6],
                                               uint8_t Peer_Resolvable_Private_Address[6],
                                               uint16_t Conn_Interval,
                                               uint16_t Conn_Latency,
                                               uint16_t Supervision_Timeout,
                                               uint8_t Master_Clock_Accuracy)
{
 
	LL_GPIO_TogglePin( GPIOB, LL_GPIO_PIN_7 );
	hci_le_connection_complete_event(Status,
								   Connection_Handle,
								   Role,
								   Peer_Address_Type,
								   Peer_Address,
								   Conn_Interval,
								   Conn_Latency,
								   Supervision_Timeout,
								   Master_Clock_Accuracy);
}

/*******************************************************************************
 * Function Name  : hci_disconnection_complete_event.
 * Description    : This event occurs when a connection is terminated.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
	LL_GPIO_TogglePin( GPIOB, LL_GPIO_PIN_15 );
	
	
	APP_FLAG_CLEAR(CONNECTED);
	/* Make the device connectable again. */
	APP_FLAG_SET(SET_CONNECTABLE);
	APP_FLAG_CLEAR(NOTIFICATIONS_ENABLED);
	APP_FLAG_CLEAR(TX_BUFFER_FULL);

	APP_FLAG_CLEAR(START_READ_TX_CHAR_HANDLE);
	APP_FLAG_CLEAR(END_READ_TX_CHAR_HANDLE);
	APP_FLAG_CLEAR(START_READ_RX_CHAR_HANDLE); 
	APP_FLAG_CLEAR(END_READ_RX_CHAR_HANDLE);  
}/* end hci_disconnection_complete_event() */


/*******************************************************************************
 * Function Name  : aci_gatt_srv_attribute_modified_event.
 * Description    : This event occurs when an attribute is modified.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
uint8_t ssss = 0;
void aci_gatt_srv_attribute_modified_event(uint16_t Connection_Handle,
                                           uint16_t Attr_Handle,
                                           uint16_t Attr_Data_Length,
                                           uint8_t Attr_Data[])
{
	Attribute_Modified_CB(Attr_Handle, Attr_Data_Length, Attr_Data);
	ssss = Attr_Data[0];
}

#if CLIENT

/*******************************************************************************
 * Function Name  : aci_gatt_clt_notification_event.
 * Description    : This event occurs when a notification is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_clt_notification_event(uint16_t Connection_Handle,
                                     uint16_t Attribute_Handle,
                                     uint16_t Attribute_Value_Length,
                                     uint8_t Attribute_Value[])
{ 
#if CLIENT
  uint16_t attr_handle;
 
  attr_handle = Attribute_Handle;
    if(attr_handle == tx_handle+1)
    {
      for(int i = 0; i < Attribute_Value_Length; i++) 
          printf("%c", Attribute_Value[i]);
    }
    else
    {
        printf("Received data from not recognized attribute handle 0x%4X\n", Attribute_Handle);
    }
#endif
}

/*******************************************************************************
 * Function Name  : aci_gatt_clt_disc_read_char_by_uuid_resp_event.
 * Description    : This event occurs when a discovery read characteristic by UUID response.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_clt_disc_read_char_by_uuid_resp_event(uint16_t Connection_Handle,
                                                uint16_t Attribute_Handle,
                                                uint8_t Attribute_Value_Length,
                                                uint8_t Attribute_Value[])
{
  printf("aci_gatt_clt_disc_read_char_by_uuid_resp_event, Connection Handle: 0x%04X\n", Connection_Handle);
  if (APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    tx_handle = Attribute_Handle;
    printf("TX Char Handle 0x%04X\n", tx_handle);
  }
  else if (APP_FLAG(START_READ_RX_CHAR_HANDLE) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
  {
    rx_handle = Attribute_Handle;
    printf("RX Char Handle 0x%04X\n", rx_handle);
  }
}

/*******************************************************************************
 * Function Name  : aci_gatt_clt_proc_complete_event.
 * Description    : This event occurs when a GATT procedure complete is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_clt_proc_complete_event(uint16_t Connection_Handle,
                                      uint8_t Error_Code)
{ 
  if (APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    printf("aci_GATT_PROCEDURE_COMPLETE_Event for START_READ_TX_CHAR_HANDLE \r\n");
    APP_FLAG_SET(END_READ_TX_CHAR_HANDLE);
  }
  else if (APP_FLAG(START_READ_RX_CHAR_HANDLE) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
  {
    printf("aci_GATT_PROCEDURE_COMPLETE_Event for START_READ_RX_CHAR_HANDLE \r\n");
    APP_FLAG_SET(END_READ_RX_CHAR_HANDLE);
  }
}

#endif /* CLIENT */

/*******************************************************************************
 * Function Name  : aci_gatt_tx_pool_available_event.
 * Description    : This event occurs when a TX pool available is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_tx_pool_available_event(uint16_t Connection_Handle,
                                      uint16_t Available_Buffers)
{       
  /* It allows to notify when at least 2 GATT TX buffers are available */
  APP_FLAG_CLEAR(TX_BUFFER_FULL);
} 

void aci_hal_end_of_radio_activity_event(uint8_t Last_State,
                                         uint8_t Next_State,
                                         uint32_t Next_State_SysTime)
{

}


void aci_gatt_srv_write_event(uint16_t Connection_Handle,
                                 uint8_t Resp_Needed,
                                 uint16_t Attribute_Handle,
                                 uint16_t Data_Length,
                                 uint8_t Data[])
{
//    uint8_t att_error = BLE_ATT_ERR_NONE;
//    uint16_t i;

//    if(Attribute_Handle == RXCharHandle + 1)
//    {
//        for(i = 0U; i < Data_Length; i++)
//        {
//            from_app[i] = Data[i];					// june 220719
//        }
//        att_error = BLE_ATT_ERR_NONE;
//        command_received = 1;						// june 220719
//    }

//    if (Resp_Needed == 1U)
//    {
//        aci_gatt_srv_resp(Connection_Handle, Attribute_Handle, att_error, 0,  NULL);
//    }
}

void aci_gatt_srv_read_event(uint16_t Connection_Handle, uint16_t Attribute_Handle, uint16_t Data_Offset)
{

	
}


