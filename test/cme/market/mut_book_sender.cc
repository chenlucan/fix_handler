
#include "core/assist/logger.h"
#include "pb/dms/dms.pb.h"
#include "cme/market/message/instrument.h"

#include "cme/market/book_sender.h"

#include "../../core/market/mock_marketi.h"

#include "mut_book_sender.h"

namespace fh
{
namespace cme
{
namespace market
{

    MutBookSender::MutBookSender()
    {
        // noop
    }

    MutBookSender::~MutBookSender()
    {
        // noop
    }

    void MutBookSender::OnMarketDisconnect_Test001()
    {
        BookSender *book_sender = nullptr;
        book_sender = new BookSender("tcp://*:5557", "tcp://*:5558");        
        if(book_sender!=nullptr)
        {
             delete book_sender;
            book_sender = nullptr;
        }        
    }
        
    //BookSender::OnMarketDisconnect case
    TEST_F(MutBookSender, OnMarketDisconnect_Test001) 
    {
        OnMarketDisconnect_Test001();
    }
    
    // BookSender::OnContractDefinition case
    TEST_F(MutBookSender, OnContractDefinition_Test001) 
    {
        BookSender *book_sender = nullptr;
        book_sender = new BookSender("tcp://*:5557", "tcp://*:5558");        
        if(book_sender!=nullptr)
        {
            pb::dms::Contract contract;
            fh::cme::market::message::Instrument instrument;
            contract.set_name(instrument.symbol);            
            book_sender->OnContractDefinition(contract);
            delete book_sender;
            book_sender = nullptr;
        }
    }

} // namespace market
} // namespace cme
} // namespace fh
