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
        qDebug() << "Create TUIO Plugin";
        if (!key.compare(QLatin1String("TuioTouch"), Qt::CaseInsensitive)) {
                return new QTuioTouchScreenHandlerThread(spec);
        }
        return 0;
}

} // namespace qtuio
