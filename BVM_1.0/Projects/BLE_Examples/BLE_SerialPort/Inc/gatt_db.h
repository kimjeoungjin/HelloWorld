

#ifndef _GATT_DB_H_
#define _GATT_DB_H_

#define CPR_START			0xF3
#define CPR_STOP			0xF1

tBleStatus Add_Bvm_Service(void);


uint8_t Add_Bvm_Service(void);
tBleStatus Breath_Update( uint16_t breath_value );
tBleStatus Bat_Update( uint8_t bat_value );
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data);


#endif /* _GATT_DB_H_ */
