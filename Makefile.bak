include config.mk

ROOT = .

SUBDIRS = src

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

cme:
	cd src && make cme
	
cmeD:
	cd src && make cmeD

.PHONY : clean
clean:
	-rm -r $(BINDIR)/*	
	-rm -r $(RLS_DIR)/*
	-rm -r $(DBG_DIR)/*
	-rm -r $(MAIN_RLS_DIR)/*
	-rm -r $(MAIN_DBG_DIR)/*
	-rm -r $(TEST_RLS_DIR)/*
	-rm -r $(TEST_DBG_DIR)/*
	-rm -r $(TEST_MAIN_RLS_DIR)/*
	-rm -r $(TEST_MAIN_DBG_DIR)/*
	
