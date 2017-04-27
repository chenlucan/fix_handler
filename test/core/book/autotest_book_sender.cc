
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
    m_current_caseid(0), m_sendL2(), m_L2ValueMap(), m_DefValueMap()
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
        if( (m_current_caseid!=fh::core::assist::common::CaseIdValue::MakePrice_1) 
            && (m_current_caseid!=fh::core::assist::common::CaseIdValue::MakePrice_2) )
        {
            return;
        }
        
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
        std::string strJson = message;
        //strJson = "{\"code\":0,\"noEvents\":[{\"eventTime\":\"1466460000000000000\"},{\"eventTime\":\"1505489400000000000\"}, {\"eventTime\":\"1466460000000000000\"},{\"eventTime\":\"1505489400000000000\"}]}";
        
        //strJson = "{\"noEvents\":[{\"eventType":\"5\",\"eventTime\":\"1466460000000000000\"}]}";
        
        //"securityID": "996791",
        //"SecurityGroup": "91", 
        //"marketSegmentID" : "99"        
        //"noEvents": [
        //    {
        //        "eventType": "5",
        //        "eventTime": "1466460000000000000"
        //    },
        //    {
        //        "eventType": "7",
        //        "eventTime": "1505489400000000000"
        //    }
        //],
        
        switch(m_current_caseid)
        {
            case fh::core::assist::common::CaseIdValue::Sd_1: // case: DatSaver_Test001
            { 
                boost::property_tree::ptree ptParse;
                std::stringstream ss(strJson);
                try
                {
                    boost::property_tree::read_json(ss, ptParse);
                    
                    boost::property_tree::ptree event_array = ptParse.get_child("message.noEvents");  // get_child得到数组对象   

                    if(event_array.size()!=2)
                    {
                        return;
                    }
                    
                    std::string securityIDValue = ptParse.get<std::string>("message.securityID");   // 获取“securityID”的value            
                    LOG_DEBUG("===== securityIDValue: ", securityIDValue.c_str(), " =====");
                                
                    fh::core::assist::common::DefineMsg_Compare t_defComp;
                    std::string securityGroupValue = ptParse.get<std::string>("message.SecurityGroup");
                    std::string defComp = "SecurityGroup="+securityGroupValue;
                    std::string marketSegmentIDValue = ptParse.get<std::string>("message.marketSegmentID");
                    defComp += ", marketSegmentID="+marketSegmentIDValue;
                    defComp += ", noEvents=[";
                    // 遍历数组
                    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, event_array)  
                    {  
                        boost::property_tree::ptree& childparse = v.second;
                        
                        std::string eventType = childparse.get<std::string>("eventType");
                        std::string eventTime = childparse.get<std::string>("eventTime");
                        LOG_DEBUG("********* eventType = ", eventType, ", eventTime = ", eventTime ," *********");
                        if("5" == eventType)
                        {
                            t_defComp.activation_date_ime  = eventTime;                    
                        }
                        else if("7" == eventType)
                        {
                            t_defComp.expiration_date_ime  = eventTime;
                        }
                        std::stringstream s;
                        write_json(s, v.second);
                        std::string event_item = s.str();
                        defComp += ", noEvents="+event_item;
                        LOG_DEBUG("=== event_item = ", event_item.c_str(), ", event_array.size = ", event_array.size(),   " =====");
                    }
                    
                    t_defComp.market_segment_id    = marketSegmentIDValue;
                    t_defComp.security_group       = securityGroupValue;
                        
                    std::string strKey = "securityID="+securityIDValue;
                    auto iterL2 = m_DefValueMap.find(strKey);
                    if(iterL2 != m_DefValueMap.end())
                    {          
                        LOG_DEBUG("===== [repalce] (", strKey, ",", iterL2->second.To_string(), ") -> (",t_defComp.To_string(), ") =====");
                        iterL2->second = t_defComp;
                    }
                    else
                    {
                        m_DefValueMap.insert(make_pair(strKey, t_defComp)); 
                        LOG_DEBUG("===== [insert] (", strKey, ",", t_defComp.To_string(), ") =====");
                    }
                }
                catch(boost::property_tree::ptree_error & e) {
                    return;
                }        
                /*try
                {
                    // parse json error: unset document::element
                    bsoncxx::document::value details = bsoncxx::from_json(strJson);
                    auto view = details.view();
                    auto colname = view["message.SecurityGroup"].get_utf8().value.to_string();
                    auto colevent = view["message.noEvents"].get_array().value;
                    
                    //LOG_DEBUG("colevent = ", colevent);            
                    LOG_DEBUG("colname: ", colname);
                }
                catch(std::exception &e)
                {
                    LOG_WARN("parse json error: ", e.what());
                }*/               
                break;
            }
            default:
            {
                LOG_INFO("other m_current_caseid: ", m_current_caseid);
                break;
            }
        }
        if(m_current_caseid==22)
        {

        }        
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
        LOG_DEBUG("==== AutoTestBookSender::CheckResult m_current_caseid = [" , m_current_caseid, "] ,contract = [", contract,  "] ====");
        switch(m_current_caseid)
        {
            case fh::core::assist::common::CaseIdValue::MakePrice_1: // case: BookManager_Test020
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    LOG_DEBUG("[MakePrice_1] check L2Vale = ", iterL2->second.c_str());
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("contract=1DVEU7, bid=[price=24135.000000, size=44][price=24130.000000, size=67], offer=",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
                }

                break;
            }
            case fh::core::assist::common::CaseIdValue::MakePrice_2: // case: BookManager_Test021
            {   
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {                    
                    LOG_DEBUG("[MakePrice_2] check L2Vale = ", iterL2->second.c_str());
                    std::string strL2Value = iterL2->second.c_str();
                    EXPECT_STREQ("contract=1DVEU7, bid=[price=24115.000000, size=93], offer=[price=24140.000000, size=5][price=24150.000000, size=59][price=24155.000000, size=33][price=24160.000000, size=33][price=24165.000000, size=39]",
                            strL2Value.c_str());
                    m_L2ValueMap.erase(contract);
                }                             
                break;
            }
            case fh::core::assist::common::CaseIdValue::Sd_1: // case: DatSaver_Test001
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("defMsgValue = ", defMsgValue.To_string());
                    EXPECT_STREQ("99", defMsgValue.market_segment_id.c_str());
                    EXPECT_STREQ("8$", defMsgValue.security_group.c_str());
                    EXPECT_STREQ("1489782300000000000", defMsgValue.activation_date_ime.c_str());
                    EXPECT_STREQ("1521207000000000000", defMsgValue.expiration_date_ime.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            default:
            {
                LOG_INFO("ignore m_current_caseid: ", m_current_caseid);
                break;
            }
        }
    }    

} // namespace book
} // namespace core
} // namespace fh
