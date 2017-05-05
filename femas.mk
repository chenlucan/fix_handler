$(BIN_PATH)/femas_market_manager.o: $(SRC_PATH)/femas/market/femas_market_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/femas_market.o: $(SRC_PATH)/femas/market/femas_market.cc
	$(COMPILE_COMMAND) -c -o $@ $<	
	
$(BIN_PATH)/femas_market_application.o: $(SRC_PATH)/femas/market/femas_market_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<		
	
$(BIN_PATH)/femas_book_manager.o: $(SRC_PATH)/femas/market/femas_book_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<					

$(BIN_PATH)/femas_market_main.o: $(SRC_PATH)/main/femas_maket/femas_market_main.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/communicator.o: $(SRC_PATH)/femas/exchange/communicator.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	
$(BIN_PATH)/femas_exchange_application.o: $(SRC_PATH)/femas/exchange/femas_exchange_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<					

$(BIN_PATH)/femas_exchange_main.o: $(SRC_PATH)/main/femas_exchange/femas_exchange_main.cc
	$(COMPILE_COMMAND) -c -o $@ $<		