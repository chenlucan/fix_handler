
$(BIN_PATH)/application.o: $(SRC_PATH)/cme/market/application.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/dat_arbitrator.o: $(SRC_PATH)/cme/market/dat_arbitrator.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/dat_processor.o: $(SRC_PATH)/cme/market/dat_processor.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/dat_replayer.o: $(SRC_PATH)/cme/market/dat_replayer.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/dat_saver.o: $(SRC_PATH)/cme/market/dat_saver.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/recovery_saver.o: $(SRC_PATH)/cme/market/recovery_saver.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/book_manager.o: $(SRC_PATH)/cme/market/book_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/book_sender.o: $(SRC_PATH)/cme/market/book_sender.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/definition_manager.o: $(SRC_PATH)/cme/market/definition_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/book_state_controller.o: $(SRC_PATH)/cme/market/book_state_controller.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/sbe_decoder.o: $(SRC_PATH)/cme/market/message/sbe_decoder.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/sbe_encoder.o: $(SRC_PATH)/cme/market/message/sbe_encoder.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/message_parser_d.o: $(SRC_PATH)/cme/market/message/message_parser_d.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/message_parser_f.o: $(SRC_PATH)/cme/market/message/message_parser_f.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/message_parser_r.o: $(SRC_PATH)/cme/market/message/message_parser_r.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/message_parser_w.o: $(SRC_PATH)/cme/market/message/message_parser_w.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/message_parser_x.o: $(SRC_PATH)/cme/market/message/message_parser_x.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/mdp_message.o: $(SRC_PATH)/cme/market/message/mdp_message.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/mdp_receiver.o: $(SRC_PATH)/cme/market/persist/mdp_receiver.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/mdp_saver.o: $(SRC_PATH)/cme/market/persist/mdp_saver.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/message_utility.o: $(SRC_PATH)/cme/market/message/message_utility.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/market_settings.o: $(SRC_PATH)/cme/market/setting/market_settings.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/channel_settings.o: $(SRC_PATH)/cme/market/setting/channel_settings.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/exchange_application.o: $(SRC_PATH)/cme/exchange/exchange_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/globex_communicator.o: $(SRC_PATH)/cme/exchange/globex_communicator.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/globex_logger.o: $(SRC_PATH)/cme/exchange/globex_logger.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/order_manager.o: $(SRC_PATH)/cme/exchange/order_manager.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/strategy_communicator.o: $(SRC_PATH)/cme/exchange/strategy_communicator.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/exchange_settings.o: $(SRC_PATH)/cme/exchange/exchange_settings.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/logger.o: $(SRC_PATH)/core/assist/logger.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/time_measurer.o: $(SRC_PATH)/core/assist/time_measurer.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/settings.o: $(SRC_PATH)/core/assist/settings.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/utility.o: $(SRC_PATH)/core/assist/utility.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/tcp_receiver.o: $(SRC_PATH)/core/tcp/tcp_receiver.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/udp_receiver.o: $(SRC_PATH)/core/udp/udp_receiver.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/zmq_receiver.o: $(SRC_PATH)/core/zmq/zmq_receiver.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/zmq_sender.o: $(SRC_PATH)/core/zmq/zmq_sender.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/zmq_receiver_test.o: $(SRC_PATH)/main/market/zmq_receiver_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/udp_receiver_test.o: $(SRC_PATH)/main/market/udp_receiver_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/udp_sender_test.o: $(SRC_PATH)/main/market/udp_sender_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/tcp_sender_test.o: $(SRC_PATH)/main/market/tcp_sender_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/sbe_test.o: $(SRC_PATH)/main/market/sbe_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/exchange_server_test.o: $(SRC_PATH)/main/exchange/server/exchange_server_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/server_application.o: $(SRC_PATH)/main/exchange/server/server_application.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/server_market.o: $(SRC_PATH)/main/exchange/server/server_market.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/strategy_test.o: $(SRC_PATH)/main/strategy/strategy_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/exchange_client_test.o: $(SRC_PATH)/main/exchange/client/exchange_client_test.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/ems.pb.o: $(SRC_PATH)/pb/ems/ems.pb.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/dms.pb.o: $(SRC_PATH)/pb/dms/dms.pb.cc
	$(COMPILE_COMMAND) -c -o $@ $<

$(BIN_PATH)/%.xml: $(SRC_PATH)/%.xml
	cp -f  $< $@
	
$(BIN_PATH)/%.ini: $(SRC_PATH)/%.ini
	cp -f  $< $@
	
$(BIN_PATH)/%.cfg: $(SRC_PATH)/%.cfg
	cp -f  $< $@
	
$(BIN_PATH)/utility_unittest.o: $(TEST_PATH)/core/assist/utility_unittest.cc
	$(COMPILE_COMMAND) -c -o $@ $<
	