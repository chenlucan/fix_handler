
#include "gmock/gmock.h"

#include <boost/algorithm/string.hpp>

#include "cme/market/market_manager.h"
#include "core/book/book_sender.h"
#include "cme/market/book_manager.h"
#include "cme/market/message/message_utility.h"
#include "core/assist/time_measurer.h"
#include "core/assist/logger.h"
#include "core/assist/utility.h"
#include "pb/dms/dms.pb.h"

#include "../../core/assist/mut_common.h"
#include "../../core/market/mock_marketi.h"
#include "../../core/market/mock_marketlisteneri.h"

#include "mut_book_manager.h"

using ::testing::AtLeast;  
using ::testing::Return;  
using ::testing::Mock;
using ::testing::AnyNumber;

namespace fh
{
namespace cme
{
namespace market
{ 
    MutBookManager::MutBookManager():
    m_datas()
    {
    }

    MutBookManager::~MutBookManager()
    {
        // noop
    }
    
    void MutBookManager::SetUp()
    {
    }
    
    void MutBookManager::TearDown()
    {
    }
    
    //BookManager case, create book state
    TEST_F(MutBookManager, BookManager_Test001)
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;  
        
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
        
            
        if(mock_market_listener!=nullptr)
        {
            book_manager = new BookManager(mock_market_listener);
            if(nullptr == book_manager)
            {                
                delete mock_market_listener;
                mock_market_listener = nullptr;
                
                return;
            }
            
            EXPECT_CALL(
                *mock_market_listener,
                OnContractDefinition(testing::_)
                ).WillRepeatedly(testing::Return());
          
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName +="mut_cmemarket_revbuf.log";
            
            std::vector<std::string> vecRevPacket = fh::core::assist::common::Read_packets(recvBufFileName, "[MutBookManager_BookManager_Test001] ");
            
            // 
            std::for_each(vecRevPacket.cbegin(), vecRevPacket.cend(),
                    [this, book_manager](const std::string &revPacket)
                    {
                        // decode
                        std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), mdp_messages);

                        LOG_INFO("seq=", seq, ", mdp_messages count=", mdp_messages.size());
                        
                        // logic
                        std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                          [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

                          
                        std::vector<char> message_types;
                        std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                                {
                                    message_types.push_back(m.message_type());
                                });
                          
                        auto message = m_datas.begin();
                        if(message!=m_datas.end())
                        {
                            // convert the message to books
                            book_manager->Parse_to_send(*message);
                            LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                        }
                        
                        m_datas.clear();
                    }
            ); 
                        
            vecRevPacket.clear();
        }  
        
        
        delete book_manager;
        book_manager = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    
    //BookManager case, create and modify book state 
    TEST_F(MutBookManager, BookManager_Test002)
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;  
        
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
        
       
        EXPECT_CALL(
            *mock_market_listener,
            OnContractDefinition(testing::_)
            ).WillRepeatedly(testing::Return());
        
        
        if(mock_market_listener!=nullptr)
        {
            book_manager = new BookManager(mock_market_listener);
            if(nullptr == book_manager)
            {                
                delete mock_market_listener;
                mock_market_listener = nullptr;
                
                return;
            }
        }
        
        std::string recvBufFileName;
        fh::core::assist::common::getAbsolutePath(recvBufFileName);        
        recvBufFileName +="mut_cmemarket_revbuf.log";

        std::vector<std::string> vecRevPacket = fh::core::assist::common::Read_packets(recvBufFileName, "[MutBookManager_BookManager_Test002] ");
        
        std::for_each(vecRevPacket.cbegin(), vecRevPacket.cend(),
                [this, book_manager](const std::string &revPacket)
                {
                    // decode
                    std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                    std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), mdp_messages);

                    LOG_INFO("seq=", seq, ", mdp_messages count=", mdp_messages.size());
                    
                    // logic                    
                    std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                      [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

                      
                    std::vector<char> message_types;
                    std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                            {
                                message_types.push_back(m.message_type());
                            });
                      
                    auto message = m_datas.begin();
                    if(message!=m_datas.end())
                    {
                        book_manager->Parse_to_send(*message);
                        LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                    }
                    
