    
COMPILER = g++

ROOT = .
SRC_PATH = $(ROOT)/src
TEST_PATH = $(ROOT)/test
BIN_PATH = $(ROOT)/bin
VENDOR_PATH = $(realpath $(ROOT)/vendor)

#define lcov
TMP_DIR = $(BIN_PATH)
LCOV := $(VENDOR_PATH)/lcov/lcov
GENHTML := $(VENDOR_PATH)/lcov/genhtml

#define include path
INCLUDE_TEST_PATH = -I$(TEST_PATH)
INCLUDE_PATH = -I$(SRC_PATH) -I$(VENDOR_PATH)/boost/include -I$(VENDOR_PATH)/gtest/include  -I$(VENDOR_PATH)/mongodb/include  \
								-I$(VENDOR_PATH)/protobuf/include -I$(VENDOR_PATH)/quickfix/include -I$(VENDOR_PATH)/sbe/include -I$(VENDOR_PATH)/zeromq/include
FEMAS_INCLUDE_PATH = -I$(VENDOR_PATH)/femas/include
REM_INCLUDE_PATH = -I$(VENDOR_PATH)/shengli/include
CTP_INCLUDE_PATH = -I$(VENDOR_PATH)/ctp/include
INCLUDE_PATH +=  $(FEMAS_INCLUDE_PATH)
INCLUDE_PATH +=  $(REM_INCLUDE_PATH)
INCLUDE_PATH +=  $(CTP_INCLUDE_PATH)

MONGODB_INCLUDE_PATH += -I$(VENDOR_PATH)/mongodb/include/bsoncxx/v_noabi -I$(VENDOR_PATH)/mongodb/include/mongocxx/v_noabi
INCLUDE_PATH +=  $(MONGODB_INCLUDE_PATH)

#define libs path
BOOST_LIBS_PATH = $(VENDOR_PATH)/boost/libs
GTEST_LIBS_PATH = $(VENDOR_PATH)/gtest/libs
MONGODB_LIBS_PATH = $(VENDOR_PATH)/mongodb/libs
PROTOBUF_LIBS_PATH = $(VENDOR_PATH)/protobuf/libs
QUICKFIX_LIBS_PATH = $(VENDOR_PATH)/quickfix/libs
SBE_LIBS_PATH = $(VENDOR_PATH)/sbe/libs
ZEROMQ_LIBS_PATH = $(VENDOR_PATH)/zeromq/libs
FEMAS_LIBS_PATH = $(VENDOR_PATH)/femas/libs
REM_LIBS_PATH = $(VENDOR_PATH)/shengli/libs
CTP_LIBS_PATH = $(VENDOR_PATH)/ctp/libs

#LIBS_PATH = -L$(VENDOR_PATH)/boost/libs -L$(VENDOR_PATH)/gtest/libs  -L$(VENDOR_PATH)/mongodb/libs
LIBS_PATH = -L$(BOOST_LIBS_PATH) -L$(GTEST_LIBS_PATH)  -L$(MONGODB_LIBS_PATH)
LIBS_PATH += -L$(FEMAS_LIBS_PATH)
LIBS_PATH += -L$(REM_LIBS_PATH)
LIBS_PATH += -L$(CTP_LIBS_PATH)
LIBS_PATH += -L$(PROTOBUF_LIBS_PATH) -L$(QUICKFIX_LIBS_PATH) -L$(SBE_LIBS_PATH) -L$(ZEROMQ_LIBS_PATH)

#define exec libs path
#EXEC_LIBS_PATH = -Wl,-rpath,$(VENDOR_PATH)/boost/libs:$(VENDOR_PATH)/gtest/libs:$(VENDOR_PATH)/mongodb/libs:$(VENDOR_PATH)/protobuf/libs:$(VENDOR_PATH)/quickfix/libs:$(VENDOR_PATH)/sbe/libs:$(VENDOR_PATH)/zeromq/libs:$(VENDOR_PATH)/femas/libs
EXEC_LIBS_PATH = -Wl,-rpath,$(BOOST_LIBS_PATH):$(GTEST_LIBS_PATH):$(MONGODB_LIBS_PATH):$(PROTOBUF_LIBS_PATH):$(QUICKFIX_LIBS_PATH):$(SBE_LIBS_PATH):$(ZEROMQ_LIBS_PATH):$(FEMAS_LIBS_PATH):$(REM_LIBS_PATH):$(CTP_LIBS_PATH)

