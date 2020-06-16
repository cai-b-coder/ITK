

#include "epm_handler_common.h"
#include <ae/ae.h>
#include <tccore\item.h>
#include <stdio.h>
#include <tc\emh.h>
#include <tc\preferences.h>
#include <epm\epm_toolkit_tc_utils.h>
#include <tccore\item.h>
#include <tccore\grm.h>
#include <tccore\imantype.h>
#include <tccore\iman_grmtype.h>
#include <tccore\aom.h>
#include <property\prop_errors.h>
#include <qry\qry.h>
#include <epm\cr.h>
#include <bom/bom.h>
#include <stdio.h>
#include <string>
#include <iterator>
#include<iostream>
#include <vector>
#include<Windows.h>
#include<ics\ics.h>
#include<ics\ics2.h>
#pragma warning(disable : 4996)
#include <tccore\aom_prop.h>


extern "C" int POM_AM__set_application_bypass(logical bypass);
using namespace std;


int jl_insert_modular(EPM_action_message_t msg)
{
	printf("发送开始");
	int ifail = ITK_ok, att_cnt = 0,i=0, item_mast_rev_count = 0,ico_att_count ,*ico_id_count,*ico_values_count;
	tag_t task_tag = NULLTAG,
		rootTask_tag = NULLTAG,
		type_tag = NULLTAG,
		old_ico_tag = NULLTAG,
		new_ico_tag = NULLTAG,
		item_class_tag = NULLTAG,
		item_class_tag2 = NULLTAG,
		rev_tag = NULLTAG, classificationObject;

	tag_t  *attachments = NULLTAG, *item_mast_rev = NULLTAG ;
	logical verdict;
	
	char* type_value = NULL, * bom_value = NULL, * orderType_value = NULL,  *item_id = NULL,**ico_values,**ico_attribute,***ico_values2,
		oldClassID[TCTYPE_class_name_size_c + 1], newClassID[TCTYPE_class_name_size_c + 1];
	//获取流程
	task_tag = msg.task;
	//获取根流程
	ifail = EPM_ask_root_task(task_tag,&rootTask_tag);
	//获取目标对象
	ifail = EPM_ask_attachments(rootTask_tag, EPM_target_attachment, &att_cnt, &attachments);

	for (i = 0; i < att_cnt; i++) {
		printf("循环开始");
		//得到目标对象的类型
		ifail = TCTYPE_ask_object_type(attachments[i], &type_tag); 
		//得到目标对象的最新版本
		//ifail = ITEM_ask_item_of_rev(attachments[i],&rev_tag);
		ifail = ITEM_ask_latest_rev(attachments[i], &rev_tag);
		
		//AOM_UIF_ask_value();
		//ITEM_ask_type();
		//ITEM_ask_rev_master_form_type
		ifail = AOM_ask_value_string(attachments[i], "object_type", &type_value); //得到item的类型
		printf("object_type为：%s\n", type_value);
		ifail = AOM_ask_value_string(attachments[i], "item_id", &item_id); //得到item的id
		printf("item_id为：%s\n", item_id);

		//得到版本下的主表单
		AOM_ask_value_tags(rev_tag, "IMAN_master_form_rev", &item_mast_rev_count, &item_mast_rev);
		//printf("test为：%s\n", item_mast_rev[0]);
		
		AOM_ask_value_string(item_mast_rev[0], "user_data_1", &bom_value);//订单属性“bom状态”的值
		AOM_ask_value_string(item_mast_rev[0], "gd6_ddlx", &orderType_value);//订单类型的值
		printf("user_data_1为：%s\n", bom_value);
		printf("user_data_2为：%s\n", orderType_value);


		//得到分类的id(通过订单类型的值)
		if (strcmp(orderType_value, "车身") == 0) {
			strcpy(oldClassID,"jlcs010201");
			strcpy(newClassID,"jlcs010101");
			printf("分类id为： %s", oldClassID);
		}
		else if (strcmp(orderType_value, "配件") == 0)
		{
			strcpy(oldClassID, "jlcs010202");
			strcpy(newClassID, "jlcs010102");
			printf("分类id为： %s", oldClassID);
		}
		else if (strcmp(orderType_value, "KD") == 0)
		{
			strcpy(oldClassID, "jlcs010203");
			strcpy(newClassID, "jlcs010103");
			printf("分类id为： %s", oldClassID);
		}

		if (strcmp(type_value,"C5_Order")!=0) {
			printf("该对象的类型不是订单 \n");
		}
		else {
			printf("该对象的类型是订单 \n");
			
		}
		
		//ITK_ask_cli_argument();
		//ITK__convert_uid_to_tag();
		POM_AM__set_application_bypass(true);
		if (strcmp(bom_value,"下发成功")==0) {
				//ICS_find_class()
				//ITK__convert_uid_to_tag
			printf("分类开始 \n");
			//ICS_ask_classification_object(attachments[i],&itemClass_tag); //通过item得到item的分类对象

			
			ICS_is_wsobject_classified(attachments[i],&verdict); //此函数检查给定的工作区对象是否已分类。
			
			/*这将根据对象id、视图/类标记和工作空间对象创建一个新的分类对象。
			如果ws_object不是#NULLTAG，ICO将对该对象进行分类。在这种情况下，
			对象的id将被忽略，但是ws_对象的id将被用作ICO id。
		    如果ws_object是#NULLTAG，则将创建一个ICO，其id为object#id。
			注意：如果要为支持两个单元系统的类创建ICO，请使用ICS ICO*/
			//ICS_create_classification_object();
			
			//函数获取由给定ID字符串标识的组/类的标记。
			ICS_find_class(oldClassID, &old_ico_tag);  //原本分类的tag id（流程中订单）
			ICS_find_class(newClassID, &new_ico_tag);	//（历史订单中的分类tag id）
			printf("类的tag： %d\n", old_ico_tag);
			cout << new_ico_tag << endl;
			//cout << itemClass_tag << endl;

			ICS_ask_classification_object(attachments[i],&item_class_tag);

			//ICS_attribute_convert_attribute_values();
			

			ICS_ico_ask_attributes(item_class_tag,&ico_att_count ,&ico_id_count, &ico_values_count,&ico_values2);
			cout << ico_att_count << endl;
			cout << *ico_id_count << endl;
			cout << *ico_values_count << endl;
			cout << ico_values2 << endl;
			//ICS_ask_attributes_of_classification_obj(item_class_tag, &ico_att_count, &ico_attribute,&ico_values);
			//ICS_attribute_set_property();
			//ICS_attribute_ask_id;

			//ICS_remove_classification() //全部删除（item和分类中的对象）
			//ICS_delete_reference_to_wsobject
			ICS_delete_reference_to_ico(item_class_tag);
			
			ICS_delete_classification_object(item_class_tag);

			//char** value = (char**)MEM_alloc(sizeof(char*));

			ICS_create_classification_object(attachments[i],item_id, new_ico_tag, &classificationObject);
			//此函数对给定的工作区对象进行分类，即将工作区对象与使用IMAN_分类关系的给定分类对象。
			ICS_classify_wsobject(attachments[i], classificationObject);
			//ICS_ico_ask_attributes(item_class_tag, &ico_att_count, &ico_id_count, &ico_values_count, &ico_values2);

			ICS_ask_classification_object(attachments[i],&item_class_tag2);
			ICS_ico_set_attributes(item_class_tag2, ico_att_count, ico_id_count, ico_values_count,(const char*** )ico_values2);
			printf("对象移到历史订单成功");	
			POM_AM__set_application_bypass(false);
			//MEM_free(oldClassID);
			//MEM_free(newClassID);
		}
		else {
			printf("对象发送到分类失败");
		}
	}

	return ifail;
}
////获取顶层 REVISION 对应的分类
//ICS_ask_classification_object(top_rev_tag,&top_classificationObject);
//if(top_classificationObject == NULL_TAG)
//{
//printf("顶层对象没有发送到分类\n");
//return  0;
//}
//ICS_ask_class_of_classification_obj(top_classificationObject,&top_class_tag);
//ICS_ask_id_name(top_class_tag, &top_class_id, &top_class_name);
//char *revbuf1[8] = { 0 };
//int num1 = 0;
//split(bl_formatted_parent_name, "/", revbuf1, &num1);
//ICS_create_classification_object
//ICS_is_wsobject_classified(
//    ICS_classify_wsobject
//AOM_UIF_ask_value();