                    m_datas.clear();
                }
        );  

        vecRevPacket.clear();        
        
        delete book_manager;
        book_manager = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    
    //BookManager case,  desc: define message, mock MarketListenerI
    TEST_F(MutBookManager, BookManager_Test003)
    {    
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr;   // 
        fh::cme::market::BookManager *book_manager = nullptr;        
        // [zeromq]
        // org_url=tcp://*:5557
        // book_url=tcp://*:5558
        std::string zeromq_org_url = "tcp://*:2000";
        std::string zeromq_book_url = "tcp://*:2001";
                
        mock_market_listener = new fh::core::market::MockMarketListenerI();    

        EXPECT_CALL(
            *mock_market_listener,
            OnContractDefinition(testing::_) // ��mock�ķ���������Ϊռλ��
            ).Times(1).WillOnce(testing::Return());            
                 
        
        if(mock_market_listener!=nullptr)
        {
            book_manager = new BookManager(mock_market_listener);
            if(nullptr == book_manager)
            {             
                LOG_ERROR("----- book_manager is nullptr, malloc failed! ------");       
                delete mock_market_listener;
                mock_market_listener = nullptr;
                return;
            }            
            
            // ��� make a mdp packet for test
            std::uint32_t *ids;
            size_t ids_len;
            std::uint32_t total_number;
            ids = new uint32_t[3]{27, 29, 41};
            if(ids!=nullptr)
            {
                ids_len = 3;
                total_number = std::numeric_limits<std::uint32_t>::max();
                LOG_DEBUG("total_number = ", total_number);
                
                std::uint32_t seq = 1; 
                char buf[BUFFER_MAX_LENGTH]={0};
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));
                
                // ���
                fh::core::assist::TimeMeasurer t;
                // get messages from packet
                std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
                
                            
                std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                  [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

                  
                std::vector<char> message_types;
                std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                        {
                            message_types.push_back(m.message_type());
                        });
                        
                LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                  
                auto message = m_datas.begin();
                if(message!=m_datas.end())
                {
                    // convert the message to books and send to zeromq for book
                    book_manager->Parse_to_send(*message);
                    // now send received messages to zeromq for save to db
                    LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                }           
                
                m_datas.clear();
                
                delete []ids;
                ids = nullptr; 
            }              
            

            delete book_manager;
            book_manager = nullptr;
            
            
            delete mock_market_listener;
            mock_market_listener = nullptr;
            
            bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
            //Mock::VerifyAndClear(mock_market_listener);
            ASSERT_TRUE(bRet);
        }       
        
    }
      

    //BookManager case,  desc: send define message
    TEST_F(MutBookManager, BookManager_Test004) 
    {
        fh::core::market::MarketListenerI *book_sender = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        
        // [zeromq]
        // org_url=tcp://*:5557
        // book_url=tcp://*:5558
        std::string zeromq_org_url = "tcp://*:2000";
        std::string zeromq_book_url = "tcp://*:2001";
        book_sender = new fh::core::book::BookSender(zeromq_org_url, zeromq_book_url);        
        
        if(book_sender!=nullptr)
        {
            book_manager = new BookManager(book_sender);
            if(nullptr == book_manager)
            {             
                LOG_ERROR("----- book_manager is nullptr, malloc failed! ------");       
                delete book_sender;
                book_sender = nullptr;
                return;
            }
            
            
            // ��� make a mdp packet for test
            std::uint32_t *ids;
            size_t ids_len;
            std::uint32_t total_number;
            ids = new uint32_t[3]{27, 29, 41};
            if(ids!=nullptr)
            {
                ids_len = 3;
                total_number = std::numeric_limits<std::uint32_t>::max();
                LOG_DEBUG("total_number = ", total_number);
                
                std::uint32_t seq = 1; 
                char buf[BUFFER_MAX_LENGTH]={0};
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));
                
                // ���
                fh::core::assist::TimeMeasurer t;
                // get messages from packet
                std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
                
                            
                std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                  [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

                  
                std::vector<char> message_types;
                std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                        {
                            message_types.push_back(m.message_type());
                        });
                        
                LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                  
                auto message = m_datas.begin();
                if(message!=m_datas.end())
                {
                    // convert the message to books and send to zeromq for book
                    book_manager->Parse_to_send(*message);
                    // now send received messages to zeromq for save to db
                    LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                }           
                
                m_datas.clear();
                
                delete []ids;
                ids = nullptr; 
            }              
            

            delete book_manager;
            book_manager = nullptr;
            
            delete book_sender;
            book_sender = nullptr;
        }
    }
        
    //BookManager case,  desc: modify book state by book
    TEST_F(MutBookManager, BookManager_Test005) 
    {
        fh::core::market::MarketListenerI *book_sender = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        
        // [zeromq]
        // org_url=tcp://*:5557
        // book_url=tcp://*:5558
        std::string zeromq_org_url = "tcp://*:2000";
        std::string zeromq_book_url = "tcp://*:2001";
        book_sender = new fh::core::book::BookSender(zeromq_org_url, zeromq_book_url);        
        
        if(book_sender!=nullptr)
        {
            book_manager = new BookManager(book_sender);
            if(nullptr == book_manager)
            {                
                delete book_sender;
                book_sender = nullptr;
                
                return;
            }
            
            
            // ��� make a mdp packet for test
            // seqID = 2 
            std::uint32_t *ids;
            size_t ids_len;
            std::uint32_t total_number;
            ids = new uint32_t[5]{4, 30, 32, 36, 37};
            if(ids!=nullptr)
            {
                ids_len = 5;
                total_number = std::numeric_limits<std::uint32_t>::max();
                LOG_DEBUG("total_number = ", total_number);
                
                std::uint32_t seq = 2; 
                char buf[BUFFER_MAX_LENGTH];
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));
                
                // ���
                fh::core::assist::TimeMeasurer t;
                // get messages from packet
                std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
                
                            
                std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                  [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

                  
                std::vector<char> message_types;
                std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                        {
                            message_types.push_back(m.message_type());
                        });
                        
                LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                  
                auto message = m_datas.begin();
                if(message!=m_datas.end())
                {
                    // convert the message to books and send to zeromq for book
                    book_manager->Parse_to_send(*message);
                    // now send received messages to zeromq for save to db
                    LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                }

                m_datas.clear();
                
                delete []ids;
                ids = nullptr; 
            }
              
            
            delete book_manager;
            book_manager = nullptr;
            
            delete book_sender;
            book_sender = nullptr;
        }
    }
    

    //BookManager case,  
    TEST_F(MutBookManager, BookManager_Test006) 
    {
        fh::core::market::MarketListenerI *book_sender = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        
        // [zeromq]
        // org_url=tcp://*:5557
        // book_url=tcp://*:5558
        std::string zeromq_org_url = "tcp://*:2000";
        std::string zeromq_book_url = "tcp://*:2001";
        book_sender = new fh::core::book::BookSender(zeromq_org_url, zeromq_book_url);        
        
        if(book_sender!=nullptr)
        {
            book_manager = new BookManager(book_sender);
            if(nullptr == book_manager)
            {                
                delete book_sender;
                book_sender = nullptr;
                
                return;
            }
            
            
            // ��� make a mdp packet for test
            // seqID = 3 
            std::uint32_t *ids;
            size_t ids_len;
            std::uint32_t total_number;
            ids = new uint32_t[2]{38, 44};
            if(ids!=nullptr)
            {
                ids_len = 2;
                total_number = std::numeric_limits<std::uint32_t>::max();
                LOG_DEBUG("total_number = ", total_number);
                
                std::uint32_t seq = 3; 
                char buf[BUFFER_MAX_LENGTH];
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));
                
                // ���
                fh::core::assist::TimeMeasurer t;
                // get messages from packet
                std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
                
                            
                std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                  [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

                  
                std::vector<char> message_types;
                std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                        {
                            message_types.push_back(m.message_type());
                        });
                        
                LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                  
                auto message = m_datas.begin();
                if(message!=m_datas.end())
                {
                    // convert the message to books and send to zeromq for book
                    book_manager->Parse_to_send(*message);
                    // now send received messages to zeromq for save to db
                    LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                }
                
                m_datas.clear();
               
                delete []ids;
                ids = nullptr;    
            }
            
            delete book_manager;
            book_manager = nullptr;
            
            delete book_sender;
            book_sender = nullptr;
        }
    }

    //BookManager:: case
    TEST_F(MutBookManager, BookManager_Test007) 
    {
        fh::core::market::MarketListenerI *book_sender = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        
        // [zeromq]
        // org_url=tcp://*:5557
        // book_url=tcp://*:5558
        std::string zeromq_org_url = "tcp://*:2000";
        std::string zeromq_book_url = "tcp://*:2001";
        book_sender = new fh::core::book::BookSender(zeromq_org_url, zeromq_book_url);        
        
        if(book_sender!=nullptr)
        {
            book_manager = new BookManager(book_sender);
            if(nullptr == book_manager)
            {                
                delete book_sender;
                book_sender = nullptr;
                
                return;
            }
            for(int i=0; i<5; i++)
            {
                // ��� make a mdp packet for test
                // seqID = 4
                std::uint32_t *ids;
                size_t ids_len;
                std::uint32_t total_number;
                ids = new uint32_t[16]{4, 27, 29, 30, 32, 33, 34, 35, 36, 37, 38, 39, 41, 42, 43, 44};
                if(ids!=nullptr)
                {   
                    ids_len = 16;
                    total_number = std::numeric_limits<std::uint32_t>::max();
                    LOG_DEBUG("total_number = ", total_number);
                    
                    std::uint32_t seq = 4; 
                    char buf[BUFFER_MAX_LENGTH];
                    std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                    
                    LOG_INFO("sent : seq=", seq, " len=", len);
                    LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));
                    
                    
                    fh::core::assist::TimeMeasurer t;
                    // get messages from packet
                    std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                    std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
                    
                                
                    std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                      [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

                      
                    std::vector<char> message_types;
                    std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                            {
                                message_types.push_back(m.message_type());
                            });
                            
                    LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                      
                    auto message = m_datas.begin();
                    if(message!=m_datas.end())
                    {                        
                        // convert the message to books and send to zeromq for book
                        book_manager->Parse_to_send(*message);
                        // now send received messages to zeromq for save to db
                        LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                    }
                    
                    m_datas.clear();
                    
                    delete []ids;
                    ids = nullptr;                    
                }                    
            }
            
            delete book_manager;
            book_manager = nullptr;
            
            delete book_sender;
            book_sender = nullptr;
        }
    }

    //BookManager::Set_definition_data case desc: first define message , sencond modify book state by book
    TEST_F(MutBookManager, BookManager_Test008) 
    {
        fh::core::market::MarketListenerI *book_sender = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        
        // [zeromq]
        // org_url=tcp://*:5557
        // book_url=tcp://*:5558
        std::string zeromq_org_url = "tcp://*:2000";
        std::string zeromq_book_url = "tcp://*:2001";
        book_sender = new fh::core::book::BookSender(zeromq_org_url, zeromq_book_url);        
        
        if(book_sender!=nullptr)
        {
            book_manager = new BookManager(book_sender);
            if(nullptr == book_manager)
            {                
                delete book_sender;
                book_sender = nullptr;
                
                return;
            }
            
            // 1. define message
            // ��� make a mdp packet for test
            // seqID = 5
            std::uint32_t *ids;
            size_t ids_len;
            std::uint32_t total_number;
            ids = new uint32_t[3]{27, 29, 41};
            if(ids!=nullptr)
            {
                ids_len = 3;
                total_number = std::numeric_limits<std::uint32_t>::max();
                LOG_DEBUG("total_number = ", total_number);
                
                std::uint32_t seq = 5; 
                char buf[BUFFER_MAX_LENGTH] = {0};
            
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));  

                fh::core::assist::TimeMeasurer t;
                // get messages from packet
                std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
 
                  
                std::vector<char> message_types;
                std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                        {
                            message_types.push_back(m.message_type());
                        });
                        
                LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                  
                // parse definition message
                book_manager->Set_definition_data(&mdp_messages);
                    
                delete []ids;
                ids = nullptr;                
            }
            
            // 2. book state message
            ids = new uint32_t[1]{32};
            if(ids!=nullptr)
            {
                ids_len = 1;
                total_number = std::numeric_limits<std::uint32_t>::max();
                LOG_DEBUG("total_number = ", total_number);
                
                std::uint32_t seq = 5; 
                char buf[BUFFER_MAX_LENGTH] = {0};
            
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));  

                fh::core::assist::TimeMeasurer t;
                // get messages from packet
                std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
                
                            
                std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                  [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

                  
                std::vector<char> message_types;
                std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                        {
                            message_types.push_back(m.message_type());
                        });
                        
                LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                  
                auto message = m_datas.begin();
                if(message!=m_datas.end())
                {
                    // convert the message to books and send to zeromq for book
                    book_manager->Parse_to_send(*message);
                    // now send received messages to zeromq for save to db
                    LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                }
                
                m_datas.clear(); 
                    
                delete []ids;
                ids = nullptr;                
            }
            
            delete book_manager;
            book_manager = nullptr;
            
            delete book_sender;
            book_sender = nullptr;
            
            //mktdata::MDEntryType::Value value = mktdata::MDEntryType::get(10);
        }
    }
    
    //BookManager::Set_recovery_data case desc: trade message
    TEST_F(MutBookManager, BookManager_Test009) 
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        
        // [zeromq]
        // org_url=tcp://*:5557
        // book_url=tcp://*:5558
        std::string zeromq_org_url = "tcp://*:2000";
        std::string zeromq_book_url = "tcp://*:2001";
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
        
        EXPECT_CALL(
            *mock_market_listener,
            OnContractDefinition(testing::_)
            ).WillRepeatedly(testing::Return());
        
        EXPECT_CALL(
            *mock_market_listener,
            OnTrade(testing::_)
            ).Times(AnyNumber());
            
            
        EXPECT_CALL(
            *mock_market_listener,
            OnL2(testing::_)
            ).Times(AnyNumber()); 
            
        EXPECT_CALL(
            *mock_market_listener,
            OnBBO(testing::_)
            ).Times(AnyNumber()); 
        
            
        if(mock_market_listener!=nullptr)
        {
            book_manager = new BookManager(mock_market_listener);
            if(nullptr == book_manager)
            {                
                delete mock_market_listener;
                mock_market_listener = nullptr;
                
                return;
            }
            
            // 1. define message
            // ��� make a mdp packet for test
            // seqID = 5
            std::uint32_t *ids;
            size_t ids_len;
            std::uint32_t total_number;
            ids = new uint32_t[3]{27, 29, 41};
            if(ids!=nullptr)
            {
                ids_len = 3;
                total_number = std::numeric_limits<std::uint32_t>::max();
                LOG_DEBUG("total_number = ", total_number);
                
                std::uint32_t seq = 5; 
                char buf[BUFFER_MAX_LENGTH] = {0};
            
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));  

                fh::core::assist::TimeMeasurer t;
                // get messages from packet
                std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
 
                  
                std::vector<char> message_types;
                std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                        {
                            message_types.push_back(m.message_type());
                        });
                        
                LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                  
                // parse definition message
                book_manager->Set_definition_data(&mdp_messages);
                    
                delete []ids;
                ids = nullptr;                
            }
            
            // 2. recovery message            
            ids = new uint32_t[2]{38, 44};
            if(ids!=nullptr)
            {
                ids_len = 2;
                total_number = std::numeric_limits<std::uint32_t>::max();
                LOG_DEBUG("total_number = ", total_number);
                
                std::uint32_t seq = 5; 
                char buf[BUFFER_MAX_LENGTH] = {0};
            
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));  

                fh::core::assist::TimeMeasurer t;
                // get messages from packet
                std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
 
                  
                std::vector<char> message_types;
                std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                        {
                            message_types.push_back(m.message_type());
                        });
                        
                LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                  
                // parse definition message
                book_manager->Set_recovery_data(&mdp_messages);
                    
                delete []ids;
                ids = nullptr;                
            }
            
            // 3. Trade message
            ids = new uint32_t[2]{36, 42};
            if(ids!=nullptr)
            {
                ids_len = 2;
                total_number = std::numeric_limits<std::uint32_t>::max();
                LOG_DEBUG("total_number = ", total_number);
                
                std::uint32_t seq = 5; 
                char buf[BUFFER_MAX_LENGTH] = {0};
            
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));  

                fh::core::assist::TimeMeasurer t;
                // get messages from packet
                std::vector<fh::cme::market::message::MdpMessage> mdp_messages;
                std::uint32_t packet_seq_num = fh::cme::market::message::utility::Pick_messages_from_packet(buf, len, mdp_messages);
                
                            
                std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                  [this](fh::cme::market::message::MdpMessage &m){ m_datas.insert(std::move(m)); });

                  
                std::vector<char> message_types;
                std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                        {
                            message_types.push_back(m.message_type());
                        });
                        
                LOG_INFO("{IN}received increment packet: ", t.Elapsed_nanoseconds(), "ns, seq=", packet_seq_num, ", len=", len, ", message=", std::string(message_types.begin(), message_types.end()));
                  
                auto message = m_datas.begin();
                if(message!=m_datas.end())
                {
                    // convert the message to books and send to zeromq for book
                    book_manager->Parse_to_send(*message);
                    // now send received messages to zeromq for save to db
                    LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                }
                
                m_datas.clear(); 
                    
                delete []ids;
                ids = nullptr;                
            }
            
            delete book_manager;
            book_manager = nullptr;
            
            delete mock_market_listener;
            mock_market_listener = nullptr;
            
            bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
            //Mock::VerifyAndClear(mock_market_listener);
            ASSERT_TRUE(bRet);
        }
    }

    //BookManager case,  reset book state
    TEST_F(MutBookManager, BookManager_Test010)
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;  
        
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
        
            
        if(mock_market_listener!=nullptr)
        {
            book_manager = new BookManager(mock_market_listener);
            if(nullptr == book_manager)
            {                
                delete mock_market_listener;
                mock_market_listener = nullptr;
                
                return;
            }
            
            EXPECT_CALL(
                *mock_market_listener,
                OnContractDefinition(testing::_)
                ).WillRepeatedly(testing::Return());
          
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName +="mut_cmemarket_revbuf.log";
            
            std::vector<std::string> vecRevPacket = fh::core::assist::common::Read_packets(recvBufFileName, "[MutBookManager_BookManager_Test010] ");
            

            // 
            std::for_each(vecRevPacket.cbegin(), vecRevPacket.cend(),
                    [this, book_manager](const std::string &revPacket)
                    {
                        // decode
                        std::vector<fh::cme::market::message::MdpMessage> mdp_messages;                        
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), mdp_messages);

                        //printf("*********** begin **************\n");
                        
                        LOG_INFO("seq=", seq, ", mdp_messages count=", mdp_messages.size());
                                           
                        std::vector<char> message_types;
                        std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                                {
                                    message_types.push_back(m.message_type());
                                });                                           
                        // logic
                        std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                          [this, book_manager](fh::cme::market::message::MdpMessage &m)
                          {                              
                              //m.Reset();
                              //printf("*********** m.Serialize = %s **************\n", m.Serialize().c_str());
                              m_datas.insert(std::move(m));
                          });


                        for(auto message = m_datas.begin(); message!=m_datas.end(); ++message)
                        {
                            //printf("*********** message->m_buffer = %p **************\n",  &(message->m_buffer[0]));
                            LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                            // convert the message to books
                            book_manager->Parse_to_send(*message);
                        }
                        
                        m_datas.clear(); 
                        
                        //printf("*********** end **************\n");
                    }
            );
         
                                    
            vecRevPacket.clear();
        }  
        
        
        delete book_manager;
        book_manager = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    
    //BookManager case,  message=WdXfX , OnL2
    TEST_F(MutBookManager, BookManager_Test011)
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;  
        
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
        
            
        if(mock_market_listener!=nullptr)
        {
            book_manager = new BookManager(mock_market_listener);
            if(nullptr == book_manager)
            {                
                delete mock_market_listener;
                mock_market_listener = nullptr;
                
                return;
            }
            
            EXPECT_CALL(
                *mock_market_listener,
                OnContractDefinition(testing::_)
                ).WillRepeatedly(testing::Return());
          
            EXPECT_CALL(
                *mock_market_listener,
                OnL2(testing::_)
                ).WillRepeatedly(testing::Return());
          
            EXPECT_CALL(
                *mock_market_listener,
                OnContractTrading(testing::_)
                ).WillRepeatedly(testing::Return());
            
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName +="mut_cmemarket_revbuf.log";
            
            std::vector<std::string> vecRevPacket = fh::core::assist::common::Read_packets(recvBufFileName, "[MutBookManager_BookManager_Test011] ");
            

            // 
            std::for_each(vecRevPacket.cbegin(), vecRevPacket.cend(),
                    [this, book_manager](const std::string &revPacket)
                    {
                        // decode
                        std::vector<fh::cme::market::message::MdpMessage> mdp_messages;                        
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), mdp_messages);

                        printf("*********** begin **************\n");
                        
                        LOG_INFO("seq=", seq, ", mdp_messages count=", mdp_messages.size());
                                           
                        std::vector<char> message_types;
                        std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                                {
                                    message_types.push_back(m.message_type());
                                });   

                        LOG_INFO("{IN}received increment packet: ", " seq=", seq, ", message=", std::string(message_types.begin(), message_types.end()));
                                
                        // logic
                        std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                          [this, book_manager](fh::cme::market::message::MdpMessage &m)
                          {                              
                              //m.Reset();
                              //printf("*********** m.Serialize = %s **************\n", m.Serialize().c_str());
                              m_datas.insert(std::move(m));
                          });


                        for(auto message = m_datas.begin(); message!=m_datas.end(); ++message)
                        {
                            LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                            // convert the message to books
                            book_manager->Parse_to_send(*message);
                        }
                        
                        m_datas.clear(); 
                        
                        printf("*********** end **************\n");
                    }
            );
         
                                    
            vecRevPacket.clear();
        }  
        
        
        delete book_manager;
        book_manager = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    
    //BookManager case,  message=d, message=X , OnTrade(mDEntryType=2)
    TEST_F(MutBookManager, BookManager_Test012)
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;  
        
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
        
            
        if(mock_market_listener!=nullptr)
        {
            book_manager = new BookManager(mock_market_listener);
            if(nullptr == book_manager)
            {                
                delete mock_market_listener;
                mock_market_listener = nullptr;
                
                return;
            }
            
            EXPECT_CALL(
                *mock_market_listener,
                OnContractDefinition(testing::_)
                ).WillRepeatedly(testing::Return());
          
            EXPECT_CALL(
                *mock_market_listener,
                OnTrade(testing::_)
                ).WillRepeatedly(testing::Return());
          
            EXPECT_CALL(
                *mock_market_listener,
                OnContractTrading(testing::_)
                ).WillRepeatedly(testing::Return());
            
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName +="mut_cmemarket_revbuf.log";
            
            std::vector<std::string> vecRevPacket = fh::core::assist::common::Read_packets(recvBufFileName, "[MutBookManager_BookManager_Test012] ");
            

            // 
            std::for_each(vecRevPacket.cbegin(), vecRevPacket.cend(),
                    [this, book_manager](const std::string &revPacket)
                    {
                        // decode
                        std::vector<fh::cme::market::message::MdpMessage> mdp_messages;                        
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), mdp_messages);

                        printf("*********** begin **************\n");
                        
                        LOG_INFO("seq=", seq, ", mdp_messages count=", mdp_messages.size());
                                           
                        std::vector<char> message_types;
                        std::for_each(mdp_messages.cbegin(), mdp_messages.cend(), [&message_types](const fh::cme::market::message::MdpMessage &m)
                                {
                                    message_types.push_back(m.message_type());
                                });   

                        LOG_INFO("{IN}received increment packet: ", " seq=", seq, ", message=", std::string(message_types.begin(), message_types.end()));
                                
                        // logic
                        std::for_each(mdp_messages.begin(), mdp_messages.end(), 
                          [this, book_manager](fh::cme::market::message::MdpMessage &m)
                          {                              
                              m_datas.insert(std::move(m));
                          });


                        for(auto message = m_datas.begin(); message!=m_datas.end(); ++message)
                        {
                            LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type());
                            // convert the message to books
                            book_manager->Parse_to_send(*message);
                        }
                        
                        m_datas.clear(); 
                        
                        printf("*********** end **************\n");
                    }
            );
         
                                    
            vecRevPacket.clear();
        }  
        
        
        delete book_manager;
        book_manager = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }

} // namespace market
} // namespace cme
} // namespace fh
