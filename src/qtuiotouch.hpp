/// UNCLASSIFIED
/// based on qevdevtouch.cpp from qt-5.0
#ifndef DCC_QTUIOTOUCH_HEAD
#define DCC_QTUIOTOUCH_HEAD

#include <QObject>
#include <QThread>

namespace qtuio {

class QTuioTouchScreenHandler : public QObject
{
        Q_OBJECT;

public:
        QTuioTouchScreenHandler(const QString& spec = QString(), QObject* parent = 0);
        virtual ~QTuioTouchScreenHandler();
};     // class QTuioTouchScreenHandler

class QTuioTouchScreenHandlerThread : public QThread
{
public:
        QTuioTouchScreenHandlerThread(const QString& spec, QObject* parent = 0);
        virtual ~QTuioTouchScreenHandlerThread();

        virtual void run();
private:
        QString spec_;
};     // class QTuioTouchScreenHandlerThread
}      // namespace qtuio
#endif // DCC_QTUIOTOUCH_HEAD
