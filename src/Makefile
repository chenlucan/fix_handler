
.PHONY: all
all: cmeD cme

include objs.mk

ROOT = ..

.PHONY : clean
clean:
	-rm -r $(BINDIR)/cme
	-rm -r $(BINDIR)/cmeD
	-rm -r $(RLS_DIR)/*
	-rm -r $(DBG_DIR)/*
	-rm -r $(MAIN_RLS_DIR)/*
	-rm -r $(MAIN_DBG_DIR)/*

$(MAIN_DBG_DIR)/main.o: main.cc
	g++ -c main.cc -o $(MAIN_DBG_DIR)/main.o $(DBG_FLAG) $(SRC_INCL) $(FLAG)


ALL_CME_DBG_OBJS =	$(MAIN_DBG_DIR)/main.o \
					$(CME_DBG_OBJS)
					
cmeD: $(ALL_CME_DBG_OBJS)
	g++ -o $(ROOT)/_run/bin/cmeD $(ALL_CME_DBG_OBJS) $(DBG_FLAG) $(SRC_INCL) $(LINK) $(FLAG)


$(MAIN_RLS_DIR)/main.o: main.cc
	g++ -c main.cc -o $(MAIN_RLS_DIR)/main.o $(RLS_FLAG) $(SRC_INCL) $(FLAG)


ALL_CME_RLS_OBJS =	$(MAIN_RLS_DIR)/main.o \
				    $(CME_RLS_OBJS)
					
cme: $(ALL_CME_RLS_OBJS)
	g++ -o $(ROOT)/_run/bin/cme $(ALL_CME_RLS_OBJS) $(RLS_FLAG) $(SRC_INCL) $(LINK) $(FLAG)

