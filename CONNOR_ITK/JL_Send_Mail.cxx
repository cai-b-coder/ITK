#include "epm_handler_common.h"

#include <tc\preferences.h>
#include <tc\tc_macros.h>
#include <tc\tc_startup.h>
#include <epm\epm.h>
#include <sa\user.h>
#include <fclasses\tc_date.h>
#include <tccore\aom.h>

#include <tccore\aom_prop.h>
#include <tccore\item.h>
#include <string>

#include <map>
#include <vector>
#include<iostream>

#include <windows.h>
#include <stdio.h>
#include <WinSock.h>
#include <iostream>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

#include "string"




#define DOFREE(obj)								\
{												\
	if(obj)										\
	{											\
	MEM_free(obj);							\
	obj = NULL;								\
	}											\
}

struct Base64Date6
{
	unsigned int d4 : 6;
	unsigned int d3 : 6;
	unsigned int d2 : 6;
	unsigned int d1 : 6;
};

// Э���м��ܲ���ʹ�õ���base64����
char ConvertToBase64(char c6);
void EncodeBase64(char *dbuf, char *buf128, int len);
void SendMail(char *email, char *body, char * server_address, int port, char * from_email, char * password);
int OpenSocket(struct sockaddr *addr);

//////////////////////////////////////////////////////////////////////////////////




