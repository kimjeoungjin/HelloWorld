
#include <stdio.h>
#include <string.h>
#include "ble_const.h" 
#include "bluenrg_lp_stack.h"
#include "osal.h"
#include "app_state.h"
#include "serial_port.h"
#include "SerialPort_config.h"
#include "gatt_db.h"


uint16_t BvmHandle, BreathCharHandle, BatCharHandle, CmdCharHandle;

extern BOOL Connection_Flag;
extern uint16_t connection_handle;

void CMD_From_Client ( uint8_t cmd_bvm )
{
	switch ( cmd_bvm )
	{
		case CPR_START : 
			Connection_Flag = TRUE;
			break;
		case CPR_STOP :
			Connection_Flag = FALSE;
			break;
	}	
}
	
BLE_GATT_SRV_CCCD_DECLARE(breath,
                     NUM_LINKS,
                     BLE_GATT_SRV_CCCD_PERM_DEFAULT,
                     BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

//BLE_GATT_SRV_CCCD_DECLARE(bat,
//                     NUM_LINKS,
//                     BLE_GATT_SRV_CCCD_PERM_DEFAULT,
//                     BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

//BLE_GATT_SRV_CCCD_DECLARE(cmd,
//                     NUM_LINKS,
//                     BLE_GATT_SRV_CCCD_PERM_DEFAULT,
//                     BLE_GATT_SRV_OP_MODIFIED_EVT_ENABLE_FLAG);

					 
static const ble_gatt_chr_def_t bvm_chars[] = {
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_NOTIFY | BLE_GATT_SRV_CHAR_PROP_READ,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = BLE_GATT_SRV_MAX_ENCRY_KEY_SIZE,
        .uuid = BLE_UUID_INIT_128(BVM_BREATH_UUID),
        .descrs = {
            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(breath),
            .descr_count = 1U,
        },
    },
    {
        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE | BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP,
        .permissions = BLE_GATT_SRV_PERM_NONE,
        .min_key_size = BLE_GATT_SRV_MAX_ENCRY_KEY_SIZE,
        .uuid = BLE_UUID_INIT_128(BVM_CMD_UUID),
//        .descrs = {
//            .descrs_p = &BLE_GATT_SRV_CCCD_DEF_NAME(bat),
//            .descr_count = 1U,
//        },
    },
//    {
//        .properties = BLE_GATT_SRV_CHAR_PROP_WRITE | BLE_GATT_SRV_CHAR_PROP_WRITE_NO_RESP,
//        .permissions = BLE_GATT_SRV_PERM_NONE,
//        .min_key_size = BLE_GATT_SRV_MAX_ENCRY_KEY_SIZE,
//        .uuid = BLE_UUID_INIT_128(BVM_CMD_UUID),

//    }
};

static const ble_gatt_srv_def_t bvm_service = {
   .type = BLE_GATT_SRV_PRIMARY_SRV_TYPE,
   .uuid = BLE_UUID_INIT_128(BVM_SERVICE_UUID),
   .chrs = {
       .chrs_p = (ble_gatt_chr_def_t *)bvm_chars,
       .chr_count = 2U,
   },
}; 



uint8_t Add_Bvm_Service(void)
{
	uint8_t ret;

	ret = aci_gatt_srv_add_service((ble_gatt_srv_def_t *)&bvm_service);
	if (ret != BLE_STATUS_SUCCESS)
	{
	
	}
	BvmHandle = aci_gatt_srv_get_service_handle( (ble_gatt_srv_def_t*)&bvm_service );
	BreathCharHandle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&bvm_chars[0]);
	CmdCharHandle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&bvm_chars[1]);
//	CmdCharHandle = aci_gatt_srv_get_char_decl_handle((ble_gatt_chr_def_t *)&bvm_chars[2]);

	return BLE_STATUS_SUCCESS; 
}


tBleStatus Breath_Update( uint16_t breath_value )
{  
	uint8_t buff[2];
	tBleStatus ret;

	buff[ 0 ] = ( breath_value & 0xFF ) ;
	buff[ 1 ] = (breath_value & 0xFF00 ) >> 8;
	
	ret = aci_gatt_srv_notify(connection_handle, BreathCharHandle + 1, 0, 2, buff);
	if (ret != BLE_STATUS_SUCCESS)
	{
	
	}

	return BLE_STATUS_SUCCESS;
}



tBleStatus Bat_Update( uint8_t bat_value )
{  
	uint8_t buff[1];
	tBleStatus ret;

	buff[ 0 ] = bat_value;

	ret = aci_gatt_srv_notify(connection_handle, BatCharHandle + 1, 0, 1, buff);
	if (ret != BLE_STATUS_SUCCESS)
	{
	
	}

	return BLE_STATUS_SUCCESS;
}

void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
	if(handle == CmdCharHandle + 1)
	{
		CMD_From_Client(att_data[0]);
	}
}

