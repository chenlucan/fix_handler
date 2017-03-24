
#include <quickfix/SocketAcceptor.h>
#include <quickfix/FileStore.h>
#include "server_application.h"

// base on quickfix/examples/ordermatch
int main( int argc, char** argv )
{
  std::string file = "exchange_server.cfg";

  try
  {
    FIX::SessionSettings settings( file );

    Application application;
    FIX::FileStoreFactory storeFactory( settings );
    FIX::ScreenLogFactory logFactory( settings );
    FIX::SocketAcceptor acceptor( application, storeFactory, settings, logFactory );

    acceptor.start();
    while ( true )
    {
      std::string value;
      std::cin >> value;

      if ( value == "#symbols" )
        application.orderMatcher().display();
      else if( value == "#quit" )
        break;
      else
        application.orderMatcher().display( value );

      std::cout << std::endl;
    }
    acceptor.stop();
    return 0;
  }
  catch ( std::exception & e )
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
}

// ./exchange_server_test
