

#pragma warning (disable: 4819) 

#include <tc/tc.h>
#include <tccore/custom.h>
#include "epm_register_handler.h"
#include "epm_handler_common.h"


#ifdef __cplusplus
extern "C" {
#endif


	
	DLLAPI int jl_register_callbacks()
	{
		int ifail = ITK_ok;
		ifail = CUSTOM_register_exit(
			"connor",
			"USERSERVICE_register_methods",
			(CUSTOM_EXIT_ftn_t)USERSERVICE_custom_register_methods);


		ifail = CUSTOM_register_exit(
			"connor",
			"USER_gs_shell_init_module",
			(CUSTOM_EXIT_ftn_t)CUST_init_module);

		return ifail;
	}

#ifdef __cplusplus
}
#endif




/**
* @}
*/