
#include "book_manager.h"
#include "time_measurer.h"
#include "logger.h"

namespace rczg
{

	BookManager::BookManager()
	: m_instruments(), m_recovery_books(), m_recovery_wait_merge(m_recovery_books.cbegin()),
	  m_parser_d(), m_parser_f(), m_parser_r(), m_parser_x(), m_parser_w(),
	  m_book_state_controller()
	{
		// noop
	}

	BookManager::~BookManager()
	{
		// noop
	}

	// set received definition messages
    void BookManager::Set_definition_data(std::vector<rczg::MdpMessage> *definition_datas)
    {
    	this->Parse_definition(*definition_datas);
    }

    // set received recovery messages
    void BookManager::Set_recovery_data(std::vector<rczg::MdpMessage> *recovery_datas)
    {
    	this->Parse_recovery(*recovery_datas);
    }

    // parse message to books and send to target zeromq
    void BookManager::Parse_to_send(const rczg::MdpMessage &message, rczg::ZmqSender &sender)
    {
    	TimeMeasurer t;

    	std::vector<Book> increment_books = this->Parse_increment(message);
    	std::vector<Book> merged_books = this->Merge_with_recovery(message.packet_seq_num(), increment_books);

        LOG_INFO("parse to books: ", t.Elapsed_nanoseconds(), "ns, count=", merged_books.size());

    	std::for_each(merged_books.cbegin(), merged_books.cend(), [this, &sender, &t](const Book &b){
    		if(m_book_state_controller.Modify_state(b))
    		{
    			sender.Send(m_book_state_controller.Get());
    			LOG_INFO("send to zmq(book state): ", t.Elapsed_nanoseconds(), "ns");
    		}
    	});

    	LOG_INFO("book parsed.");
    }

    void BookManager::Parse_definition(const std::vector<rczg::MdpMessage> &messages)
    {
		// parse definition data, 35=d
		std::for_each(messages.cbegin(), messages.cend(),
				[this](const rczg::MdpMessage &message){
					this->Update_definition(message);
				}
		);
    }

    void BookManager::Parse_recovery(const std::vector<rczg::MdpMessage> &messages)
    {
		// parse revocery data, 35=W
		std::for_each(messages.cbegin(), messages.cend(),
				[this](const rczg::MdpMessage &message){
					std::vector<Book> books;
					m_parser_w.Parse(message, books);
					BookManager::Move_append(books, m_recovery_books);
				}
		);

    	m_recovery_wait_merge = m_recovery_books.cbegin();
    }

    std::vector<Book> BookManager::Parse_increment(const rczg::MdpMessage &message)
    {
    	std::vector<Book> books;

    	char type = message.message_type();
    	if(type == 'f')
    	{
    		m_parser_f.Parse(message, books);
    	}
    	else if(type == 'd')
    	{
			this->Update_definition(message);
    	}
    	else if(type == 'R')
    	{
    		m_parser_r.Parse(message, books);
    	}
    	else if(type == 'X')
    	{
    		m_parser_x.Parse(message, books);
    	}
    	else
    	{
    		LOG_DEBUG("message type is ", type, ", ignore.");
    	}

    	return books;
    }

    void BookManager::Update_definition(const rczg::MdpMessage &message)
    {
		std::vector<Instrument> instruments;
		m_parser_d.Parse(message, instruments);

		std::for_each(instruments.begin(), instruments.end(),
			[this](Instrument &i){
				LOG_DEBUG("new definition: ", i.Serialize());
				m_instruments[i.securityID] = std::move(i);
				this->On_definition_changed(i.securityID);
			}
		);
    }

    std::vector<Book>  BookManager::Merge_with_recovery(std::uint32_t message_seq, std::vector<Book> &increment_books)
    {
    	std::vector<Book> merged_books;
    	auto old_pos = m_recovery_wait_merge;

    	// 首先将 recovery books 中 LastMsgSeqNumProcessed 在该 message_seq 之前（包括）的 books 保存下来
    	while(m_recovery_wait_merge != m_recovery_books.cend() && m_recovery_wait_merge->packet_seq_num <= message_seq)
    	{
    		merged_books.push_back(std::ref(*m_recovery_wait_merge));
    		++m_recovery_wait_merge;
    	}

    	// 看看 increment_books 中有没有 SecurityID 在 recovery books 的 [之前保存下的位置，末尾] 中存在
    	for(auto pos = old_pos; pos != m_recovery_books.cend(); ++pos)
    	{
    		// 只看 LastMsgSeqNumProcessed 在该 increment book 的 message_seq 之后（包括）的数据
    		if(pos->packet_seq_num >= message_seq)
    		{
    			// 删除 increment books 中和当前 recovery book 一致的数据 （SecurityID 一样）
    			increment_books.erase(
    					std::remove_if(increment_books.begin(), increment_books.end(), [pos](Book &ib){ return ib.securityID == pos->securityID; }),
						increment_books.end()
				);
    		}
    	}

    	// 剩下的 increment books 是需要保存的
    	BookManager::Move_append(increment_books, merged_books);

    	return merged_books;
    }

    void BookManager::On_definition_changed(std::uint32_t security_id)
    {
    	m_book_state_controller.Create_or_shrink(m_instruments.at(security_id));
    }

    // move vector contents to another vector
    void BookManager::Move_append(std::vector<Book>& src, std::vector<Book>& dst)
    {
    	if (dst.empty())
    	{
			dst = std::move(src);
		}
		else
		{
			dst.reserve(dst.size() + src.size());
			std::move(std::begin(src), std::end(src), std::back_inserter(dst));
			src.clear();
		}
    }

}
