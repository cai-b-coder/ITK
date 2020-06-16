

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
#include <vector>
#include <iostream>
#pragma warning(disable : 4996)
#include <tccore\aom_prop.h>

extern "C" int POM_AM__set_application_bypass(logical bypass);
using namespace std;

//�ָ��ַ�������
void split(char* src, const char* separator, char** dest, int* num) {
	/*
		src Դ�ַ������׵�ַ(buf�ĵ�ַ)
		separator ָ���ķָ��ַ�
		dest �������ַ���������
		num �ָ�����ַ����ĸ���
	*/
	char* pNext;
	int count = 0;
	if (src == NULL || strlen(src) == 0) //�������ĵ�ַΪ�ջ򳤶�Ϊ0��ֱ����ֹ 
		return;
	if (separator == NULL || strlen(separator) == 0) //��δָ���ָ���ַ�����ֱ����ֹ 
		return;
	pNext = (char*)strtok(src, separator); //����ʹ��(char *)����ǿ������ת��(��Ȼ��д�еı������в������ָ�����)
	while (pNext != NULL) {
		*dest++ = pNext;
		++count;
		pNext = (char*)strtok(NULL, separator);  //����ʹ��(char *)����ǿ������ת��
	}
	*num = count;
}

/**
*�������ݼ������������ݼ�������JAVA�������ͼƬ(pdf)
*
*/
int insert_pdf(tag_t dataset, char *ext,char *second_tag_type)
{
	printf("insert_pdf \r\n");
	tag_t spec_dataset_rev = NULLTAG,
		ref_object = NULLTAG;
	
	AE_reference_type_t reference_type;
	//ITKCALL(AE_ask_dataset_latest_rev(dataset, &spec_dataset_rev));
	AE_ask_dataset_latest_rev(dataset, &spec_dataset_rev);

	//char ref_name[WSO_name_size_c + 1] = "CAEAnalysisData";
	//char ref_name[WSO_name_size_c + 1] = "PDF_Reference";
	  char ref_name[WSO_name_size_c + 1] = "";
	//ITKCALL(AE_ask_dataset_named_ref(spec_dataset_rev, ref_name, &reference_type, &ref_object));
	if (strcmp(second_tag_type, "PDF") == 0) {
		strcpy(ref_name,"PDF_Reference");
		AE_ask_dataset_named_ref(spec_dataset_rev, ref_name, &reference_type, &ref_object);
	}
	else {
		strcpy(ref_name, "CAEAnalysisData");
		AE_ask_dataset_named_ref(spec_dataset_rev, ref_name, &reference_type, &ref_object);
	
	}
	
	if (reference_type == AE_PART_OF)
	{
		char pathname[SS_MAXPATHLEN] = "";
		//ITKCALL(IMF_ask_file_pathname(ref_object, SS_WNT_MACHINE, pathname));
		//���ش����������TcFile�����������ļ�������·������·�����ĸ�ʽ��ȡ���ڻ������͵�ֵ
		IMF_ask_file_pathname(ref_object, SS_WNT_MACHINE, pathname);
		char origin_file_name[IMF_filename_size_c + 1] = "";
		//ITKCALL(IMF_ask_original_file_name(ref_object, origin_file_name));
		IMF_ask_original_file_name(ref_object, origin_file_name);
		char new_ds_name[WSO_name_size_c + 1] = "";
		char *new_file_name = USER_new_file_name(new_ds_name, ref_name, ext, 0);//����Ϊ���ݼ������ļ�����
																				//char *new_file_name = USER_new_file_name(new_ds_name, ref_name, "pdf", 0);
		char *temp_dir = getenv("temp");		//C:\Windows\temp
		char * tc_root_dir = getenv("tc_root");		//C:\Siemens\Teamcenter11
		char img_file[SS_MAXPATHLEN] = "";
		char temp_file[SS_MAXPATHLEN] = "";
		char temp_file2[SS_MAXPATHLEN] = "";
		char jar_file[SS_MAXPATHLEN] = "";
		//-----------------------------
		strcpy(temp_file, temp_dir);
		strcat(temp_file, "\\");
		strcat(temp_file, new_file_name); //C:\Windows\temp\\new_ds_name.pdf  ����·��
										  //printf("�����pdf·�����֣� %s\n",temp_file);
										  //--------------------------
		strcpy(temp_file2, temp_dir);
		strcat(temp_file2, "\\");
		strcat(temp_file2, new_file_name);
		strcat(temp_file2, ".pdf");        //C:\Windows\temp\\new_ds_name.pdf.pdf  ���·��
										   //printf("�����pdf·�����֣� %s\n", temp_file);
										   //-----------------------------4
		strcpy(img_file, tc_root_dir);
		//strcat(img_file, "\\bin\\");
		strcat(img_file, "\\");
		strcat(img_file, "sign.jpg");  //C:\Siemens\Teamcenter11\\bin
									   //-----------------------------
		strcpy(jar_file, tc_root_dir);
		strcat(jar_file, "\\bin\\");
		strcat(jar_file, "insert_pdf.jar");

		//ITKCALL(IMF_export_file(ref_object, temp_file));
		//ITKCALL(IMF_export_file(ref_object, temp_file2));
		//��TcFile�������ϵͳ�е�����·������·������������ڵ�ǰ����Ŀ¼��(file_tag,pathname)
		IMF_export_file(ref_object, temp_file);
		IMF_export_file(ref_object, temp_file2);
		int   iCnt;
		char *user_lib_env, pTempStr[500];
		char local_path[MAX_PATH] = "";
		char cmd[256] = "";
		//strcpy( cmd, user_lib_env );
		strcpy(cmd, "java -jar \"");
		strcat(cmd, jar_file);
		strcat(cmd, "\" \"");
		strcat(cmd, temp_file2);
		strcat(cmd, "\" \"");
		strcat(cmd, temp_file);
		//strcat(cmd, "\" \"");
		//strcat(cmd, img_file);
		strcat(cmd, "\"");
		printf("\n%s\n", cmd);
		system(cmd);
		/*
		java -jar "C:\Siemens\Teamcenter11\bin\insert_pdf.jar"
		"C:\Users\ADMINI~1\AppData\Local\Temp\f__pdf_hya04288i4tlp.pdf.pdf" 
		"C:\Users\ADMINI~1\AppData\Local\Temp\f__pdf_hya04288i4tlp.pdf"
		*/
		//strcpy(user_lib_env, local_path);
		tag_t new_file_tag = NULLTAG;
		IMF_file_t file_descriptor;
		/*
		ITKCALL(IMF_import_file(temp_file, new_file_name, SS_BINARY, &new_file_tag, &file_descriptor));
		ITKCALL(IMF_set_original_file_name(new_file_tag, origin_file_name));
		ITKCALL(IMF_close_file(file_descriptor));
		ITKCALL(AOM_save(new_file_tag));
		ITKCALL(AOM_unlock(new_file_tag));
		ITKCALL(AOM_lock(spec_dataset_rev));
		ITKCALL(AE_remove_dataset_named_ref_by_tag(spec_dataset_rev, ref_name, ref_object));
		ITKCALL(AE_add_dataset_named_ref(spec_dataset_rev, ref_name, AE_PART_OF, new_file_tag));
		ITKCALL(AOM_save(spec_dataset_rev));
		ITKCALL(AOM_unlock(spec_dataset_rev));
		*/
		IMF_import_file(temp_file, new_file_name, SS_BINARY, &new_file_tag, &file_descriptor);
		IMF_set_original_file_name(new_file_tag, origin_file_name);
		IMF_close_file(file_descriptor);
		AOM_save(new_file_tag);
		AOM_unlock(new_file_tag);
		AOM_lock(spec_dataset_rev);
		AE_remove_dataset_named_ref_by_tag(spec_dataset_rev, ref_name, ref_object);
		AE_add_dataset_named_ref(spec_dataset_rev, ref_name, AE_PART_OF, new_file_tag);
		AOM_save(spec_dataset_rev);
		AOM_unlock(spec_dataset_rev);
		//free(new_file_name);
		//free(temp_dir);
	}

	return ITK_ok;
}
/**
	��word���ˮӡ
*/
int insert_word(tag_t dataset, char* ext,char *type,tag_t rev)
{
	printf("word���ˮӡ��ʼת��pdf��ʼ \r\n");
	tag_t spec_dataset_rev = NULLTAG,
		ref_object = NULLTAG,
	ref_object2 = NULLTAG;
	char* object_name = NULL;
	AE_reference_type_t reference_type;
	AE_reference_type_t reference_type2;
	//ITKCALL(AE_ask_dataset_latest_rev(dataset, &spec_dataset_rev));
	AE_ask_dataset_latest_rev(dataset, &spec_dataset_rev);
	char ref_name[WSO_name_size_c + 1] = "word";
	char ref_name2[WSO_name_size_c + 1] = "PDF_Reference";
	//ITKCALL(AE_ask_dataset_named_ref(spec_dataset_rev, ref_name, &reference_type, &ref_object));
	AE_ask_dataset_named_ref(spec_dataset_rev, ref_name, &reference_type, &ref_object);

	AOM_ask_value_string(dataset,"object_name",&object_name); //ԭ��word���ݼ���object_name 


	if (reference_type == AE_PART_OF)
	{
		char pathname[SS_MAXPATHLEN] = "";
		//ITKCALL(IMF_ask_file_pathname(ref_object, SS_WNT_MACHINE, pathname));
		//���ش����������TcFile�����������ļ�������·������·�����ĸ�ʽ��ȡ���ڻ������͵�ֵ
		IMF_ask_file_pathname(ref_object, SS_WNT_MACHINE, pathname);
		char origin_file_name[IMF_filename_size_c + 1] = "";
		//ITKCALL(IMF_ask_original_file_name(ref_object, origin_file_name));
		IMF_ask_original_file_name(ref_object, origin_file_name);
		
		char new_ds_name[WSO_name_size_c + 1] = "";
		char* new_file_name = USER_new_file_name(new_ds_name, ref_name, ext, 0);//����Ϊ���ݼ������ļ�����
																				//char *new_file_name = USER_new_file_name(new_ds_name, ref_name, "pdf", 0);
		char* temp_dir = getenv("temp");		//C:\Windows\temp
		char* tc_root_dir = getenv("tc_root");		//C:\Siemens\Teamcenter11
		char img_file[SS_MAXPATHLEN] = "";
		char temp_file[SS_MAXPATHLEN] = "";
		char temp_file2[SS_MAXPATHLEN] = "";
		char temp_file3[SS_MAXPATHLEN] = ""; //pdf���ˮӡ֮�����ɵ��ļ���·��
		char jar_file[SS_MAXPATHLEN] = ""; //ˮӡjar����·��
		char jar_file2[SS_MAXPATHLEN] = ""; //ת����pdf jar����·�� 
		//-----------------------------
		char new_file_name2[WSO_name_size_c + 1] = "";
		strcpy(new_file_name2, new_file_name);
		strcat(new_file_name2, ".pdf"); //new_file_name.pdf  ת����pdf�ļ�������

		char new_file_name3[WSO_name_size_c + 1] = "";
		strcpy(new_file_name3, new_file_name);
		strcat(new_file_name3, "new.pdf"); //new_file_namenew.pdf  ���ˮӡ��pdf�ļ�������

		
		strcpy(temp_file, temp_dir);
		strcat(temp_file, "\\");
		strcat(temp_file, new_file_name); //C:\Windows\temp\\new_ds_name.doc||docx  ����·��
		strcat(temp_file,type);			  //printf("�����pdf·�����֣� %s\n",temp_file);
										  
		strcpy(temp_file2, temp_dir);
		strcat(temp_file2, "\\"); 
		strcat(temp_file2, new_file_name2); //C:\Windows\temp\\new_ds_name.pdf  ���·��
		

		strcpy(temp_file3, temp_dir);
		strcat(temp_file3, "\\");
		strcat(temp_file3, new_file_name3);
		//strcat(temp_file2, ".docx");        //C:\Windows\temp\\new_ds_name.pdf.pdf  ���·��
		//strcat(temp_file3, type);			  //printf("�����pdf·�����֣� %s\n", temp_file);
										   
		strcpy(img_file, tc_root_dir);
		//strcat(img_file, "\\bin\\");
		strcat(img_file, "\\");
		strcat(img_file, "sign.jpg");  //C:\Siemens\Teamcenter11\\bin
									  
		strcpy(jar_file, tc_root_dir);
		strcat(jar_file, "\\bin\\");
		strcat(jar_file, "insert_pdf.jar");
		
		strcpy(jar_file2, tc_root_dir);
		strcat(jar_file2, "\\bin\\");
		strcat(jar_file2, "change.jar");

		//ITKCALL(IMF_export_file(ref_object, temp_file));
		//ITKCALL(IMF_export_file(ref_object, temp_file2));
		//��TcFile�������ϵͳ�е�����·������·������������ڵ�ǰ����Ŀ¼��(file_tag,pathname)
		IMF_export_file(ref_object, temp_file);
		//IMF_export_file(ref_object, temp_file2);

		int   iCnt;
		char* user_lib_env, pTempStr[500];
		char local_path[MAX_PATH] = "";
		char cmd[256] = "";
		char cmd2[256] ="";
		//strcpy( cmd, user_lib_env );
		strcpy(cmd, "java -jar \"");
		strcat(cmd, jar_file2);
		strcat(cmd, "\" \"");
		strcat(cmd, temp_file);
		strcat(cmd, "\" \"");
		strcat(cmd, temp_file2);
		//strcat(cmd, "\" \"");
		//strcat(cmd, img_file); 
		strcat(cmd, "\""); //cmd = Java -jar input��\�� output(\)  ת��pdf��cmd 
		printf("\n%s\n", cmd);

		strcpy(cmd2, "java -jar \"");
		strcat(cmd2, jar_file);
		strcat(cmd2, "\" \"");
		strcat(cmd2, temp_file2);
		strcat(cmd2, "\" \"");
		strcat(cmd2, temp_file3);
		//strcat(cmd2, "\" \"");
		//strcat(cmd2, img_file);
		strcat(cmd2, "\"");
		printf("\n%s\n", cmd2);				//���ˮӡ��cmd

		//��workת����pdf   //pdf·����temp_file3 
		system(cmd); 


		//�õ�pdf���������֣��õ�tc��ת����pdf������
		int num = 0,tc_num = 0;
		char* revbuf[8] = { }; //��ŷָ������ַ��� 
		char* revbuf2[3] ={ };
		split(origin_file_name, ".", revbuf, &num); //���ú������зָ�  �õ����ֺͺ�׺��
		split(object_name, ".", revbuf2, &tc_num);

		//�½�pdf���ݼ�
		tag_t item=NULLTAG;
		ITEM_ask_item_of_rev(rev,&item);
		const char name[AE_datasettype_name_size_c + 1] = "PDF";
		//cout << name << endl;
		tag_t datasetType = NULLTAG,tool = NULLTAG;
		tag_t newDatasetTag = NULLTAG;  //�����ݼ���tag
		tag_t relation = NULLTAG;
		AE_find_datasettype(name, &datasetType);

		AE_ask_datasettype_def_tool(datasetType,&tool);

		AE_create_dataset(datasetType, revbuf2[0],"", &newDatasetTag);

		AE_set_dataset_tool(newDatasetTag,tool);

		AE_set_dataset_format(newDatasetTag, name);

		AOM_save(newDatasetTag);

		ITEM_attach_rev_object(rev, newDatasetTag, ITEM_specification_atth);

		ITEM_save_item(item);

		//AE_create_dataset_with_id(datasetType, object_name,"wordתpdf","1079","A",&newDatasetTag);
		//printf("�½����ݼ���tag�� %s\n", newDatasetTag);
		//cout << newDatasetTag << endl;
		//GRM_create_relation(rev, newDatasetTag, relation_type,NULLTAG, &relation);

		//AOM_lock(relation_type);
		//AOM_unlock(relation_type);
		//GRM_set_user_data(relation_type, newDatasetTag);
		//AOM_save(relation_type);
		//GRM_save_relation(relation);
		
		//��ת��֮���pdf���ˮӡ
		system(cmd2);
		
		/*
		java -jar "C:\Siemens\Teamcenter11\bin\insert_pdf.jar"
		"C:\Users\ADMINI~1\AppData\Local\Temp\f__pdf_hya04288i4tlp.pdf.pdf"
		"C:\Users\ADMINI~1\AppData\Local\Temp\f__pdf_hya04288i4tlp.pdf"
																  .excel.xlsx||xls
																  .excel
		*/
		//AE_ask_dataset_named_ref(relation, ref_name2, &reference_type2, &ref_object2);
		//char* new_file_name2 = USER_new_file_name(new_ds_name2, ref_name2, "pdf", 0);

		
		//strcpy(user_lib_env, local_path);
		tag_t new_file_tag = NULLTAG;
		IMF_file_t file_descriptor;

		IMF_import_file(temp_file3, new_file_name3, SS_BINARY, &new_file_tag, &file_descriptor);
		IMF_set_original_file_name(new_file_tag, strcat(revbuf[0],".pdf"));
		IMF_close_file(file_descriptor);
		AOM_save(new_file_tag);
		AOM_unlock(new_file_tag);
		AOM_lock(newDatasetTag);
		//AE_remove_dataset_named_ref_by_tag(spec_dataset_rev, ref_name, ref_object);
		AE_add_dataset_named_ref(newDatasetTag, ref_name2, AE_PART_OF, new_file_tag);
		AOM_save(newDatasetTag);
		AOM_unlock(newDatasetTag);

		//free(new_file_name);
		//free(temp_dir);
	}

	return ITK_ok;
}
/**
	��excel���ˮӡ
*/
int insert_excel2(tag_t dataset, char* ext,char* type ,tag_t rev)
{
	printf("excel���ˮӡ��ʼ \r\n");
	tag_t spec_dataset_rev = NULLTAG,
		ref_object = NULLTAG;
	AE_reference_type_t reference_type;
	
	//ITKCALL(AE_ask_dataset_latest_rev(dataset, &spec_dataset_rev));
	AE_ask_dataset_latest_rev(dataset, &spec_dataset_rev);
	char ref_name[WSO_name_size_c + 1] = "excel";
	//ITKCALL(AE_ask_dataset_named_ref(spec_dataset_rev, ref_name, &reference_type, &ref_object));
	AE_ask_dataset_named_ref(spec_dataset_rev, ref_name, &reference_type, &ref_object);
	if (reference_type == AE_PART_OF)
	{
		char pathname[SS_MAXPATHLEN] = "";
		//ITKCALL(IMF_ask_file_pathname(ref_object, SS_WNT_MACHINE, pathname));
		//���ش����������TcFile�����������ļ�������·������·�����ĸ�ʽ��ȡ���ڻ������͵�ֵ
		IMF_ask_file_pathname(ref_object, SS_WNT_MACHINE, pathname);
		char origin_file_name[IMF_filename_size_c + 1] = "";
		//ITKCALL(IMF_ask_original_file_name(ref_object, origin_file_name));
		IMF_ask_original_file_name(ref_object, origin_file_name);
		char new_ds_name[WSO_name_size_c + 1] = "";
		char* new_file_name = USER_new_file_name(new_ds_name, ref_name, ext, 0);//����Ϊ���ݼ������ļ�����
																				//char *new_file_name = USER_new_file_name(new_ds_name, ref_name, "pdf", 0);
		char* temp_dir = getenv("temp");		//C:\Windows\temp
		char* tc_root_dir = getenv("tc_root");		//C:\Siemens\Teamcenter11
		char img_file[SS_MAXPATHLEN] = "";
		char temp_file[SS_MAXPATHLEN] = "";
		char temp_file2[SS_MAXPATHLEN] = "";
		char jar_file[SS_MAXPATHLEN] = "";
		//-----------------------------
		//strcpy(new_file_name,".pdf");

		strcpy(temp_file, temp_dir);
		strcat(temp_file, "\\");
		strcat(temp_file, new_file_name); //C:\Windows\temp\\new_ds_name.pdf  ����·��
										  //printf("�����pdf·�����֣� %s\n",temp_file);
										  //--------------------------
		strcpy(temp_file2, temp_dir);
		strcat(temp_file2, "\\");
		strcat(temp_file2, new_file_name);
		//strcat(temp_file2, ".xlsx");        //C:\Windows\temp\\new_ds_name.pdf.pdf  ���·��
		strcat(temp_file2, type);							   //printf("�����pdf·�����֣� %s\n", temp_file);
										   //-----------------------------4
		strcpy(img_file, tc_root_dir);
		//strcat(img_file, "\\bin\\");
		strcat(img_file, "\\");
		strcat(img_file, "sign.jpg");  //C:\Siemens\Teamcenter11\\bin
									   //-----------------------------
		strcpy(jar_file, tc_root_dir);
		strcat(jar_file, "\\bin\\");
		strcat(jar_file, "insert_pdf.jar");

		//ITKCALL(IMF_export_file(ref_object, temp_file));
		//ITKCALL(IMF_export_file(ref_object, temp_file2));
		//��TcFile�������ϵͳ�е�����·������·������������ڵ�ǰ����Ŀ¼��(file_tag,pathname)
		IMF_export_file(ref_object, temp_file);
		IMF_export_file(ref_object, temp_file2);
		int   iCnt;
		char* user_lib_env, pTempStr[500];
		char local_path[MAX_PATH] = "";
		char cmd[256] = "";
		//strcpy( cmd, user_lib_env );
		strcpy(cmd, "java -jar \"");
		strcat(cmd, jar_file);
		strcat(cmd, "\" \"");
		strcat(cmd, temp_file2);
		strcat(cmd, "\" \"");
		strcat(cmd, temp_file);
		//strcat(cmd, "\" \"");
		//strcat(cmd, img_file);
		strcat(cmd, "\"");
		printf("\n%s\n", cmd);
		system(cmd);
		/*
		java -jar "C:\Siemens\Teamcenter11\bin\insert_pdf.jar" "C:\Users\ADMINI~1\AppData\Local\Temp\f__pdf_hya04288i4tlp.pdf.pdf" "C:\Users\ADMINI~1\AppData\Local\Temp\f__pdf_hya04288i4tlp.pdf"
		*/
		//strcpy(user_lib_env, local_path);
		tag_t new_file_tag = NULLTAG;
		IMF_file_t file_descriptor;
		/*
		ITKCALL(IMF_import_file(temp_file, new_file_name, SS_BINARY, &new_file_tag, &file_descriptor));
		ITKCALL(IMF_set_original_file_name(new_file_tag, origin_file_name));
		ITKCALL(IMF_close_file(file_descriptor));
		ITKCALL(AOM_save(new_file_tag));
		ITKCALL(AOM_unlock(new_file_tag));
		ITKCALL(AOM_lock(spec_dataset_rev));
		ITKCALL(AE_remove_dataset_named_ref_by_tag(spec_dataset_rev, ref_name, ref_object));
		ITKCALL(AE_add_dataset_named_ref(spec_dataset_rev, ref_name, AE_PART_OF, new_file_tag));
		ITKCALL(AOM_save(spec_dataset_rev));
		ITKCALL(AOM_unlock(spec_dataset_rev));
		*/
		IMF_import_file(temp_file, new_file_name, SS_BINARY, &new_file_tag, &file_descriptor);
		IMF_set_original_file_name(new_file_tag, origin_file_name);
		IMF_close_file(file_descriptor);
		AOM_save(new_file_tag);
		AOM_unlock(new_file_tag);
		AOM_lock(spec_dataset_rev);
		AE_remove_dataset_named_ref_by_tag(spec_dataset_rev, ref_name, ref_object);
		AE_add_dataset_named_ref(spec_dataset_rev, ref_name, AE_PART_OF, new_file_tag);
		AOM_save(spec_dataset_rev);
		AOM_unlock(spec_dataset_rev);
		//free(new_file_name);
		//free(temp_dir);
	}

	return ITK_ok;
}
/**
	��excelת����pdf֮�����ˮӡ
*/
int insert_excel(tag_t dataset, char* ext, char* type, tag_t rev){
	printf("excel���ˮӡ��ʼת��pdf��ʼ \r\n");
	tag_t spec_dataset_rev = NULLTAG,
		ref_object = NULLTAG;
	
	char* object_name = NULL;
	AE_reference_type_t reference_type;
	//ITKCALL(AE_ask_dataset_latest_rev(dataset, &spec_dataset_rev));
	AE_ask_dataset_latest_rev(dataset, &spec_dataset_rev);
	char ref_name[WSO_name_size_c + 1] = "excel";
	char ref_name2[WSO_name_size_c + 1] = "PDF_Reference";
	//ITKCALL(AE_ask_dataset_named_ref(spec_dataset_rev, ref_name, &reference_type, &ref_object));
	AE_ask_dataset_named_ref(spec_dataset_rev, ref_name, &reference_type, &ref_object);

	AOM_ask_value_string(dataset, "object_name", &object_name); //ԭ��excel���ݼ���object_name 


	if (reference_type == AE_PART_OF)
	{
		char pathname[SS_MAXPATHLEN] = "";
		//ITKCALL(IMF_ask_file_pathname(ref_object, SS_WNT_MACHINE, pathname));
		//���ش����������TcFile�����������ļ�������·������·�����ĸ�ʽ��ȡ���ڻ������͵�ֵ
		IMF_ask_file_pathname(ref_object, SS_WNT_MACHINE, pathname);
		char origin_file_name[IMF_filename_size_c + 1] = "";
		//ITKCALL(IMF_ask_original_file_name(ref_object, origin_file_name));
		IMF_ask_original_file_name(ref_object, origin_file_name);

		char new_ds_name[WSO_name_size_c + 1] = "";
		char* new_file_name = USER_new_file_name(new_ds_name, ref_name, ext, 0);//����Ϊ���ݼ������ļ�����
																				//char *new_file_name = USER_new_file_name(new_ds_name, ref_name, "pdf", 0);
		char* temp_dir = getenv("temp");		//C:\Windows\temp
		char* tc_root_dir = getenv("tc_root");		//C:\Siemens\Teamcenter11
		char img_file[SS_MAXPATHLEN] = "";
		char temp_file[SS_MAXPATHLEN] = "";
		char temp_file2[SS_MAXPATHLEN] = "";
		char temp_file3[SS_MAXPATHLEN] = ""; //pdf���ˮӡ֮�����ɵ��ļ���·��
		char jar_file[SS_MAXPATHLEN] = ""; //ˮӡjar����·��
		char jar_file2[SS_MAXPATHLEN] = ""; //ת����pdf jar����·�� 
		//-----------------------------
		char new_file_name2[WSO_name_size_c + 1] = "";
		strcpy(new_file_name2, new_file_name);
		strcat(new_file_name2, ".pdf"); //new_file_name.pdf  ת����pdf�ļ�������

		char new_file_name3[WSO_name_size_c + 1] = "";
		strcpy(new_file_name3, new_file_name);
		strcat(new_file_name3, "new.pdf"); //new_file_namenew.pdf  ���ˮӡ��pdf�ļ�������


		strcpy(temp_file, temp_dir);
		strcat(temp_file, "\\");
		strcat(temp_file, new_file_name); //C:\Windows\temp\\new_ds_name.doc||docx  ����·��
		strcat(temp_file, type);			  //printf("�����pdf·�����֣� %s\n",temp_file);

		strcpy(temp_file2, temp_dir);
		strcat(temp_file2, "\\");
		strcat(temp_file2, new_file_name2); //C:\Windows\temp\\new_ds_name.pdf  ���·��


		strcpy(temp_file3, temp_dir);
		strcat(temp_file3, "\\");
		strcat(temp_file3, new_file_name3);
		//strcat(temp_file2, ".docx");        //C:\Windows\temp\\new_ds_name.pdf.pdf  ���·��
		//strcat(temp_file3, type);			  //printf("�����pdf·�����֣� %s\n", temp_file);

		strcpy(img_file, tc_root_dir);
		//strcat(img_file, "\\bin\\");
		strcat(img_file, "\\");
		strcat(img_file, "sign.jpg");  //C:\Siemens\Teamcenter11\\bin

		strcpy(jar_file, tc_root_dir);
		strcat(jar_file, "\\bin\\");
		strcat(jar_file, "insert_pdf.jar");

		strcpy(jar_file2, tc_root_dir);
		strcat(jar_file2, "\\bin\\");
		strcat(jar_file2, "change.jar");

		//ITKCALL(IMF_export_file(ref_object, temp_file));
		//ITKCALL(IMF_export_file(ref_object, temp_file2));
		//��TcFile�������ϵͳ�е�����·������·������������ڵ�ǰ����Ŀ¼��(file_tag,pathname)
		IMF_export_file(ref_object, temp_file);
		//IMF_export_file(ref_object, temp_file2);

		int   iCnt;
		char* user_lib_env, pTempStr[500];
		char local_path[MAX_PATH] = "";
		char cmd[256] = "";
		char cmd2[256] = "";
		//strcpy( cmd, user_lib_env );
		strcpy(cmd, "java -jar \"");
		strcat(cmd, jar_file2);
		strcat(cmd, "\" \"");
		strcat(cmd, temp_file);
		strcat(cmd, "\" \"");
		strcat(cmd, temp_file2);
		//strcat(cmd, "\" \"");
		//strcat(cmd, img_file); 
		strcat(cmd, "\""); //cmd = Java -jar input��\�� output(\)  ת��pdf��cmd 
		printf("\n%s\n", cmd);

		strcpy(cmd2, "java -jar \"");
		strcat(cmd2, jar_file);
		strcat(cmd2, "\" \"");
		strcat(cmd2, temp_file2);
		strcat(cmd2, "\" \"");
		strcat(cmd2, temp_file3);
		//strcat(cmd2, "\" \"");
		//strcat(cmd2, img_file);
		strcat(cmd2, "\"");
		printf("\n%s\n", cmd2);				//���ˮӡ��cmd

		//��workת����pdf   //pdf·����temp_file3 
		system(cmd);


		//�õ�pdf���������֣��õ�tc��ת����pdf������
		int num = 0, tc_num = 0;
		char* revbuf[8] = { }; //��ŷָ������ַ��� 
		char* revbuf2[3] = { };
		split(origin_file_name, ".", revbuf, &num); //���ú������зָ�  �õ����ֺͺ�׺��
		split(object_name, ".", revbuf2, &tc_num);

		//�½�pdf���ݼ�
		tag_t item = NULLTAG;
		ITEM_ask_item_of_rev(rev, &item);
		const char name[AE_datasettype_name_size_c + 1] = "PDF";
		//cout << name << endl;
		tag_t datasetType = NULLTAG, tool = NULLTAG;
		tag_t newDatasetTag = NULLTAG;  //�����ݼ���tag
		tag_t relation = NULLTAG;
		AE_find_datasettype(name, &datasetType);

		AE_ask_datasettype_def_tool(datasetType, &tool);

		AE_create_dataset(datasetType, revbuf2[0], "", &newDatasetTag);

		AE_set_dataset_tool(newDatasetTag, tool);

		AE_set_dataset_format(newDatasetTag, name);

		AOM_save(newDatasetTag);

		ITEM_attach_rev_object(rev, newDatasetTag, ITEM_specification_atth);

		ITEM_save_item(item);

		//AE_create_dataset_with_id(datasetType, object_name,"wordתpdf","1079","A",&newDatasetTag);
		//printf("�½����ݼ���tag�� %s\n", newDatasetTag);
		//cout << newDatasetTag << endl;
		//GRM_create_relation(rev, newDatasetTag, relation_type,NULLTAG, &relation);

		//AOM_lock(relation_type);
		//AOM_unlock(relation_type);
		//GRM_set_user_data(relation_type, newDatasetTag);
		//AOM_save(relation_type);
		//GRM_save_relation(relation);

		//��ת��֮���pdf���ˮӡ
		system(cmd2);

		/*
		java -jar "C:\Siemens\Teamcenter11\bin\insert_pdf.jar"
		"C:\Users\ADMINI~1\AppData\Local\Temp\f__pdf_hya04288i4tlp.pdf.pdf"
		"C:\Users\ADMINI~1\AppData\Local\Temp\f__pdf_hya04288i4tlp.pdf"
																  .excel.xlsx||xls
																  .excel
		*/
		//AE_ask_dataset_named_ref(relation, ref_name2, &reference_type2, &ref_object2);
		//char* new_file_name2 = USER_new_file_name(new_ds_name2, ref_name2, "pdf", 0);


		//strcpy(user_lib_env, local_path);
		tag_t new_file_tag = NULLTAG;
		IMF_file_t file_descriptor;

		IMF_import_file(temp_file3, new_file_name3, SS_BINARY, &new_file_tag, &file_descriptor);
		IMF_set_original_file_name(new_file_tag, strcat(revbuf[0], ".pdf"));
		IMF_close_file(file_descriptor);
		AOM_save(new_file_tag);
		AOM_unlock(new_file_tag);
		AOM_lock(newDatasetTag);
		//AE_remove_dataset_named_ref_by_tag(spec_dataset_rev, ref_name, ref_object);
		AE_add_dataset_named_ref(newDatasetTag, ref_name2, AE_PART_OF, new_file_tag);
		AOM_save(newDatasetTag);
		AOM_unlock(newDatasetTag);

		//free(new_file_name);
		//free(temp_dir);
	}

	return ITK_ok;

}