int JL_Send_Mail(EPM_action_message_t msg)
{
	printf("��ʼִ��======JL_Send_Mail\n");

	int ifail = ITK_ok;
	tag_t root_task = NULLTAG;
	tag_t * attachments = NULLTAG;
	int count = 0;
	char * ProcessOwner = NULL;
	tag_t process_assignee_user = NULLTAG;
	tag_t process_assignee_person = NULL_TAG;
	tag_t default_group = NULL_TAG;
	char * group = NULL;
	date_t  process_creation_date;

	//�ʼ�����
	char *EmailTo = "";
	//char serverAddress[] = "";
	char * server_address = NULL;
	int port = 25;
	char * from_email = "";
	char * password = "";
	char * port_string = NULL;
	char * ChangeType = NULL;
	char * ChangeTheOddNumber = NULL;
	char  EmailContents[5120] = "";
	char EmailTo1[5120] = "";
	char from_email1[5120] = "";
	//��ȡ��ѡ��
	int valueNum = 0;
	PREF_ask_char_value("JL_current_user_server_address", valueNum, &server_address);
	PREF_ask_char_value("JL_current_user_server_address_port", valueNum, &port_string);
	PREF_ask_char_value("JL_current_user_mail_password", valueNum, &password);
	printf("JL_current_user_server_address��%s=======================\n", server_address);
	port = atoi(port_string);
	//��ȡ����
	char all_ID[5120] = "";
	char all_ID_name[5120] = "";
	EPM_ask_root_task(msg.task, &root_task);
	EPM_ask_attachments(root_task, EPM_target_attachment, &count, &attachments);
	for (int i = 0; i < count; i++) {
		char * item_id, *object_name;

		AOM_ask_value_string(attachments[i], "item_id", &item_id);
		AOM_ask_value_string(attachments[i], "object_name", &object_name);
		sprintf_s(all_ID, "%s ", item_id);
		sprintf_s(all_ID_name, "%s,%s ", item_id, object_name);

	}
	printf("all_ID��%s=======================\n", all_ID);
	printf("all_ID_name��%s=======================\n", all_ID_name);

	//��ȡ����(������͡��������)
	TC_argument_list_t * arguments = msg.arguments;
	int arg_cnt = TC_number_of_arguments(arguments);
	printf("TC_number_of_arguments %d\n", arg_cnt);
	char * arg = NULL, *temp_key = NULL, *temp_val = NULL;
	for (int ii = 0; ii<arg_cnt; ii++)
	{
		arg = TC_next_argument(arguments);
		ITK_ask_argument_named_value((const char*)arg, &temp_key, &temp_val);
		printf("temp_key��%s=======================\n", temp_key);
		printf("temp_val��%s=======================\n", temp_val);
		if (tc_strcmp(temp_key, "change_type") == 0) {
			ChangeType = temp_val;
		}
		else {
			printf("temp_key1��=======================\n");
		}
		if (tc_strcmp(temp_key, "change_the_odd_number") == 0) {
			ChangeTheOddNumber = temp_val;
		}
		else
		{
			printf("temp_key2��=======================\n");
		}
	}
	printf("change_type��%s=======================\n", ChangeType);
	printf("ChangeTheOddNumber��%s=======================\n", ChangeTheOddNumber);
	//��ȡ����������
	AOM_ask_value_tag(msg.task, "fnd0Assignee", &process_assignee_user);
	AOM_ask_value_tag(process_assignee_user, "default_group", &default_group);
	AOM_ask_value_tag(process_assignee_user, "person", &process_assignee_person);
	AOM_UIF_ask_value(process_assignee_user, "user_name", &ProcessOwner);
	AOM_ask_value_string_at(process_assignee_person, "PA9", 0, &from_email);
	AOM_UIF_ask_value(default_group, "name", &group);
	AOM_ask_value_date(msg.task, "creation_date", &process_creation_date);
	char * date_string = NULL;
	DATE_date_to_string(process_creation_date, "%Y-%m-%d %H:%M", &date_string);
	strcpy_s(from_email1, from_email);
	//strncpy_s(from_email1, from_email, strlen(from_email) - 2);
	printf("����������ʦ��%s=======================\n", ProcessOwner);
	printf("����������ʦ�ʼ���ַ��%s=======================\n", from_email1);
	printf("����������ʦʱ�䣺%s=======================\n", date_string);
	//��ȡ�ڵ�ѡ�е���
	int valid_signoffs_count = 0;
	tag_t * valid_signoffs_tags = NULL;
	AOM_ask_value_tags(msg.task, "valid_signoffs", &valid_signoffs_count, &valid_signoffs_tags);

	char assign_users[5120] = "";

	if (valid_signoffs_count != 0)
	{
		for (int j = 0; j < valid_signoffs_count; j++)
		{
			char * assign_user_name = NULL;
			tag_t assign_user = NULL_TAG;
			tag_t person = NULL_TAG;
			//tag_t Email = NULL_TAG;
			//������
			AOM_ask_value_tag(valid_signoffs_tags[j], "fnd0Assignee", &assign_user);
			if (assign_user != NULL_TAG)
			{
				AOM_UIF_ask_value(assign_user, "user_name", &assign_user_name);
				AOM_ask_value_tag(assign_user, "person", &person);
				AOM_ask_value_string_at(person, "PA9", 0, &EmailTo);
				strcpy_s(EmailTo1, EmailTo);
				//strncpy_s(EmailTo1, EmailTo, strlen(EmailTo1) - 2);
				//sprintf_s(EmailContents, "From: \"%s\"<%s>\r\n To: \"%s\"<%s>\r\n Subject: %s %s ֪ͨ��%s��\r\n\r\n %s��\n���ã���%s(����)%s(��Ա)������%s %s��%s��%s��ʽ������Ч���������Ϊ%s��\n���ڽ����ڵ�¼Teamcenterϵͳ���ľ�������Ϣ��\n����·��Ϊ�����ҵĹ����б�->�鿴Ҫִ�е������µĸ�����",
				//ProcessOwner, from_email, assign_user_name, EmailTo, all_ID, ChangeType, ChangeTheOddNumber, assign_user_name, group, ProcessOwner, all_ID, all_ID_name, ChangeType, date_string, ChangeTheOddNumber);
				sprintf_s(EmailContents, "From: \"%s\"<%s>\r\n"
					"To: \"%s\"<%s>\r\n"
					"Subject: %s %s ֪ͨ��%s��\r\n\r\n"
					"%s��\n���ã���%s(����)%s(��Ա)������%s %s��%s��%s��ʽ������Ч���������Ϊ%s��\n���ڽ����ڵ�¼Teamcenterϵͳ���ľ�������Ϣ��\n����·��Ϊ�����ҵĹ����б�->�鿴Ҫִ�е������µĸ�����",
					ProcessOwner, from_email, assign_user_name, EmailTo, all_ID, ChangeType, ChangeTheOddNumber, assign_user_name,
					group, ProcessOwner, all_ID, all_ID_name, ChangeType, date_string, ChangeTheOddNumber
				);

				//SendMail("yangjh@connor.net.cn", EmailContents, "smtp.connor.net.cn", 25, "yangjh@connor.net.cn", "ll15271125547+");



				/*char EmailContents1[5120] = "";
				char server_address1[5120] = "";

				char password1[5120] = "";

				strcpy_s(EmailContents1, EmailContents);
				strcpy_s(server_address1, server_address);

				strcpy_s(password1, password);*/
				//EmailTo1[strlen(EmailTo1) - 1] = 0;
				//from_email1[strlen(from_email1) - 1] = 0;


				printf("EmailTo��%s\n EmailContents��%s\n server_address��%s\n port��%d\n from_email��%s\n password��%s\n", EmailTo, EmailContents, server_address, port, from_email, password);
				SendMail(EmailTo1, EmailContents, server_address, port, from_email1, password);
				printf("�����ˣ�%s=======================\n", EmailContents);
				printf("�����ˣ�%=======================\n", assign_user_name);
				printf("�������ʼ���ַ��%s=======================\n", EmailTo1);
				//ƴ�Ӹ�����
				sprintf_s(assign_users, "%s%s,", assign_users, assign_user_name, "");
			}
			DOFREE(assign_user_name);
		}
	}
	printf("�ڵ�ѡ�е�������=======================================%s\n", assign_users);



	//SendMail(EmailTo, EmailContents, serverAddress, port, from_email, password);*/
	return 0;

}


