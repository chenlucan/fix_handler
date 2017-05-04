    
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

INCLUDE_TEST_PATH = -I$(TEST_PATH)
INCLUDE_PATH = -I$(SRC_PATH) -I$(VENDOR_PATH)/boost/include -I$(VENDOR_PATH)/gtest/include  -I$(VENDOR_PATH)/mongodb/include  \
								-I$(VENDOR_PATH)/protobuf/include -I$(VENDOR_PATH)/quickfix/include -I$(VENDOR_PATH)/sbe/include -I$(VENDOR_PATH)/zeromq/include \
								 -I$(VENDOR_PATH)/mongodb/include/bsoncxx/v_noabi -I$(VENDOR_PATH)/mongodb/include/mongocxx/v_noabi
LIBS_PATH = -L$(VENDOR_PATH)/boost/libs -L$(VENDOR_PATH)/gtest/libs  -L$(VENDOR_PATH)/mongodb/libs  \
								-L$(VENDOR_PATH)/protobuf/libs -L$(VENDOR_PATH)/quickfix/libs -L$(VENDOR_PATH)/sbe/libs -L$(VENDOR_PATH)/zeromq/libs
EXEC_LIBS_PATH = -Wl,-rpath,$(VENDOR_PATH)/boost/libs:$(VENDOR_PATH)/gtest/libs:$(VENDOR_PATH)/mongodb/libs:$(VENDOR_PATH)/protobuf/libs:$(VENDOR_PATH)/quickfix/libs:$(VENDOR_PATH)/sbe/libs:$(VENDOR_PATH)/zeromq/libs
LIBS = -lpthread -lboost_system -lzmq -lstdc++ -lquickfix -lmongocxx -lbsoncxx -lmongoc -lbson -lprotobuf -lgcov
TEST_LIBS = -lgmock
RELEASE_FLAGS = -O3 -DNDEBUG -Ofast
DBG_FLAGS = -g -rdynamic
COV_FLAG := -fprofile-arcs -ftest-coverage
FLAGS = -std=c++11 -Wall -Wno-pragmas $(DBG_FLAGS) $(COV_FLAG)
COMPILE_COMMAND = $(COMPILER) $(INCLUDE_PATH) $(LIBS_PATH) $(EXEC_LIBS_PATH) $(LIBS) $(FLAGS)
TEST_COMPILE_COMMAND = $(COMPILER) $(INCLUDE_PATH) $(INCLUDE_TEST_PATH) $(LIBS_PATH) $(EXEC_LIBS_PATH) $(LIBS) $(FLAGS)
LINT_COMMAND = $(TEST_PATH)/cpplint.py

SETTINGS = $(BIN_PATH)/market_config.xml $(BIN_PATH)/market_settings.ini  $(BIN_PATH)/exchange_server.cfg \
					  $(BIN_PATH)/exchange_settings.ini  $(BIN_PATH)/exchange_client.cfg $(BIN_PATH)/persist_settings.ini  \
					  $(BIN_PATH)/mut_cmemarket_revbuf.log $(BIN_PATH)/market_by_price_1.log $(BIN_PATH)/market_by_price_2.log \
					  $(BIN_PATH)/market_609_426_sd_1.log $(BIN_PATH)/market_609_426_sd_2.log $(BIN_PATH)/market_627_426_fs_1.log \
					  $(BIN_PATH)/market_627_426_fs_2.log $(BIN_PATH)/market_627_426_qm.log $(BIN_PATH)/market_627_427_sm_1.log \
					  $(BIN_PATH)/market_627_427_sm_2.log $(BIN_PATH)/market_627_427_sm_3.log $(BIN_PATH)/market_627_427_sm_4.log \
					  $(BIN_PATH)/market_627_427_sm_5.log $(BIN_PATH)/market_627_427_sm_6.log $(BIN_PATH)/market_627_1_cr_make_price_1.log \
					  $(BIN_PATH)/market_627_1_cr_make_price_2.log $(BIN_PATH)/market_627_5_2_msm_1.log $(BIN_PATH)/market_627_5_2_msm_3.log \
					  $(BIN_PATH)/market_627_5_3_msm_5.log $(BIN_PATH)/market_627_5_3_msm_6.log $(BIN_PATH)/market_627_5_3_msm_7.log \
					  $(BIN_PATH)/market_609_5_3_sdmfo_1.log $(BIN_PATH)/market_627_5_3_sdmff_1.log
ALL_OBJS =  $(filter-out $(wildcard $(BIN_PATH)/*_test.o), $(wildcard $(BIN_PATH)/*.o)) 
TEST_OBJS = $(BIN_PATH)/utility_unittest.o $(BIN_PATH)/mut_common.o $(BIN_PATH)/mut_book_sender.o $(BIN_PATH)/mut_book_manager.o  \
						   $(BIN_PATH)/mut_market_manager.o $(BIN_PATH)/autotest_book_sender.o $(BIN_PATH)/mut_dat_saver.o
COMM_OBJS = $(BIN_PATH)/sbe_encoder.o $(BIN_PATH)/utility.o $(BIN_PATH)/message_utility.o $(BIN_PATH)/logger.o \
						   $(BIN_PATH)/mdp_message.o $(BIN_PATH)/sbe_to_json.o $(BIN_PATH)/sbe_decoder.o $(BIN_PATH)/settings.o \
						   $(BIN_PATH)/time_measurer.o $(BIN_PATH)/zmq_sender.o $(BIN_PATH)/zmq_receiver.o \
						   $(BIN_PATH)/ems.pb.o $(BIN_PATH)/dms.pb.o
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
TEST_TARGET = $(BIN_PATH)/utest
    
default: all;
    
include objs.mk
    
all: createdir usender tsender market sbe ptest eserver strategy eclient copyfile original orgsend ufsender
 
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
	
market: $(BIN_PATH)/market_test.o $(BIN_PATH)/dat_processor.o $(BIN_PATH)/udp_receiver.o $(BIN_PATH)/dat_saver.o \
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
		
copyfile: $(SETTINGS)

test: $(ALL_OBJS) $(TEST_OBJS)
	$(TEST_COMPILE_COMMAND) $(TEST_LIBS) -o $(TEST_TARGET) $^
html:
	cd $(BIN_PATH) && gcov $(ALL_CXXFILES) -o ./ && $(LCOV) -c -d ./ -o coverage.info \
	&& $(GENHTML) -o ./result ./coverage.info

lint:
	$(LINT_COMMAND) --root=$(SRC_PATH) --linelength=200 --filter=-legal/copyright --extensions=cc,h  $(ALL_FILES)
	
clean:
	@rm -rf $(BIN_PATH)/* 

	
