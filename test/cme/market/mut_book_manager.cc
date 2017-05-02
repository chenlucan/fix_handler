
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
#include "../../core/book/autotest_book_sender.h"

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
    m_datas(), m_vecRevPacket()
    {
    }

    MutBookManager::~MutBookManager()
    {
        // noop
    }
    
    void MutBookManager::SetUp()
    {
        m_vecRevPacket.clear();
    }
    
    void MutBookManager::TearDown()
    {
    }
    
    #ifdef _MUT_CME_ALL
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
            
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "[MutBookManager_BookManager_Test001] ");
            
            // 
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
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
                        
            m_vecRevPacket.clear();
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

        fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "[MutBookManager_BookManager_Test002] ");
        
        std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
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

        m_vecRevPacket.clear();        
        
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
            OnContractDefinition(testing::_) // 被mock的方法，参数为占位符
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
            
            // 组包 make a mdp packet for test
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
                
                // 解包
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
            
            
            // 组包 make a mdp packet for test
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
                
                // 解包
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
            
            
            // 组包 make a mdp packet for test
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
                char buf[BUFFER_MAX_LENGTH] ={0};
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));
                
                // 解包
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
            
            
            // 组包 make a mdp packet for test
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
                char buf[BUFFER_MAX_LENGTH] = {0};
                std::uint16_t len = fh::cme::market::message::utility::Make_packet(buf, seq, ids, ids_len);
                
                LOG_INFO("sent : seq=", seq, " len=", len);
                LOG_DEBUG("msg=", fh::core::assist::utility::Hex_str(buf, len));
                
                // 解包
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
                // 组包 make a mdp packet for test
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
            // 组包 make a mdp packet for test
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
            // 组包 make a mdp packet for test
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
            
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "[MutBookManager_BookManager_Test010] ");
            

            // 
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
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
         
                                    
            m_vecRevPacket.clear();
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
            
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "[MutBookManager_BookManager_Test011] ");
            

            // 
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
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
         
                                    
            m_vecRevPacket.clear();
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
            
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "[MutBookManager_BookManager_Test012] ");
            

            // 
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
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
         
                                    
            m_vecRevPacket.clear();
        }  
        
        
        delete book_manager;
        book_manager = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    
    //BookManager case,  create bbo
    TEST_F(MutBookManager, BookManager_Test013)
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
            
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "[MutBookManager_BookManager_Test013] ");
            

            // 
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
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
         
                                    
            m_vecRevPacket.clear();
        }  
        
        
        delete book_manager;
        book_manager = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }

    //BookManager case,  recovery data
    TEST_F(MutBookManager, BookManager_Test014)
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
                OnBBO(testing::_)
                ).WillRepeatedly(testing::Return());
            
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName +="mut_cmemarket_revbuf.log";
            
            
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            
            
            // definition all received.
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "[MutBookManager_BookManager_Test014_D] ");            
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
            
            book_manager->Set_definition_data(&definition_datas);
            
            m_vecRevPacket.clear();
            
            std::vector<fh::cme::market::message::MdpMessage> recovery_datas;  // recovery message
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "[MutBookManager_BookManager_Test014_R] ");  
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &recovery_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), recovery_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", recovery_datas count=", recovery_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
            
            // set received recovery messages
            book_manager->Set_recovery_data(&recovery_datas);
            
            m_vecRevPacket.clear();            
                        
            recovery_datas.clear();
            definition_datas.clear();
        }  
        
        
        delete book_manager;
        book_manager = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    #endif
    
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 以下测试用例为：cme autocert+ MDP3.0和ILink 上的对应的系列测试用例
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // case desc: 
    // Confirm quantities and prices on the book for the selected instrument.
    // Instrument: 1DVEU7 SecurityID: 996791
    // out: send L2: contract=1DVEU7, bid=[price=24135.000000, size=44][price=24130.000000, size=67], offer=
    //
    TEST_F(MutBookManager, BookManager_Test020)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            book_manager = new BookManager(autotest_book_sender);
            if(nullptr == book_manager)
            {
                LOG_ERROR("----- book_manager is nullptr, malloc failed! ------");       
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }
            
            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::MakePrice_1;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_by_price_1.log";
            
            //std::vector<std::string> vecRevPacket;
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message

            m_vecRevPacket.clear();
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](", "=");            
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
             
            book_manager->Set_definition_data(&definition_datas);
            
            m_vecRevPacket.clear();
            
            std::vector<fh::cme::market::message::MdpMessage> recovery_datas;  // recovery message
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.124:19627](");  
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &recovery_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), recovery_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", recovery_datas count=", recovery_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
            
            // set received recovery messages
            book_manager->Set_recovery_data(&recovery_datas);
            m_vecRevPacket.clear();
            
            // 
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, book_manager, autotest_book_sender](const std::string &revPacket)
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

                        int iXMsgNum = m_datas.size();
                        for(auto message = m_datas.begin(); message!=m_datas.end(); ++message)
                        {
                            LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type(), ",iXMsgNum=" ,iXMsgNum);
                            // convert the message to books                     
                            book_manager->Parse_to_send(*message);
                        }
                        
                        m_datas.clear();
                        
                        printf("*********** end **************\n");
                    }
            );         
                                    
            m_vecRevPacket.clear();

            // 数据校验
            if(autotest_book_sender_check!=nullptr)
            {
                std::string construct = "contract=1DVEU7";
                autotest_book_sender_check->CheckResult(construct);
            }
            
            
            delete book_manager;
            book_manager = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
    }
    
    //
    // case desc: 
    // Confirm quantities and prices on the book for the selected instrument.
    // Instrument: 1DVEU7 SecurityID: 996791
    // out: out:send L2: contract=1DVEU7, bid=[price=24115.000000, size=93], offer=[price=24140.000000, size=5][price=24150.000000, size=59][price=24155.000000,
    // size=33][price=24160.000000, size=33][price=24165.000000, size=39]
    //
    TEST_F(MutBookManager, BookManager_Test021)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            book_manager = new BookManager(autotest_book_sender);
            if(nullptr == book_manager)
            {
                LOG_ERROR("----- book_manager is nullptr, malloc failed! ------");       
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::MakePrice_2;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_by_price_2.log";
                        
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](");            
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
             
            book_manager->Set_definition_data(&definition_datas);
            
            m_vecRevPacket.clear();
            
            std::vector<fh::cme::market::message::MdpMessage> recovery_datas;  // recovery message
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.124:19627](");  
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &recovery_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), recovery_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", recovery_datas count=", recovery_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
            
            // set received recovery messages
            book_manager->Set_recovery_data(&recovery_datas);
            m_vecRevPacket.clear();
            
            // 
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, book_manager, autotest_book_sender](const std::string &revPacket)
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

                        int iXMsgNum = m_datas.size();
                        for(auto message = m_datas.begin(); message!=m_datas.end(); ++message)
                        {
                            LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type(), ",iXMsgNum=" ,iXMsgNum);
                            // convert the message to books                     
                            book_manager->Parse_to_send(*message);
                        }
                        
                        m_datas.clear();
                        
                        printf("*********** end **************\n");
                    }
            );       
  
            m_vecRevPacket.clear();            
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string construct = "contract=1DVEU7";
                autotest_book_sender_check->CheckResult(construct);
            }

            delete book_manager;
            book_manager = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
    }
    
    //
    // case desc: 
    // Statistic Messages: A trade summary was sent. Please validate the following values.
    // Instrument: 1DVEH8 
    // out: OnTrade contract=1DVEH8, last=[price=23675.000000, size=33]
    //
    TEST_F(MutBookManager, BookManager_Test022)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            book_manager = new BookManager(autotest_book_sender);
            if(nullptr == book_manager)
            {
                LOG_ERROR("----- book_manager is nullptr, malloc failed! ------");       
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Sm_1;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_427_sm_1.log";
                        
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=01
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](");            
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
             
            book_manager->Set_definition_data(&definition_datas);
            
            m_vecRevPacket.clear();
            
            std::vector<fh::cme::market::message::MdpMessage> recovery_datas;  // recovery message
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.124:19627](");  
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &recovery_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), recovery_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", recovery_datas count=", recovery_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
            
            // set received recovery messages
            book_manager->Set_recovery_data(&recovery_datas);
            m_vecRevPacket.clear();
            
            // 
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, book_manager, autotest_book_sender](const std::string &revPacket)
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

                        int iXMsgNum = m_datas.size();
                        for(auto message = m_datas.begin(); message!=m_datas.end(); ++message)
                        {
                            LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type(), ",iXMsgNum=" ,iXMsgNum);
                            // convert the message to books                     
                            book_manager->Parse_to_send(*message);
                        }
                        
                        m_datas.clear();
                        
                        printf("*********** end **************\n");
                    }
            );       
  
            m_vecRevPacket.clear();            
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string construct = "contract=1DVEH8";
                autotest_book_sender_check->CheckResult(construct);
            }

            delete book_manager;
            book_manager = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
    }
    
    //
    // case desc: 
    // Statistic Messages: A trade cancel was sent. Please validate the following values.
    // Instrument:1DVEU7 SecurityID:996791
    // out: OnTrade: contract=1DVEU7, last=[price=24140.000000, size=668]
    //
    TEST_F(MutBookManager, BookManager_Test023)
    {
        fh::core::market::MarketListenerI *autotest_book_sender = nullptr; 
        fh::cme::market::BookManager *book_manager = nullptr;        

        autotest_book_sender = new fh::core::book::AutoTestBookSender(); 
        
        if(autotest_book_sender!=nullptr)
        {
            book_manager = new BookManager(autotest_book_sender);
            if(nullptr == book_manager)
            {
                LOG_ERROR("----- book_manager is nullptr, malloc failed! ------");       
                delete autotest_book_sender;
                autotest_book_sender = nullptr;
                return;
            }

            fh::core::book::AutoTestBookSender *autotest_book_sender_check = dynamic_cast<fh::core::book::AutoTestBookSender *>(autotest_book_sender);
            int caseId = fh::core::assist::common::CaseIdValue::Sm_2;
            autotest_book_sender_check->SetCaseId(caseId);
            //   [627]UDP Incremental:224.0.28.79, 11627
            //   [627]UDP Incremental:224.0.25.101, 11727
            //   [627]UDP Definition:224.0.28.79, 16627
            //   [627]UDP Recovery:224.0.28.124, 19627
            
            std::string recvBufFileName;
            fh::core::assist::common::getAbsolutePath(recvBufFileName);        
            recvBufFileName += "market_627_427_sm_2.log";
                        
            std::vector<fh::cme::market::message::MdpMessage> definition_datas; // define message
            // received define message
            // udp received from [224.0.28.79:16627](1368)=
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:16627](");            
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &definition_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), definition_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", definition_datas count=", definition_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
             
            book_manager->Set_definition_data(&definition_datas);
            
            m_vecRevPacket.clear();
            
            std::vector<fh::cme::market::message::MdpMessage> recovery_datas;  // recovery message
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.124:19627](");  
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, &recovery_datas](const std::string &revPacket)
                    {
                        // decode                                                
                        std::uint32_t seq = fh::cme::market::message::utility::Pick_messages_from_packet(revPacket.data(), revPacket.size(), recovery_datas);

                        printf("*********** [begin] parse define message **************\n");
                        
                        LOG_INFO("seq=", seq, ", recovery_datas count=", recovery_datas.size());

                        printf("*********** [end] parse define message **************\n");
                    }
            );
            
            // set received recovery messages
            book_manager->Set_recovery_data(&recovery_datas);
            m_vecRevPacket.clear();
            
            // 
            fh::core::assist::common::Read_packets(m_vecRevPacket, recvBufFileName, "udp received from [224.0.28.79:11627](");
            std::for_each(m_vecRevPacket.cbegin(), m_vecRevPacket.cend(),
                    [this, book_manager, autotest_book_sender](const std::string &revPacket)
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

                        int iXMsgNum = m_datas.size();
                        for(auto message = m_datas.begin(); message!=m_datas.end(); ++message)
                        {
                            LOG_INFO("{BE}processed: seq=", message->packet_seq_num(), ", type=", message->message_type(), ",iXMsgNum=" ,iXMsgNum);
                            // convert the message to books                     
                            book_manager->Parse_to_send(*message);
                        }
                        
                        m_datas.clear();
                        
                        printf("*********** end **************\n");
                    }
            );       
  
            m_vecRevPacket.clear();            
            
            // 数据校验            
            if(autotest_book_sender_check!=nullptr)
            {
                std::string construct = "contract=1DVEU7";
                autotest_book_sender_check->CheckResult(construct);
            }

            delete book_manager;
            book_manager = nullptr;
            
            delete autotest_book_sender;
            autotest_book_sender = nullptr;
        }
    }
    
    
} // namespace market
} // namespace cme
} // namespace fh
