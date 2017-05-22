
$(BIN_PATH)/market_simulater.o: $(SRC_PATH)/tmalpha/market/market_simulater.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/tmalpha_market_application.o: $(SRC_PATH)/tmalpha/market/tmalpha_market_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/exchange_simulater.o: $(SRC_PATH)/tmalpha/exchange/exchange_simulater.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/tmalpha_exchange_application.o: $(SRC_PATH)/tmalpha/exchange/tmalpha_exchange_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/trade_market_state.o: $(SRC_PATH)/tmalpha/trade/trade_market_state.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/trade_market_manager.o: $(SRC_PATH)/tmalpha/trade/trade_market_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/trade_order_box.o: $(SRC_PATH)/tmalpha/trade/trade_order_box.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/trade_order_manager.o: $(SRC_PATH)/tmalpha/trade/trade_order_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/trade_simulater.o: $(SRC_PATH)/tmalpha/trade/trade_simulater.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/tmalpha_trade_application.o: $(SRC_PATH)/tmalpha/trade/tmalpha_trade_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/simulater_market.o: $(SRC_PATH)/tmalpha/trade/market/simulater_market.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/simulater_exchange.o: $(SRC_PATH)/tmalpha/trade/exchange/simulater_exchange.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/trade_matching_alpha_test.o: $(SRC_PATH)/main/tradematching/trade_matching_alpha_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<
    
$(BIN_PATH)/trade_matching_exchange_alpha_test.o: $(SRC_PATH)/main/tradematching/trade_matching_exchange_alpha_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/trade_matching_trade_alpha_test.o: $(SRC_PATH)/main/tradematching/trade_matching_trade_alpha_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/mut_market_simulater.o: $(TEST_PATH)/tmalpha/market/mut_market_simulater.cc
	$(TEST_COMPILE_COMMAND) -c -o $@ $<
    
$(BIN_PATH)/mut_exchange_simulater.o: $(TEST_PATH)/tmalpha/exchange/mut_exchange_simulater.cc
	$(TEST_COMPILE_COMMAND) -c -o $@ $<
    
$(BIN_PATH)/mut_trade_simulater.o: $(TEST_PATH)/tmalpha/trade/mut_trade_simulater.cc
	$(TEST_COMPILE_COMMAND) -c -o $@ $<
    