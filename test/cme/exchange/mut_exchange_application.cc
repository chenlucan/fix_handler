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
        fix_setting_file +=CME_EXCHANGE_CLIENT_CFG;
        app_setting_file +=CME_EXCHANGE_SETTINGS_INI;
        
        fh::cme::exchange::ExchangeApplication exchangeApp(fix_setting_file, app_setting_file);
        exchangeApp.Start();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        exchangeApp.Stop();
    }
    
    // boost.asio example
    // synchronous timer
    TEST_F(MutExchangeApplication, ExchangeApplication_Test002)
    {
        boost::asio::io_service ios;  
  
        boost::asio::deadline_timer t(ios, boost::posix_time::seconds(2));  
      
        std::cout << t.expires_at() << std::endl;  
      
        t.wait();
        std::cout << "hello asio" << std::endl;
    }
    
    // asynchronous timer
    void Print(const boost::system::error_code& error)  
    {  
        std::cout << "hello asio" << std::endl;  
    }  
    
    TEST_F(MutExchangeApplication, ExchangeApplication_Test003)
    {
        boost::asio::io_service ios;  
  
        boost::asio::deadline_timer t(ios, boost::posix_time::seconds(2));  
      
        t.async_wait(Print);  
      
        std::cout << "it show before t expired." << std::endl;
        ios.run();
        std::cout<<"runned"<<std::endl;
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
    
    TEST_F(MutExchangeApplication, ExchangeApplication_Test004)
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
