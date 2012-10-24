/// UNCLASSIFIED
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "qtuioplugin.hpp"
#include "qtuiotouch.hpp"
#include <QDebug>

namespace qtuio {

QTuioPlugin::QTuioPlugin(QObject* parent)
        : QGenericPlugin(parent)
{
}

QTuioPlugin::~QTuioPlugin()
{
}

QObject*
QTuioPlugin::create(const QString& key, const QString& spec)
{
        if (!key.compare(QLatin1String("TuioTouch"), Qt::CaseInsensitive)) {
                QScopedPointer<QTuioTouchScreenHandler> myHandler(new QTuioTouchScreenHandler(spec));
                myHandler->start();
                return myHandler.take();
        }
        return 0;
}

} // namespace qtuio