LIBS = -lpthread -lboost_system -lzmq -lstdc++ -lquickfix -lmongocxx -lbsoncxx -lmongoc -lbson -lprotobuf -lgcov
LIBS_CTP = -lpthread -lboost_system -lzmq -lstdc++ -lquickfix -lmongocxx -lbsoncxx -lmongoc -lbson -lprotobuf -lgcov
FEMAS_LIBS = -lUSTPmduserapiAF -lUSTPtraderapiAF
REM_LIBS = -lEESQuoteApi -lEESTraderApi
CTP_LIBS = -lthostmduserapi -lthosttraderapi
LIBS += $(FEMAS_LIBS)
LIBS += $(REM_LIBS)
LIBS_CTP += $(CTP_LIBS)

TEST_LIBS = -lgmock $(CTP_LIBS)
RELEASE_FLAGS = -O3 -DNDEBUG -Ofast
DBG_FLAGS = -g -rdynamic
COV_FLAG := -fprofile-arcs -ftest-coverage
FLAGS = -std=c++11 -Wall -Wno-pragmas $(DBG_FLAGS) $(COV_FLAG)
COMPILE_COMMAND = $(COMPILER) $(INCLUDE_PATH) $(LIBS_PATH) $(EXEC_LIBS_PATH) $(LIBS) $(FLAGS)
COMPILE_COMMAND_CTP = $(COMPILER) $(INCLUDE_PATH) $(LIBS_PATH) $(EXEC_LIBS_PATH) $(LIBS_CTP) $(FLAGS)
TEST_COMPILE_COMMAND = $(COMPILER) $(INCLUDE_PATH) $(INCLUDE_TEST_PATH) $(LIBS_PATH) $(EXEC_LIBS_PATH) $(LIBS) $(FLAGS)
LINT_COMMAND = $(TEST_PATH)/cpplint.py

#define setting
CME_SETTINGS = $(BIN_PATH)/cme_market_config.xml $(BIN_PATH)/cme_market_settings.ini  $(BIN_PATH)/cme_exchange_server.cfg \
					  $(BIN_PATH)/cme_exchange_settings.ini  $(BIN_PATH)/cme_exchange_client.cfg $(BIN_PATH)/persist_settings.ini \
                      $(BIN_PATH)/trade_matching_settings.ini
SETTINGS += $(CME_SETTINGS)
                      
UT_MARKET_SETTINGS =  $(BIN_PATH)/mut_cmemarket_revbuf.log $(BIN_PATH)/market_by_price_1.log $(BIN_PATH)/market_by_price_2.log \
					  $(BIN_PATH)/market_609_426_sd_1.log $(BIN_PATH)/market_609_426_sd_2.log $(BIN_PATH)/market_627_426_fs_1.log \
					  $(BIN_PATH)/market_627_426_fs_2.log $(BIN_PATH)/market_627_426_qm.log $(BIN_PATH)/market_627_427_sm_1.log \
					  $(BIN_PATH)/market_627_427_sm_2.log $(BIN_PATH)/market_627_427_sm_3.log $(BIN_PATH)/market_627_427_sm_4.log \
					  $(BIN_PATH)/market_627_427_sm_5.log $(BIN_PATH)/market_627_427_sm_6.log $(BIN_PATH)/market_627_1_cr_make_price_1.log \
					  $(BIN_PATH)/market_627_1_cr_make_price_2.log $(BIN_PATH)/market_627_5_2_msm_1.log $(BIN_PATH)/market_627_5_2_msm_3.log \
					  $(BIN_PATH)/market_627_5_3_msm_5.log $(BIN_PATH)/market_627_5_3_msm_6.log $(BIN_PATH)/market_627_5_3_msm_7.log \
					  $(BIN_PATH)/market_609_5_3_sdmfo_1.log $(BIN_PATH)/market_627_5_3_sdmff_1.log $(BIN_PATH)/market_627_5_3_qty_pc_1st_instr.log \
					  $(BIN_PATH)/market_627_5_3_qty_pc_2nd_instr.log
SETTINGS += $(UT_MARKET_SETTINGS)

