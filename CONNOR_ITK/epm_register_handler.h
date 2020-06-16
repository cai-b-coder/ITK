

#ifndef EPM_REGISTER_HANDLER_CUSTOM
#define EPM_REGISTER_HANDLER_CUSTOM

#include <ict/ict_userservice.h>

#ifdef __cplusplus
extern "C" {
#endif

	extern DLLAPI int CUST_init_module(int *, va_list);
	extern DLLAPI int USERSERVICE_custom_register_methods();


#ifdef __cplusplus
}
#endif

#endif 