/**
*��PDF�в���ͼƬhandler
*
*/
int jl_insert_img_pdf(EPM_action_message_t msg)
{
	//printf("%s\n","pdf����ͼƬ��ʼ");
	//��������
	POM_AM__set_application_bypass(true);
	int ifail = ITK_ok, att_cnt = 0, i = 0;
	tag_t task_tag = NULLTAG,
		rootTask_tag = NULLTAG,
		*attachments = NULL,
		relation_type = NULLTAG,
		type_tag = NULLTAG;
	char tgt_type[WSO_name_size_c + 1] = "",
		type_class[TCTYPE_class_name_size_c + 1] = "",
		rev_type[ITEM_type_size_c + 1] = "";
	map<tag_t, string> rev_map;
	//��ȡ����
	task_tag = msg.task;
	//��ȡ������
	//ITKCALL(ifail = EPM_ask_root_task(task_tag, &rootTask_tag));
	ifail = EPM_ask_root_task(task_tag, &rootTask_tag);
	//��ȡ����Ŀ�����
	//ITKCALL(ifail = EPM_ask_attachments(rootTask_tag, EPM_target_attachment, &att_cnt, &attachments));
	ifail = EPM_ask_attachments(rootTask_tag, EPM_target_attachment, &att_cnt, &attachments);
	for (i = 0; i < att_cnt; i++)
	{

		
		//ITKCALL(TCTYPE_ask_object_type(attachments[i], &type_tag));
		TCTYPE_ask_object_type(attachments[i], &type_tag);
		//ITKCALL(ifail = TCTYPE_ask_class_name(type_tag, type_class));
		ifail = TCTYPE_ask_class_name(type_tag, type_class);
		printf("type_class : %s \r\n", type_class);

		//���˵��ǰ汾����
		if (((strstr(type_class, "Revision") != NULL) || (strstr(type_class, "revision") != NULL))
			&& (strstr(type_class, "Master") == NULL) && (strstr(type_class, "master") == NULL)
			&& (strstr(type_class, "BOM") == NULL) && (strstr(type_class, "bom") == NULL) && (strstr(type_class, "Bom") == NULL))
		{
			//���汾����д�뵽����
			rev_map.insert(pair<tag_t, string>(attachments[i], ""));
		}
	}

	//
	//ITKCALL(GRM_find_relation_type(IMAN_specification_rtype, &relation_type));//ԭ����
	//ITKCALL(GRM_create_relation(pTags[i],new_dataset,relation_type,NULLTAG,&relation));
	GRM_find_relation_type(IMAN_specification_rtype, &relation_type);

	//printf("%s\n","����������ϵ�������ƵĹ�ϵ���ͱ�ǡ�");

	//�����汾����
	map<tag_t, string>::iterator rev_it;
	for (rev_it = rev_map.begin(); rev_it != rev_map.end(); rev_it++)
	{
		//printf("%s\n", "111111111");
		//ITKCALL(ITEM_ask_rev_type(rev_it->first, rev_type));
		ITEM_ask_rev_type(rev_it->first, rev_type);
		tag_t * second_tags = NULL;
		int second_tags_count = 0;
		//if(strcmp(rev_type,"JF3_CPRevision")==0){
		if (true) {
			//������ݼ������߼�
			//ITKCALL(GRM_list_secondary_objects_only(rev_it->first, relation_type, &second_tags_count, &second_tags));
			GRM_list_secondary_objects_only(rev_it->first, relation_type, &second_tags_count, &second_tags);
			for (i = 0; i < second_tags_count; i++) {
				char * second_tag_type = NULL;
				//ITKCALL(AOM_ask_value_string(second_tags[i], "object_type", &second_tag_type));
				AOM_ask_value_string(second_tags[i], "object_type", &second_tag_type);
				printf(" second_tag_type => %s \r\n", second_tag_type);

				if (strcmp(second_tag_type, "PDF") == 0 || strcmp(second_tag_type, "CAEAnalysisDS") == 0) {
					insert_pdf(second_tags[i], "pdf",second_tag_type);
					printf("%s\n", "pdfˮӡ��ӳɹ�");
				}
				else if (strcmp(second_tag_type, "MSWordX") == 0)
				{
					insert_word(second_tags[i], "word",".docx", rev_it->first);
					printf("wordˮӡ��ӳɹ�\n");
				}
				else if (strcmp(second_tag_type, "MSWord") == 0)
				{
					insert_word(second_tags[i], "word", ".doc", rev_it->first);
					printf("wordˮӡ��ӳɹ�\n");
				}
				else if (strcmp(second_tag_type, "MSExcelX") == 0 ) {
					
					insert_excel(second_tags[i], "excel",".xlsx", rev_it->first);
					printf("excelˮӡ��ӳɹ�\n");
				}
				else if (strcmp(second_tag_type, "MSExcel") == 0)
				{
					insert_excel(second_tags[i], "excel", ".xls", rev_it->first);
					printf("excelˮӡ��ӳɹ�\n");
				}
				if (second_tag_type != NULL) {
					MEM_free(second_tag_type);
					second_tag_type = NULL;
				}
			}
		}

		if (second_tags != NULL) {
			MEM_free(second_tags);
			second_tags = NULL;
		}
	}


	if (attachments != NULL) {
		MEM_free(attachments);
		attachments = NULL;
	}
	POM_AM__set_application_bypass(false);
	return ifail;
}


