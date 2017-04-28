
#include <thread>
#include <chrono>
#include "core/assist/logger.h"
#include "core/assist/utility.h"
#include "tmalpha/market/market_simulater.h"

namespace fh
{
namespace tmalpha
{
namespace market
{

    MarketSimulater::MarketSimulater(DataProvider *provider, DataConsumer *replayer)
    : m_provider(provider), m_replayer(replayer), m_messages(),
      m_is_fetch_end(false), m_is_stopped(true), m_mutex(), m_condition(), m_speed(1)
    {
        // noop
    }

    MarketSimulater::~MarketSimulater()
    {
        // noop
    }

    void MarketSimulater::Add_replay_listener(fh::tmalpha::market::MarketReplayListener *replay_listener)
    {
        m_replay_listeners.push_back(replay_listener);
    }

    void MarketSimulater::Speed(float speed)
    {
        m_speed = speed <= 0 ? 1 : speed;
    }

    void MarketSimulater::Start()
    {
        // 先启动重放（读取数据）线程
        m_is_stopped = false;
        std::thread replayer([this](){this->Replay();});

        // 开始读取数据
        std::uint64_t total = m_provider->Total_count();
        LOG_INFO("Start. There are ", total, " messages waiting for replay.");

        std::uint64_t last_message = 0;
        while(!m_is_stopped)
        {
            // 先检索出一拨数据
            std::vector<std::string> messages;
            std::uint64_t count = m_provider->Query(messages, last_message);
            LOG_INFO("pick messages count: ", count);
            if(count == 0)
            {
                // 没有数据了，退出，并等待重放线程结束
                LOG_INFO("all messages fetched.");
                m_is_fetch_end = true;
                break;
            }

            // 将数据保存起来，并保存最后一条消息的识别 ID，下次的检索就从这条数据以下开始
            std::unique_lock<std::mutex> locker(m_mutex);
            for(std::string &m : messages) { m_messages.push(std::move(m)); }
            last_message = m_provider->Message_identify(m_messages.back());

            // 然后通知重放线程开始读取，并等待重放线程通知（当数据不足规定条数时，重放线程会通知本线程去继续获取数据）
            m_condition.notify_all();
            m_condition.wait(locker);
        }

        // 最后通知下重放线程（防止此时它还在等待数据）
        m_condition.notify_all();
        replayer.join();

        LOG_INFO("Exit.");
    }

    void MarketSimulater::Stop()
    {
        LOG_INFO("replay is stopped.");
        m_is_stopped = true;
    }

    bool MarketSimulater::Is_runing() const
    {
        return !m_is_stopped;
    }

    void MarketSimulater::Replay()
    {
        // 上一条 message 的实际交易所发送的时间
        std::uint64_t last_send_time = 0;
        // 上一条 message 的重放时的发送的时间
        std::uint64_t last_replay_time = 0;

        while(!m_is_stopped)
        {
            std::unique_lock<std::mutex> locker(m_mutex);
            if(!m_messages.empty())
            {
                // 获取马上要发送的 message 的实际交易所发送的时间
                std::uint64_t send_time= m_provider->Message_send_time(m_messages.front());

                // 根据上一条消息的交易所发送时间和本次消息的交易所发送时间，以及上一条消息的重放时间，以及重放速率
                // 计算出需要休眠多长时间，以保证重放和实际交易所发送按照同样地频率进行
                this->Sleep(last_send_time, send_time, last_replay_time);

                // 上一条消息的交易所发送时间和本次消息的交易所发送时间一样的场合是不需要重置 last_replay_time 的（为了重放间隔的精度）
                if(send_time != last_send_time)
                {
                    last_replay_time = fh::core::assist::utility::Current_time_ns();
                    last_send_time = send_time;
                }

                // 处理并删除下一条 message
                this->Consume_one();

                // 如果剩下的消息数量不足了，通知 provider 去拉取新的数据
                if(m_messages.size() < 100) m_condition.notify_all();

                // 发送最新行情状态
                this->Show_states();
            }
            else if(m_is_fetch_end)
            {
                // 所有的消息都处理完了
                LOG_INFO("all messages replayed");
                return;
            }
            else
            {
                // 保存的消息都处理完了，等待 provider 拉取到新的数据后唤醒自己
                m_condition.wait(locker);
            }
        }
    }

    void MarketSimulater::Consume_one()
    {
        LOG_DEBUG("consume: ", m_messages.front());
        m_replayer->Consume(m_messages.front());
        m_messages.pop();
    }

    void MarketSimulater::Show_states()
    {
        std::unordered_map<std::string , pb::dms::L2> states = m_replayer->Get_state();
        for(auto *listener : m_replay_listeners) { listener->On_state_changed(states); }
    }

    void MarketSimulater::Sleep(std::uint64_t last_send_time, std::uint64_t current_send_time, std::uint64_t last_replay_time) const
    {
        // 第一条 message 发送前无需等待
        if(last_send_time == 0) return;

        // 上一条的交易所发送时间和本条的交易所发送时间一样或者更靠前，
        // 说明是同一次发送出来的多条消息或者是不同的端口之前来的消息，无需等待
        if(current_send_time <= last_send_time) return;

        // 根据重放速率计算出下次重放应该经过的时间间隔
        std::uint64_t interval = (std::uint64_t)((current_send_time - last_send_time) / m_speed);

        // 根据上一条 message 的实际重放时间，加上计算出的发送间隔，算出下一次应该重放的时间
        std::uint64_t next_replay_time = last_replay_time + interval;

        // 等待到该时间点后返回（如果当前时间已经过了上面计算出的应该重放时间，无需等待）
        std::this_thread::sleep_until(std::chrono::time_point<std::chrono::system_clock>(std::chrono::nanoseconds(next_replay_time)));
    }

}   // namespace market
}   // namespace tmalpha
}   // namespace fh
