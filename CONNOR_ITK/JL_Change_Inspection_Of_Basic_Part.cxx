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
	printf("开始执行\n");


	int  ifail, attachments_num, relation_num ;
	tag_t  rootTask, *attachments, *C8RelationModification;
	char   *item_id, *c8BasicPartMaterialNum, *C8RelationModification_object_string;
	logical IsBasicParts;

	EPM_decision_t  EPM_decision = EPM_go;
	//查询属性
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
	const char query_name_ggtz[QRY_name_size_c + 1] = { "基础件物料号查询" };

	//获取任务对象
	EPM_ask_root_task(msg.task, &rootTask);
	//获取任务目标对象
	EPM_ask_attachments(rootTask, EPM_target_attachment, &attachments_num, &attachments);

	for (int i = 0; i < attachments_num; i++)
	{
		//获取目标任务对象“关联变更件”下的所有对象
		AOM_ask_value_tags(attachments[i], "C8RelationModification", &relation_num, &C8RelationModification);
		AOM_ask_value_string(attachments[i], "item_id", &item_id);
		for (int j = 0; j < relation_num; j++) {
			AOM_ask_value_logical(C8RelationModification[j], "c8IsBasicParts", &IsBasicParts);
			//printf("IsBasicParts---------------- %d", IsBasicParts);
			if (IsBasicParts)
			{
				printf("目标下第[%d]个组件是基础件！\n", j+1);
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
					printf("[%s]的基础件不是[%s]\n", C8RelationModification_object_string, item_id);
					EPM_decision = EPM_nogo;
					return EPM_decision;
				}
			}
		}

		//查询对比
		//ITKCALL( QRY_find(query_name_ggtz, &query_tag) );
		QRY_find(query_name_ggtz, &query_tag);
		if (query_tag == NULLTAG)
		{
			printf("query_tag=NULLTAG,没找到 [%s]查询构建器！\n", query_name_ggtz);
			EPM_decision = EPM_nogo;
			return EPM_decision;
			
		}
		else
		{
			printf("找到查询构建器\n");
		}
		//ITKCALL(QRY_find_user_entries(query_tag, &entry_count, &entries, &values));//entries：查询条件表达式；values：查询条件值（MEM_free释放）
		QRY_find_user_entries(query_tag, &entry_count, &entries, &values);
		if (entry_count == 0)
		{
			printf("entry_count=0,查询构建器中没有配置查询条件！\n");
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
			if (tc_strcmp(entries[n], "基础件物料号") == 0) {
				//strcpy(values[n],nowtime1);
				
				tc_strcpy(other_entrys[0], "基础件物料号");
				tc_strcpy(other_values[0], item_id);//*				printf("other_values[0] %s\n",other_values[0]);
			}
			printf("values[%d]=%s\n", n, values[n]);
		}

		//ITKCALL(status = QRY_execute(query_tag, 1, other_entrys, other_values, &num_found, &results));
		status = QRY_execute(query_tag, 1, other_entrys, other_values, &num_found, &results);
		if (status != ITK_ok) {
			EMH_ask_error_text(status, &message);
			printf("程序异常 : \"%d\",\"%s\"\n", status, message);
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
			printf("num_found=0,查询构建器中没找到满足条件的数据！\n");
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
		printf("查询构建器得到ITEM数num_found: \"%d\" \n", num_found);
		if (num_found != relation_num) {
			printf("[%s]“关联变更件”下的组件不完整！\n", item_id);
			EPM_decision = EPM_nogo;
			return EPM_decision;
		}
	}

	return EPM_decision;
}


