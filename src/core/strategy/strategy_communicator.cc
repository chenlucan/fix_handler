
#include "core/assist/logger.h"
#include "core/assist/time_measurer.h"
#include "core/strategy/invalid_order.h"
#include "core/strategy/strategy_communicator.h"

namespace fh
{
namespace core
{
namespace strategy
{

    StrategyReceiver::StrategyReceiver(const std::string &url) : fh::core::zmq::ZmqReceiver(url)
    {
        // noop
    }

    StrategyReceiver::~StrategyReceiver()
    {
        // noop
    }

    void StrategyReceiver::Set_processor(std::function<void(char *, const size_t)> processor)
    {
        m_processor = processor;
    }

    void StrategyReceiver::Save(char *data, size_t size)
    {
        LOG_DEBUG("received: ", std::string(data, size));
        m_processor(data, size);
    }

    StrategyCommunicator::StrategyCommunicator(const std::string &send_url,const std::string &org_url, const std::string &receive_url)
    : m_org_sender(org_url), m_sender(send_url), m_receiver(receive_url), m_exchange(nullptr)
    {
        // noop
    }

    StrategyCommunicator::StrategyCommunicator(const std::string &send_url, const std::string &receive_url)
    : m_org_sender(""), m_sender(send_url), m_receiver(receive_url), m_exchange(nullptr)
    {
        // noop
    }

    StrategyCommunicator::~StrategyCommunicator()
    {
        // noop
    }

    void StrategyCommunicator::Start_receive()
    {
        m_receiver.Set_processor(std::bind(&StrategyCommunicator::On_from_strategy, this, std::placeholders::_1, std::placeholders::_2));
        m_receiver.Start_receive();
    }

    void StrategyCommunicator::Set_exchange(core::exchange::ExchangeI *exchange)
    {
        m_exchange = exchange;
    }

    void StrategyCommunicator::On_from_strategy(char *data, size_t size)
    {
        // 收到策略模块的信息后，将对应的指令发送到交易所
        LOG_INFO("received from stategy: ", std::string(data, size));

        fh::core::assist::TimeMeasurer t;
        this->Order_request(data, size);
        LOG_INFO("order processe used: ", t.Elapsed_nanoseconds(), "ns");
    }

    ::pb::ems::Order StrategyCommunicator::Create_order(const char *data, size_t size)
    {
        pb::ems::Order strategy_order;
        if(!strategy_order.ParseFromArray(data, size))
        {
            throw InvalidOrder("order parse error");
        }

        return strategy_order;
    }

    void StrategyCommunicator::Order_request(const char *data, size_t size)
    {
        ::pb::ems::Order strategy_order;
        try
        {
            // 第一个字节指示 MsgType：1:D 2:F 3:G 4:H 5:AF 6:CA
            std::uint8_t msg_type = data[0] - '1';  // 转换成 0,1,2,3,4,5
            switch(msg_type)
            {
                case 0:     // D: New Order
                {
                    strategy_order = StrategyCommunicator::Create_order(data + 1, size - 1);                    
                    
                    Check_order(strategy_order);

                    m_exchange->Add(strategy_order);
                }
                    break;
                case 1:     // F: Order Cancel Request
                {
                    // 撤单不做校验，保证撤单成功
                    strategy_order = StrategyCommunicator::Create_order(data + 1, size - 1); 
                    m_exchange->Delete(strategy_order);
                }
                    break;
               case 2:     // G: Order Cancel-Replace Request
               {
                    strategy_order = StrategyCommunicator::Create_order(data + 1, size - 1);                    
                    
                    Check_order(strategy_order);
                    
                    m_exchange->Change(strategy_order);
                }
                    break;
                case 3:     // H: Order Status Request
                {
                    strategy_order = StrategyCommunicator::Create_order(data + 1, size - 1);                    
                    
                    Check_order(strategy_order);
                    
                    m_exchange->Query(strategy_order);
                }
                    break;
                case 4:     // AF: Order Mass Status Request
                    m_exchange->Query_mass(data + 1, size - 1);
                    break;
                case 5:     // CA: Order Mass Action Request
                    m_exchange->Delete_mass(data + 1, size - 1);
                    break;
                default:
                    LOG_WARN("unknow order message type: ", msg_type);
                    break;
            }
        }
        catch(InvalidOrder &error)
        {
            LOG_ERROR("invalid order: ", error.what());
            strategy_order.set_status(pb::ems::OrderStatus::OS_Rejected);
            strategy_order.set_message(error.what());
            Reject_order(strategy_order);
        }
    }

