/*=====================================================================================================================
                    Copyright(c) 2012 ORIGIN.
                             Unpublished - All rights reserved
=======================================================================================================================
File description:

    Filename: ocilib.h
    Module  : OCI

        This Header file of OCI library Package.

=======================================================================================================================
Date               Name              Description of Change
1-Feb-2015         Ray				Initialize creation
$HISTORY$
=====================================================================================================================*/
#include <oci.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <malloc.h>
//#include <limits.h>

#define OCI_FAIL 1
#define OCI_OK 0

#ifdef __cplusplus
extern "C"
{
#endif
	/**
    * 连接数据库.
    * @param username     - <I> 用户名
    * @param password     - <I> 密码
    * @param dbname	      - <I> 数据库SID
    * @return - OCI_OK or error code
    *
    *    ORACLE 数据库的连接与封装函数
    */
	extern int ConnServer(char *username,char *password,char *dbname);


	/**
    * 不带输入参数的执行SQL语句.
    * @param SQL			- <I> SQL语句
    * @return - OCI_OK or error code
    *
    *    ORACLE 数据库的连接与封装函数
    */
	extern int ExecuteSQLNoInputParam(char *SQL);

	/**
    * 带输入参数的执行SQL语句.
    * @param SQL			     - <I> SQL语句
    * @param inputValueCount     - <I> 输入参数数量
    * @param inputValue			 - <I> 输入参数值
    * @return - OCI_OK or error code
    *
    *    ORACLE 数据库的连接与封装函数
    */
	extern int ExecuteSQL(char *SQL,int inputValueCount,char **inputValue);

	/**
    * 不带输入参数的查询SQL语句.
    * @param SQL					 - <I> SQL语句
    * @param outputColumn			 - <O> 输出表的列的数量
    * @param outputValueCount        - <O> 输出表的行的数量
    * @param outputValue			 - <O> 输出表内容
    * @return - OCI_OK or error code
    *
    *    ORACLE 数据库的连接与封装函数
    */
	extern int QuerySQLNoInputParam(char *SQL, int * outputColumn, int * outputValueCount, char **** outputValue);

	/**
    * 带输入参数的查询SQL语句.
    * @param SQL					 - <I> SQL语句
    * @param inputValueCount         - <I> 输入参数数量
    * @param inputValue			     - <I> 输入参数值
    * @param outputColumn			 - <O> 输出表的列的数量
    * @param outputValueCount        - <O> 输出表的行的数量
    * @param outputValue			 - <O> 输出表内容
    * @return - OCI_OK or error code
    *
    *    ORACLE 数据库的连接与封装函数
    */
	extern int QuerySQL(char *SQL, int inputValueCount, char ** inputValue, int * outputColumn, int * outputValueCount, char **** outputValue);

	/**
    * 断开数据库连接.
    *
    *    ORACLE 数据库的连接与封装函数
    */
	extern void DisConnServer();

#ifdef __cplusplus
}
#endif
