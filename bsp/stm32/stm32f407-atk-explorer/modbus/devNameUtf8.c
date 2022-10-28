#include  "devNameUtf8.h"
char name1[]="接地环流";
char name2[]="局放";
char name3[]="沉降仪";
char name4[]="三轴测振仪";


//后期需要从flash中读取ID  需要给所有设备编号 
void  devIDFlashRead()
{
		rt_strcpy(mcu.devID  ,"1000000000001");
		rt_strcpy(devi[0].ID,"A000000000001");
		rt_strcpy(devi[0].model,"GY280");
		rt_strcpy(devi[0].type,"1");
		rt_strcpy(devi[0].name,name1);

		rt_strcpy(devi[1].ID,"A000000000002");
		rt_strcpy(devi[1].model,"GY281");
		rt_strcpy(devi[1].type,"2");
		rt_strcpy(devi[1].name,name2);

		rt_strcpy(devi[2].ID,"A000000000003");
		rt_strcpy(devi[2].model,"GY282");
		rt_strcpy(devi[2].type,"3");
		rt_strcpy(devi[2].name,name3);

		rt_strcpy(devi[3].ID,"A000000000004");
		rt_strcpy(devi[3].model,"GY283");
		rt_strcpy(devi[3].type,"4");
	  rt_strcpy(devi[3].name,name4);
//		rt_strcpy(devi[2].ID,"A000000000003");
//		rt_strcpy(devi[3].ID,"A000000000004");
}
