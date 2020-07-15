/*=====================================================================================================================
                    Copyright(c) 2012 ORIGIN.
                             Unpublished - All rights reserved
=======================================================================================================================
File description:

    Filename: ocilib.cxx
    Module  : OCI

        This file describes OCI library Package.

=======================================================================================================================
Date               Name              Description of Change
1-Feb-2015         Ray	li     Initialize creation
$HISTORY$
=====================================================================================================================*/
#include "ocilib.h"
#include <stdlib.h>

#define NUM 100
#define USERNAME "infodba"
#define PASSWORD "infodba"
#define DBNAME "tc"


// 定义句柄结构
typedef struct{
	OCIEnv	*p_env;                 //OCI environment handle	 环境句柄
	OCIError	*p_err;             //OCI error handle			 错误句柄
	OCISvcCtx	*p_svc;             //OCI service context handel 服务上下文句柄
	OCIServer	*p_ser;             //OCI server handle          服务器句柄
	OCISession	*p_usr;             //OCI user session handle    用户会话句柄
	OCIStmt	*p_sql;                 //OCI statement handle       语句句柄
	OCIDefine	*p_dfn;             //OCI define handle			 定义句柄
	OCIBind	*p_bnd;                 //OCI bind handle			 绑定句柄
}OCIHandleInfo;


// 定义错误句柄结构
typedef struct{ 
	OCIEnv     *p_env; 
	OCIError   *p_err; 
	OCISvcCtx  *p_svc; 
	OCIStmt    *p_sql; 
	OCIDefine  *p_dfn; 
	OCIBind    *p_bnd; 
}OCIDATA;


// 定义执行语句时候的输入参数
typedef struct{
	char value[NUM][NUM];
	char type[NUM][NUM];
}SqlField;


// 定义查询语句时候的输入,输出参数
typedef struct{
	int  naIntValue[NUM];
	int  nIntNum;
	char caCharValue[500][500];
	int  nCharNum;
}SqlSelField;

OCIHandleInfo *ociHandle = NULL;
OCIHandleInfo ociHand;

int InitHandle();
int _ExeSQL(char *SQL,char ** inputValue,int inputValueCount);
int _QuerySQL(char *SQL, SqlSelField *pOutField, SqlSelField *pSelField);
int GetDataFromQuery(int *pRc, SqlSelField *pOutField);
void QuitFreeHandle();




/****************************************************************************************************************************************
函数名称： 
函数功能：
入口参数：
出口参数： 
备 注： 
*****************************************************************************************************************************************/ 


// 初始化Handler
int InitHandle()
{
	int swResult;

	ociHandle = &ociHand;


	/*create OCI environment*/
	if(swResult = OCIEnvCreate(&ociHandle->p_env,OCI_DEFAULT,NULL,NULL,NULL,NULL,0,NULL))               //环境句柄
	{
		printf("environment create error!\n\n");
		return -1;
	}
	else 
	{
		printf("environment create success!\n\n");
		//return 0;
	}

	/*init handle*/	
	if(swResult = OCIHandleAlloc(ociHandle->p_env,(dvoid **)&ociHandle->p_ser,OCI_HTYPE_SERVER,0,NULL))    //服务器句柄
	{
		printf("init server handle error!\n\n");
		return -1;
	}


	if(swResult = OCIHandleAlloc(ociHandle->p_env,(dvoid **)&ociHandle->p_err,OCI_HTYPE_ERROR,0,NULL))    //错误句柄
	{
		printf("init error handle error!\n\n");
		return -1;
	}


	if(swResult = OCIHandleAlloc(ociHandle->p_env,(dvoid **)&ociHandle->p_usr,OCI_HTYPE_SESSION,0,NULL))    //事务句柄
	{
		printf("init session handle error!\n\n");
		return -1;
	}


	if(swResult = OCIHandleAlloc(ociHandle->p_env,(dvoid **)&ociHandle->p_svc,OCI_HTYPE_SVCCTX,0,NULL))    //上下文句柄
	{
		printf("init service context handle error!\n\n");
		return -1;
	}


	if(swResult = OCIHandleAlloc(ociHandle->p_env,(dvoid **)&ociHandle->p_sql,OCI_HTYPE_STMT,0,NULL))        //SQL语句句柄
	{
		printf("init statement handle error!\n\n");
		return -1;
	}

	printf("init handle success!\n\n");

	return 0;
}



