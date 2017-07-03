include ../config.mk

ROOT = ..

SRC_INCL =	-I"$(ROOT)/src" \
			-I"$(ROOT)/$(QUICKFIX_INCLUDE)"

SRC_DBG_BUILD = $(DBG_FLAG) $(SRC_INCL) $(FLAG)
SRC_RLS_BUILD = $(RLS_FLAG) $(SRC_INCL) $(FLAG)

#cme
$(DBG_DIR)/cme_market_cmemarket.o: $(SRC_DIR)/cme/market/cmemarket.cc
	g++ -c $(SRC_DIR)/cme/market/cmemarket.cc -o $(DBG_DIR)/cme_market_cmemarket.o $(SRC_DBG_BUILD)

CME_DBG_OBJS =	$(DBG_DIR)/cme_market_cmemarket.o




#cme
$(RLS_DIR)/cme_market_cmemarket.o: $(SRC_DIR)/cme/market/cmemarket.cc
	g++ -c $(SRC_DIR)/cme/market/cmemarket.cc -o $(RLS_DIR)/cme_market_cmemarket.o $(SRC_RLS_BUILD)

CME_RLS_OBJS =	$(RLS_DIR)/cme_market_cmemarket.o
