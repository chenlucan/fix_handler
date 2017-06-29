
$(BIN_PATH)/market_data_converter.o: $(SRC_PATH)/tool/market_data_converter.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/data_converter_test.o: $(SRC_PATH)/main/tool/data_converter_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

