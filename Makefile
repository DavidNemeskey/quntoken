# Fejlesztoi konfiguracio megadasa (milyen modulok es tesztek forduljanak).
# Uj valtozat keszitesenel egy uj config fajlt kell kesziteni es azt
# megadni a default helyett. Peldaul:
#   make clean
#   make all CONFIG_FILE=alter_config.mk
CONFIG_FILE = default_config.mk
include $(CONFIG_FILE)

# konyvatarak
TARGET_DIR		= bin
LIB_DIR			= lib
TMP_DIR			= tmp
SOURCE_DIR		= src
CPP_DIR			= $(SOURCE_DIR)/cpp
SCRIPTS_DIR		= $(SOURCE_DIR)/scripts
GTEST_DIR		= googletest/googletest
GTEST_HEADERS	= $(GTEST_DIR)/include/gtest/*.h $(GTEST_DIR)/include/gtest/internal/*.h
QUEX_DIR		= quex


### forditok kapcsoloi ########################################################

# c preprocesszor
CPPFLAGS +=	-isystem $(GTEST_DIR)/include

# g++ kapcsoloi altalaban
CXXFLAGS +=	-Wall \
			-Wextra \
			-Wconversion \
			-std=c++11 \
			-I$(CPP_DIR) \
			-I$(TMP_DIR) \
			-Werror \
			# -static \
			# -g

# g++ kapcsoloi quex-es fajlokhoz
CXXFLAGS_QUEX =	$(CXXFLAGS) \
				-I$(QUEX_DIR) \
				-I./ \
				-DQUEX_OPTION_ASSERTS_DISABLED \
				-DQUEX_OPTION_MULTI \
				# -DQUEX_OPTION_CONVERTER_ICONV \
				# -liconv
				# -DENCODING_NAME='"UTF8"' \
				# -static \

# g++ kapcsoloi gtest-es fajlokhoz
CXXFLAGS_GTEST =	$(CXXFLAGS) \
					-pthread \

# a quex kapcsoloi
QUEXFLAGS =	-i $^ \
			-b 1 \
			--odir $(TMP_DIR)/ \
			--codec utf8
			# --iconv
			# --icu
# Megj1: icu konverter használata:
#   - telepíteni kell a libicu52-t és a libicu-dev-et
#   - quex-nek kell az --icu kapcsoló
#   - g++-nak linkelésnél kell a `icu-config --ldflags`, ami visszaadja a
#     linkernek szánt paramétereket (valami érthetetlen oknál fogva nem mindegy
#     az `icu-config --ldflags` helye a kapcsolók között!)
#   - példa: ../quex/Demo/Cpp/003/-ban
# Megj2: -b 2 is lehetne, de ekkor egyes unicode char. propert.-eket
# használó szabálynál pampog a quex, hogy 2 bájtba nem fér minden bele

# parhuzamositas
NUMCPUS = `grep -c '^processor' /proc/cpuinfo`
MAKE += -j $(NUMCPUS)

#####  M A I N   T A R G E T S  ###############################################
all: common quntoken test

.PHONY: all


quntoken: $(TARGET_DIR)/quntoken

.PHONY: quntoken


test: $(TARGET_DIR)/test
	./$(TARGET_DIR)/test 2>/dev/null

.PHONY: test


install: prepare install_gtest install_quex

.PHONY: install


update: update_gtest update_quex

.PHONY: update


clean:
	rm -rfv $(TARGET_DIR)/*
	rm -rfv $(LIB_DIR)/*
	rm -rfv $(TMP_DIR)/*

.PHONY: clean


######  A U X I L I A R Y   T A R G E T S  ####################################
### common files
common:
	$(MAKE) quex_files
	$(MAKE) object_files

### binaries
$(TARGET_DIR)/quntoken: $(TMP_DIR)/main.o $(LIB_DIR)/libquntoken.a
	$(CXX) $< -L$(LIB_DIR) -lquntoken -o $@
	# $(CXX) $< -L$(LIB_DIR) -lquntoken `icu-config --ldflags` -o $@

$(TARGET_DIR)/test: $(TMP_DIR)/test.o $(LIB_DIR)/libquntoken.a $(TMP_DIR)/gtest.a
	$(CXX) $(CXXFLAGS_GTEST) -lpthread $^ -o $@
	# $(CXX) $(CXXFLAGS_GTEST) -lpthread $^ -o $@ `icu-config --ldflags`

### libraries
$(LIB_DIR)/libquntoken.a: $(TMP_DIR)/prep.o $(TMP_DIR)/snt.o $(TMP_DIR)/sntcorr.o $(TMP_DIR)/token.o $(TMP_DIR)/converter.o $(TMP_DIR)/qx_module.o $(TMP_DIR)/qx_module_queue.o $(TMP_DIR)/quntoken_api.o
	$(AR) rscv $@ $^

# TODO: .o files should have been compiled using option -fpic for this
#$(LIB_DIR)/libquntoken.so: $(TMP_DIR)/prep.o $(TMP_DIR)/snt.o $(TMP_DIR)/sntcorr.o $(TMP_DIR)/token.o $(TMP_DIR)/converter.o
#	mkdir -p $(LIB_DIR)
#	$(CXX) -shared $^ -Wl,-soname,libquntoken.so -o $@


### object files
object_files: $(TMP_DIR)/prep.o $(TMP_DIR)/snt.o $(TMP_DIR)/sntcorr.o $(TMP_DIR)/token.o $(TMP_DIR)/converter.o $(TMP_DIR)/qx_module.o $(TMP_DIR)/qx_module_queue.o $(TMP_DIR)/quntoken_api.o $(TMP_DIR)/test.o $(TMP_DIR)/main.o

.PHONY: object_files

$(TMP_DIR)/main.o: $(CPP_DIR)/main.cpp $(CPP_DIR)/*.h $(CPP_DIR)/quntoken_api.h
	$(CXX) -o $@ $(CXXFLAGS_QUEX) -c $<

$(TMP_DIR)/test.o: $(TMP_DIR)/test.cpp $(CPP_DIR)/*.h quex_files
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS_QUEX) -c $<

$(TMP_DIR)/quntoken_api.o: $(CPP_DIR)/quntoken_api.cpp $(CPP_DIR)/quntoken_api.h $(CPP_DIR)/qx_module_queue.h
	$(CXX) -o $@ $(CXXFLAGS_QUEX) -c $<

$(TMP_DIR)/qx_module_queue.o: $(CPP_DIR)/qx_module_queue.cpp $(CPP_DIR)/qx_module_queue.h $(CPP_DIR)/quntoken_api.h $(CPP_DIR)/converter.h $(CPP_DIR)/qx_module.h quex_files
	$(CXX) -o $@ $(CXXFLAGS_QUEX) -c $<

$(TMP_DIR)/qx_module.o: $(CPP_DIR)/qx_module.cpp $(CPP_DIR)/qx_module.h $(CPP_DIR)/quntoken_api.h quex_files
	$(CXX) -o $@ $(CXXFLAGS_QUEX) -c $<

$(TMP_DIR)/converter.o: $(CPP_DIR)/converter.cpp $(CPP_DIR)/converter.h $(CPP_DIR)/quntoken_api.h
	$(CXX) -o $@ $(CXXFLAGS) -c $<

$(TMP_DIR)/prep.o: $(TMP_DIR)/prep_prep_lexer.cpp
	$(CXX) -o $@ $(CXXFLAGS_QUEX) -c $<

$(TMP_DIR)/snt.o: $(TMP_DIR)/snt_snt_lexer.cpp
	$(CXX) -o $@ $(CXXFLAGS_QUEX) -c $<

$(TMP_DIR)/sntcorr.o: $(TMP_DIR)/sntcorr_sntcorr_lexer.cpp
	$(CXX) -o $@ $(CXXFLAGS_QUEX) -c $<

$(TMP_DIR)/token.o: $(TMP_DIR)/token_token_lexer.cpp
	$(CXX) -o $@ $(CXXFLAGS_QUEX) -c $<


### quex
# parancsok
QUEX_CMD		= export QUEX_PATH=$(QUEX_DIR) ; $(QUEX_DIR)/quex-exe.py

quex_files: $(TMP_DIR)/prep_prep_lexer.cpp $(TMP_DIR)/snt_snt_lexer.cpp $(TMP_DIR)/sntcorr_sntcorr_lexer.cpp $(TMP_DIR)/token_token_lexer.cpp

.PHONY: quex_files

$(TMP_DIR)/prep_prep_lexer.cpp: $(DEFINITIONS) $(PREP_MODULE)
	$(QUEX_CMD)	$(QUEXFLAGS) \
				-o prep::prep_lexer \
				--token-id-prefix PREP_

$(TMP_DIR)/snt_snt_lexer.cpp: $(DEFINITIONS) $(SNT_MODULE)
	$(QUEX_CMD)	$(QUEXFLAGS) \
				-o snt::snt_lexer \
				--token-id-prefix SNT_

$(TMP_DIR)/sntcorr_sntcorr_lexer.cpp: $(DEFINITIONS) $(TMP_DIR)/abbrev.qx $(SNTCORR_MODULE)
	$(QUEX_CMD)	$(QUEXFLAGS) \
				-o sntcorr::sntcorr_lexer \
				--token-id-prefix SNTCORR_

$(TMP_DIR)/token_token_lexer.cpp: $(DEFINITIONS) $(TOKEN_MODULE)
	$(QUEX_CMD)	$(QUEXFLAGS) \
				-o token::token_lexer \
				--token-id-prefix TOKEN_

# roviditeseket tartalmayo abbrev.qx generalasa
$(TMP_DIR)/abbrev.qx: $(SCRIPTS_DIR)/generate_abbrev.qx.py $(ABBREVIATIONS)
	./$< -d $(word 2, $^) -o $@


### test.cpp
$(TMP_DIR)/test.cpp: $(SCRIPTS_DIR)/test.tmpl2cpp.py $(CPP_DIR)/test.cpp.tmpl $(TEST_FILES)
	./$< -t $(word 2, $^) -d $(wordlist 3, $(words $^), $^) -o $@

### gtest
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

$(TMP_DIR)/gtest-all.o: $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS_GTEST) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $@

$(TMP_DIR)/gtest_main.o: $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS_GTEST) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $@

$(TMP_DIR)/gtest.a : $(TMP_DIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(TMP_DIR)/gtest_main.a : $(TMP_DIR)/gtest-all.o $(TMP_DIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^


######  I N S T A L L   A N D   U P D A T E  ##################################
CMD_INSTALL_GTEST = git clone https://github.com/google/googletest.git
CMD_UPDATE_GTEST = cd $(GTEST_DIR) ; git pull
QUEX_STABLE_VERSION = quex-0.65.4
QUEX_LINK = downloads.sourceforge.net/project/quex/HISTORY/0.65/$(QUEX_STABLE_VERSION).tar.gz
# CMD_INSTALL_QUEX = cd $(TMP_DIR) ; \
# 				   svn checkout https://svn.code.sf.net/p/quex/code/trunk ; \
#                    mv trunk/ ../$(QUEX_DIR)
CMD_INSTALL_QUEX = \
	rm -rvf $(QUEX_DIR) ; \
	cd $(TMP_DIR) ; \
	wget $(QUEX_LINK) ; \
	tar zxvf $(QUEX_STABLE_VERSION).tar.gz ; \
	mv $(QUEX_STABLE_VERSION)/ ../$(QUEX_DIR) ; \
	rm $(QUEX_STABLE_VERSION).tar.gz
CMD_UPDATE_QUEX = cd $(QUEX_DIR) ; svn up

prepare:
	mkdir -p $(TARGET_DIR)
	mkdir -p $(TMP_DIR)
	mkdir -p $(LIB_DIR)

.PHONY: prepare

install_gtest:
	if ! [ -d $(GTEST_DIR) ] ; then $(CMD_INSTALL_GTEST) ; fi

.PHONY: install_gtest

# TODO: a jelenlegi verzioval nem fordul a quex. A 0.65.4-os az utolso
# hasznalhato verzio. Csak a downgrade utan lesz jo.
install_quex:
	if ! [ -d $(QUEX_DIR) ] ; then $(CMD_INSTALL_QUEX) ; fi

.PHONY: install_quex

# # Ez csak egy biztonsagi lehetoseg. Utana nem lesz frissitheto a quex (TODO).
# downgrade_quex:
# 	rm -rf $(QUEX_DIR) ; \
# 	cd $(TMP_DIR) ; \
# 	wget $(QUEX_LINK) ; \
# 	tar zxvf $(QUEX_STABLE_VERSION).tar.gz ; \
# 	mv $(QUEX_STABLE_VERSION)/ ../$(QUEX_DIR) ; \
# 	rm $(QUEX_STABLE_VERSION).tar.gz

.PHONY: downgrade_quex

update_gtest:
	if [ -d $(GTEST_DIR) ] ; then $(CMD_UPDATE_GTEST) ; fi

.PHONY: update_gtest

update_quex:
	if [ -d $(QUEX_DIR) ] ; then $(CMD_UPDATE_QUEX) ; fi

.PHONY: update_quex




