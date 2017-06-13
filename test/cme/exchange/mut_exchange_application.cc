#include <thread>
#include "cme/exchange/exchange_application.h"
#include "cme/exchange/exchange_settings.h"
#include "core/assist/logger.h"

#include "../../core/assist/mut_common.h"
#include "mut_exchange_application.h"

#include <boost/asio.hpp> 
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>  
#include <boost/function.hpp> 

// #include <boost/thread.hpp>

#include <sys/inotify.h>

namespace fh
{
namespace cme
{
namespace exchange
{
    MutExchangeApplication::MutExchangeApplication()
    {
    }
    
    MutExchangeApplication::~MutExchangeApplication()
    {
        // noop
    }
    
    void MutExchangeApplication::SetUp()
    {
    }
    
    void MutExchangeApplication::TearDown()
    {
    }
    
    TEST_F(MutExchangeApplication, ExchangeApplication_Test001)
    {
        std::string fix_setting_file;
        std::string app_setting_file;
        fh::core::assist::common::getAbsolutePath(fix_setting_file);   
        app_setting_file = fix_setting_file;     
        fix_setting_file +="exchange_client.cfg";
        app_setting_file +="exchange_settings.ini";
        
        fh::cme::exchange::ExchangeApplication exchangeApp(fix_setting_file, app_setting_file);
        exchangeApp.Start();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        exchangeApp.Stop();
    }
    
    // boost.asio example
    // synchronous timer
    TEST_F(MutExchangeApplication, ExchangeApplication_Test002)
    {
        boost::asio::io_service ios;    // 所有的asio程序必须要有一个io_service对象  
  
        // 定时器io_service作为构造函数参数，两秒钟之后定时器终止  
        boost::asio::deadline_timer t(ios, boost::posix_time::seconds(2));  
      
        std::cout << t.expires_at() << std::endl; // 查看终止的绝对事件  
      
        t.wait();                       // 调用wait同步等待  
        std::cout << "hello asio" << std::endl;
        //可以把它与thread库的sleep()函数对比研究一下，两种虽然都是等待，但内部机制完全不同：thread库的sleep()使用了互斥量和条件变量，在线程中等待，而asio则是调用了操作系统的异步机制，如select，epool等完成。
        //同步定时器的用法很简单，但它演示了asio程序的基本结构和流程：一个asio程序首先要定义一个io_service对象，它是前摄器模式中最重的proactor角色，然后我们声明一个IO操作(这里是定时器),并把它挂接在io_service
    }
    
    // asynchronous timer
    // 代码大致与同步定时器相等，增加了回调函数，并使用io_service.run()和定时器的async_wait()方法
    void Print(const boost::system::error_code& error)  
    {  
        std::cout << "hello asio" << std::endl;  
    }  
    
    TEST_F(MutExchangeApplication, ExchangeApplication_Test003)
    {
        boost::asio::io_service ios;    // 所有的asio程序必须要有一个io_service对象  
  
        // 定时器io_service作为构造函数参数，两秒钟之后定时器终止  
        boost::asio::deadline_timer t(ios, boost::posix_time::seconds(2));  
      
        t.async_wait(Print);   // 调用wait异步等待，传入回调函数，立即返回  
      
        std::cout << "it show before t expired." << std::endl;
        ios.run();   //很重要，异步IO必须   
        std::cout<<"runned"<<std::endl;//将与hello asio一起输出，说明run()是阻塞函数
        //调用io_service的run()成员函数，它启动前摄器的事件处理循环，阻塞等待所有的操作完成并分派事件。
        //如果不调用run()那么虽然操作被异步执行了，但没有一个等待它完成的机制，回调函数将得不到执行机会。
    }
    
    void start_notify_handler();
    void start_accept_handler();

    // this stuff goes into your class, only global for the simplistic demo
    boost::asio::streambuf buf(1024);
    boost::asio::io_service io_svc;
    boost::asio::posix::stream_descriptor stream_desc(io_svc);
    boost::asio::ip::tcp::socket sock(io_svc);
    boost::asio::ip::tcp::endpoint end(boost::asio::ip::tcp::v4(), 1234);
    boost::asio::ip::tcp::acceptor acceptor(io_svc, end);

