

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
	printf("���Ϳ�ʼ");
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
	//��ȡ����
	task_tag = msg.task;
	//��ȡ������
	ifail = EPM_ask_root_task(task_tag,&rootTask_tag);
	//��ȡĿ�����
	ifail = EPM_ask_attachments(rootTask_tag, EPM_target_attachment, &att_cnt, &attachments);

	for (i = 0; i < att_cnt; i++) {
		printf("ѭ����ʼ");
		//�õ�Ŀ����������
		ifail = TCTYPE_ask_object_type(attachments[i], &type_tag); 
		//�õ�Ŀ���������°汾
		//ifail = ITEM_ask_item_of_rev(attachments[i],&rev_tag);
		ifail = ITEM_ask_latest_rev(attachments[i], &rev_tag);
		
		//AOM_UIF_ask_value();
		//ITEM_ask_type();
		//ITEM_ask_rev_master_form_type
		ifail = AOM_ask_value_string(attachments[i], "object_type", &type_value); //�õ�item������
		printf("object_typeΪ��%s\n", type_value);
		ifail = AOM_ask_value_string(attachments[i], "item_id", &item_id); //�õ�item��id
		printf("item_idΪ��%s\n", item_id);

		//�õ��汾�µ�����
		AOM_ask_value_tags(rev_tag, "IMAN_master_form_rev", &item_mast_rev_count, &item_mast_rev);
		//printf("testΪ��%s\n", item_mast_rev[0]);
		
		AOM_ask_value_string(item_mast_rev[0], "user_data_1", &bom_value);//�������ԡ�bom״̬����ֵ
		AOM_ask_value_string(item_mast_rev[0], "gd6_ddlx", &orderType_value);//�������͵�ֵ
		printf("user_data_1Ϊ��%s\n", bom_value);
		printf("user_data_2Ϊ��%s\n", orderType_value);


		//�õ������id(ͨ���������͵�ֵ)
		if (strcmp(orderType_value, "����") == 0) {
			strcpy(oldClassID,"jlcs010201");
			strcpy(newClassID,"jlcs010101");
			printf("����idΪ�� %s", oldClassID);
		}
		else if (strcmp(orderType_value, "���") == 0)
		{
			strcpy(oldClassID, "jlcs010202");
			strcpy(newClassID, "jlcs010102");
			printf("����idΪ�� %s", oldClassID);
		}
		else if (strcmp(orderType_value, "KD") == 0)
		{
			strcpy(oldClassID, "jlcs010203");
			strcpy(newClassID, "jlcs010103");
			printf("����idΪ�� %s", oldClassID);
		}

		if (strcmp(type_value,"C5_Order")!=0) {
			printf("�ö�������Ͳ��Ƕ��� \n");
		}
		else {
			printf("�ö���������Ƕ��� \n");
			
		}
		
		//ITK_ask_cli_argument();
		//ITK__convert_uid_to_tag();
		POM_AM__set_application_bypass(true);
		if (strcmp(bom_value,"�·��ɹ�")==0) {
				//ICS_find_class()
				//ITK__convert_uid_to_tag
			printf("���࿪ʼ \n");
			//ICS_ask_classification_object(attachments[i],&itemClass_tag); //ͨ��item�õ�item�ķ������

			
			ICS_is_wsobject_classified(attachments[i],&verdict); //�˺����������Ĺ����������Ƿ��ѷ��ࡣ
			
			/*�⽫���ݶ���id����ͼ/���Ǻ͹����ռ���󴴽�һ���µķ������
			���ws_object����#NULLTAG��ICO���Ըö�����з��ࡣ����������£�
			�����id�������ԣ�����ws_�����id��������ICO id��
		    ���ws_object��#NULLTAG���򽫴���һ��ICO����idΪobject#id��
			ע�⣺���ҪΪ֧��������Ԫϵͳ���ഴ��ICO����ʹ��ICS ICO*/
			//ICS_create_classification_object();
			
			//������ȡ�ɸ���ID�ַ�����ʶ����/��ı�ǡ�
			ICS_find_class(oldClassID, &old_ico_tag);  //ԭ�������tag id�������ж�����
			ICS_find_class(newClassID, &new_ico_tag);	//����ʷ�����еķ���tag id��
			printf("���tag�� %d\n", old_ico_tag);
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

			//ICS_remove_classification() //ȫ��ɾ����item�ͷ����еĶ���
			//ICS_delete_reference_to_wsobject
			ICS_delete_reference_to_ico(item_class_tag);
			
			ICS_delete_classification_object(item_class_tag);

			//char** value = (char**)MEM_alloc(sizeof(char*));

			ICS_create_classification_object(attachments[i],item_id, new_ico_tag, &classificationObject);
			//�˺����Ը����Ĺ�����������з��࣬����������������ʹ��IMAN_�����ϵ�ĸ����������
			ICS_classify_wsobject(attachments[i], classificationObject);
			//ICS_ico_ask_attributes(item_class_tag, &ico_att_count, &ico_id_count, &ico_values_count, &ico_values2);

			ICS_ask_classification_object(attachments[i],&item_class_tag2);
			ICS_ico_set_attributes(item_class_tag2, ico_att_count, ico_id_count, ico_values_count,(const char*** )ico_values2);
			printf("�����Ƶ���ʷ�����ɹ�");	
			POM_AM__set_application_bypass(false);
			//MEM_free(oldClassID);
			//MEM_free(newClassID);
		}
		else {
			printf("�����͵�����ʧ��");
		}
	}

	return ifail;
}
////��ȡ���� REVISION ��Ӧ�ķ���
//ICS_ask_classification_object(top_rev_tag,&top_classificationObject);
//if(top_classificationObject == NULL_TAG)
//{
//printf("�������û�з��͵�����\n");
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