/////////////////////////////////////////////////////////////////////////

char ConvertToBase64(char uc)
{
	if (uc < 26)
	{
		return 'A' + uc;
	}
	if (uc < 52)
	{
		return 'a' + (uc - 26);
	}
	if (uc < 62)
	{
		return '0' + (uc - 52);
	}
	if (uc == 62)
	{
		return '+';
	}
	return '/';
}

// base64��ʵ��
void EncodeBase64(char *dbuf, char *buf128, int len)
{
	struct Base64Date6 *ddd = NULL;
	int           i = 0;
	char          buf[256] = { 0 };
	char          *tmp = NULL;
	char          cc = '\0';

	memset(buf, 0, 256);
	strcpy_s(buf, 256, buf128);
	for (i = 1; i <= len / 3; i++)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
		dbuf[(i - 1) * 4 + 3] = ConvertToBase64((unsigned int)ddd->d4);
	}
	if (len % 3 == 1)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = '=';
		dbuf[(i - 1) * 4 + 3] = '=';
	}
	if (len % 3 == 2)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
		dbuf[(i - 1) * 4 + 3] = '=';
	}
	return;
}
// �����ʼ� email:Ŀ������  body:����  server_address:smtp��ַ  port:�˿�  from_email:�����û�����  password:���롢��Ȩ��
void SendMail(char *email, char *body, char * server_address, int port, char * from_email, char * password)
{
	int     sockfd = { 0 };
	char    buf[1500] = { 0 };
	char    rbuf[1500] = { 0 };
	char    login[128] = { 0 };
	char    pass[128] = { 0 };
	WSADATA WSAData;
	struct sockaddr_in their_addr = { 0 };
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	memset(&their_addr, 0, sizeof(their_addr));

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(port);
	hostent* hptr = gethostbyname(server_address);
	memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
	printf("IP of %s is : %d:%d:%d:%d\n", server_address,
		their_addr.sin_addr.S_un.S_un_b.s_b1,
		their_addr.sin_addr.S_un.S_un_b.s_b2,
		their_addr.sin_addr.S_un.S_un_b.s_b3,
		their_addr.sin_addr.S_un.S_un_b.s_b4);

	// �����ʼ���������������Ӻ�û����Ӧ����2 �����������
	sockfd = OpenSocket((struct sockaddr *)&their_addr);
	memset(rbuf, 0, 1500);
	while (recv(sockfd, rbuf, 1500, 0) == 0)
	{
		cout << "reconnect..." << endl;
		Sleep(2);
		sockfd = OpenSocket((struct sockaddr *)&their_addr);
		memset(rbuf, 0, 1500);
	}

	cout << rbuf << endl;

	// EHLO
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "EHLO HYL-PC\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "EHLO REceive: " << rbuf << endl;

	// AUTH LOGIN
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "AUTH LOGIN\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Auth Login Receive: " << rbuf << endl;

	// USER
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, from_email);//��������˺�
	memset(login, 0, 128);
	EncodeBase64(login, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 UserName: " << buf << endl;
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "User Login Receive: " << rbuf << endl;

	// PASSWORD
	sprintf_s(buf, 1500, password);//�����������
	memset(pass, 0, 128);
	EncodeBase64(pass, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", pass);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 Password: " << buf << endl;

	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Password Receive: " << rbuf << endl;

	// MAIL FROM
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "MAIL FROM: <%s>\r\n", from_email);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "set Mail From Receive: " << rbuf << endl;

	// RCPT TO ��һ���ռ���
	sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", email);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Tell Sendto Receive: " << rbuf << endl;
	// DATA ׼����ʼ�����ʼ�����
	sprintf_s(buf, 1500, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Prepare Receive: " << rbuf << endl;

	// �����ʼ����ݣ�\r\n.\r\n���ݽ������
	sprintf_s(buf, 1500, "%s\r\n.\r\n", body);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Receive: " << rbuf << endl;

	// QUIT
	sprintf_s(buf, 1500, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Quit Receive: " << rbuf << endl;

	//������
	closesocket(sockfd);
	WSACleanup();
	return;
}
// ��TCP Socket����
int OpenSocket(struct sockaddr *addr)
{
	int sockfd = 0;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cout << "Open sockfd(TCP) error!" << endl;
		exit(-1);
	}
	if (connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
	{
		cout << "Connect sockfd(TCP) error!" << endl;
		exit(-1);
	}
	return sockfd;
}