    void StrategyCommunicator::Check_order(const ::pb::ems::Order &strategy_order)
    {
        // check ClOrderID
        if(!strategy_order.has_client_order_id())
        {
          throw fh::core::strategy::InvalidOrder("Missing field: cl_order_id");
        }
       
        // check contract
        if(!strategy_order.has_contract() )
        {
            throw fh::core::strategy::InvalidOrder("Missing field: contract");
        }
        
        // check side
        if(!strategy_order.has_buy_sell() )
        {
            throw fh::core::strategy::InvalidOrder("Missing field: side");
        }
        else if((strategy_order.buy_sell()!=pb::ems::BuySell::BS_Buy) 
            &&(strategy_order.buy_sell()!=pb::ems::BuySell::BS_Sell) )
        {
            throw fh::core::strategy::InvalidOrder("Invalid side");
        }


        // check price
        if(!strategy_order.has_price() )
        {
            throw fh::core::strategy::InvalidOrder("Missing field: price");
        }         
        
        std::string strPrice = strategy_order.price();
        if(!fh::core::assist::utility::Is_price_valid(strPrice))
        {
            throw fh::core::strategy::InvalidOrder("Invalid price");
        }
        
        // check OrderQty
        if(!strategy_order.has_quantity() )
        {
            throw fh::core::strategy::InvalidOrder("Missing field: quantity");
        }
        else if(strategy_order.quantity() == 0)
        {
            throw fh::core::strategy::InvalidOrder("Invalid OrderQty");
        }

        // check OrdType
        if(!strategy_order.has_order_type() )
        {
            throw fh::core::strategy::InvalidOrder("Missing field: order_type");
        }
        else if( (strategy_order.order_type() != pb::ems::OrderType::OT_Limit)
            && (strategy_order.order_type() != pb::ems::OrderType::OT_Market) )
        {
            throw fh::core::strategy::InvalidOrder("Invalid OrdType");
        }

        return;
    }
    
    void StrategyCommunicator::Reject_order(const ::pb::ems::Order &order)
    {
        LOG_INFO("send reject order result:  (O)", fh::core::assist::utility::Format_pb_message(order));
        m_sender.Send("O" + order.SerializeAsString());        
    }
    
    // implement of ExchangeListenerI
    void StrategyCommunicator::OnOrder(const ::pb::ems::Order &order)
    {
        LOG_INFO("send order result:  (O)", fh::core::assist::utility::Format_pb_message(order));
        m_sender.Send("O" + order.SerializeAsString());
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnFill(const ::pb::ems::Fill &fill)
    {
        LOG_INFO("send order result:  (F)", fh::core::assist::utility::Format_pb_message(fill));
        m_sender.Send("F" + fill.SerializeAsString());
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnPosition(const core::exchange::PositionVec& position)
    {

    }

    // implement of MarketListenerI
    void StrategyCommunicator::OnContractDefinition(const pb::dms::Contract &contract)
    {
        //
        LOG_INFO("send Definition: ", fh::core::assist::utility::Format_pb_message(contract));
        m_sender.Send("C" + contract.SerializeAsString());
    }

    void StrategyCommunicator::OnOrginalMessage(const std::string &message)
    {
        LOG_INFO("send Original Message, size=", message.size(), " message=", message);
        m_org_sender.Send(message);
    }		

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnExchangeReady(boost::container::flat_map<std::string, std::string>)
    {
        LOG_INFO("trade server is ready");
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnContractAuctioning(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [Auction]");
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnContractNoTrading(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [NoTrading]");
    }

    // implement of ExchangeListenerI
    void StrategyCommunicator::OnContractTrading(const std::string &contract)
    {
        LOG_INFO("contract[", contract, "] status update [Trading]");
    }

} // namespace strategy
} // namespace core
} // namespace fh