    // this gets called on file system activity
    void notify_handler(const boost::system::error_code&,
                        std::size_t transferred)
    {
        size_t processed = 0;
        while(transferred - processed >= sizeof(inotify_event))
        {
            const char* cdata = processed
                                + boost::asio::buffer_cast<const char*>(buf.data());
            const inotify_event* ievent =
                                     reinterpret_cast<const inotify_event*>(cdata);
            processed += sizeof(inotify_event) + ievent->len;
            if(ievent->len > 0 && ievent->mask & IN_OPEN)
                std::cout << "Someone opened " << ievent->name << '\n';
        }
        start_notify_handler();
    }

    // this gets called when nsomeone connects to you on TCP port 1234
    void accept_handler(const boost::system::error_code&)
    {
        std::cout << "Someone connected from " 
                  << sock.remote_endpoint().address() << '\n';
        sock.close(); // dropping connection: this is just a demo
        start_accept_handler();
    }

    void start_notify_handler()
    {
        stream_desc.async_read_some( buf.prepare(buf.max_size()),
            boost::bind(&notify_handler, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

    void start_accept_handler()
    {
        acceptor.async_accept(sock,
            boost::bind(&accept_handler, boost::asio::placeholders::error));
    }
    // 异步定时器使用bind，async_wait()接受的回调函数类型是固定的，必须使用bind库来绑定参数以适配它的接口
    TEST_F(MutExchangeApplication, ExchangeApplication_Test004)
    {
        // int raw_fd = inotify_init(); // error handling ignored
        // stream_desc.assign(raw_fd);
        // inotify_add_watch(raw_fd, ".", IN_OPEN);
        // start_notify_handler();
        // start_accept_handler();
        // io_svc.run();
        
        
        // boost::asio::io_service io_service;
        // boost::asio::ip::udp::socket udp_socket(io_service);
        
        // boost::asio::ip::udp::endpoint local_add(boost::asio::ip::address::from_string("192.168.0.141"), 2000);
        // //给scoket对象绑定local_add对象
        // udp_socket.open(local_add.protocol());
        // udp_socket.bind(local_add);
        
        // udp_socket.cancel();        
        // io_service.stop();
    }
      


    class handler
    {
    public:
      handler(boost::asio::io_service& io)
        : m_strand(io),
          m_timer1(io, boost::posix_time::seconds(1)),
          m_timer2(io, boost::posix_time::seconds(1)),
          m_count(0)
      {
        m_timer1.async_wait(m_strand.wrap(boost::bind(&handler::message1, this)));
        m_timer2.async_wait(m_strand.wrap(boost::bind(&handler::message2, this)));
      }

      ~handler()
      {
        std::cout << "The last count : " << m_count << "\n";
      }

      void message1()
      {
        if (m_count < 10)
        {
          std::cout << "Timer 1: " << m_count << "\n";
          ++m_count;

          m_timer1.expires_at(m_timer1.expires_at() + boost::posix_time::seconds(1));
          m_timer1.async_wait(m_strand.wrap(boost::bind(&handler::message1, this)));
        }
      }

      void message2()
      {
        if (m_count < 10)
        {
          std::cout << "Timer 2: " << m_count << "\n";
          ++m_count;

          m_timer2.expires_at(m_timer2.expires_at() + boost::posix_time::seconds(1));
          m_timer2.async_wait(m_strand.wrap(boost::bind(&handler::message2, this)));
        }
      }

    private:
      boost::asio::strand m_strand;
      boost::asio::deadline_timer m_timer1;
      boost::asio::deadline_timer m_timer2;
      int m_count;
    };      
    
    TEST_F(MutExchangeApplication, ExchangeApplication_Test005)
    {
        boost::asio::io_service io;
        handler h(io);
        std::thread t(boost::bind(&boost::asio::io_service::run, &io));
        io.run();
        t.join();
    }
    
} // namespace exchange
} // namespace cme
} // namespace fh
