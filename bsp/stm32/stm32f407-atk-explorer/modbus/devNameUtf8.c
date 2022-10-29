#include  "devNameUtf8.h"
char name1[]="接地环流";
char name2[]="局放";
char name3[]="沉降仪";
char name4[]="三轴测振仪";
//与 modbusName的汉字相同 不过此处为UFT8格式

//后期需要从flash中读取ID  需要给所有设备编号 
void  devIDFlashRead()
{
	  //MODBUS_NUM
	
	
	
	  STMFLASH_Read(FLASH_SAVE_ADDR,(uint32_t*)modbusFlash,sizeof(modbusFlash));
		rt_strcpy(mcu.devID  ,"1000000000001");
		rt_strcpy(devi[CIRCULA].ID,"A000000000001");
		rt_strcpy(devi[CIRCULA].model,"GY280");
		rt_strcpy(devi[CIRCULA].type,"1");
		rt_strcpy(devi[CIRCULA].name,name1);

		rt_strcpy(devi[PARTDISCHAG].ID,"A000000000002");
		rt_strcpy(devi[PARTDISCHAG].model,"GY281");
		rt_strcpy(devi[PARTDISCHAG].type,"2");
		rt_strcpy(devi[PARTDISCHAG].name,name2);

		rt_strcpy(devi[PRESSSETTL].ID,"A000000000003");
		rt_strcpy(devi[PRESSSETTL].model,"GY282");
		rt_strcpy(devi[PRESSSETTL].type,"3");
		rt_strcpy(devi[PRESSSETTL].name,name3);

		rt_strcpy(devi[THREEAXIS].ID,"A000000000004");
		rt_strcpy(devi[THREEAXIS].model,"GY283");
		rt_strcpy(devi[THREEAXIS].type,"4");
	  rt_strcpy(devi[THREEAXIS].name,name4);
		
		
		

//		rt_strcpy(devi[2].ID,"A000000000003");
//		rt_strcpy(devi[3].ID,"A000000000004");
}
