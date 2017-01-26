    
COMPILER = g++

SRC_PATH = .
BIN_PATH = .

LIBS_PATH = -L/usr/lib64
LIBS = -lpthread -lboost_system -lzmq -lstdc++
RELEASE_FLAGS = -O3 -DNDEBUG -Ofast
DBG_FLAGS = -g -rdynamic
FLAGS = -std=c++11 -Wall -Wno-pragmas $(DBG_FLAGS)

OBJS = $(BIN_PATH)/*.o
SENDER_TARGET = $(BIN_PATH)/udp_sender_test
RECEIVER_TARGET = $(BIN_PATH)/udp_receiver_test
SBE_TARGET = $(BIN_PATH)/sbe_test
ZMQ_TARGET = $(BIN_PATH)/zmq_receiver_test
    
%.o: %.cc
	$(COMPILER) $(LIBS_PATH) $(LIBS) $(FLAGS) -c -o $@ $<

all: usender receiver sbe zmqrec
    
usender: $(BIN_PATH)/udp_sender_test.o $(BIN_PATH)/sbe_encoder.o $(BIN_PATH)/utility.o $(BIN_PATH)/logger.o
	$(COMPILER) $(LIBS_PATH) $(LIBS) $(FLAGS) -o $(SENDER_TARGET) $?

receiver: $(BIN_PATH)/dat_processor.o $(BIN_PATH)/udp_receiver.o $(BIN_PATH)/udp_receiver_test.o $(BIN_PATH)/sbe_encoder.o $(BIN_PATH)/sbe_decoder.o $(BIN_PATH)/utility.o $(BIN_PATH)/mdp_message.o $(BIN_PATH)/dat_saver.o $(BIN_PATH)/tcp_receiver.o $(BIN_PATH)/dat_arbitrator.o $(BIN_PATH)/application.o $(BIN_PATH)/zmq_sender.o $(BIN_PATH)/settings.o $(BIN_PATH)/logger.o
	$(COMPILER) $(LIBS_PATH) $(LIBS) $(FLAGS) -o $(RECEIVER_TARGET) $?

sbe: $(BIN_PATH)/sbe_encoder.o $(BIN_PATH)/sbe_decoder.o $(BIN_PATH)/sbe_test.o $(BIN_PATH)/utility.o $(BIN_PATH)/logger.o
	$(COMPILER) $(LIBS_PATH) $(LIBS) $(FLAGS) -o $(SBE_TARGET) $?

zmqrec: $(BIN_PATH)/zmq_receiver.o $(BIN_PATH)/zmq_receiver_test.o $(BIN_PATH)/logger.o $(BIN_PATH)/utility.o $(BIN_PATH)/sbe_encoder.o
	$(COMPILER) $(LIBS_PATH) $(LIBS) $(FLAGS) -o $(ZMQ_TARGET) $?

clean:
	-rm -f $(OBJS) 
	-rm -f $(SENDER_TARGET)
	-rm -f $(RECEIVER_TARGET)
	-rm -f $(SBE_TARGET)
	-rm -f $(ZMQ_TARGET)
