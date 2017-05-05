$(BIN_PATH)/rem_market_manager.o: $(SRC_PATH)/rem/market/rem_market_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/rem_market.o: $(SRC_PATH)/rem/market/rem_market.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/rem_market_application.o: $(SRC_PATH)/rem/market/rem_market_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<		
	
$(BIN_PATH)/rem_book_manager.o: $(SRC_PATH)/rem/market/rem_book_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<					

$(BIN_PATH)/rem_market_main.o: $(SRC_PATH)/main/rem_maket/rem_market_main.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/rem_communicator.o: $(SRC_PATH)/rem/exchange/rem_communicator.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/rem_exchange_application.o: $(SRC_PATH)/rem/exchange/rem_exchange_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<					

$(BIN_PATH)/rem_exchange_main.o: $(SRC_PATH)/main/rem_exchange/rem_exchange_main.cc
	$(COMPILE_COMMAND) -c -o $@ $<	