/// UNCLASSIFIED
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "qtuiotouch.hpp"
// based on implementation of qevdevtouch.cpp from Qt 5.0 Beta 1

namespace qtuio {

QTuioTouchScreenHandler::QTuioTouchScreenHandler(const QString& spec, QObject* parent)
        : QObject(parent)
{
        setObjectName(QLatin1String("TUIO Touch Handler"));
}

QTuioTouchScreenHandler::~QTuioTouchScreenHandler()
{
}

QTuioTouchScreenHandlerThread::QTuioTouchScreenHandlerThread(const QString& spec, QObject* parent)
        : QThread(parent)
        , spec_(spec)
{
        start();
}

QTuioTouchScreenHandlerThread::~QTuioTouchScreenHandlerThread()
{
        quit();
        wait();
}

void
QTuioTouchScreenHandlerThread::run()
{
        QScopedPointer<QTuioTouchScreenHandler> handler(new QTuioTouchScreenHandler(spec_));
        exec();
}

} // namespace qtuio
