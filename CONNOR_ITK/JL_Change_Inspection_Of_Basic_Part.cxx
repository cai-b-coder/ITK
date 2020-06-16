#include "epm_handler_common.h"

#include <tc\preferences.h>
#include <tc\tc_macros.h>
#include <epm\epm.h>

#include <tccore\aom.h>

#include <tccore\aom_prop.h>
#include <tccore\item.h>
#include <string>

#include <map>
#include <vector>
#include<iostream>

#include "string"

using namespace std;



EPM_decision_t JL_Change_Inspection_Of_Basic_Part(EPM_rule_message_t msg)
{
	printf("��ʼִ��\n");


	int  ifail, attachments_num, relation_num ;
	tag_t  rootTask, *attachments, *C8RelationModification;
	char   *item_id, *c8BasicPartMaterialNum, *C8RelationModification_object_string;
	logical IsBasicParts;

	EPM_decision_t  EPM_decision = EPM_go;
	//��ѯ����
	int status;
	int num_found = 0;
	int entry_count = 0;
	char *message;
	tag_t query_tag = NULLTAG;
	char** entries;
	char** values;
	char *other_values[1];
	char *other_entrys[1];
	tag_t* items;
	tag_t* results;
	other_values[0] = (char*)calloc(48, sizeof(char));
	other_entrys[0] = (char*)calloc(48, sizeof(char));
	const char query_name_ggtz[QRY_name_size_c + 1] = { "���������ϺŲ�ѯ" };

	//��ȡ�������
	EPM_ask_root_task(msg.task, &rootTask);
	//��ȡ����Ŀ�����
	EPM_ask_attachments(rootTask, EPM_target_attachment, &attachments_num, &attachments);

	for (int i = 0; i < attachments_num; i++)
	{
		//��ȡĿ��������󡰹�����������µ����ж���
		AOM_ask_value_tags(attachments[i], "C8RelationModification", &relation_num, &C8RelationModification);
		AOM_ask_value_string(attachments[i], "item_id", &item_id);
		for (int j = 0; j < relation_num; j++) {
			AOM_ask_value_logical(C8RelationModification[j], "c8IsBasicParts", &IsBasicParts);
			//printf("IsBasicParts---------------- %d", IsBasicParts);
			if (IsBasicParts)
			{
				printf("Ŀ���µ�[%d]������ǻ�������\n", j+1);
				EPM_decision = EPM_nogo;
				return EPM_decision;
			}
			else
			{

				AOM_ask_value_string(C8RelationModification[j], "c8BasicPartMaterialNum", &c8BasicPartMaterialNum);
				AOM_ask_value_string(C8RelationModification[j], "object_string", &C8RelationModification_object_string);
				AOM_ask_value_string(attachments[i], "item_id", &item_id);
				if (strcmp(item_id, c8BasicPartMaterialNum) != 0)
				{
					printf("[%s]�Ļ���������[%s]\n", C8RelationModification_object_string, item_id);
					EPM_decision = EPM_nogo;
					return EPM_decision;
				}
			}
		}

		//��ѯ�Ա�
		//ITKCALL( QRY_find(query_name_ggtz, &query_tag) );
		QRY_find(query_name_ggtz, &query_tag);
		if (query_tag == NULLTAG)
		{
			printf("query_tag=NULLTAG,û�ҵ� [%s]��ѯ��������\n", query_name_ggtz);
			EPM_decision = EPM_nogo;
			return EPM_decision;
			
		}
		else
		{
			printf("�ҵ���ѯ������\n");
		}
		//ITKCALL(QRY_find_user_entries(query_tag, &entry_count, &entries, &values));//entries����ѯ�������ʽ��values����ѯ����ֵ��MEM_free�ͷţ�
		QRY_find_user_entries(query_tag, &entry_count, &entries, &values);
		if (entry_count == 0)
		{
			printf("entry_count=0,��ѯ��������û�����ò�ѯ������\n");
			//ITK_exit_module(1);
			if (entries != NULL) {
				MEM_free(entries);
				entries = NULL;
			}
			if (values != NULL) {
				MEM_free(values);
				values = NULL;
			}
			EPM_decision = EPM_nogo;
			return EPM_decision;
		}

		for (int n = 0; n<entry_count; n++)
		{
			printf("entries[%d]=%s\n", n, entries[n]);
			if (tc_strcmp(entries[n], "���������Ϻ�") == 0) {
				//strcpy(values[n],nowtime1);
				
				tc_strcpy(other_entrys[0], "���������Ϻ�");
				tc_strcpy(other_values[0], item_id);//*				printf("other_values[0] %s\n",other_values[0]);
			}
			printf("values[%d]=%s\n", n, values[n]);
		}

		//ITKCALL(status = QRY_execute(query_tag, 1, other_entrys, other_values, &num_found, &results));
		status = QRY_execute(query_tag, 1, other_entrys, other_values, &num_found, &results);
		if (status != ITK_ok) {
			EMH_ask_error_text(status, &message);
			printf("�����쳣 : \"%d\",\"%s\"\n", status, message);
			MEM_free(message);
			if (entries != NULL) {
				MEM_free(entries);
				entries = NULL;
			}
			if (values != NULL) {
				MEM_free(values);
				values = NULL;
			}
			EPM_decision = EPM_nogo;
			return EPM_decision;
		}
		if (num_found == 0)
		{
			printf("num_found=0,��ѯ��������û�ҵ��������������ݣ�\n");
			//ITK_exit_module(1);
			if (entries != NULL) {
				MEM_free(entries);
				entries = NULL;
			}
			if (values != NULL) {
				MEM_free(values);
				values = NULL;
			}

		}
		printf("��ѯ�������õ�ITEM��num_found: \"%d\" \n", num_found);
		if (num_found != relation_num) {
			printf("[%s]��������������µ������������\n", item_id);
			EPM_decision = EPM_nogo;
			return EPM_decision;
		}
	}

	return EPM_decision;
}


