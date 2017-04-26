
#include "gtest/gtest.h"

#include "core/assist/logger.h"
#include "autotest_book_sender.h"

extern int g_current_id;
namespace fh
{
namespace core
{
namespace book
{

    AutoTestBookSender::AutoTestBookSender():
    m_current_caseid(0)
    {
        // noop
    }

    AutoTestBookSender::~AutoTestBookSender()
    {
        // noop
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnMarketDisconnect(core::market::MarketI* market)
    {
        // noop
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnMarketReconnect(core::market::MarketI* market)
    {
        // noop
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnContractDefinition(const pb::dms::Contract &contract)
    {
        // 前面加个 C 标记是 definition 数据
        LOG_INFO("send Definition: ", fh::core::assist::utility::Format_pb_message(contract));
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnBBO(const pb::dms::BBO &bbo)
    {
        // 前面加个 B 标记是 BBO 数据
        LOG_INFO("send BBO: ", fh::core::assist::utility::Format_pb_message(bbo));
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnBid(const pb::dms::Bid &bid)
    {
        // 前面加个 D 标记是 bid 数据
        LOG_INFO("send Bid: ", fh::core::assist::utility::Format_pb_message(bid));
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnOffer(const pb::dms::Offer &offer)
    {
        // 前面加个 O 标记是 offer 数据
        LOG_INFO("send Offer: ", fh::core::assist::utility::Format_pb_message(offer));        
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnL2(const pb::dms::L2 &l2)
    {
        // 前面加个 L 标记是 L2 数据
        LOG_INFO("send L2: ", fh::core::assist::utility::Format_pb_message(l2));  
        m_sendL2 = fh::core::assist::utility::Format_pb_message(l2);
        std::string strL2 = m_sendL2;
        std::string strContractKey = ", bid";
        auto pos = strL2.find(strContractKey);
        if(pos != std::string::npos)
        {
            strL2.erase(pos, strL2.size());
            LOG_DEBUG("===== strL2=[", strL2.c_str(), "] ,pos=[", pos, "] =====");
            
            auto iterL2 = m_L2ValueMap.find(strL2);
            if(iterL2 != m_L2ValueMap.end())
            {
                iterL2->second = m_sendL2;
                LOG_DEBUG("===== repalce =====");
            }
            else
            {
                m_L2ValueMap.insert(make_pair(strL2, m_sendL2)); 
                LOG_DEBUG("===== insert =====");
            }             
        }
        
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnL3()
    {
        // noop
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnTrade(const pb::dms::Trade &trade)
    {
        // 前面加个 T 标记是 trade 数据
        LOG_INFO("send Trade: ", fh::core::assist::utility::Format_pb_message(trade));
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnContractAuctioning(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [Auction]");
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnContractNoTrading(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [NoTrading]");
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnContractTrading(const std::string &contract)
    {
		LOG_INFO("contract[", contract, "] status update [Trading]");
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnOrginalMessage(const std::string &message)
    {
        LOG_INFO("send Original Message, size=", message.size(), " message=", message);
    }
    
    void AutoTestBookSender::SetCaseId(const int &caseId)
    {
        m_current_caseid = caseId;
    }
    
    int  AutoTestBookSender::GetCaseId()
    {
        return m_current_caseid;
    }
            
    // check the result of case
    void AutoTestBookSender::CheckResult(const std::string &contract)
    {
        LOG_DEBUG("==== AutoTestBookSender::CheckResult ====");
        switch(m_current_caseid)
        {
            case 20: // case: BookManager_Test020
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("contract=1DVEU7, bid=[price=24135.000000, size=44][price=24130.000000, size=67], offer=",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
                }

                break;
            }
            case 21: // case: BookManager_Test021
            {   
                std::string strL2Value = m_L2ValueMap.at(contract);
                EXPECT_STREQ("contract=1DVEU7, bid=[price=24115.000000, size=93], offer=[price=24140.000000, size=5][price=24150.000000, size=59][price=24155.000000, size=33][price=24160.000000, size=33][price=24165.000000, size=39]",
                        strL2Value.c_str());
                m_L2ValueMap.erase(contract);                
                break;
            }
            default:
            {
                LOG_INFO("ignore m_current_caseid: ", m_current_caseid);
                break;
            }
        }      
       
        LOG_DEBUG("m_L2ValueMap size = ", m_L2ValueMap.size());
    }    

} // namespace book
} // namespace core
} // namespace fh
