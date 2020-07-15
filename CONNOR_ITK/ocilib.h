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
    * �������ݿ�.
    * @param username     - <I> �û���
    * @param password     - <I> ����
    * @param dbname	      - <I> ���ݿ�SID
    * @return - OCI_OK or error code
    *
    *    ORACLE ���ݿ���������װ����
    */
	extern int ConnServer(char *username,char *password,char *dbname);


	/**
    * �������������ִ��SQL���.
    * @param SQL			- <I> SQL���
    * @return - OCI_OK or error code
    *
    *    ORACLE ���ݿ���������װ����
    */
	extern int ExecuteSQLNoInputParam(char *SQL);

	/**
    * �����������ִ��SQL���.
    * @param SQL			     - <I> SQL���
    * @param inputValueCount     - <I> �����������
    * @param inputValue			 - <I> �������ֵ
    * @return - OCI_OK or error code
    *
    *    ORACLE ���ݿ���������װ����
    */
	extern int ExecuteSQL(char *SQL,int inputValueCount,char **inputValue);

	/**
    * ������������Ĳ�ѯSQL���.
    * @param SQL					 - <I> SQL���
    * @param outputColumn			 - <O> �������е�����
    * @param outputValueCount        - <O> �������е�����
    * @param outputValue			 - <O> ���������
    * @return - OCI_OK or error code
    *
    *    ORACLE ���ݿ���������װ����
    */
	extern int QuerySQLNoInputParam(char *SQL, int * outputColumn, int * outputValueCount, char **** outputValue);

	/**
    * ����������Ĳ�ѯSQL���.
    * @param SQL					 - <I> SQL���
    * @param inputValueCount         - <I> �����������
    * @param inputValue			     - <I> �������ֵ
    * @param outputColumn			 - <O> �������е�����
    * @param outputValueCount        - <O> �������е�����
    * @param outputValue			 - <O> ���������
    * @return - OCI_OK or error code
    *
    *    ORACLE ���ݿ���������װ����
    */
	extern int QuerySQL(char *SQL, int inputValueCount, char ** inputValue, int * outputColumn, int * outputValueCount, char **** outputValue);

	/**
    * �Ͽ����ݿ�����.
    *
    *    ORACLE ���ݿ���������װ����
    */
	extern void DisConnServer();

#ifdef __cplusplus
}
#endif
