
$(BIN_PATH)/market_simulater.o: $(SRC_PATH)/tmalpha/market/market_simulater.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/tmalpha_market_application.o: $(SRC_PATH)/tmalpha/market/tmalpha_market_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/trade_matching_alpha_test.o: $(SRC_PATH)/main/tradematching/trade_matching_alpha_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<
    