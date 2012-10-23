/// UNCLASSIFIED
#ifndef Q_TUIO_PLUGIN_HEAD
#define Q_TUIO_PLUGIN_HEAD

#include <QtGui/qgenericplugin.h>

namespace qtuio {

class __attribute__ ((visibility ("default"))) QTuioPlugin : public QGenericPlugin
{
        Q_OBJECT;
        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "qtuio.json");

public:
        QTuioPlugin(QObject* parent = 0);
        virtual ~QTuioPlugin();

        virtual QObject* create(const QString& key, const QString& specification);
};     // class Q
}      // namespace qtuio
#endif // Q_TUIO_PLUGIN_HEAD
