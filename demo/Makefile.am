include $(top_srcdir)/build-aux/autotroll.mk

ACLOCAL_AMFLAGS = -I m4 --install

AM_CFLAGS = -fPIE -fvisibility=hidden $(QT_CFLAGS)
AM_CPPFLAGS = $(QT_CPPFLAGS)
AM_CXXFLAGS = -fPIE -fvisibility=hidden -fvisibility-inlines-hidden $(QT_CXXFLAGS)
AM_LDFLAGS = -fPIE -pie $(QT_LDFLAGS)

check_PROGRAMS = \
	src/demo

BUILT_SOURCES = \
	src/graphicsview.moc.cpp \
	src/mainwindow.moc.cpp \
	src/mainwindow.ui.hpp

# demo
src_demo_SOURCES = \
	$(BUILT_SOURCES) \
	src/graphicsview.cpp \
	src/graphicsview.hpp \
	src/main.cpp \
	src/mainwindow.cpp \
	src/mainwindow.hpp
src_demo_LDADD = \
	$(QT_LIBS)
