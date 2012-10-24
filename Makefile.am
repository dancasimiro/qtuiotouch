include $(top_srcdir)/build-aux/autotroll.mk

ACLOCAL_AMFLAGS = -I m4 --install

AM_CFLAGS = -fvisibility=hidden
AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CXXFLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
AM_LDFLAGS =

noinst_LTLIBRARIES = \
	src/dosc/libdosc.la \
	src/tuio/libtuio.la

lib_LTLIBRARIES = src/libqtuiotouch.la

check_PROGRAMS = \
	src/dosc/doscdump \
	src/tuio/tuiodump

BUILT_SOURCES = \
	src/qasioioservice.moc.cpp \
	src/qtuioplugin.moc.cpp \
	src/qtuiotouch.moc.cpp

EXTRA_DIST = src/qtuio.json


# libdosc.la
src_dosc_libdosc_la_SOURCES = \
	src/dosc/decoder.cpp \
	include/dosc/decoder.hpp \
	include/dosc/bytes.hpp \
	include/dosc/asio/udp_receiver.hpp
src_dosc_libdosc_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	@BOOST_CPPFLAGS@
src_dosc_libdosc_la_LDFLAGS = \
	$(AM_LDFLAGS) \
	@BOOST_LDFLAGS@

# doscdump
src_dosc_doscdump_SOURCES = \
	src/dosc/doscdump.cpp
src_dosc_doscdump_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	@BOOST_CPPFLAGS@
src_dosc_doscdump_LDFLAGS = \
	$(AM_LDFLAGS) \
	@BOOST_LDFLAGS@
src_dosc_doscdump_LDADD = \
	src/dosc/libdosc.la \
	@BOOST_ASIO_LIB@ \
	@BOOST_SYSTEM_LIB@

# libtuio.la
src_tuio_libtuio_la_SOURCES = \
	src/tuio/tuio.cpp \
	include/tuio/tuio.hpp
src_tuio_libtuio_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	@BOOST_CPPFLAGS@
src_tuio_libtuio_la_LDFLAGS = \
	$(AM_LDFLAGS) \
	@BOOST_LDFLAGS@

# tuiodump
src_tuio_tuiodump_SOURCES = \
	src/tuio/tuiodump.cpp
src_tuio_tuiodump_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	@BOOST_CPPFLAGS@
src_tuio_tuiodump_LDFLAGS = \
	$(AM_LDFLAGS) \
	@BOOST_LDFLAGS@
src_tuio_tuiodump_LDADD = \
	src/dosc/libdosc.la \
	src/tuio/libtuio.la \
	@BOOST_ASIO_LIB@ \
	@BOOST_SYSTEM_LIB@

# libqtuiotouch.la
src_libqtuiotouch_la_SOURCES = \
	$(BUILT_SOURCES) \
	src/qasioioservice.hpp \
	src/qasioioservice.cpp \
	src/qtuioplugin.hpp \
	src/qtuioplugin.cpp \
	src/qtuiotouch.hpp \
	src/qtuiotouch.cpp
src_libqtuiotouch_la_CXXFLAGS = \
	$(AM_CXXFLAGS) \
	$(QT_CXXFLAGS)
src_libqtuiotouch_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	$(QT_CPPFLAGS) \
	@BOOST_CPPFLAGS@
src_libqtuiotouch_la_LDFLAGS = \
	-module \
	-avoid-version \
	-no-undefined \
	-export-dynamic \
	$(AM_LDFLAGS) \
	$(QT_LDFLAGS) \
	@BOOST_LDFLAGS@
src_libqtuiotouch_la_LIBADD = \
	$(QT_LIBS) \
	src/dosc/libdosc.la \
	src/tuio/libtuio.la \
	@BOOST_ASIO_LIB@ \
	@BOOST_SYSTEM_LIB@
