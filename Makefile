    
COMPILER = g++

ROOT = .
SRC_PATH = $(ROOT)/src
TEST_PATH = $(ROOT)/test
BIN_PATH = $(ROOT)/bin

INCLUDE_PATH = -I$(SRC_PATH)
LIBS_PATH = -L/usr/lib64 -L/usr/local/lib
LIBS = -lpthread -lboost_system -lzmq -lstdc++ -lquickfix -lmongocxx -lbsoncxx -lmongoc -lbson -lprotobuf
TEST_LIBS = -lgmock
RELEASE_FLAGS = -O3 -DNDEBUG -Ofast
DBG_FLAGS = -g -rdynamic
FLAGS = -std=c++11 -Wall -Wno-pragmas $(DBG_FLAGS)
COMPILE_COMMAND = $(COMPILER) $(INCLUDE_PATH) $(LIBS_PATH) $(LIBS) $(FLAGS)
LINT_COMMAND = $(TEST_PATH)/cpplint.py

SETTINGS = $(BIN_PATH)/market_config.xml $(BIN_PATH)/market_settings.ini  $(BIN_PATH)/exchange_server.cfg \
					  $(BIN_PATH)/exchange_settings.ini  $(BIN_PATH)/exchange_client.cfg
ALL_OBJS =  $(filter-out $(wildcard $(BIN_PATH)/*_test.o), $(wildcard $(BIN_PATH)/*.o)) 
TEST_OBJS = $(BIN_PATH)/utility_unittest.o
COMM_OBJS = $(BIN_PATH)/sbe_encoder.o $(BIN_PATH)/utility.o $(BIN_PATH)/message_utility.o $(BIN_PATH)/logger.o \
						   $(BIN_PATH)/mdp_message.o $(BIN_PATH)/sbe_decoder.o $(BIN_PATH)/settings.o \
						   $(BIN_PATH)/time_measurer.o $(BIN_PATH)/zmq_sender.o $(BIN_PATH)/zmq_receiver.o $(BIN_PATH)/ems.pb.o
ALL_FILES = $(shell find $(SRC_PATH) -name '*.h' -or -name '*.cc')						   
						   
SENDER_TARGET = $(BIN_PATH)/udp_sender_test
TSENDER_TARGET = $(BIN_PATH)/tcp_sender_test
RECEIVER_TARGET = $(BIN_PATH)/udp_receiver_test
SBE_TARGET = $(BIN_PATH)/sbe_test
ZMQ_TARGET = $(BIN_PATH)/zmq_receiver_test
EXCHANGE_SERVER_TARGET = $(BIN_PATH)/exchange_server_test
STRATEGY_TARGET = $(BIN_PATH)/strategy_test
EXCHANGE_CLIENT_TARGET = $(BIN_PATH)/exchange_client_test
TEST_TARGET = $(BIN_PATH)/utest
    
default: all;
    
include objs.mk
    
all: createdir usender tsender receiver sbe zmqrec eserver strategy eclient copyfile

createdir:
	mkdir -p ${BIN_PATH}

usender: $(BIN_PATH)/udp_sender_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(SENDER_TARGET) $?

tsender: $(BIN_PATH)/tcp_sender_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(TSENDER_TARGET) $?
	
receiver: $(BIN_PATH)/udp_receiver_test.o $(BIN_PATH)/dat_processor.o $(BIN_PATH)/udp_receiver.o $(BIN_PATH)/dat_saver.o \
               $(BIN_PATH)/tcp_receiver.o $(BIN_PATH)/dat_arbitrator.o $(BIN_PATH)/application.o $(BIN_PATH)/market_settings.o \
               $(BIN_PATH)/recovery_saver.o $(BIN_PATH)/dat_replayer.o $(BIN_PATH)/book_manager.o $(BIN_PATH)/book_state_controller.o\
               $(BIN_PATH)/message_parser_d.o $(BIN_PATH)/message_parser_f.o $(BIN_PATH)/message_parser_r.o $(BIN_PATH)/channel_settings.o \
               $(BIN_PATH)/message_parser_x.o $(BIN_PATH)/message_parser_w.o \
               $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(RECEIVER_TARGET) $?

sbe: $(BIN_PATH)/sbe_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(SBE_TARGET) $?

zmqrec: $(BIN_PATH)/mdp_receiver.o $(BIN_PATH)/mdp_saver.o $(BIN_PATH)/zmq_receiver_test.o $(BIN_PATH)/market_settings.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(ZMQ_TARGET) $?

eserver: $(BIN_PATH)/exchange_server_test.o $(BIN_PATH)/server_application.o $(BIN_PATH)/server_market.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(EXCHANGE_SERVER_TARGET) $?

strategy: $(BIN_PATH)/strategy_test.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(STRATEGY_TARGET) $?

eclient: $(BIN_PATH)/exchange_client_test.o $(BIN_PATH)/exchange_application.o $(BIN_PATH)/strategy_communicator.o \
			 $(BIN_PATH)/exchange_settings.o $(BIN_PATH)/globex_communicator.o $(BIN_PATH)/order_manager.o \
			 $(BIN_PATH)/globex_logger.o $(COMM_OBJS) 
	$(COMPILE_COMMAND) -o $(EXCHANGE_CLIENT_TARGET) $?

copyfile: $(SETTINGS)

test: $(ALL_OBJS) $(TEST_OBJS)
	$(COMPILE_COMMAND) $(TEST_LIBS) -o $(TEST_TARGET) $^

lint:
	$(LINT_COMMAND) --root=$(SRC_PATH) --linelength=200 --filter=-legal/copyright --extensions=cc,h  $(ALL_FILES)
	
clean:
	-rm -f $(BIN_PATH)/* 

	