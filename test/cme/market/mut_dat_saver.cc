
#include "gmock/gmock.h"

#include <boost/iterator/counting_iterator.hpp>

#include <thread>                // std::thread, std::this_thread::yield
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable

#include "cme/market/dat_saver.h"
#include "core/assist/time_measurer.h"
#include "cme/market/message/message_utility.h"
#include "core/assist/time_measurer.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"

#include "../../core/assist/mut_common.h"
#include "../../core/market/mock_marketi.h"
#include "../../core/market/mock_marketlisteneri.h"
#include "../../core/book/autotest_book_sender.h"

#include "mut_dat_saver.h"

namespace fh
{
namespace cme
{
namespace market
{
    MutDatSaver::MutDatSaver():
    m_vecRevPacket()
    {
        // noop
    }

    MutDatSaver::~MutDatSaver()
    {
        // noop
    }
    
    void MutDatSaver::SetUp()
    {
        m_vecRevPacket.clear();
    }
    
    void MutDatSaver::TearDown()
    {
    }
    
        
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic_bool is_finished(false);
    
    // case desc: 
    // Security Definition Messages For Options and Spreads.
    // Instrument: 0EMDH8 C1330 SecurityID: 998870
    // check: {market_segment_id=99 security_group=8$ activation_date_ime=1489782300000000000 expiration_date_ime=1521207000000000000}
    //
    TEST_F(MutDatSaver, DatSaver_Test001)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------"); 
                delete pcme_data;
                pcme_data = nullptr;
                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Sd_1;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [609]UDP Incremental:224.0.28.122, 11609
            //   [609]UDP Incremental:224.0.25.101, 11709
            //   [609]UDP Definition:224.0.28.122, 16609
            //   [609]UDP Recovery:224.0.28.122, 19609
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_609_426_sd_1.log";            
            
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.122:16609](1176)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.122:16609](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "securityID=998870";
                autotest_book_sender_check->CheckResult(securityID);
                //autotest_book_sender_check->OnOrginalMessage(securityID);
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    // case desc: 
    // Security Definition Messages For Options and Spreads.
    // Instrument: UD:$8: RR 0424990802 SecurityID: 990802
    // check: 
    //
    TEST_F(MutDatSaver, DatSaver_Test002)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");    
                delete pcme_data;
                pcme_data = nullptr;                    
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Sd_2;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [609]UDP Incremental:224.0.28.122, 11609
            //   [609]UDP Incremental:224.0.25.101, 11709
            //   [609]UDP Definition:224.0.28.122, 16609
            //   [609]UDP Recovery:224.0.28.122, 19609
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_609_426_sd_2.log";            
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.122:16609](1176)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.122:16609](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            m_vecRevPacket.clear();
            
            // received increment packet , message=d
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.122:11609](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] increment packet **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] increment packet **************\n");
                    }
            ); 
            
            
            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "securityID=990802";
                autotest_book_sender_check->CheckResult(securityID);
                //autotest_book_sender_check->OnOrginalMessage(securityID);
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
        
    // case desc: 
    // Steps of the test Security Definition Messages For Futures and Spreads.
    // Instrument: 1DVEU7 SecurityID: 996791
    // check: {market_segment_id=99 security_group=91 activation_date_ime=1466460000000000000 expiration_date_ime=1505489400000000000}
    //
    TEST_F(MutDatSaver, DatSaver_Test003)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;   
                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Fs_1;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_426_fs_1.log";     
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.122:16609](1176)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "securityID=996791";
                autotest_book_sender_check->CheckResult(securityID);
                //autotest_book_sender_check->OnOrginalMessage(securityID);
            }           

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    // case desc: 
    // Steps of the test Security Definition Messages For Futures and Spreads.
    // Instrument: 1DVEU7-1DVEH8 SecurityID: 996781
    // check: {market_segment_id=99 security_group=91 activation_date_ime=1481929200000000000 expiration_date_ime=1505489400000000000}
    //
    TEST_F(MutDatSaver, DatSaver_Test004)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Fs_2;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_426_fs_2.log";     
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "securityID=996781";
                autotest_book_sender_check->CheckResult(securityID);
                //autotest_book_sender_check->OnOrginalMessage(securityID);
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }

    // case desc: 
    // Request For Quote Message.
    // Instrument: 1DVEU7-1DVEH8 SecurityID: 996781
    // check: {"orderQty" : "290"}
    //
    TEST_F(MutDatSaver, DatSaver_Test005)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Order_Qty;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_426_qm.log";     
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            m_vecRevPacket.clear();
            
            // received increment packet
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.25.101:11727](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] increment packet **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] increment packet **************\n");
                    }
            ); 
            
            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "securityID=996781";
                autotest_book_sender_check->CheckResult(securityID);                
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    //
    // case desc: 
    // Statistic Messages: 
    // 3 Electronic volume statistics were sent. Please validate the following values and A trading session high price was sent. Please validate the following values.
    // Instrument:1DVEU7 SecurityID:996791
    // out:  "mDEntryPx" : { "mantissa" : "241450000000", "exponent" : "-7" }, "mDEntrySize" : "47"
    //
    TEST_F(MutDatSaver, DatSaver_Test006)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Sm_3;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_427_sm_3.log";
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            m_vecRevPacket.clear();
            
            // received increment packet
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] increment packet **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] increment packet **************\n");
                    }
            ); 
            
            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "securityID=996791";
                autotest_book_sender_check->CheckResult(securityID);                
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }

    //
    // case desc: 
    // Statistic Messages: 
    // 6 A threshold limits were sent. Please validate the following values.
    // Instrument:1DVEU7 SecurityID:996791
    // out:  "highLimitPrice" : { "mantissa" : "242200000000", "exponent" : "-7" }, "lowLimitPrice" : { "mantissa" : "240650000000", "exponent" : "-7" }
    //
    TEST_F(MutDatSaver, DatSaver_Test007)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Sm_6;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_427_sm_6.log";
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            m_vecRevPacket.clear();
            
            // received increment packet
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] increment packet **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] increment packet **************\n");
                    }
            ); 
            
            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "securityID=996791";
                autotest_book_sender_check->CheckResult(securityID);                
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    //
    // case desc: 
    // Confirm quantities and prices for book and statistic messages for the selected instrument.
    // Instrument: 1DVEU7 SecurityID: 996791
    // out: "securityTradingStatus" : "21"
    //
    TEST_F(MutDatSaver, DatSaver_Test008)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Market_State_Message_1;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_5_2_msm_1.log";
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            m_vecRevPacket.clear();           

            
            // received increment packet
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] increment packet **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] increment packet **************\n");
                    }
            ); 
            
            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "type=SecurityStatus30"; //"sbeType=f"
                autotest_book_sender_check->CheckResult(securityID);                
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
    
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    //
    // case desc: 
    // Confirm quantities and prices for book and statistic messages for the selected instrument.
    // Instrument: 1DVEU7 SecurityID: 996791
    // out: "securityTradingStatus" : "21"
    //
    TEST_F(MutDatSaver, DatSaver_Test009)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
    
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Market_State_Message_2;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_5_2_msm_3.log";
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            m_vecRevPacket.clear();           

            
            // received increment packet
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] increment packet **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] increment packet **************\n");
                    }
            ); 
            
            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "type=SecurityStatus30"; //"sbeType=f"
                autotest_book_sender_check->CheckResult(securityID);                
            }

            delete dat_saver;
            dat_saver = nullptr;

            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    //
    // case desc: 
    // Confirm quantities and prices for book and statistic messages for the selected instrument.
    // Instrument: 1DVEU7 SecurityID: 996791
    // out: "securityTradingStatus" : "17"
    //
    TEST_F(MutDatSaver, DatSaver_Test010)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
    
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Market_State_Message_3;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_5_3_msm_5.log";
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            m_vecRevPacket.clear();           

            
            // received increment packet
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] increment packet **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] increment packet **************\n");
                    }
            ); 
            
            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "type=SecurityStatus30"; //"sbeType=f"
                autotest_book_sender_check->CheckResult(securityID);                
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    //
    // case desc: 
    // Confirm quantities and prices for book and statistic messages for the selected instrument.
    // Instrument: 1DVEU7 SecurityID: 996791
    // out: "securityTradingStatus" : "2"
    //
    TEST_F(MutDatSaver, DatSaver_Test011)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
    
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Market_State_Message_4;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_5_3_msm_6.log";
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            m_vecRevPacket.clear();           

            
            // received increment packet
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] increment packet **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] increment packet **************\n");
                    }
            ); 
            
            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "type=SecurityStatus30"; //"sbeType=f"
                autotest_book_sender_check->CheckResult(securityID);                
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    //
    // case desc: 
    // Confirm quantities and prices for book and statistic messages for the selected instrument.
    // Instrument: 1DVEU7 SecurityID: 996791
    // out: "securityTradingStatus" : "4"
    //
    TEST_F(MutDatSaver, DatSaver_Test012)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
    
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Market_State_Message_5;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_5_3_msm_7.log";
            
            m_vecRevPacket.clear();
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            m_vecRevPacket.clear();
            
            
            // received increment packet
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] increment packet **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] increment packet **************\n");
                    }
            ); 
            
            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "type=SecurityStatus30"; //"sbeType=f"
                autotest_book_sender_check->CheckResult(securityID);                
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    // case desc: 
    // Security Definition Messages For Options
    // Instrument: 0EMDH8 C1330 SecurityID: 998870
    // check: {market_segment_id=99 security_group=8$ activation_date_ime=1489782300000000000 expiration_date_ime=1521207000000000000 strike_price=1330000000000}
    //
    TEST_F(MutDatSaver, DatSaver_Test013)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");     
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Sdmfo_1;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [609]UDP Incremental:224.0.28.122, 11609
            //   [609]UDP Incremental:224.0.25.101, 11709
            //   [609]UDP Definition:224.0.28.122, 16609
            //   [609]UDP Recovery:224.0.28.122, 19609
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_609_5_3_sdmfo_1.log";            
            
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.122:16609](1176)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.122:16609](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "securityID=998870";
                autotest_book_sender_check->CheckResult(securityID);
                //autotest_book_sender_check->OnOrginalMessage(securityID);
            }

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    
    // case desc: 
    // Security Definition Messages For Futures.
    // Instrument: 1DVEU7 SecurityID: 996791
    // check: {market_segment_id=99 security_group=91 activation_date_ime=1466460000000000000 expiration_date_ime=1505489400000000000}
    //
    TEST_F(MutDatSaver, DatSaver_Test014)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data == nullptr)                
            {
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
    
            dat_saver = new DatSaver(pcme_data, autotest_book_sender);
            if(nullptr == dat_saver)
            {
                LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");  
                delete pcme_data;
                pcme_data = nullptr;                
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Sdmff_1;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [609]UDP Incremental:224.0.28.122, 11609
            //   [609]UDP Incremental:224.0.25.101, 11709
            //   [609]UDP Definition:224.0.28.122, 16609
            //   [609]UDP Recovery:224.0.28.122, 19609
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_5_3_sdmff_1.log";            
            
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.122:16609](1176)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] define message **************\n");
                                               
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] define message **************\n");
                    }
            ); 

            std::for_each(definition_datas.begin(), definition_datas.end(), 
                [this, autotest_book_sender, dat_saver](fh::cme::market::message::MdpMessage &m)
                {                              
                    //dat_saver->Send_message(m);
                    static fh::core::assist::TimeMeasurer t;

                    // send to db
                    std::string json = m.Serialize();
                    if(json == "")  // 返回是空说明是不需要保存的消息
                    {
                        LOG_INFO("message not need be saved: ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                    else
                    {
                        autotest_book_sender->OnOrginalMessage(json);
                        //LOG_INFO("{DB}sent to zmq(original data): ", t.Elapsed_nanoseconds(), "ns, seq=", m.packet_seq_num(), " type=", m.message_type());
                    }
                });
              
                        
            m_vecRevPacket.clear();
                      
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string securityID = "securityID=996791";
                autotest_book_sender_check->CheckResult(securityID);
                //autotest_book_sender_check->OnOrginalMessage(securityID);
            }

            delete dat_saver;
            dat_saver = nullptr;
                        
            delete pcme_data;
            pcme_data = nullptr;    
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }

    // 消费者线程.
    void consume_15(/* int n */void *pObject)
    {
        LOG_DEBUG("===== [begin] consume_15 =====");
        if(!pObject)
        {
            LOG_DEBUG("===== [exit] consume_15 : pObject is null, exit! =====");
            return;
        }
        
        while(!is_finished)
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
        }        
        
        fh::cme::market::DatSaver *dat_saver = (fh::cme::market::DatSaver *)pObject;
        
        // for (int i = 0; i < n; ++i) {
            // std::unique_lock <std::mutex> lck(mtx);
            // cv.wait(lck, shipment_available);
            // std::cout << cargo << '\n';
            // cargo = 0;
        // }
        
        dat_saver->Start_save();
        LOG_DEBUG("===== [end] consume_15 =====");
    }
    
    // 生产者线程.
    void produce_15(void *pObject)
    {
        LOG_DEBUG("===== [begin] produce_15 =====");
        if(!pObject)
        {
            LOG_DEBUG("===== [exit] produce_15 : pObject is null, exit! =====");
            return;
        }
        
        fh::cme::market::DatSaver *dat_saver = (fh::cme::market::DatSaver *)pObject;
        std::string recvBufFileName;
        fh::core::assist::common::getAbsolutePath(recvBufFileName);    
        recvBufFileName += "market_609_5_3_sdmfo_1.log";            

        std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
        // received define message
        // udp received from [224.0.28.122:16609](1176)=01
        std::vector<std::string> vecRevPacket;
        vecRevPacket.clear();
        
        fh::core::assist::common::Read_packets(vecRevPacket, recvBufFileName, "udp received from [224.0.28.122:16609](", "=");
        for(auto &revPacket : vecRevPacket)
        {
            // printf("*********** [begin] define message **************\n");
            
            definition_datas.clear();
            // decode                                                
            std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);
            //if(seq==1)
            {
                //dat_saver->Insert_data(seq, definition_datas);
                dat_saver->Get_cme_data()->Insert_increment_data(seq, definition_datas);
            }            
            
            LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

            // printf("*********** [end] define message **************\n");
        }
        
        is_finished = true;
        
        while(dat_saver->Get_data_count()!=0)
        {
            LOG_DEBUG("===== data isn't null, sleep =====");
            std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
        }
        
        dat_saver->Stop();
        LOG_DEBUG("===== [end] produce_15 =====");
    }
    
    TEST_F(MutDatSaver, DatSaver_Test015)
    {        
        is_finished = false;
        
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;
        
        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data)                
            {
                dat_saver = new DatSaver(pcme_data, autotest_book_sender);
                if(nullptr == dat_saver)
                {
                    LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");     
                    delete pcme_data;
                    pcme_data = nullptr;    
                    
                    delete autotest_book_sender;
                    autotest_book_sender = nullptr;
                    return;
                }

                // logic 
                std::thread threads[2];

                threads[0] = std::thread(produce_15, dat_saver);  // producer
                threads[1] = std::thread(consume_15, dat_saver);  // consumer

                //std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
                //dat_saver->Stop();

                for (auto & th:threads)
                {
                    th.join();
                }
            }

        

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
                  
    }
    //#endif
    
    // 消费者线程.
    void consume_16(/* int n */void *pObject)
    {
        LOG_DEBUG("===== [begin] consume_16 =====");
        if(!pObject)
        {
            LOG_DEBUG("===== [exit] consume_16 : pObject is null, exit! =====");
            return;
        }
        
        /*while(!is_finished)
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
        }*/
        
        fh::cme::market::DatSaver *dat_saver = (fh::cme::market::DatSaver *)pObject;
        
        // for (int i = 0; i < n; ++i) {
            // std::unique_lock <std::mutex> lck(mtx);
            // cv.wait(lck, shipment_available);
            // std::cout << cargo << '\n';
            // cargo = 0;
        // }
        
        dat_saver->Start_save();        
        LOG_DEBUG("===== [end] consume_16 =====");
    }
    
    // 生产者线程.
    void produce_16(void *pObject)
    {
        LOG_DEBUG("===== [begin] produce_16 =====");
        if(!pObject)
        {
            LOG_DEBUG("===== [exit] produce_16 : pObject is null, exit! =====");
            return;
        }
        
        fh::cme::market::DatSaver *dat_saver = (fh::cme::market::DatSaver *)pObject;
        std::string recvBufFileName;
        fh::core::assist::common::getAbsolutePath(recvBufFileName);    
        recvBufFileName += "market_609_5_3_sdmfo_1.log";            

        std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
        // received define message
        // udp received from [224.0.28.122:16609](1176)=01
        std::vector<std::string> vecRevPacket;
        vecRevPacket.clear();
        
        std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
        
        fh::core::assist::common::Read_packets(vecRevPacket, recvBufFileName, "udp received from [224.0.28.122:16609](", "=");
        for(auto &revPacket : vecRevPacket)
        {
            // printf("*********** [begin] define message **************\n");
            
            definition_datas.clear();
            // decode                                                
            std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);
            
            //dat_saver->Insert_data(seq, definition_datas);  
            dat_saver->Get_cme_data()->Insert_increment_data(seq, definition_datas);            
            
            LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

            // printf("*********** [end] define message **************\n");
        }
        
        is_finished = true;
        
        while(dat_saver->Get_data_count()!=0)
        {
            LOG_DEBUG("===== data isn't null, sleep =====");
            std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
        }
        
        dat_saver->Stop();
        LOG_DEBUG("===== [end] produce_16 =====");
    }
    // Get_first_data_seq wait
    TEST_F(MutDatSaver, DatSaver_Test016)
    {
        is_finished = false;
        
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        
        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data)
            {
                dat_saver = new DatSaver(pcme_data, autotest_book_sender);
                if(nullptr == dat_saver)
                {
                    LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");   
                    delete pcme_data;
                    pcme_data = nullptr;    
                    
                    delete autotest_book_sender;
                    autotest_book_sender = nullptr;
                    return;
                }

                // logic 
                std::thread threads[2];
                
                threads[0] = std::thread(consume_16, dat_saver);  // consumer
                threads[1] = std::thread(produce_16, dat_saver);  // producer

                
                //std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
                //dat_saver->Stop();
                
                for (auto & th:threads)
                {
                    th.join();
                }
            }
            
        

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }                  
    }
    
    // 消费者线程.
    void consume_17(/* int n */void *pObject)
    {
        LOG_DEBUG("===== [begin] consume_17 =====");
        if(!pObject)
        {
            LOG_DEBUG("===== [exit] consume_17 : pObject is null, exit! =====");
            return;
        }

        
        fh::cme::market::DatSaver *dat_saver = (fh::cme::market::DatSaver *)pObject;
        
        dat_saver->Start_save();
        LOG_DEBUG("===== [end] consume_17 =====");
    }
    
    // 生产者线程.
    void produce_17(void *pObject)
    {
        LOG_DEBUG("===== [begin] produce_17 =====");
        if(!pObject)
        {
            LOG_DEBUG("===== [exit] produce_17 : pObject is null, exit! =====");
            return;
        }
        
        fh::cme::market::DatSaver *dat_saver = (fh::cme::market::DatSaver *)pObject;
        
        int i=10;
        while(i--)
        {
            LOG_DEBUG("===== sleep =====");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        dat_saver->Stop();
        LOG_DEBUG("===== [end] produce_17 =====");
    }
    // read thread: no data, Get_increment_first_data_seq wait, other thread call stop
    TEST_F(MutDatSaver, DatSaver_Test017)
    {
        is_finished = false;
        
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::DatSaver *dat_saver = nullptr;        

        
        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            fh::cme::market::CmeData *pcme_data = new fh::cme::market::CmeData();
            if(pcme_data)
            {
                dat_saver = new DatSaver(pcme_data, autotest_book_sender);
                if(nullptr == dat_saver)
                {
                    LOG_ERROR("----- dat_saver is nullptr, malloc failed! ------");   
                    delete pcme_data;
                    pcme_data = nullptr;    
                    
                    delete autotest_book_sender;
                    autotest_book_sender = nullptr;
                    return;
                }

                // logic 
                std::thread threads[2];
                
                threads[0] = std::thread(consume_17, dat_saver);  // consumer
                threads[1] = std::thread(produce_17, dat_saver);  // producer
               
                for (auto & th:threads)
                {
                    th.join();
                }
            }
            
        

            delete dat_saver;
            dat_saver = nullptr;
            
            delete pcme_data;
            pcme_data = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
    }
    
} // namespace market
} // namespace cme
} // namespace fh
