


#ifndef EPM_HANDLER_COMMON
#define EPM_HANDLER_COMMON

#include <epm/epm.h>
#include <string>


#include <vector>
#include <map>
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")rename("BOF","adoBOF")
#include <atlstr.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	EPM_decision_t JL_Change_Inspection_Of_Basic_Part(EPM_rule_message_t msg);
	int JL_Send_Mail(EPM_action_message_t msg);
	//int JL_Change_Inspection_Of_Basic_Part(EPM_action_message_t msg);
	int jl_insert_img_pdf(EPM_action_message_t msg);
	int jl_insert_modular(EPM_action_message_t msg);
	int jl_dmu_auto(EPM_action_message_t msg);

	//user service end
#ifdef __cplusplus
}
#endif

#endif 


/**
* @}
*/