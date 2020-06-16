
#include <server_exits/user_server_exits.h>
#include <epm/epm.h>


#include <time.h>
#include <bom/bom_msg.h>

/**
* @headerfile			user's header files
*/
#include "epm_register_handler.h"
#include "epm_handler_common.h"



// Method and Workflow Handler
extern DLLAPI int CUST_init_module(int *decision, va_list args)
{
	int ifail = ITK_ok;
	//基础件变更检查
	ifail = EPM_register_rule_handler("JL_Change_Inspection_Of_Basic_Part", "JL_Change_Inspection_Of_Basic_Part",
		(EPM_rule_handler_t)JL_Change_Inspection_Of_Basic_Part);
	if (ifail)
	{
		printf("register JL_Change_Inspection_Of_Basic_Part failed\n");
	}
	else
	{
		printf("register JL_Change_Inspection_Of_Basic_Part successfully\n");
	}
	//变更信息邮件通知
	ifail = EPM_register_action_handler("JL_Send_Mail", "JL_Send_Mail",
		(EPM_action_handler_t)JL_Send_Mail);

	if (ifail)
	{
		printf("register JL_Send_Mail failed\n");
	}
	else
	{
		printf("register JL_Send_Mail successfully\n");
	}
	

	//PDF插入图片
	ifail = EPM_register_action_handler("jl_insert_img_pdf", "jl_insert_img_pdf",
		(EPM_action_handler_t)jl_insert_img_pdf);
	if (ifail == ITK_ok)
	{
		fprintf(stdout, "Registering action handler tm_insert_img_pdf completed!\n");
	}
	else
	{
		fprintf(stdout, "Registering action handler tm_insert_img_pdf failed %d!\n", ifail);
	}

	//订单移至历史订单
	ifail = EPM_register_action_handler("jl_insert_modular","增加模块",
		(EPM_action_handler_t)jl_insert_modular);
	if (ifail == ITK_ok)
	{
		fprintf(stdout, "Registering action handler 订单移至历史订单 completed!\n");
	}
	else
	{
		fprintf(stdout, "Registering action handler 订单移至历史订单 failed %d!\n", ifail);
	}

	return ifail;
}
//
////register service method
extern DLLAPI int USERSERVICE_custom_register_methods()
{
	return 0;
}