FEMAS_SETTINGS = $(BIN_PATH)/femas_config.ini 
REM_SETTINGS = $(BIN_PATH)/rem_config.ini 
CTP_SETTINGS = $(BIN_PATH)/ctp_config.ini
SETTINGS += $(FEMAS_SETTINGS)
SETTINGS += $(REM_SETTINGS)
SETTINGS += $(CTP_SETTINGS)

ALL_OBJS =  $(filter-out $(wildcard $(BIN_PATH)/*_test.o), $(wildcard $(BIN_PATH)/*.o)) 
#define test_obj
TEST_OBJS = $(BIN_PATH)/utility_unittest.o 

TEST_CME_OBJS = $(BIN_PATH)/mut_common.o $(BIN_PATH)/mut_book_sender.o $(BIN_PATH)/mut_book_manager.o  \
				$(BIN_PATH)/mut_market_manager.o $(BIN_PATH)/autotest_book_sender.o $(BIN_PATH)/mut_dat_saver.o $(BIN_PATH)/mut_globex_communicator.o $(BIN_PATH)/mut_exchange_application.o \
				$(BIN_PATH)/mut_order_manager.o $(BIN_PATH)/mut_strategy_communicator.o
TEST_OBJS += $(TEST_CME_OBJS)

TEST_FEMAS_OBJS = $(BIN_PATH)/mut_femas_book_manager.o $(BIN_PATH)/mut_femas_matket.o $(BIN_PATH)/mut_femas_market_manager.o $(BIN_PATH)/mut_femas_exchange.o 
TEST_OBJS += $(TEST_FEMAS_OBJS)

#TEST_CTP_OBJS = $(BIN_PATH)/mut_custom_manager.o $(BIN_PATH)/mut_ctp_exchange.o $(BIN_PATH)/mut_custom_md_spi.o 
#TEST_OBJS += $(TEST_CTP_OBJS)

TEST_SIMULATOR_OBJS = $(BIN_PATH)/mut_market_simulater.o $(BIN_PATH)/mut_exchange_simulater.o $(BIN_PATH)/mut_trade_simulater.o $(BIN_PATH)/mut_replay_simulater.o
TEST_OBJS += $(TEST_SIMULATOR_OBJS)

COMM_OBJS = $(BIN_PATH)/sbe_encoder.o $(BIN_PATH)/utility.o $(BIN_PATH)/message_utility.o $(BIN_PATH)/logger.o \
						   $(BIN_PATH)/mdp_message.o $(BIN_PATH)/sbe_to_json.o $(BIN_PATH)/sbe_decoder.o $(BIN_PATH)/settings.o \
						   $(BIN_PATH)/time_measurer.o $(BIN_PATH)/zmq_sender.o $(BIN_PATH)/zmq_receiver.o \
						   $(BIN_PATH)/ems.pb.o $(BIN_PATH)/dms.pb.o $(BIN_PATH)/strategy_communicator.o $(BIN_PATH)/book_sender.o
ALL_FILES = $(shell find $(SRC_PATH) -name '*.h' -or -name '*.cc')
SRC_PATH_TEST = $(realpath $(ROOT)/src)						   
ALL_CXXFILES = $(shell find $(SRC_PATH_TEST) -name '*.cc')	
						   
SENDER_TARGET = $(BIN_PATH)/udp_sender_test
F_SENDER_TARGET = $(BIN_PATH)/udp_file_sender_test
TSENDER_TARGET = $(BIN_PATH)/tcp_sender_test
MARKET_TARGET = $(BIN_PATH)/market_test
SBE_TARGET = $(BIN_PATH)/sbe_test
PACKET_TARGET = $(BIN_PATH)/packet_test
EXCHANGE_SERVER_TARGET = $(BIN_PATH)/exchange_server_test
STRATEGY_TARGET = $(BIN_PATH)/strategy_test
EXCHANGE_CLIENT_TARGET = $(BIN_PATH)/exchange_client_test
ORIGINAL_SAVER_TARGET = $(BIN_PATH)/original_saver_test
ORIGINAL_SENDER_TARGET = $(BIN_PATH)/original_sender_test
ORIGINAL_READER_TARGET = $(BIN_PATH)/original_reader_test
TRADE_MATCHING_ALPHA_TARGET = $(BIN_PATH)/trade_matching_alpha_test
TRADE_MATCHING_EXCHANGE_ALPHA_TARGET = $(BIN_PATH)/trade_matching_exchange_alpha_test
TRADE_MATCHING_TRADE_ALPHA_TARGET = $(BIN_PATH)/trade_matching_trade_alpha_test
TRADE_MATCHING_REPLAY_ALPHA_TARGET = $(BIN_PATH)/trade_matching_replay_alpha_test
TEST_TARGET = $(BIN_PATH)/utest
FEMAS_MARKET_TARGET = $(BIN_PATH)/femas_market_test
FEMAS_EXCHANGE_TARGET = $(BIN_PATH)/femas_exchange_test
DATE_CONVERTER_TOOL_TARGET = $(BIN_PATH)/data_converter_test
REM_MARKET_TARGET = $(BIN_PATH)/rem_market_test
REM_EXCHANGE_TARGET = $(BIN_PATH)/rem_exchange_test
REM_EFH_MARKET_TARGET = $(BIN_PATH)/rem_efhmarket_test
CTP_MARKET_TARGET = $(BIN_PATH)/ctp_market_test
CTP_EXCHANGE_TARGET = $(BIN_PATH)/ctp_exchange_test   

default: all;

include tmobjs.mk
include cmeobjs.mk
include femas.mk
include toolobjs.mk
include rem.mk
include ctp.mk 
  
all: createdir rem_efhmarket rem_exchange_test rem_market femas_exchange_test femas_market usender tsender market sbe ptest eserver strategy eclient copyfile original orgsend ufsender orgread tmalpha tmalphaex tmalphatrade tmalphareplay dataconverter ctp_market ctp_exchange_test

femas_exchange_test: $(BIN_PATH)/femas_exchange_main_test.o $(BIN_PATH)/femas_exchange_application.o $(BIN_PATH)/communicator.o $(BIN_PATH)/FemasUstpFtdcTraderManger.o \
			 $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(FEMAS_EXCHANGE_TARGET) $?

femas_market: $(BIN_PATH)/femas_market_main_test.o $(BIN_PATH)/femas_market_manager.o $(BIN_PATH)/femas_market.o $(BIN_PATH)/femas_market_application.o \
              $(BIN_PATH)/femas_book_manager.o $(BIN_PATH)/Femas_book_replayer.o $(BIN_PATH)/Femas_book_convert.o \
			 $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(FEMAS_MARKET_TARGET) $?	 
	
rem_exchange_test: $(BIN_PATH)/rem_exchange_main_test.o $(BIN_PATH)/rem_exchange_application.o $(BIN_PATH)/rem_communicator.o $(BIN_PATH)/RemEESTraderApiManger.o \
			 $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(REM_EXCHANGE_TARGET) $?

rem_market: $(BIN_PATH)/rem_market_main_test.o $(BIN_PATH)/rem_market_manager.o $(BIN_PATH)/rem_market.o $(BIN_PATH)/rem_market_application.o \
              $(BIN_PATH)/rem_book_manager.o $(BIN_PATH)/Rem_book_convert.o \
			 $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(REM_MARKET_TARGET) $?	 
	
rem_efhmarket: $(BIN_PATH)/rem_efh_market_main_test.o $(BIN_PATH)/rem_efhmarket.o $(BIN_PATH)/rem_guava_quote.o $(BIN_PATH)/rem_socket_multicast.o $(BIN_PATH)/rem_efhmarket_manager.o $(BIN_PATH)/EFHRem_book_convert.o \
			 $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(REM_EFH_MARKET_TARGET) $?		
  
ctp_exchange_test: $(BIN_PATH)/ctp_exchange_main_test.o $(BIN_PATH)/ctp_exchange_application.o $(BIN_PATH)/ctpcommunicator.o $(BIN_PATH)/ctp_trader_spi.o $(BIN_PATH)/ApiCommand.o $(BIN_PATH)/ComfirmSettlementCommand.o $(BIN_PATH)/CommandQueue.o $(BIN_PATH)/InsertOrderCommand.o  $(BIN_PATH)/LoginCommand.o  $(BIN_PATH)/WithdrawOrderCommand.o $(BIN_PATH)/AccountID.o $(BIN_PATH)/LoginOutCommand.o $(BIN_PATH)/QueryOrderCommand.o  $(BIN_PATH)/QueryPositionCommand.o $(BIN_PATH)/ReqQryInstrumentCommand.o  $(BIN_PATH)/ReqQryTradeCommand.o\
			 $(COMM_OBJS) 
	$(COMPILE_COMMAND_CTP) -o $(CTP_EXCHANGE_TARGET) $?

ctp_market: $(BIN_PATH)/ctp_market_main_test.o $(BIN_PATH)/MDWrapper.o $(BIN_PATH)/MDAccountID.o $(BIN_PATH)/custom_md_spi.o $(BIN_PATH)/custom_manager.o $(BIN_PATH)/ctp_market_application.o \
			 $(COMM_OBJS) 
	$(COMPILE_COMMAND_CTP) -o $(CTP_MARKET_TARGET) $?	 
   
createdir:
	mkdir -p ${BIN_PATH}
	mkdir -p ${BIN_PATH}/result
	mkdir -p ${BIN_PATH}/result_ut
usender: $(BIN_PATH)/udp_sender_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(SENDER_TARGET) $?

ufsender: $(BIN_PATH)/udp_file_sender_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(F_SENDER_TARGET) $?

tsender: $(BIN_PATH)/tcp_sender_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(TSENDER_TARGET) $?
	
market: $(BIN_PATH)/market_test.o $(BIN_PATH)/dat_processor.o $(BIN_PATH)/udp_receiver.o $(BIN_PATH)/dat_saver.o $(BIN_PATH)/cme_data.o\
               $(BIN_PATH)/tcp_receiver.o $(BIN_PATH)/dat_arbitrator.o $(BIN_PATH)/market_application.o $(BIN_PATH)/market_settings.o \
               $(BIN_PATH)/recovery_saver.o $(BIN_PATH)/dat_replayer.o $(BIN_PATH)/book_manager.o $(BIN_PATH)/book_state_controller.o \
               $(BIN_PATH)/message_parser_d.o $(BIN_PATH)/message_parser_f.o $(BIN_PATH)/message_parser_r.o $(BIN_PATH)/channel_settings.o \
               $(BIN_PATH)/message_parser_x.o $(BIN_PATH)/message_parser_w.o $(BIN_PATH)/definition_manager.o $(BIN_PATH)/status_manager.o \
               $(BIN_PATH)/cme_market.o $(BIN_PATH)/market_manager.o $(BIN_PATH)/book_sender.o $(BIN_PATH)/recovery_manager.o \
               $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(MARKET_TARGET) $?

sbe: $(BIN_PATH)/sbe_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(SBE_TARGET) $?

ptest: $(BIN_PATH)/packet_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(PACKET_TARGET) $?

eserver: $(BIN_PATH)/exchange_server_test.o $(BIN_PATH)/server_application.o $(BIN_PATH)/server_market.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(EXCHANGE_SERVER_TARGET) $?

strategy: $(BIN_PATH)/strategy_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(STRATEGY_TARGET) $?

eclient: $(BIN_PATH)/exchange_client_test.o $(BIN_PATH)/exchange_application.o $(BIN_PATH)/strategy_communicator.o \
			 $(BIN_PATH)/exchange_settings.o $(BIN_PATH)/globex_communicator.o $(BIN_PATH)/order_manager.o \
			 $(BIN_PATH)/globex_logger.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(EXCHANGE_CLIENT_TARGET) $?

original: $(BIN_PATH)/original_saver_test.o $(BIN_PATH)/mongo.o $(BIN_PATH)/original_saver.o $(BIN_PATH)/original_receiver.o \
				$(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(ORIGINAL_SAVER_TARGET) $?
		
orgsend: $(BIN_PATH)/original_sender_test.o $(COMM_OBJS)
	$(COMPILE_COMMAND) -o $(ORIGINAL_SENDER_TARGET) $? 

orgread: $(BIN_PATH)/original_reader_test.o $(BIN_PATH)/mongo.o $(COMM_OBJS)
	$(COMPILE_COMMAND) -o $(ORIGINAL_READER_TARGET) $? 

tmalpha: $(BIN_PATH)/trade_matching_alpha_test.o $(BIN_PATH)/tmalpha_market_application.o $(BIN_PATH)/market_simulater.o $(BIN_PATH)/book_replayer.o \
                $(BIN_PATH)/book_sender.o $(BIN_PATH)/message_parser_d.o $(BIN_PATH)/message_parser_f.o $(BIN_PATH)/message_parser_r.o \
                $(BIN_PATH)/recovery_manager.o \
                $(BIN_PATH)/message_parser_x.o $(BIN_PATH)/message_parser_w.o $(BIN_PATH)/definition_manager.o $(BIN_PATH)/status_manager.o \
	    		$(BIN_PATH)/book_manager.o $(BIN_PATH)/mongo.o $(BIN_PATH)/book_state_controller.o $(COMM_OBJS)
	$(COMPILE_COMMAND) -o $(TRADE_MATCHING_ALPHA_TARGET) $? 

tmalphaex: $(BIN_PATH)/trade_matching_exchange_alpha_test.o $(BIN_PATH)/tmalpha_exchange_application.o $(BIN_PATH)/exchange_simulater.o \
					 $(BIN_PATH)/tmalpha_market_application.o $(BIN_PATH)/market_simulater.o $(BIN_PATH)/book_replayer.o $(BIN_PATH)/book_manager.o  \
	                 $(BIN_PATH)/recovery_manager.o \
                     $(BIN_PATH)/book_sender.o $(BIN_PATH)/message_parser_d.o $(BIN_PATH)/message_parser_f.o $(BIN_PATH)/message_parser_r.o \
                     $(BIN_PATH)/message_parser_x.o $(BIN_PATH)/message_parser_w.o $(BIN_PATH)/definition_manager.o $(BIN_PATH)/status_manager.o \
				     $(BIN_PATH)/mongo.o $(BIN_PATH)/book_state_controller.o $(BIN_PATH)/strategy_communicator.o $(COMM_OBJS)
	$(COMPILE_COMMAND) -o $(TRADE_MATCHING_EXCHANGE_ALPHA_TARGET) $? 
				     
tmalphatrade: $(BIN_PATH)/trade_matching_trade_alpha_test.o	$(BIN_PATH)/trade_market_state.o $(BIN_PATH)/trade_market_manager.o \
						   $(BIN_PATH)/trade_order_box.o $(BIN_PATH)/trade_order_manager.o $(BIN_PATH)/trade_simulater.o \
						   $(BIN_PATH)/tmalpha_trade_application.o $(BIN_PATH)/simulater_exchange.o $(BIN_PATH)/simulater_market.o \
						   $(BIN_PATH)/book_sender.o $(BIN_PATH)/strategy_communicator.o $(COMM_OBJS)
	$(COMPILE_COMMAND) -o $(TRADE_MATCHING_TRADE_ALPHA_TARGET) $? 
						   	
tmalphareplay: $(BIN_PATH)/trade_matching_replay_alpha_test.o	$(BIN_PATH)/replay_exchange.o $(BIN_PATH)/replay_market.o \
                             $(BIN_PATH)/replay_order_matcher.o $(BIN_PATH)/replay_simulater.o $(BIN_PATH)/tmalpha_replay_application.o \
                             $(BIN_PATH)/book_sender.o $(BIN_PATH)/strategy_communicator.o $(BIN_PATH)/mongo.o $(COMM_OBJS)
	$(COMPILE_COMMAND) -o $(TRADE_MATCHING_REPLAY_ALPHA_TARGET) $? 

dataconverter: $(BIN_PATH)/market_data_converter.o $(BIN_PATH)/data_converter_test.o \
							$(BIN_PATH)/femas_book_manager.o $(BIN_PATH)/Femas_book_convert.o $(BIN_PATH)/book_convert.o\
              				$(BIN_PATH)/mongo.o $(COMM_OBJS)
	$(COMPILE_COMMAND) -o $(DATE_CONVERTER_TOOL_TARGET) $? 
	
copyfile: $(SETTINGS)

test: $(ALL_OBJS) $(TEST_OBJS) $(COMM_OBJS)
	$(TEST_COMPILE_COMMAND) $(TEST_LIBS) -o $(TEST_TARGET) $? 
html:
	cd $(BIN_PATH) && gcov $(ALL_CXXFILES) -o ./ && $(LCOV) -c -d ./ -o coverage.info \
	&& $(GENHTML) -o ./result ./coverage.info

lint:
	$(LINT_COMMAND) --root=$(SRC_PATH) --linelength=200 --filter=-legal/copyright --extensions=cc,h  $(ALL_FILES)
	
clean:
	@rm -rf $(BIN_PATH)/* 

	
