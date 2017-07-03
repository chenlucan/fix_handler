
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
    void AutoTestBookSender::OnTurnover(const pb::dms::Turnover &turnover)
    {
        LOG_INFO("OnTurnover: ", fh::core::assist::utility::Format_pb_message(turnover));
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnOffer(const pb::dms::Offer &offer)
    {
        // 前面加个 O 标记是 offer 数据
        LOG_INFO("send Offer: ", fh::core::assist::utility::Format_pb_message(offer));  
        std::string strOrignalOffer = fh::core::assist::utility::Format_pb_message(offer);
        switch(m_current_caseid)
        {
            case fh::core::assist::common::CaseIdValue::Sm_5: // case: BookManager_Test025            
            {
                std::string strOffer = strOrignalOffer;
                std::string strContractKey = ", offer=[";
                auto pos = strOffer.find(strContractKey);
                if(pos != std::string::npos)
                {
                    std::string strPriceValue = strOffer;
                    strOffer.erase(pos, strOffer.size());
                    LOG_DEBUG("===== strOffer=[", strOffer.c_str(), "] ,pos=[", pos, "] =====");
                    
                    strPriceValue.erase(0, pos+strContractKey.size());
                    std::string strEndKey = ", size=";              
                    pos = strPriceValue.find(strEndKey);
                    if(pos != std::string::npos)
                    {
                        strPriceValue.erase(pos, strPriceValue.size());
                    }
                    
                    LOG_DEBUG("===== strPriceValue=[", strPriceValue.c_str(), "] ,pos=[", pos, "] =====");
                    
                    auto iterL2 = m_L2ValueMap.find(strOffer);
                    if(iterL2 != m_L2ValueMap.end())
                    {
                        iterL2->second = strPriceValue;
                        LOG_DEBUG("===== [OnOffer] repalce (", iterL2->second, " -> ", strPriceValue.c_str(), ")",  " =====");
                    }
                    else
                    {
                        m_L2ValueMap.insert(make_pair(strOffer, strPriceValue));
                        LOG_DEBUG("===== [OnOffer] insert (", strOffer, " , ", strPriceValue.c_str(), ")",  " =====");
                    }
                }   
        
                break;
            } 
            default:
            {
                LOG_INFO("other m_current_caseid: ", m_current_caseid);
                break;
            }
        }        
    }

    // implement of MarketListenerI
    void AutoTestBookSender::OnL2(const pb::dms::L2 &l2)
    {
        // 前面加个 L 标记是 L2 数据
        LOG_INFO("send L2: ", fh::core::assist::utility::Format_pb_message(l2));  
        m_sendL2 = fh::core::assist::utility::Format_pb_message(l2);
        if( (m_current_caseid!=fh::core::assist::common::CaseIdValue::MakePrice_1) 
            && (m_current_caseid!=fh::core::assist::common::CaseIdValue::MakePrice_2)
            && (m_current_caseid!=fh::core::assist::common::CaseIdValue::ChannelReset_MakePrice_1)
            && (m_current_caseid!=fh::core::assist::common::CaseIdValue::ChannelReset_MakePrice_2)
            && (m_current_caseid!=fh::core::assist::common::CaseIdValue::qty_pc_1st_instr)
            && (m_current_caseid!=fh::core::assist::common::CaseIdValue::qty_pc_2nd_instr) )
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
        std::string strOrignalTrade = fh::core::assist::utility::Format_pb_message(trade);
        switch(m_current_caseid)
        {
            case fh::core::assist::common::CaseIdValue::Sm_1: // case: BookManager_Test022
            case fh::core::assist::common::CaseIdValue::Sm_2: // case: BookManager_Test023            
            {
                std::string strTrade = strOrignalTrade;
                std::string strContractKey = ", last=";
                auto pos = strTrade.find(strContractKey);
                if(pos != std::string::npos)
                {
                    strTrade.erase(pos, strTrade.size());
                    LOG_DEBUG("===== strTrade=[", strTrade.c_str(), "] ,pos=[", pos, "] =====");
                    
                    auto iterL2 = m_L2ValueMap.find(strTrade);
                    if(iterL2 != m_L2ValueMap.end())
                    {
                        iterL2->second = strOrignalTrade;
                        LOG_DEBUG("===== [OnTrade] repalce (", iterL2->second, " -> ", strOrignalTrade.c_str(), ")",  " =====");
                    }
                    else
                    {
                        m_L2ValueMap.insert(make_pair(strTrade, strOrignalTrade)); 
                        LOG_DEBUG("===== [OnTrade] insert (", strTrade, " , ", strOrignalTrade.c_str(), ")",  " =====");
                    }           
                }   
        
                break;
            }
            case fh::core::assist::common::CaseIdValue::Sm_4: // case: BookManager_Test024            
            {
                std::string strTrade = strOrignalTrade;
                std::string strContractKey = ", last=[";
                auto pos = strTrade.find(strContractKey);
                if(pos != std::string::npos)
                {
                    std::string strPriceValue = strTrade;
                    strTrade.erase(pos, strTrade.size());
                    LOG_DEBUG("===== strTrade=[", strTrade.c_str(), "] ,pos=[", pos, "] =====");
                    
                                        
                    strPriceValue.erase(0, pos+strContractKey.size());
                    std::string strEndKey = ", size=";                  
                    pos = strPriceValue.find(strEndKey);
                    if(pos != std::string::npos)
                    {
                        strPriceValue.erase(pos, strPriceValue.size());
                    }
                    
                    LOG_DEBUG("===== strPriceValue=[", strPriceValue.c_str(), "] ,pos=[", pos, "] =====");
                    
                    auto iterL2 = m_L2ValueMap.find(strTrade);
                    if(iterL2 != m_L2ValueMap.end())
                    {
                        iterL2->second = strPriceValue;
                        LOG_DEBUG("===== [OnTrade] repalce (", iterL2->second, " -> ", strPriceValue.c_str(), ")",  " =====");
                    }
                    else
                    {
                        m_L2ValueMap.insert(make_pair(strTrade, strPriceValue));
                        LOG_DEBUG("===== [OnTrade] insert (", strTrade, " , ", strPriceValue.c_str(), ")",  " =====");
                    }           
                }   
        
                break;
            }
            default:
            {
                LOG_INFO("other m_current_caseid: ", m_current_caseid);
                break;
            }
        }
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
        LOG_INFO("=== send Original Message, size=", message.size(), " message=", message, ", m_current_caseid= ", m_current_caseid, " ===");
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
            case fh::core::assist::common::CaseIdValue::Sd_1:    // case: DatSaver_Test001
            case fh::core::assist::common::CaseIdValue::Sd_2:    // case: DatSaver_Test002
            case fh::core::assist::common::CaseIdValue::Fs_1:    // case: DatSaver_Test003
            case fh::core::assist::common::CaseIdValue::Fs_2:    // case: DatSaver_Test004
            case fh::core::assist::common::CaseIdValue::Sdmff_1: // case: DatSaver_Test014            
            { 
                boost::property_tree::ptree ptParse;
                std::stringstream ss(strJson);
                try
                {
                    boost::property_tree::read_json(ss, ptParse);
                    
                    boost::property_tree::ptree event_array = ptParse.get_child("message.noEvents");  // get_child得到数组对象

                    if(event_array.size()!=0) // 有message.noEvents信息
                    {
                        //获取“securityID”的value
                        std::string securityIDValue = ptParse.get<std::string>("message.securityID");            
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
            case fh::core::assist::common::CaseIdValue::Order_Qty: // case: DatSaver_Test005
            {
                boost::property_tree::ptree ptParse;
                std::stringstream ss(strJson);
                try
                {
                    boost::property_tree::read_json(ss, ptParse);  
                   
                    boost::property_tree::ptree norelatedsym_array = ptParse.get_child("message.noRelatedSym");  // get_child得到数组对象

                    if(norelatedsym_array.size()!=0) // 有message.noRelatedSym
                    {            
                        fh::core::assist::common::DefineMsg_Compare t_defComp; 
                    
                        std::string securityIDValue;                              
                        // 遍历数组
                        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, norelatedsym_array)  
                        {  
                            boost::property_tree::ptree& childparse = v.second;
                            securityIDValue = childparse.get<std::string>("securityID");   
                            std::string orderQtyValue = childparse.get<std::string>("orderQty");                            
                            LOG_DEBUG("********* orderQtyValue = ", orderQtyValue, ", securityIDValue = ", securityIDValue.c_str(), " *********");
                            t_defComp.order_qty    = orderQtyValue;
                            
                            std::stringstream s;
                            write_json(s, v.second);
                            std::string norelatedsym_item = s.str();
                            std::string defComp = "securityID="+securityIDValue;  
                            defComp += ", noRelatedSym="+norelatedsym_item;
                            LOG_DEBUG("=== norelatedsym_item = ", norelatedsym_item.c_str(), ", norelatedsym_array.size = ", norelatedsym_array.size(),   " =====");
                            
                            std::string strKey = "securityID="+securityIDValue;
                            auto iterL2 = m_DefValueMap.find(strKey);
                            if(iterL2 != m_DefValueMap.end())
                            {          
                                LOG_DEBUG("===== [repalce] (", strKey, ",", iterL2->second.To_order_qty_string(), ") -> (",t_defComp.To_order_qty_string(), ") =====");
                                iterL2->second = t_defComp;
                            }
                            else
                            {
                                m_DefValueMap.insert(make_pair(strKey, t_defComp)); 
                                LOG_DEBUG("===== [insert] (", strKey, ",", t_defComp.To_order_qty_string(), ") =====");
                            }
                        }
                    } 
                }
                catch(boost::property_tree::ptree_error & e) {
                    return;
                }
                break;
            }
            case fh::core::assist::common::CaseIdValue::Sm_3: // case: DatSaver_Test006
            {
                boost::property_tree::ptree ptParse;
                std::stringstream ss(strJson);
                try
                {
                    boost::property_tree::read_json(ss, ptParse);  
                   
                    boost::property_tree::ptree nomdentries_array = ptParse.get_child("message.noMDEntries");  // get_child得到数组对象

                    if(nomdentries_array.size()!=0) // 有message.noMDEntries
                    {            
                        fh::core::assist::common::DefineMsg_Compare t_defComp; 
                    
                        std::string securityIDValue;                              
                        // 遍历数组
                        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, nomdentries_array)  
                        {  
                            boost::property_tree::ptree& childparse = v.second;
                            securityIDValue = childparse.get<std::string>("securityID");   
                                                        
                                                    
                            bool is_mdentry_size = false;
                            //auto itEntrySize = ptParse.find("message.noMDEntries.mDEntrySize");

                            auto itEntrySize = childparse.find("mDEntrySize");
                            if(itEntrySize!=childparse.not_found())
                            {
                                std::string mDEntrySize = childparse.get<std::string>("mDEntrySize");                            
                                LOG_DEBUG("********* mDEntrySize = ", mDEntrySize, ", securityIDValue = ", securityIDValue.c_str(), " *********");
                                t_defComp.mdentry_size    = mDEntrySize;
                                is_mdentry_size = true;
                            }
                            else
                            {
                                LOG_DEBUG("********* not found mDEntrySize *********");
                            }
                            
                            bool is_mdentry_px = false;
                            boost::property_tree::ptree::assoc_iterator itEntryPx = childparse.find("mDEntryPx");
                            if(itEntryPx!=childparse.not_found())
                            {
                                std::string mDEntryPxMantissa = childparse.get<std::string>("mDEntryPx.mantissa");
                                std::string mDEntryPxExponent = childparse.get<std::string>("mDEntryPx.exponent");                            
                                LOG_DEBUG("********* [mDEntryPxMantissa = ", mDEntryPxMantissa, ", mDEntryPxExponent = ",mDEntryPxExponent, "], securityIDValue = ", securityIDValue.c_str(), " *********");
                                t_defComp.mdentry_px_mantissa    = mDEntryPxMantissa;
                                t_defComp.mdentry_px_exponent    = mDEntryPxExponent;
                                is_mdentry_px = true;
                            }
                            else
                            {
                                LOG_DEBUG("********* not found mDEntryPx *********");
                            }
                                                        
                                                      
                            std::stringstream s;
                            write_json(s, v.second);
                            std::string nomdentries_item = s.str();                            
                            LOG_DEBUG("=== nomdentries_item = ", nomdentries_item.c_str(), ", nomdentries_array.size = ", nomdentries_array.size(),   " =====");
                            
                            
                            if((true == is_mdentry_size) || (true == is_mdentry_px))
                            {
                                std::string strKey = "securityID="+securityIDValue;
                                auto iterL2 = m_DefValueMap.find(strKey);
                                if(iterL2 != m_DefValueMap.end())
                                {   
                                    LOG_DEBUG("===== [repalce] (", strKey, ",", iterL2->second.To_mdentry_price_size_string(), ") -> (",t_defComp.To_mdentry_price_size_string(), ") =====");
                                    if(true == is_mdentry_size)
                                    {
                                        iterL2->second.mdentry_size = t_defComp.mdentry_size;
                                    }
                                    
                                    if(true == is_mdentry_px)
                                    {
                                        iterL2->second.mdentry_px_mantissa = t_defComp.mdentry_px_mantissa;
                                        iterL2->second.mdentry_px_exponent = t_defComp.mdentry_px_exponent;
                                    }
                                }
                                else
                                {
                                    m_DefValueMap.insert(make_pair(strKey, t_defComp)); 
                                    LOG_DEBUG("===== [insert] (", strKey, ",", t_defComp.To_mdentry_price_size_string(), ") =====");
                                }
                            }
                            
                        }
                    } 
                }
                catch(boost::property_tree::ptree_error & e) {
                    return;
                }
                break;
            }
            case fh::core::assist::common::CaseIdValue::Sm_6: // case: DatSaver_Test007
            {
                boost::property_tree::ptree ptParse;
                std::stringstream ss(strJson);
                try
                {
                    boost::property_tree::read_json(ss, ptParse);  
                   
                    boost::property_tree::ptree nomdentries_array = ptParse.get_child("message.noMDEntries");  // get_child得到数组对象

                    if(nomdentries_array.size()!=0) // 有message.noMDEntries
                    {            
                        fh::core::assist::common::DefineMsg_Compare t_defComp; 
                    
                        std::string securityIDValue;                              
                        // 遍历数组
                        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, nomdentries_array)  
                        {  
                            boost::property_tree::ptree& childparse = v.second;
                            securityIDValue = childparse.get<std::string>("securityID");   
                                                        
                            bool is_highlimit_px = false;
                            boost::property_tree::ptree::assoc_iterator itHighLimitPrice = childparse.find("highLimitPrice");
                            if(itHighLimitPrice!=childparse.not_found())
                            {
                                std::string highLimitPriceMantissa = childparse.get<std::string>("highLimitPrice.mantissa");
                                std::string highLimitPriceExponent = childparse.get<std::string>("highLimitPrice.exponent");                           
                                LOG_DEBUG("********* [highLimitPriceMantissa = ", highLimitPriceMantissa, ", highLimitPriceExponent = ",highLimitPriceExponent, "], securityIDValue = ", securityIDValue.c_str(), " *********");
                                t_defComp.highlimit_price_mantissa    = highLimitPriceMantissa;
                                t_defComp.highlimit_price_exponent    = highLimitPriceExponent;
                                is_highlimit_px = true;
                            }
                            else
                            {
                                LOG_DEBUG("********* not found highlimit_px *********");
                            }
                               
                            bool is_lowlimit_px = false;
                            boost::property_tree::ptree::assoc_iterator itLowLimitPrice = childparse.find("lowLimitPrice");
                            if(itLowLimitPrice!=childparse.not_found())
                            {
                                std::string lowLimitPriceMantissa = childparse.get<std::string>("lowLimitPrice.mantissa");
                                std::string lowLimitPriceExponent = childparse.get<std::string>("lowLimitPrice.exponent");                           
                                LOG_DEBUG("********* [lowLimitPriceMantissa = ", lowLimitPriceMantissa, ", lowLimitPriceExponent = ",lowLimitPriceExponent, "], securityIDValue = ", securityIDValue.c_str(), " *********");
                                t_defComp.lowlimit_price_mantissa    = lowLimitPriceMantissa;
                                t_defComp.lowlimit_price_exponent    = lowLimitPriceExponent;
                                is_lowlimit_px = true;
                            }
                            else
                            {
                                LOG_DEBUG("********* not found highlimit_px *********");
                            }                               
                                                      
                            std::stringstream s;
                            write_json(s, v.second);
                            std::string nomdentries_item = s.str();                            
                            LOG_DEBUG("=== nomdentries_item = ", nomdentries_item.c_str(), ", nomdentries_array.size = ", nomdentries_array.size(),   " =====");
                            
                            
                            if((true == is_highlimit_px) || (true == is_lowlimit_px))
                            {
                                std::string strKey = "securityID="+securityIDValue;
                                auto iterL2 = m_DefValueMap.find(strKey);
                                if(iterL2 != m_DefValueMap.end())
                                {   
                                    LOG_DEBUG("===== [repalce] (", strKey, ",", iterL2->second.To_limit_price_string(), ") -> (",t_defComp.To_limit_price_string(), ") =====");
                                    if(true == is_highlimit_px)
                                    {
                                        iterL2->second.highlimit_price_mantissa = t_defComp.highlimit_price_mantissa;
                                        iterL2->second.highlimit_price_exponent = t_defComp.highlimit_price_exponent;
                                    }
                                    
                                    if(true == is_lowlimit_px)
                                    {
                                        iterL2->second.lowlimit_price_mantissa = t_defComp.lowlimit_price_mantissa;
                                        iterL2->second.lowlimit_price_exponent = t_defComp.lowlimit_price_exponent;
                                    }
                                }
                                else
                                {
                                    m_DefValueMap.insert(make_pair(strKey, t_defComp)); 
                                    LOG_DEBUG("===== [insert] (", strKey, ",", t_defComp.To_limit_price_string(), ") =====");
                                }
                            }
                            
                        }
                    } 
                }
                catch(boost::property_tree::ptree_error & e) {
                    return;
                }
                break;
            }
            case fh::core::assist::common::CaseIdValue::Market_State_Message_1: // case: DatSaver_Test008
            {
                boost::property_tree::ptree ptParse;
                std::stringstream ss(strJson);
                try
                {
                    boost::property_tree::read_json(ss, ptParse);  
                    
                    std::string typeValue = ptParse.get<std::string>("message.type");
                    std::string securityTradingStatusValue = ptParse.get<std::string>("message.securityTradingStatus");
                    
                    fh::core::assist::common::DefineMsg_Compare t_defComp; 
                    t_defComp.security_trading_status = securityTradingStatusValue;
                    
                    std::string strKey = "type="+typeValue;
                    auto iterL2 = m_DefValueMap.find(strKey);
                    if(iterL2 != m_DefValueMap.end())
                    {   
                        LOG_DEBUG("===== [repalce] (", strKey, ",", iterL2->second.To_security_trading_status_string(), ") -> (",t_defComp.To_security_trading_status_string(), ") =====");
                        iterL2->second.security_trading_status = t_defComp.security_trading_status;
                        //iterL2->second.security_trading_event = t_defComp.security_trading_event;
                    }
                    else
                    {
                        m_DefValueMap.insert(make_pair(strKey, t_defComp));
                        LOG_DEBUG("===== [insert] (", strKey, ",", t_defComp.To_security_trading_status_string(), ") =====");
                    }            
                }
                catch(boost::property_tree::ptree_error & e) {
                    return;
                }
                break;
            }
            case fh::core::assist::common::CaseIdValue::Market_State_Message_2: // case: DatSaver_Test009
            {
                boost::property_tree::ptree ptParse;
                std::stringstream ss(strJson);
                try
                {
                    boost::property_tree::read_json(ss, ptParse);  
                    
                    boost::property_tree::ptree items = ptParse.get_child("message");  // get_child得到子对象
                    
                    bool is_security_trading_status = false;
                    std::string typeValue;
                    fh::core::assist::common::DefineMsg_Compare t_defComp; 
                    for (boost::property_tree::ptree::iterator it=items.begin();it!=items.end();++it)  
                    {  
                        //遍历读出数据
                        std::string key=it->first;//key
                        std::string value =it->second.data();
                        
                        std::string strType = "type";
                        std::string strSecurityTradingStatus = "securityTradingStatus";
                        if(strSecurityTradingStatus == key)
                        {
                            t_defComp.security_trading_status = value;
                            is_security_trading_status = true;
                        }
                        else if(strType == key)
                        {
                            typeValue = value;
                        }
                        LOG_DEBUG("===== first: ", key, ", second: ", value.c_str(), " =====");
                    }
                    
                    
                    if(true == is_security_trading_status)
                    {
                        std::string strKey = "type="+typeValue;
                        auto iterL2 = m_DefValueMap.find(strKey);
                        if(iterL2 != m_DefValueMap.end())
                        {   
                            LOG_DEBUG("===== [repalce] (", strKey, ",", iterL2->second.To_security_trading_status_string(), ") -> (",t_defComp.To_security_trading_status_string(), ") =====");
                            iterL2->second.security_trading_status = t_defComp.security_trading_status;
                            //iterL2->second.security_trading_event = t_defComp.security_trading_event;
                        }
                        else
                        {
                            m_DefValueMap.insert(make_pair(strKey, t_defComp));
                            LOG_DEBUG("===== [insert] (", strKey, ",", t_defComp.To_security_trading_status_string(), ") =====");
                        } 
                    }              
                }
                catch(boost::property_tree::ptree_error & e) {
                    return;
                }
                break;
            }
            case fh::core::assist::common::CaseIdValue::Market_State_Message_3: // case: DatSaver_Test010
            case fh::core::assist::common::CaseIdValue::Market_State_Message_4: // case: DatSaver_Test011
            case fh::core::assist::common::CaseIdValue::Market_State_Message_5: // case: DatSaver_Test012
            {
                boost::property_tree::ptree ptParse;
                std::stringstream ss(strJson);
                try
                {
                    boost::property_tree::read_json(ss, ptParse);  
                    
                    std::string typeValue = ptParse.get<std::string>("message.type");
                    std::string securityTradingStatusValue = ptParse.get<std::string>("message.securityTradingStatus");
                    
                    fh::core::assist::common::DefineMsg_Compare t_defComp; 
                    t_defComp.security_trading_status = securityTradingStatusValue;
                    
                    std::string strKey = "type="+typeValue;
                    auto iterL2 = m_DefValueMap.find(strKey);
                    if(iterL2 != m_DefValueMap.end())
                    {   
                        LOG_DEBUG("===== [repalce] (", strKey, ",", iterL2->second.To_security_trading_status_string(), ") -> (",t_defComp.To_security_trading_status_string(), ") =====");
                        iterL2->second.security_trading_status = t_defComp.security_trading_status;
                        //iterL2->second.security_trading_event = t_defComp.security_trading_event;
                    }
                    else
                    {
                        m_DefValueMap.insert(make_pair(strKey, t_defComp));
                        LOG_DEBUG("===== [insert] (", strKey, ",", t_defComp.To_security_trading_status_string(), ") =====");
                    }            
                }
                catch(boost::property_tree::ptree_error & e) {
                    return;
                }
                break;
            }
            case fh::core::assist::common::CaseIdValue::Sdmfo_1: // case: DatSaver_Test013            
            { 
                boost::property_tree::ptree ptParse;
                std::stringstream ss(strJson);
                try
                {
                    boost::property_tree::read_json(ss, ptParse);
                    
                    boost::property_tree::ptree event_array = ptParse.get_child("message.noEvents");  // get_child得到数组对象

                    if(event_array.size()!=0) // 有message.noEvents信息
                    {
                        //获取“securityID”的value
                        std::string securityIDValue = ptParse.get<std::string>("message.securityID");            
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
                        
                        std::string strikePriceMantissaValue = ptParse.get<std::string>("message.strikePrice.mantissa");
                        std::string strikePriceExponentValue = ptParse.get<std::string>("message.strikePrice.exponent");
                        
                        t_defComp.market_segment_id        = marketSegmentIDValue;
                        t_defComp.security_group           = securityGroupValue;                        
                        t_defComp.strike_price_mantissa    = strikePriceMantissaValue;
                        t_defComp.strike_price_exponent    = strikePriceExponentValue;
                            
                        std::string strKey = "securityID="+securityIDValue;
                        auto iterL2 = m_DefValueMap.find(strKey);
                        if(iterL2 != m_DefValueMap.end())
                        {  
                            LOG_DEBUG("===== [repalce] (", strKey, ",", iterL2->second.To_security_definition_messages_for_options_string(), ") -> (",t_defComp.To_security_definition_messages_for_options_string(), ") =====");
                            iterL2->second = t_defComp;
                        }
                        else
                        {
                            m_DefValueMap.insert(make_pair(strKey, t_defComp)); 
                            LOG_DEBUG("===== [insert] (", strKey, ",", t_defComp.To_security_definition_messages_for_options_string(), ") =====");
                        }
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
                    LOG_DEBUG("[Sd_1] check defMsgValue = ", defMsgValue.To_string());
                    EXPECT_STREQ("99", defMsgValue.market_segment_id.c_str());
                    EXPECT_STREQ("8$", defMsgValue.security_group.c_str());
                    EXPECT_STREQ("1489782300000000000", defMsgValue.activation_date_ime.c_str());
                    EXPECT_STREQ("1521207000000000000", defMsgValue.expiration_date_ime.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            case fh::core::assist::common::CaseIdValue::Sd_2: // case: DatSaver_Test002
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Sd_2] check defMsgValue = ", defMsgValue.To_string());
                    EXPECT_STREQ("99", defMsgValue.market_segment_id.c_str());
                    EXPECT_STREQ("$8", defMsgValue.security_group.c_str());
                    EXPECT_STREQ("1492985133327000000", defMsgValue.activation_date_ime.c_str());
                    EXPECT_STREQ("1493416740000000000", defMsgValue.expiration_date_ime.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            case fh::core::assist::common::CaseIdValue::Fs_1: // case: DatSaver_Test003
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Fs_1] check defMsgValue = ", defMsgValue.To_string());
                    EXPECT_STREQ("99", defMsgValue.market_segment_id.c_str());
                    EXPECT_STREQ("91", defMsgValue.security_group.c_str());
                    EXPECT_STREQ("1466460000000000000", defMsgValue.activation_date_ime.c_str());
                    EXPECT_STREQ("1505489400000000000", defMsgValue.expiration_date_ime.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            case fh::core::assist::common::CaseIdValue::Fs_2: // case: DatSaver_Test004
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Fs_2] check defMsgValue = ", defMsgValue.To_string());
                    EXPECT_STREQ("99", defMsgValue.market_segment_id.c_str());
                    EXPECT_STREQ("91", defMsgValue.security_group.c_str());
                    EXPECT_STREQ("1481929200000000000", defMsgValue.activation_date_ime.c_str());
                    EXPECT_STREQ("1505489400000000000", defMsgValue.expiration_date_ime.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            case fh::core::assist::common::CaseIdValue::Order_Qty: // case: DatSaver_Test005
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Order_Qty] check defMsgValue = ", defMsgValue.To_order_qty_string());
                    EXPECT_STREQ("290", defMsgValue.order_qty.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            case fh::core::assist::common::CaseIdValue::Sm_1: // case: BookManager_Test022
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    LOG_DEBUG("[Sm_1] check TradeVale = ", iterL2->second.c_str());
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("contract=1DVEH8, last=[price=23675.000000, size=33]",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
                }

                break;
            }
            case fh::core::assist::common::CaseIdValue::Sm_2: // case: BookManager_Test023
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    LOG_DEBUG("[Sm_2] check TradeVale = ", iterL2->second.c_str());
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("contract=1DVEU7, last=[price=24140.000000, size=668]",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
                }

                break;
            }
            case fh::core::assist::common::CaseIdValue::Sm_3: // case: DatSaver_Test006
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Sm_3] check defMsgValue = ", defMsgValue.To_mdentry_price_size_string());
                    EXPECT_STREQ("241450000000", defMsgValue.mdentry_px_mantissa.c_str());
                    EXPECT_STREQ("-7", defMsgValue.mdentry_px_exponent.c_str());
                    EXPECT_STREQ("47", defMsgValue.mdentry_size.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            case fh::core::assist::common::CaseIdValue::Sm_4: // case: BookManager_Test024
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    LOG_DEBUG("[Sm_4] check Trade mDEntryPx = ", iterL2->second.c_str());
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("price=24145.000000",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
                }

                break;
            }
            case fh::core::assist::common::CaseIdValue::Sm_5: // case: BookManager_Test025
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    LOG_DEBUG("[Sm_5] check Offer mDEntryPx = ", iterL2->second.c_str());
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("price=23670.000000",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
                }

                break;
            }
            case fh::core::assist::common::CaseIdValue::Sm_6: // case: DatSaver_Test007
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Sm_6] check defMsgValue = ", defMsgValue.To_limit_price_string());
                    EXPECT_STREQ("242200000000", defMsgValue.highlimit_price_mantissa.c_str());
                    EXPECT_STREQ("-7", defMsgValue.highlimit_price_exponent.c_str());
                    EXPECT_STREQ("240650000000", defMsgValue.lowlimit_price_mantissa.c_str());
                    EXPECT_STREQ("-7", defMsgValue.lowlimit_price_exponent.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            case fh::core::assist::common::CaseIdValue::ChannelReset_MakePrice_1: // case: BookManager_Test026
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    LOG_DEBUG("[ChannelReset_MakePrice_1] check L2Vale = ", iterL2->second.c_str());
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("contract=1DVEU7, bid=[price=24135.000000, size=62][price=24130.000000, size=7][price=24125.000000, size=74][price=24120.000000, size=26][price=24115.000000, size=94], offer=[price=24140.000000, size=49][price=24145.000000, size=62][price=24150.000000, size=80][price=24155.000000, size=54][price=24160.000000, size=60]",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
                }

                break;
            }
            case fh::core::assist::common::CaseIdValue::ChannelReset_MakePrice_2: // case: BookManager_Test027
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    LOG_DEBUG("[ChannelReset_MakePrice_2] check L2Vale = ", iterL2->second.c_str());
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("contract=1DVEU7, bid=[price=24135.000000, size=62][price=24130.000000, size=7][price=24125.000000, size=74][price=24120.000000, size=26][price=24115.000000, size=94], offer=[price=24140.000000, size=49][price=24145.000000, size=62][price=24150.000000, size=80][price=24155.000000, size=54][price=24160.000000, size=60]",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
                }

                break;
            }
            case fh::core::assist::common::CaseIdValue::Market_State_Message_1: // case: DatSaver_Test008
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Market_State_Message_1] check defMsgValue = ", defMsgValue.To_security_trading_status_string());
                    EXPECT_STREQ("21", defMsgValue.security_trading_status.c_str());                    
                    
                    m_DefValueMap.erase(contract);
                }

                break;
            } 
            case fh::core::assist::common::CaseIdValue::Market_State_Message_2: // case: DatSaver_Test009
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Market_State_Message_2] check defMsgValue = ", defMsgValue.To_security_trading_status_string());
                    EXPECT_STREQ("21", defMsgValue.security_trading_status.c_str());                    
                    
                    m_DefValueMap.erase(contract);
                }

                break;
            }
            case fh::core::assist::common::CaseIdValue::Market_State_Message_3: // case: DatSaver_Test010
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Market_State_Message_3] check defMsgValue = ", defMsgValue.To_security_trading_status_string());
                    EXPECT_STREQ("17", defMsgValue.security_trading_status.c_str());                    
                    
                    m_DefValueMap.erase(contract);
                }

                break;
            } 
            case fh::core::assist::common::CaseIdValue::Market_State_Message_4: // case: DatSaver_Test011
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Market_State_Message_4] check defMsgValue = ", defMsgValue.To_security_trading_status_string());
                    EXPECT_STREQ("2", defMsgValue.security_trading_status.c_str());                    
                    
                    m_DefValueMap.erase(contract);
                }

                break;
            } 
            case fh::core::assist::common::CaseIdValue::Market_State_Message_5: // case: DatSaver_Test012
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Market_State_Message_5] check defMsgValue = ", defMsgValue.To_security_trading_status_string());
                    EXPECT_STREQ("4", defMsgValue.security_trading_status.c_str());                    
                    
                    m_DefValueMap.erase(contract);
                }

                break;
            }    
            case fh::core::assist::common::CaseIdValue::Sdmfo_1: // case: DatSaver_Test013
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Sdmfo_1] check defMsgValue = ", defMsgValue.To_security_definition_messages_for_options_string());
                    EXPECT_STREQ("99", defMsgValue.market_segment_id.c_str());
                    EXPECT_STREQ("8$", defMsgValue.security_group.c_str());
                    EXPECT_STREQ("1489782300000000000", defMsgValue.activation_date_ime.c_str());
                    EXPECT_STREQ("1521207000000000000", defMsgValue.expiration_date_ime.c_str());
                    EXPECT_STREQ("1330000000000", defMsgValue.strike_price_mantissa.c_str());
                    EXPECT_STREQ("-7", defMsgValue.strike_price_exponent.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            case fh::core::assist::common::CaseIdValue::Sdmff_1: // case: DatSaver_Test014
            {
                auto iterDefMsg = m_DefValueMap.find(contract);
                if(iterDefMsg!=m_DefValueMap.end())
                {
                    auto defMsgValue = iterDefMsg->second;
                    LOG_DEBUG("[Sdmff_1] check defMsgValue = ", defMsgValue.To_string());
                    EXPECT_STREQ("99", defMsgValue.market_segment_id.c_str());
                    EXPECT_STREQ("91", defMsgValue.security_group.c_str());
                    EXPECT_STREQ("1466460000000000000", defMsgValue.activation_date_ime.c_str());
                    EXPECT_STREQ("1505489400000000000", defMsgValue.expiration_date_ime.c_str());
                    
                    m_DefValueMap.erase(contract);
                }
                               
                break;
            }
            case fh::core::assist::common::CaseIdValue::qty_pc_1st_instr: // case: BookManager_Test028
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    LOG_DEBUG("[qty_pc_1st_instr] check L2Vale = ", iterL2->second.c_str());
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("contract=1DVEU7, bid=[price=24135.000000, size=40][price=24130.000000, size=22][price=24125.000000, size=54][price=24120.000000, size=76], offer=[price=24155.000000, size=159][price=24160.000000, size=184][price=24165.000000, size=133][price=24170.000000, size=9]",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
                }

                break;
            } 
            case fh::core::assist::common::CaseIdValue::qty_pc_2nd_instr: // case: BookManager_Test029
            {
                auto iterL2 = m_L2ValueMap.find(contract);
                if(iterL2!=m_L2ValueMap.end())
                {
                    LOG_DEBUG("[qty_pc_2nd_instr] check L2Vale = ", iterL2->second.c_str());
                    // EXPECT_STRNE EXPECT_STREQ
                    EXPECT_STREQ("contract=0EMDU7, bid=[price=167740.000000, size=69][price=167730.000000, size=35][price=167720.000000, size=53][price=167710.000000, size=57][price=167700.000000, size=28][price=167690.000000, size=1][price=167680.000000, size=94][price=167670.000000, size=38][price=167660.000000, size=21], offer=[price=167780.000000, size=126][price=167790.000000, size=207][price=167800.000000, size=191][price=167810.000000, size=172][price=167820.000000, size=85][price=167830.000000, size=33][price=167840.000000, size=179]",
                        iterL2->second.c_str());
                        
                    m_L2ValueMap.erase(contract);
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
