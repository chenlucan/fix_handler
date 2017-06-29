#ifndef __FH_REM_socket_mul_H__
#define __FH_REM_socket_mul_H__

/*!****************************************************************************
 @note   Copyright (coffee), 2005-2015, Shengli Tech. Co., Ltd.
 @file   socket_multicast.h
 @date   2015/4/29   17:17
 @author zhou.hu
 
 @brief     ������Ҫʵ���鲥���ݵĽ��ա�

ע��: ��ʾ�������ṩһ�ֽ����ҹ�˾EFH�鲥�����һ��ͨ�÷���������ͻ�������
����Ч�Ľ���EFH�鲥����ķ�ʽ�Ƽ����ǲ����Լ�����Ч�Ľ����ҹ�˾EFH�����ݡ�

 @note 
******************************************************************************/

#pragma once


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <map>

using std::string;
using std::map;





#define MY_SOCKET							int		

#define SL_SOCK_DEFAULT						0
#define SL_SOCK_SEND						1
#define SL_SOCK_RECV						2
#define SL_SOCK_ERROR						3

///socket�ļ�������ȱʡֵ
#define MY_SOCKET_DEFAULT					-1
///socket������Ϣ
#define MY_SOCKET_ERROR						-1	
///���Ľ��ջ�������
#define	RCV_BUF_SIZE						65535
///������������֧�ֵĿͻ��˵�������
#define MAX_SOCKET_CONNECT					1024



///-----------------------------------------------------------------------------
///�ص������¼�
///-----------------------------------------------------------------------------
enum SOCKET_EVENT
{
	EVENT_CONNECT,				//���ӳɹ��¼�
	EVENT_REMOTE_DISCONNECT,	//���Ӷ˶Ͽ��¼�
	EVENT_LOCALE_DISCONNECT,	//�����Ͽ��¼�
	EVENT_NETWORK_ERROR,		//�������
	EVENT_RECEIVE,				//���ݽ����¼�
	EVENT_SEND,					//���ݷ��ͽ����¼�
	EVENT_RECEIVE_BUFF_FULL,	//���ջ�������
	EVENT_UNKNOW,				//δ����״̬
};

class socket_event
{
public:
	virtual ~socket_event() {}
	/// \brief ���յ��鲥���ݵĻ�Ԙ�¼�
	virtual void on_receive_message(int id, const char* buff, unsigned int len) = 0;
};


class socket_multicast
{
public:
	socket_multicast();
	virtual ~socket_multicast(void);
	/// \brief �鲥ʵ����ʼ��
	bool sock_init(const string& remote_ip, unsigned short remote_port,const string& local_ip, unsigned short local_port, int id, socket_event* ptr_event);
	/// \brief �鲥ʵ���ر�
	bool sock_close();

protected:
	//----------------------------------------------------------------------------
	//�������Ա����
	//----------------------------------------------------------------------------
	/// \brief �鲥���շ��źŵ��̺߳���(linux ��)
	static void* socket_server_event_thread(void* ptr_param);			

	/// \brief �鲥���շ��źŵĴ�����
	void* on_socket_server_event_thread();

	/// \brief �����鲥�źŴ����߳�
	bool start_server_event_thread();										
	/// \brief ֹͣ�鲥�źŴ����߳�
	bool stop_server_event_thread();	
	
	/// \brief ��ͻ�����Ļص��¼�
	bool report_user(SOCKET_EVENT eventType, int id, const char *buff, unsigned int size);
	/// \brief ��־��¼�ӿ�
	void log_msg(const string& msg);

protected:
	socket_event*			m_event;				///< �ص��ӿ�
	bool					m_thrade_quit_flag;		///< �źż���߳��˳���־		

	string					m_remote_ip;			///< �鲥IP
	unsigned short			m_remote_port;			///< �鲥�˿�
	string					m_local_ip;				///< ����IP
	unsigned short			m_local_port;			///< ���ض˿�
	int						m_id;					///< ���ӱ��
	MY_SOCKET				m_sock;					///< �׽ӿ�
};






#endif
