
#include "core/assist/logger.h"
#include "pb/dms/dms.pb.h"
#include "cme/market/message/instrument.h"

#include "core/book/book_sender.h"

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
        fh::core::book::BookSender *book_sender = nullptr;
        //book_sender = new BookSender("tcp://*:5557", "tcp://*:5558");      
        book_sender = new fh::core::book::BookSender("tcp://*:2000", "tcp://*:2001");             
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
    
    // BookSender::OnContractDefinition case, desc:check const parameter after it is called 
    TEST_F(MutBookSender, OnContractDefinition_Test001) 
    {
        fh::core::book::BookSender *book_sender = nullptr;
        //book_sender = new BookSender("tcp://*:5557", "tcp://*:5558");     
        book_sender = new fh::core::book::BookSender("tcp://*:2000", "tcp://*:2001");         
        if(book_sender!=nullptr)
        {
            pb::dms::Contract contract;
            fh::cme::market::message::Instrument instrument;
            contract.set_name(instrument.symbol);
            auto contractTypeValue = ::pb::dms::ContractType::CT_Futures;
            contract.set_contract_type(contractTypeValue);
            book_sender->OnContractDefinition(contract);
            delete book_sender;
            book_sender = nullptr;
            ASSERT_EQ(contract.contract_type(), ::pb::dms::ContractType::CT_Futures);
        }
    }
    
    // BookSender::OnBBO case
    TEST_F(MutBookSender, OnBBO_Test001) 
    {
        fh::core::book::BookSender *book_sender = nullptr;
        //book_sender = new BookSender("tcp://*:5557", "tcp://*:5558"); 
        book_sender = new fh::core::book::BookSender("tcp://*:2000", "tcp://*:2001"); 
        if(book_sender!=nullptr)
        {
            pb::dms::BBO bbo_info;
            fh::cme::market::message::Instrument instrument;
            std::string symbol = "bbo_info_symbol_12345";
            bbo_info.set_contract(symbol);
            book_sender->OnBBO(bbo_info);
            delete book_sender;
            book_sender = nullptr;
        }
    }

} // namespace market
} // namespace cme
} // namespace fh
