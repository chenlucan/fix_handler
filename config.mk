DBG_FLAG = -g  
RLS_FLAG = -O3

ROOT = .
INSTALLDIR = $(ROOT)/_run
BINDIR     = $(INSTALLDIR)/bin

BOOST_INCLUDE = vendor/boost_1_56_0/include
BOOST_LIB     = vendor/boost_1_56_0/lib

QUICKFIX_INCLUDE   = vendor/quickfix/include
QUICKFIX_LIB       = vendor/quickfix/lib

GTEST_INCLUDE = vendor/gmock-1.7.0/gtest/include/
GMOCK_INCLUDE = vendor/gmock-1.7.0/include

GMOCK_LIB = vendor/gmock-1.7.0/lib

PROTOBUF_INCLUDE = vendor/protobuf-2.6.1/include
PROTOBUF_SRC     = vendor/protobuf-2.6.1/src
PROTOBUF_LIB     = vendor/protobuf-2.6.1/lib
PROTOC           = vendor/protobuf-2.6.1/bin/protoc
PROTO_PATH       = src/pb
PROTO_BUILD_PB_PATH = src/build/pb
PROTO_BUILD_PATH = src/build

FLAG = -std=c++11

RLS_DIR      = $(ROOT)/_build/src/rls
DBG_DIR      = $(ROOT)/_build/src/dbg
TEST_RLS_DIR = $(ROOT)/_build/test/rls
TEST_DBG_DIR = $(ROOT)/_build/test/dbg
MAIN_RLS_DIR = $(ROOT)/_build/src/main/rls
MAIN_DBG_DIR = $(ROOT)/_build/src/main/dbg
TEST_MAIN_DBG_DIR = $(ROOT)/_build/test/main/dbg
TEST_MAIN_RLS_DIR = $(ROOT)/_build/test/main/rls

SRC_DIR  = $(ROOT)/src
TEST_DIR = $(ROOT)/test

LINK = -L"$(ROOT)/$(BOOST_LIB)"    \
       -L"$(ROOT)/$(QUICKFIX_LIB)" \
       -L"$(ROOT)/$(PROTOBUF_LIB)" \
       -pthread \
       -ldl