
#ifndef __FH_REM_guava_udp_H__
#define __FH_REM_guava_udp_H__
/*!****************************************************************************
 @note   Copyright (coffee), 2005-2014, Shengli Tech. Co., Ltd.
 @file   guava_quote.h
 @date   2014/8/27   12:44
 @author zhou.hu
 
 @brief   ���ļ���EFH�����鲥�ӿڵ�ʾ������

 ע��: ��ʾ�������ṩһ�ֽ����ҹ�˾EFH�鲥�����һ��ͨ�÷���������ͻ�������
 ����Ч�Ľ���EFH�鲥����ķ�ʽ�Ƽ����ǲ����Լ�����Ч�Ľ����ҹ�˾EFH�����ݡ�

 @note 
******************************************************************************/
#pragma once
#include <vector>
#include "rem_socket_multicast.h"

using std::vector;

#define MAX_IP_LEN				32

#define QUOTE_FLAG_SUMMARY		4


#pragma pack(push, 1)

struct guava_udp_normal
{
	unsigned int	m_sequence;				///<�Ự���
	char			m_exchange_id;			///<�г�  0 ��ʾ�н�  1��ʾ����
	char			m_channel_id;			///<ͨ�����
	char			m_quote_flag;			///<�����־  0 ��time sale,��lev1, 
											///           1 ��time sale,��lev1, 
											///           2 ��time sale,��lev1, 
											///           3 ��time sale,��lev1
	char			m_symbol[8];			///<��Լ
	char			m_update_time[9];		///<������ʱ��(��)
	int				m_millisecond;			///<������ʱ��(����)

	double			m_last_px;				///<���¼�
	int				m_last_share;			///<���³ɽ���
	double			m_total_value;			///<�ɽ����
	double			m_total_pos;			///<�ֲ���
	double			m_bid_px;				///<�������
	int				m_bid_share;			///<��������
	double			m_ask_px;				///<��������
	int				m_ask_share;			///<��������
};


struct multicast_info
{
	char	m_remote_ip[MAX_IP_LEN];		///< �鲥����Զ�˵�ַ
	int		m_remote_port;					///< �鲥����Զ�˶˿�
	char	m_local_ip[MAX_IP_LEN];			///< �鲥������ַ
	int		m_local_port;					///< �鲥�����˿�
};


#pragma pack(pop)



class guava_quote_event
{
public:
	virtual ~guava_quote_event() {}
	/// \brief ���յ��鲥���ݵĻص��¼�
	virtual void on_receive_nomal(guava_udp_normal* data) = 0;
};

class guava_quote : public socket_event
{
public:
	guava_quote(void);
	~guava_quote(void);

	/// \brief ��ʼ��
	bool init(multicast_info cffex, guava_quote_event* event);

	/// \brief �ر�
	void close();

private:
	/// \brief �鲥���ݽ��ջص��ӿ�
	virtual void on_receive_message(int id, const char* buff, unsigned int len);


private:
	socket_multicast		m_udp;				///< UDP������սӿ�

	multicast_info			m_cffex_info;		///< �н�ӿ���Ϣ
	int						m_cffex_id;			///< �н�������ͨ��

	guava_quote_event*		m_ptr_event;		///< ����ص��¼��ӿ�
};



#endif

