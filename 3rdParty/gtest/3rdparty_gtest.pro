#-------------------------------------------------
#
# Project created by QtCreator 2011-07-01T10:35:00
#
#-------------------------------------------------

QT       -= core
TARGET = 3rdparty_gtest
TEMPLATE = lib
CONFIG += staticlib
CONFIG += create_prl
CONFIG += link_prl

builddir = ../../build
DESTDIR = $$builddir

# specify builddir BEFORE  include make-settings.pri
! include( ../../make-test-settings.pri ) {
    error( Couldn't find the make-test-settings.pri file! )
}

GTEST_DIR = gtest-1.6.0__stripped
GTEST_INCLUDE = $$GTEST_DIR/include
GTEST_INCLUDE_DIR = $$GTEST_DIR/include/gtest
GTEST_SRC_DIR = $$GTEST_DIR/src

HEADERS = $$GTEST_INCLUDE_DIR/gtest-death-test.h \
          $$GTEST_INCLUDE_DIR/gtest.h \
          $$GTEST_INCLUDE_DIR/gtest-message.h \
          $$GTEST_INCLUDE_DIR/gtest-param-test.h \
          $$GTEST_INCLUDE_DIR/gtest-param-test.h.pump \
          $$GTEST_INCLUDE_DIR/gtest_pred_impl.h \
          $$GTEST_INCLUDE_DIR/gtest-printers.h \
          $$GTEST_INCLUDE_DIR/gtest_prod.h \
          $$GTEST_INCLUDE_DIR/gtest-spi.h \
          $$GTEST_INCLUDE_DIR/gtest-test-part.h \
          $$GTEST_INCLUDE_DIR/gtest-typed-test.h
# Add internals to headers
HEADERS += $$GTEST_INCLUDE_DIR/internal/gtest-death-test-internal.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-filepath.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-internal.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-linked_ptr.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-param-util-generated.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-param-util-generated.h.pump \
           $$GTEST_INCLUDE_DIR/internal/gtest-param-util.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-port.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-string.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-tuple.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-tuple.h.pump \
           $$GTEST_INCLUDE_DIR/internal/gtest-type-util.h \
           $$GTEST_INCLUDE_DIR/internal/gtest-type-util.h.pump


SOURCES = $$GTEST_SRC_DIR/gtest-all.cc \
          $$GTEST_SRC_DIR/gtest.cc \
          $$GTEST_SRC_DIR/gtest-death-test.cc \
          $$GTEST_SRC_DIR/gtest-filepath.cc \
#          $$GTEST_SRC_DIR/gtest-internal-inl.h \
          $$GTEST_SRC_DIR/gtest_main.cc \
          $$GTEST_SRC_DIR/gtest-port.cc \
          $$GTEST_SRC_DIR/gtest-printers.cc \
          $$GTEST_SRC_DIR/gtest-test-part.cc \
          $$GTEST_SRC_DIR/gtest-typed-test.cc

INCLUDEPATH += $${GTEST_INCLUDE}
INCLUDEPATH += $${GTEST_DIR}
