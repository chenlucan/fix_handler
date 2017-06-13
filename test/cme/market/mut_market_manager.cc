#include "gmock/gmock.h"

#include <zmq.hpp>

#include "cme/market/market_manager.h"
#include "cme/market/cme_market.h"
#include "core/assist/logger.h"

#include "../../core/assist/mut_common.h"
#include "../../core/market/mock_marketlisteneri.h"

#include "mut_market_manager.h"

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
    
    void sigusr_handle(int signo)
    {
        LOG_DEBUG("signal ", signo, " received");
        pthread_t             self = pthread_self();

        LOG_DEBUG("==== sigusr_handle  self = [", self, "] =====");
                
        pthread_exit(0);
        return;
    } 

    MutMarketManager::MutMarketManager()
    {
    }

    MutMarketManager::~MutMarketManager()
    {
    }
    
    void MutMarketManager::SetUp()
    {
    }
    
    void MutMarketManager::TearDown()
    {
    }
    
    TEST_F(MutMarketManager, MarketManager_Test001)
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
                    
        if(nullptr == mock_market_listener)
        {
            return;
        }

        fh::cme::market::setting::Channel channel;
        channel.id = "360";
        channel.label = "COMEX Globex Futures";
        std::vector<fh::cme::market::setting::Channel> target_channels;        
        target_channels.push_back(channel);

        if(target_channels.empty())
        {
            // 如果没有 channel，直接终止
            LOG_WARN("all channels not found, exit");
            delete mock_market_listener;
            mock_market_listener = nullptr;
            return;
        }
        
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(app_setting_file);        
        app_setting_file +="market_settings.ini";
        
        fh::cme::market::setting::MarketSettings app_settings(app_setting_file);
        fh::cme::market::CmeMarket * cmeMarket = new CmeMarket(mock_market_listener, target_channels, app_settings);
        
        if(nullptr == cmeMarket)
        {
            delete mock_market_listener;
            mock_market_listener = nullptr;
            return;
        }
        
        delete cmeMarket;
        cmeMarket = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    
    
    TEST_F(MutMarketManager, MarketManager_Test002)
    {
        fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
        mock_market_listener = new fh::core::market::MockMarketListenerI();        
                    
        if(nullptr == mock_market_listener)
        {
            return;
        }

        fh::cme::market::setting::Channel channel;
        channel.id = "360";
        channel.label = "COMEX Globex Futures";
        std::vector<fh::cme::market::setting::Channel> target_channels;        
        target_channels.push_back(channel);

        if(target_channels.empty())
        {
            // 如果没有 channel，直接终止
            LOG_WARN("all channels not found, exit");
            delete mock_market_listener;
            mock_market_listener = nullptr;
            return;
        }
        
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(app_setting_file);        
        app_setting_file +="market_settings.ini";
        
        fh::cme::market::setting::MarketSettings app_settings(app_setting_file);
        fh::cme::market::CmeMarket * cmeMarket = new CmeMarket(mock_market_listener, target_channels, app_settings);
        
        if(nullptr == cmeMarket)
        {
            delete mock_market_listener;
            mock_market_listener = nullptr;
            return;
        }
        
        // cmeMarket->Start();
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        // cmeMarket->Stop(); // 有exit(0)，不能调用
        delete cmeMarket;
        cmeMarket = nullptr;
        
        delete mock_market_listener;
        mock_market_listener = nullptr;
        
        bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
        ASSERT_TRUE(bRet);
    }
    
    TEST_F(MutMarketManager, MarketManager_Test003)
    {    
        try
        {
            fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
            mock_market_listener = new fh::core::market::MockMarketListenerI();        
                        
            if(nullptr == mock_market_listener)
            {
                return;
            }

            fh::cme::market::setting::Channel channel;
            channel.id = "360";
            channel.label = "COMEX Globex Futures";
            std::vector<fh::cme::market::setting::Channel> target_channels;        
            target_channels.push_back(channel);

            if(target_channels.empty())
            {
                // 如果没有 channel，直接终止
                LOG_WARN("all channels not found, exit");
                delete mock_market_listener;
                mock_market_listener = nullptr;
                return;
            }
            
            std::string app_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);        
            app_setting_file +="market_settings_exception.ini";
            //  [ERROR] [mut_market_manager.cc(197)    ] Exception: ./bin/market_settings_exception.ini: cannot open file

            
            fh::cme::market::setting::MarketSettings app_settings(app_setting_file);
            fh::cme::market::CmeMarket * cmeMarket = new CmeMarket(mock_market_listener, target_channels, app_settings);
            
            if(nullptr == cmeMarket)
            {
                delete mock_market_listener;
                mock_market_listener = nullptr;
                return;
            }
            
            delete cmeMarket;
            cmeMarket = nullptr;
            
            delete mock_market_listener;
            mock_market_listener = nullptr;
            
            bool bRet = Mock::VerifyAndClearExpectations(mock_market_listener);
            ASSERT_TRUE(bRet);
        }
        catch (std::exception& e)
        {
            LOG_ERROR("Exception: ", e.what());
        }
    }
    
    // Exception: <unspecified file>: cannot open file
    TEST_F(MutMarketManager, MarketManager_Test004)
    {    
        try
        {
            fh::cme::market::setting::MarketSettings app_settings("");
            std::pair<std::string, std::string> auth = app_settings.Get_auth();
        }
        catch (std::exception& e)
        {
            LOG_ERROR("Exception: ", e.what());
        }
    }

    void  Sig_hand_test(int signo)
    {
        LOG_DEBUG("***************** [Sig_hand_test] signal=[", signo, "] received *****************");
        pthread_t             self = pthread_self();

        LOG_DEBUG("***************** Sig_hand_test self = [", (void *)self, "] *****************");
        
        pthread_exit(0);
        return;
    }

    void Callback_market(const std::string &channel_id)
    {
        LOG_DEBUG("===== Callback_market =====");
    }
    #if 1
    TEST_F(MutMarketManager, MarketManager_Test005)
    {    
        try
        {
            struct sigaction        sa_usr;
            memset(&sa_usr, 0, sizeof(sa_usr));
            sigemptyset(&sa_usr.sa_mask);
            sa_usr.sa_flags = 0;
            sa_usr.sa_handler = Sig_hand_test;

            int rc = sigaction(SIGUSR1, &sa_usr, NULL);
            LOG_DEBUG("===== sigaction rc=[", rc, "] =====");

            std::string app_setting_file;
            std::string channel_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);
            channel_setting_file = app_setting_file;      
            app_setting_file +="market_settings.ini";
            channel_setting_file += "market_config.xml";
            
            std::string channel_id;            
            std::vector<fh::cme::market::setting::Channel> target_channels = fh::core::assist::common::Get_target_channels(channel_id, channel_setting_file);        
            
            if(target_channels.empty())
            {
                // 如果没有 channel，直接终止
                LOG_WARN("all channels not found, exit");
                return;
            }

            auto iterChannel = target_channels.cbegin();
            

            fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
            mock_market_listener = new fh::core::market::MockMarketListenerI();        
                        
            if(nullptr == mock_market_listener)
            {
                return;
            }   

            fh::cme::market::setting::MarketSettings app_settings(app_setting_file);

            MarketManager *market = new MarketManager(
                                std::bind(&Callback_market, std::placeholders::_1),
                                mock_market_listener, *iterChannel, app_settings);

            if(nullptr == market)
            {
                delete mock_market_listener;
                mock_market_listener = nullptr;
                return;
            }

            // logic 
            market->Start();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            market->Stop();

            delete market;
            market = nullptr;

            delete mock_market_listener;
            mock_market_listener = nullptr;
        }
        catch (std::exception& e)
        {
            LOG_ERROR("Exception: ", e.what());
        }
    }

    TEST_F(MutMarketManager, MarketManager_Test006)
    {    
        try
        {
            std::string app_setting_file;
            std::string channel_setting_file;
            fh::core::assist::common::getAbsolutePath(app_setting_file);
            channel_setting_file = app_setting_file;      
            app_setting_file +="market_settings.ini";
            channel_setting_file += "market_config.xml";
            
            std::string channel_id;            
            std::vector<fh::cme::market::setting::Channel> target_channels = fh::core::assist::common::Get_target_channels(channel_id, channel_setting_file);        
            
            if(target_channels.empty())
            {
                // 如果没有 channel，直接终止
                LOG_WARN("all channels not found, exit");
                return;
            }

            auto iterChannel = target_channels.cbegin();
            

            fh::core::market::MockMarketListenerI *mock_market_listener = nullptr; 
            mock_market_listener = new fh::core::market::MockMarketListenerI();        
                        
            if(nullptr == mock_market_listener)
            {
                return;
            }   

            fh::cme::market::setting::MarketSettings app_settings(app_setting_file);

            MarketManager *market = new MarketManager(
                                std::bind(&Callback_market, std::placeholders::_1),
                                mock_market_listener, *iterChannel, app_settings);

            if(nullptr == market)
            {
                delete mock_market_listener;
                mock_market_listener = nullptr;
                return;
            }

            // logic 
            market->Start();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            market->Stop();

            delete market;
            market = nullptr;

            delete mock_market_listener;
            mock_market_listener = nullptr;
        }
        catch (std::exception& e)
        {
            LOG_ERROR("Exception: ", e.what());
        }
    }   
    #else
    // thread kill
    std::thread *ptid_udp_incrementals = nullptr;
    int counter = 0;

    static void on_signal_term(int sig){
        std::cout << "on SIGTERM:" << std::this_thread::get_id() << std::endl;
        pthread_exit(NULL); 
    }

    void threadPosixKill(void)
    {
        signal(SIGTERM, on_signal_term);
        signal(SIGINT,  on_signal_term);// kill 'n' to quit 

        ptid_udp_incrementals = new std::thread( [](){
        while(true){
            ++counter;
            std::cout << "counter=" << counter << std::endl;
        }
        });
         
        if(!ptid_udp_incrementals)
        {
            return;
        }

        pthread_t tid = ptid_udp_incrementals->native_handle();
        // pthread_t tid;
        std::cout << "tid=" << tid << std::endl;
        // 确保子线程已经在运行。
        //std::this_thread::sleep_for( std::chrono::seconds(10) );
        std::this_thread::sleep_for( std::chrono::nanoseconds(100) );

        pthread_kill(tid, SIGTERM);
        ptid_udp_incrementals->join();
        delete ptid_udp_incrementals;
        ptid_udp_incrementals = nullptr;
        std::cout << "thread destroyed." << std::endl;
    }

    TEST_F(MutMarketManager, MarketManager_Test007)
    {
        //threadPosixKill();
    }
    
    // kill multithread
    void threadfunc(void *parm)
    {
        LOG_DEBUG("Worker thread is start.!");
        // pthread_t             self = pthread_self();
        // pthread_id_np_t       tid;
        // int                   rc = 0;

        // pthread_getunique_np(&self, &tid);
        // printf("Thread 0x%.8x %.8x entered\n", tid);
        // int errno = 0;
        // rc = sleep(30);
        // std::this_thread::sleep_for( std::chrono::seconds(100) );
        // if (rc != 0 && errno == EINTR) {
            // printf("Thread 0x%.8x %.8x got a signal delivered to it\n",
                // tid);
            // return NULL;
        // }
        // printf("Thread 0x%.8x %.8x did not get expected results! rc=%d, errno=%d\n",
              // tid, rc, errno);
        while(1)
        {
            LOG_DEBUG("LOOP\n");
            std::this_thread::sleep_for( std::chrono::seconds(1) );
        }
        LOG_DEBUG("Worker thread is ending.!");
        return;
    }
    
    #define NUMTHREADS (3)
    TEST_F(MutMarketManager, MarketManager_Test008)
    {
        int                     rc;        
        pthread_t               threads[NUMTHREADS];
        std::thread *worker_thread[NUMTHREADS];

        printf("Set up the alarm handler for the process\n");
        struct sigaction        old_sa;
        struct sigaction        sa_usr;
        memset(&old_sa, 0, sizeof(old_sa));
        memset(&sa_usr, 0, sizeof(sa_usr));
        sigemptyset(&sa_usr.sa_mask);
        //sa_usr.sa_flags = 0;
        sa_usr.sa_flags=SA_SIGINFO;//表示使用sa_sigaction指示的函数，处理完恢复默认，不阻塞处理过程中到达下在被处理的信号
        sa_usr.sa_handler = sigusr_handle;

        // SIGUSR1
        // rc = sigaction(SIGALRM, &sa_usr, NULL);
        rc = sigaction(SIGUSR1, &sa_usr, &old_sa);  // 主线程执行一次     
        
        for(int i=0; i<NUMTHREADS; i++)
        {
            worker_thread[i] = new std::thread(threadfunc, (void*)NULL);
            if(worker_thread[i])
            {
                threads[i] = worker_thread[i]->native_handle();

                // std::cout << "threads[i]=" << threads[i] << std::endl;
                LOG_DEBUG("===== threads[i]= [", threads[i], "] =====");
            }            
        }
     

        // 确保子线程已经在运行。            
        //std::this_thread::sleep_for( std::chrono::nanoseconds(100) ); 
        std::this_thread::sleep_for( std::chrono::seconds(3) );

        for(int i=0; i<NUMTHREADS; i++)
        {
            //rc = pthread_kill(threads[i], SIGALRM);
            rc = pthread_kill(threads[i], SIGUSR1);

            if(rc == ESRCH)
            {
                LOG_DEBUG("[1]指定的线程不存在或者是已经终止\n"); 
            } 
            else if(rc == EINVAL)
            {  
                LOG_DEBUG("[2]调用传递一个无用的信号\n");  
            }
            else  
            {
                LOG_DEBUG("[3]线程存在\n"); 
            }
        }

        for(int i=0; i<NUMTHREADS; i++)
        {
            rc = pthread_join(threads[i], NULL);            
            LOG_DEBUG("pthread_ioin() rc= ", rc);            
        }

        LOG_DEBUG("Main completed\n");     

        return;
    }

    TEST_F(MutMarketManager, MarketManager_Test009)
    {
        int                     rc;        
        pthread_t               threads[NUMTHREADS];
        std::thread *worker_thread[NUMTHREADS];

        printf("Set up the alarm handler for the process\n");
        struct sigaction        sa_usr;
        memset(&sa_usr, 0, sizeof(sa_usr));
        sigemptyset(&sa_usr.sa_mask);
        //sa_usr.sa_flags = 0;
        sa_usr.sa_flags=SA_SIGINFO;//表示使用sa_sigaction指示的函数，处理完恢复默认，不阻塞处理过程中到达下在被处理的信号
        sa_usr.sa_handler = sigusr_handle;

        // SIGUSR1
        // rc = sigaction(SIGALRM, &sa_usr, NULL);
        //rc = sigaction(SIGUSR1, &sa_usr, NULL);
       
        
        for(int i=0; i<NUMTHREADS; i++)
        {
            worker_thread[i] = new std::thread(threadfunc, (void*)NULL);
            if(worker_thread[i])
            {
                threads[i] = worker_thread[i]->native_handle();

                // std::cout << "threads[i]=" << threads[i] << std::endl;
                LOG_DEBUG("===== threads[i]= [", threads[i], "] =====");
            }            
        }
     

        // 确保子线程已经在运行。            
        //std::this_thread::sleep_for( std::chrono::nanoseconds(100) ); 
        std::this_thread::sleep_for( std::chrono::seconds(3) );

        for(int i=0; i<NUMTHREADS; i++)
        {
            //rc = pthread_kill(threads[i], SIGALRM);
            rc = pthread_kill(threads[i], SIGUSR1);

            if(rc == ESRCH)
            {
                LOG_DEBUG("[1]指定的线程不存在或者是已经终止\n"); 
            } 
            else if(rc == EINVAL)
            {  
                LOG_DEBUG("[2]调用传递一个无用的信号\n");  
            }
            else  
            {
                LOG_DEBUG("[3]线程存在\n"); 
            }
        }

        for(int i=0; i<NUMTHREADS; i++)
        {
            //rc = pthread_join(threads[i], NULL);            
            //LOG_DEBUG("pthread_ioin() rc= ", rc);
            worker_thread[i]->join();            
        }

        for(int i=0; i<NUMTHREADS; i++)
        {
            delete worker_thread[i];
            worker_thread[i] = nullptr;
        }

        LOG_DEBUG("Main completed\n");     

        return;
    }

    TEST_F(MutMarketManager, MarketManager_Test009)
    {
        int                     rc;        
        pthread_t               threads[NUMTHREADS];
        std::thread *worker_thread[NUMTHREADS];

        printf("Set up the alarm handler for the process\n");
        struct sigaction        sa_usr;
        memset(&sa_usr, 0, sizeof(sa_usr));
        sigemptyset(&sa_usr.sa_mask);
        sa_usr.sa_flags = 0;
        //sa_usr.sa_flags=SA_SIGINFO;//表示使用sa_sigaction指示的函数，处理完恢复默认，不阻塞处理过程中到达下在被处理的信号
        sa_usr.sa_handler = sigusr_handle;

        // SIGUSR1
        //rc = sigaction(SIGALRM, &sa_usr, NULL);
        //rc = sigaction(SIGUSR1, &sa_usr, NULL);
        
        for(int i=0; i<NUMTHREADS; i++)
        {
            worker_thread[i] = new std::thread(threadfunc, (void*)NULL);
            if(worker_thread[i])
            {
                threads[i] = worker_thread[i]->native_handle();

                // std::cout << "threads[i]=" << threads[i] << std::endl;
                LOG_DEBUG("===== threads[i]= [", threads[i], "] =====");
            }            
        }
     

        // 确保子线程已经在运行。            
        //std::this_thread::sleep_for( std::chrono::nanoseconds(100) ); 
        std::this_thread::sleep_for( std::chrono::seconds(3) );

        for(int i=0; i<NUMTHREADS; i++)
        {
            rc = pthread_kill(threads[i], SIGUSR1);

            if(rc == ESRCH)
            {
                LOG_DEBUG("[1]指定的线程不存在或者是已经终止\n"); 
            } 
            else if(rc == EINVAL)
            {  
                LOG_DEBUG("[2]调用传递一个无用的信号\n");  
            }
            else  
            {
                LOG_DEBUG("[3]线程存在\n"); 
            }
        }

        for(int i=0; i<NUMTHREADS; i++)
        {
            //rc = pthread_join(threads[i], NULL);            
            //LOG_DEBUG("pthread_ioin() rc= ", rc);
            worker_thread[i]->join();            
        }

        for(int i=0; i<NUMTHREADS; i++)
        {
            delete worker_thread[i];
            worker_thread[i] = nullptr;
        }

        LOG_DEBUG("Main completed\n");     

        return;
    }
    #endif
    
    #if 1
    ::zmq::context_t g_context(1);
    ::zmq::socket_t g_receiver(g_context, ZMQ_PULL);
    
    // test zmqReceiver active stop
    void recvThreadfunc(void *parm)
    {
        LOG_DEBUG("recvThreadfunc is start.!");
        
        while(1)
        {
            LOG_DEBUG("LOOP");
            ::zmq::message_t message;
            g_receiver.recv(&message);
            //std::this_thread::sleep_for( std::chrono::seconds(1) );
        }
        LOG_DEBUG("recvThreadfunc is ending.!");
        return;
    }
    
    
    TEST_F(MutMarketManager, MarketManager_Test010)
    {
        std::string url("tcp://localhost:5557");
        g_receiver.connect(url);
        
        int                     rc;        
        pthread_t               tid;
        std::thread *worker_thread = nullptr;
        
        worker_thread = new std::thread(recvThreadfunc, (void*)NULL);
        if(worker_thread)
        {
            tid = worker_thread->native_handle();

            LOG_DEBUG("===== tid= [", (void *)tid, "] =====");
            
            // 确保子线程已经在运行。            
            //std::this_thread::sleep_for( std::chrono::nanoseconds(100) ); 
            std::this_thread::sleep_for( std::chrono::seconds(3) );

            rc = pthread_kill(tid, SIGUSR1);

            if(rc == ESRCH)
            {
                LOG_DEBUG("[1]指定的线程不存在或者是已经终止"); 
            } 
            else if(rc == EINVAL)
            {  
                LOG_DEBUG("[2]调用传递一个无用的信号");
            }
            else  
            {
                LOG_DEBUG("[3]线程存在"); 
            }
            
            worker_thread->join();

            delete worker_thread;
            worker_thread = nullptr;
        }
        
        g_receiver.close();
        g_context.close();
        
        LOG_DEBUG("Main completed\n");
    }
    #endif

} // namespace market
} // namespace cme
} // namespace fh
