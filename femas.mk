$(BIN_PATH)/femas_market_manager.o: $(SRC_PATH)/femas/market/femas_market_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/femas_market.o: $(SRC_PATH)/femas/market/femas_market.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/femas_market_application.o: $(SRC_PATH)/femas/market/femas_market_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<		
	
$(BIN_PATH)/femas_book_manager.o: $(SRC_PATH)/femas/market/femas_book_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<					

$(BIN_PATH)/femas_market_main_test.o: $(SRC_PATH)/main/femas_maket/femas_market_main.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/communicator.o: $(SRC_PATH)/femas/exchange/communicator.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/FemasUstpFtdcTraderManger.o: $(SRC_PATH)/femas/exchange/FemasUstpFtdcTraderManger.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/femas_exchange_application.o: $(SRC_PATH)/femas/exchange/femas_exchange_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<					

$(BIN_PATH)/femas_exchange_main_test.o: $(SRC_PATH)/main/femas_exchange/femas_exchange_main.cc
	$(COMPILE_COMMAND) -c -o $@ $<		
	
$(BIN_PATH)/mut_femas_book_manager.o: $(TEST_PATH)/femas/market/mut_femas_book_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/mut_femas_matket.o: $(TEST_PATH)/femas/market/mut_femas_matket.cc
	$(COMPILE_COMMAND) -c -o $@ $<		
	
$(BIN_PATH)/mut_femas_market_manager.o: $(TEST_PATH)/femas/market/mut_femas_market_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<		
	
$(BIN_PATH)/mut_femas_exchange.o: $(TEST_PATH)/femas/exchange/mut_femas_exchange.cc
	$(COMPILE_COMMAND) -c -o $@ $<			