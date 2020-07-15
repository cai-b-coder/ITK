#include <epm\epm.h>
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")rename("BOF","adoBOF")
#include <atlstr.h>
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
#include "ocilib.h"
#include<ics\ics.h>
#include<ics\ics2.h>
#pragma warning(disable : 4996)
#include <tccore\aom_prop.h>
using namespace std;
//#include "E:/20200520/DM_ITK/jf_itk/jf_itk/jf_itk/ocilib.cxx"

int jl_dmu_auto(EPM_action_message_t msg) {
	::CoInitialize(NULL);   //初始化OLE/COM库环境
	HRESULT hr = NULL;
	_ConnectionPtr m_pConnection; // 数据库
	_RecordsetPtr m_pRecordset; // 命令
	_CommandPtr m_pCommand; // 记录
	
	int ifail = EPM_go, count = 0, arg_cnt = 0;
	int outputColumn = 0, outputValueCount = 0;
	tag_t root_task = NULL_TAG,item_tag;
	tag_t* attachments = NULL;
	//char arg1value[1024] = "";
	//char* arg = NULL, * argflag = NULL, * argvalue = NULL;
	char* item_id,*uid="";
	char* control_name = "btnDmu";  //DMU验证通过按钮的name
	char* style = "false";

	char*** outputValue = (char***)calloc(1024, sizeof(char**));

	//获取流程任务
	EPM_ask_root_task(msg.task, &root_task);
	//获取流程任务目标文件夹中的当前对象
	EPM_ask_attachments(root_task, EPM_target_attachment, &count, &attachments);//count 为流程中目标文件夹中的对象数量  attachments 为当前对象数据集
	//获取参数个数
	arg_cnt = TC_number_of_arguments(msg.arguments);

	if (arg_cnt > 0)
	{
		for (int i = 0; i < arg_cnt; i++)
		{
			ifail = AOM_ask_value_string(attachments[i], "item_id", &item_id); //得到item的id
			ifail = ITEM_find_item(item_id,&item_tag);
			ITK__convert_tag_to_uid(item_tag,&uid);
		}

	}

	hr = m_pConnection.CreateInstance(_uuidof(Connection));//创建连接对象实例
	if (SUCCEEDED(hr))
	{

		m_pConnection->ConnectionString = ("Provider = SQLOLEDB.1; Persist Security Info = False; User ID = infodba; Password=infodba; Initial Catalog = TCTEST2; Data Source = 10.110.81.13 ");
		printf("\n==========================================================\n");
		hr = m_pConnection->Open("", "", "", adConnectUnspecified);//打开数据库
		if (FAILED(hr))
		{
			printf("Open Failed!");
			return 0;
		}
		else
		{
			printf("开始修改控件\n");
			CString strSql;
			CString updateSql;
	//		strSql.Format(_T("INSERT INTO [生产科数据库].[dbo].[tb_Process_Bom]([LinesNo],[ItemCode],[ItemName],[DrawingNo],[ProcessName],[EquiNo],[EquiName],[Designer],[ItemType],[CustomerDrawingNo],[MBomCode],[Material],[ProcessOwner],[RevNo],[OBID],[SysNo],[State]) VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s','%s','%s')"), LineID, ItemCode, ItemName, DrawingNo, ProcessName, EquiNo, EquiNames, Designer, Classify, CustomerDrawing, MBOMCode, MaterialNam, ProcessOwner, RevNo, OBID, SysNo, "UnCheck");
			strSql.Format(_T("SELECT ENABLED FROM GD_CONTROLSTYPE_TABLE WHERE ORDER_TAGU = '%s' AND CONTROL_NAME = '%s'"),uid,control_name);
			char* describeSelect = (char*)calloc(1024, sizeof(char));
			char* describeUpdate = (char*)calloc(1024, sizeof(char));
			//label_45,label_46
			char* label45 = (char*)calloc(1024, sizeof(char));
			char* label46 = (char*)calloc(1024, sizeof(char));
			updateSql.Format(_T("UPDATE GD_CONTROLSTYPE_TABLE SET ENABLED ='%s' WHERE  ORDER_TAGU = '%s' AND CONTROL_NAME = '%s'"),style,uid,control_name);
			
			sprintf(describeSelect,"SELECT ENABLED FROM GD_CONTROLSTYPE_TABLE WHERE ORDER_TAGU = '%s' AND CONTROL_NAME = '%s'", uid, control_name);
			sprintf(describeUpdate, "UPDATE GD_CONTROLSTYPE_TABLE SET ENABLED ='%s'  WHERE  ORDER_TAGU = '%s' AND CONTROL_NAME = '%s'", style, uid, control_name);
			sprintf(label45,"UPDATE GD_CONTROLSTYPE_TABLE SET BACKGROUND = 'GREEN' WHERE CONTROL_NAME = 'label_45' AND OEDER_TAGU = '%s'",uid);
			sprintf(label46, "UPDATE GD_CONTROLSTYPE_TABLE SET BACKGROUND = 'GREEN' WHERE CONTROL_NAME = 'label_46' AND OEDER_TAGU = '%s'", uid);
			//sprintf();
			
			printf(describeSelect);
			printf(describeUpdate);
			printf(label45);
			printf(label46);
			try
			{
				QuerySQLNoInputParam(describeSelect, &outputColumn, &outputValueCount, &outputValue);
				printf("状态为: %s",**outputValue);
				if (strcmp(outputValue[0][0], "true")==0) {
					m_pConnection->Execute(_bstr_t(describeUpdate), 0, adCmdText);
					
				};		
				m_pConnection->Execute(_bstr_t(label45), 0, adCmdText);
				m_pConnection->Execute(_bstr_t(label46), 0, adCmdText);
				//m_pConnection->Execute(_bstr_t(updateDescribe), 0, adCmdText);
				printf("更新结束\n");
			}
			catch (_com_error e)
			{
				printf(e.Description());
				return 1;
			}
			//strSql = NULL;
			//describe = NULL;
		}
	}
	else
	{
		printf("Create instance of connection failed!");
		return 1;
		//你是猪嘛 我笑了
	}
}