// 连接数据库服务器
int ConnServer(char *username,char *password,char *dbname)
{
	int		swResult;
	char	errbuf[100]={0};
	int		errcode;

	if(InitHandle() == -1)//初始化句柄
		return -1;

	if(swResult = OCILogon(ociHandle->p_env,ociHandle->p_err,&ociHandle->p_svc,(text *)username,strlen(username),(text *)password,strlen(password),(text *)dbname,strlen(dbname)))
	{
		OCIErrorGet((dvoid *)ociHandle->p_err, (ub4) 1, (text *) NULL, &errcode, (ub1 *)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
		printf("Error - %.*s/n", 512, errbuf);        
		return -1;
	}
	else 
		printf("数据库连接成功!\n\n");
	return 0;
}



// SQL语句的陈述(执行SQL语句)
int _ExeSQL(char *SQL,char ** inputValue,int inputValueCount)
{
	int swResult,i;
	int	errcode;	

	//设置绑定变量
	OCIBind     *p_bndp[100];

	//准备SQL语句
	if(swResult = OCIStmtPrepare(ociHandle->p_sql,ociHandle->p_err,(text *)SQL,strlen(SQL),OCI_NTV_SYNTAX,OCI_DEFAULT))
	{
		printf("prepare SQL statements error!\n\n");
	}
	else
	{
		printf("prepare SQL statements success!\n\n");
	}


	// 绑定输入变量
	for(i=0;i<inputValueCount;i++)
	{
		char	errbuf[100]={0};
		if(swResult = OCIBindByPos(ociHandle->p_sql,&p_bndp[i],ociHandle->p_err,i+1,(dvoid *)inputValue[i],(sb4)strlen(inputValue[i])+1,SQLT_STR, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT))
		{
			OCIErrorGet((dvoid *)ociHandle->p_err, (ub4) 1, (text *) NULL, &errcode, (ub1 *)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
			printf("Bind Error - %.*s/n", 512, errbuf);  	
			return -1;
		}
	}

	//执行SQL statements
	if(swResult = OCIStmtExecute(ociHandle->p_svc,ociHandle->p_sql,ociHandle->p_err,1,0,NULL,NULL,OCI_DEFAULT | OCI_COMMIT_ON_SUCCESS))
	{
		char	errbuf[100]={0};
		OCIErrorGet((dvoid *)ociHandle->p_err, (ub4) 1, (text *) NULL, &errcode, (ub1 *)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
		printf("execute SQL statement Error - %.*s\n", 512, errbuf);   
		return -1;
	}
	else
	{
		printf("execute SQL statement success!\n\n");
	}
	return 0;
}




// 查询SQL
int _QuerySQL(char *SQL, SqlSelField *pOutField, SqlSelField *pSelField )
{
	sword status;
	int		rc= 0,ret=0;
	char	errbuf[100]={0};
	int		maxNum = 2048;
	char	chTag[8];
	int		iIndex = 0;
	int		outputColumn = 0;
	int		errcode=0;
	char	nullValue =  '\0'; // 
	sb2     sb2aInd[30] = {'\0'}; 

	// 准备SQL语句
	status = OCIStmtPrepare(ociHandle->p_sql, ociHandle->p_err, (text *)SQL,(ub4)strlen(SQL), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);

	if (status != OCI_SUCCESS)
	{                
		printf("SQL Preparing failed/n");
		return -1;
	}


	// 绑定int类型的, 定义SQL语句时 :1,:2, 定义输入变量
	for (iIndex=0; iIndex<pSelField->nIntNum; iIndex++)
	{
		memset(chTag, 0, 8);
		sprintf(chTag, ":%d", iIndex+1);
		if(rc = OCIBindByName(ociHandle->p_sql, (OCIBind **)&ociHandle->p_dfn, ociHandle->p_err, (text *)chTag,(sb4)strlen((char *)chTag),(dvoid *) &pSelField->naIntValue[iIndex], sizeof(int), SQLT_INT,(dvoid *) 0,(ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT))
		{
			OCIErrorGet((dvoid *)ociHandle->p_err, (ub4) 1, (text *) NULL, &errcode, (ub1 *)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
			printf("BindByPos Error of ociHandle->p_sql - %.*s\n", 512, errbuf);   
			return -1;
		}
	}

	// 绑定char *类型的, 定义SQL语句时 :3,:4, 定义输入变量
	for (iIndex=0; iIndex<pSelField->nCharNum; iIndex++)
	{
		int  n=0;
		memset(chTag, 0, 8);
		n = pSelField->nIntNum + iIndex +1;
		sprintf(chTag, ":%d", n);
		if(rc = OCIBindByName(ociHandle->p_sql, (OCIBind **)&ociHandle->p_dfn, ociHandle->p_err, (text *)chTag,(sb4)strlen((char *)chTag),(dvoid *) &pSelField->caCharValue[iIndex], NUM, SQLT_STR,(dvoid *) 0,(ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT))
		{
			OCIErrorGet((dvoid *)ociHandle->p_err, (ub4) 1, (text *) NULL, &errcode, (ub1 *)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
			printf("BindByPos Error of ociHandle->p_sql - %.*s\n", 512, errbuf);   
			return -1;
		}
	}


	// 执行SQL语句
	if (rc = OCIStmtExecute(ociHandle->p_svc, ociHandle->p_sql, ociHandle->p_err, (ub4) 0, (ub4) 0,(CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_STMT_SCROLLABLE_READONLY))
	{
		OCIErrorGet((dvoid *)ociHandle->p_err, (ub4) 1, (text *) NULL, &errcode, (ub1 *)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
		printf("execute SQL  Error - %.*s\n", 512, errbuf);   
		return -1;
	}
	else
	{
		printf("execute SQL  success!\n\n");
	}


	if (ret = OCIAttrGet (ociHandle->p_sql, (ub4)OCI_HTYPE_STMT, (dvoid *) &outputColumn, (ub4 *) 0, (ub4)OCI_ATTR_PARAM_COUNT, ociHandle->p_err))
	{
		OCIErrorGet((dvoid *)ociHandle->p_err, (ub4) 1, (text *) NULL, &errcode, (ub1 *)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
		printf("Get OCIAttr Error of ociHandle->p_sql - %.*s\n", 512, errbuf);   
		return -1;
	}


	// 绑定int类型的, 定义SQL语句时 :1,:2, 定义输出变量
	for (iIndex=0; iIndex<pOutField->nIntNum; iIndex++)
	{
		if(rc = OCIDefineByPos(ociHandle->p_sql, &ociHandle->p_dfn, ociHandle->p_err, iIndex+1, (dvoid *)&pOutField->naIntValue[iIndex], sizeof(int), SQLT_INT, (dvoid *) 0, (ub2 *)0,(ub2 *)0, OCI_DEFAULT))
		{
			OCIErrorGet((dvoid *)ociHandle->p_err, (ub4) 1, (text *) NULL, &errcode, (ub1 *)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
			printf("DefineByPos Error of ociHandle->p_sql - %.*s\n", 512, errbuf);   
			return -1;
		}
	}


	// 绑定char *类型的, 定义SQL语句时 :1,:2, 定义输出变量
	for (iIndex=0; iIndex<outputColumn; iIndex++)
	{
		int  n = iIndex+1 ;
		if (rc = OCIDefineByPos(ociHandle->p_sql, &ociHandle->p_dfn, ociHandle->p_err, n, (dvoid *)&pOutField->caCharValue[iIndex], 1000*sizeof(char), SQLT_STR, (dvoid *) 0, (ub2 *)0,(ub2 *)0, OCI_DEFAULT))
		{
			OCIErrorGet((dvoid *)ociHandle->p_err, (ub4) 1, (text *) NULL, &errcode, (ub1 *)errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
			printf("DefineByPos Error of ociHandle->p_sql - %.*s\n", 512, errbuf);   
			return -1;
		}
	}

	return 0;
	
}



// 释放Handler
void QuitFreeHandle()
{
	// 退出服务器
	OCILogoff(ociHandle->p_svc,ociHandle->p_err);
	printf("Quit success!\n");

	// 释放句柄
	OCIHandleFree(ociHandle->p_ser,OCI_HTYPE_SERVER);            //释放服务器句柄
	OCIHandleFree(ociHandle->p_err,OCI_HTYPE_ERROR);            //释放错误句柄
	OCIHandleFree(ociHandle->p_usr,OCI_HTYPE_SESSION);        //释放事务句柄
	OCIHandleFree(ociHandle->p_svc,OCI_HTYPE_SVCCTX);            //释放上下文句柄
	OCIHandleFree(ociHandle->p_sql,OCI_HTYPE_STMT);            //释放SQL语句句柄
}


// 执行类操作
int ExecuteSQL(char *SQL,int valueCount,char **value)
{
	int i=0;

	if(i=_ExeSQL(SQL,value,valueCount))
	{
		QuitFreeHandle();
		return -1;
	}

	return 0;
}


// 有输入参数的查询
int QuerySQL(char *SQL, int inputValueCount, char ** inputValue, int * outputColumn, int * outputValueCount, char **** outputValue)
{
	int i = 0, j=0 ,ret =0;

	int times = 0, temp=0;

	SqlSelField infield;
	SqlSelField outField;

	// 初始化参数结构
	infield.nCharNum = inputValueCount;
	infield.nIntNum = 0;
	outField.nCharNum = 0;
	outField.nIntNum = 0;

	 * outputColumn = 0;
	* outputValueCount = 0;

	for(i=0;i<inputValueCount;i++)
	{
		strcpy(infield.caCharValue[i],inputValue[i]);
	}

	// 执行查询语句
	

	if (ret = _QuerySQL(SQL,&outField,&infield))
	{
		printf("SQL查询失败!\n");
		QuitFreeHandle();
		return -1;
	}
	
	// 获取结果列数
	ret= OCIAttrGet (ociHandle->p_sql, (ub4)OCI_HTYPE_STMT, (dvoid *) outputColumn, (ub4 *) 0, (ub4)OCI_ATTR_PARAM_COUNT, ociHandle->p_err);

	// 提取最后一行,便于去行数
	ret = OCIStmtFetch2(ociHandle->p_sql, ociHandle->p_err,1, OCI_FETCH_LAST,0, OCI_DEFAULT);
	// 获取结果行数
	ret= OCIAttrGet (ociHandle->p_sql, (ub4)OCI_HTYPE_STMT, (dvoid *) outputValueCount, (ub4 *) 0, (ub4)OCI_ATTR_ROW_COUNT, ociHandle->p_err);

	if(*outputValueCount == 0)
		return 0;


	// 开始分配内存并且存储
	ret = OCIStmtFetch2(ociHandle->p_sql, ociHandle->p_err,1, OCI_FETCH_FIRST,0, OCI_DEFAULT);
	*outputValue = (char ***) calloc ((*outputValueCount)+1,sizeof(char**));
	do
	{
		(*outputValue)[i] = (char **) calloc ((*outputColumn)+1,sizeof(char *));
		for(j=0;j<(*outputColumn);j++)
		{
			(*outputValue)[i][j] = (char *) calloc (1000,sizeof(char));
			strcpy((*outputValue)[i][j],outField.caCharValue[j]);
			//printf("outValue[%d][%d] = %s , 地址=%d, 所指向地址= %d\n ",i,j,(*outputValue)[i][j],&(*outputValue)[i][j],(*outputValue)[i][j]);
		}
		i++;
	}
	while((ret = OCIStmtFetch2(ociHandle->p_sql, ociHandle->p_err,1, OCI_FETCH_NEXT, 1, OCI_DEFAULT)!= OCI_NO_DATA));

	return 0;
}


// 无输入参数的查询
int QuerySQLNoInputParam(char *SQL, int * outputColumn, int * outputValueCount, char **** outputValue)
{
	return QuerySQL(SQL,0,NULL,outputColumn,outputValueCount,outputValue);
}

// 无参数操作
int ExecuteSQLNoInputParam(char *SQL)
{
	return ExecuteSQL(SQL,0,NULL);
}

// 断开连接
void DisConnServer()
{
	if(ociHandle != NULL)
		QuitFreeHandle();
}

