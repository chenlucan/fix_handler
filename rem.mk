$(BIN_PATH)/rem_market_manager.o: $(SRC_PATH)/rem/market/rem_market_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/rem_market.o: $(SRC_PATH)/rem/market/rem_market.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/rem_market_application.o: $(SRC_PATH)/rem/market/rem_market_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<		
	
$(BIN_PATH)/rem_book_manager.o: $(SRC_PATH)/rem/market/rem_book_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<					

$(BIN_PATH)/rem_market_main_test.o: $(SRC_PATH)/main/rem_maket/rem_market_main.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/rem_communicator.o: $(SRC_PATH)/rem/exchange/rem_communicator.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/rem_exchange_application.o: $(SRC_PATH)/rem/exchange/rem_exchange_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<					

$(BIN_PATH)/rem_exchange_main_test.o: $(SRC_PATH)/main/rem_exchange/rem_exchange_main.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/rem_efh_market_main_test.o: $(SRC_PATH)/main/rem_maket/rem_efh_market_main.cc
	$(COMPILE_COMMAND) -c -o $@ $<		
	
$(BIN_PATH)/rem_efhmarket.o: $(SRC_PATH)/rem/efhmarket/rem_efhmarket.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/rem_guava_quote.o: $(SRC_PATH)/rem/efhmarket/rem_guava_quote.cpp
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/rem_efhmarket_manager.o: $(SRC_PATH)/rem/efhmarket/rem_efhmarket_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/rem_socket_multicast.o: $(SRC_PATH)/rem/efhmarket/rem_socket_multicast.cpp
	g++ -c -g -Wall -DDEBUG -I $(SRC_PATH)/rem/efhmarket/ -o $@ $<		
	
$(BIN_PATH)/mut_rem_book_manager.o: $(TEST_PATH)/rem/market/mut_rem_book_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/mut_rem_matket.o: $(TEST_PATH)/rem/market/mut_rem_matket.cc
	$(COMPILE_COMMAND) -c -o $@ $<		
	
$(BIN_PATH)/mut_rem_market_manager.o: $(TEST_PATH)/rem/market/mut_rem_market_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<			
	
$(BIN_PATH)/mut_rem_exchange.o: $(TEST_PATH)/rem/exchange/mut_rem_exchange.cc
	$(COMPILE_COMMAND) -c -o $@ $<	