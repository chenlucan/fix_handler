
$(BIN_PATH)/market_simulater.o: $(SRC_PATH)/tmalpha/market/market_simulater.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/tmalpha_market_application.o: $(SRC_PATH)/tmalpha/market/tmalpha_market_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/exchange_simulater.o: $(SRC_PATH)/tmalpha/exchange/exchange_simulater.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/tmalpha_exchange_application.o: $(SRC_PATH)/tmalpha/exchange/tmalpha_exchange_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/trade_matching_alpha_test.o: $(SRC_PATH)/main/tradematching/trade_matching_alpha_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<
    
$(BIN_PATH)/trade_matching_exchange_alpha_test.o: $(SRC_PATH)/main/tradematching/trade_matching_exchange_alpha_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/mut_market_simulater.o: $(TEST_PATH)/tmalpha/market/mut_market_simulater.cc
	$(TEST_COMPILE_COMMAND) -c -o $@ $<
    
$(BIN_PATH)/mut_exchange_simulater.o: $(TEST_PATH)/tmalpha/exchange/mut_exchange_simulater.cc
	$(TEST_COMPILE_COMMAND) -c -o $@ $<
    