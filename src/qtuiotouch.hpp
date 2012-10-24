/// UNCLASSIFIED
/// based on qevdevtouch.cpp from qt-5.0
#ifndef DCC_QTUIOTOUCH_HEAD
#define DCC_QTUIOTOUCH_HEAD

#include <QObject>
#include <QScopedPointer>

namespace qtuio {

namespace detail {
struct tuio_impl;
};

/// \note could templatize this on the TUIO transport type. Assume UDP for now.
class QTuioTouchScreenHandler : public QObject
{
        Q_OBJECT;

public:
        QTuioTouchScreenHandler(const QString& spec = QString(), QObject* parent = 0);
        virtual ~QTuioTouchScreenHandler();

        void start();
        void stop();
private:
        QString spec_;
        QScopedPointer<detail::tuio_impl> tuio_;
};     // class QTuioTouchScreenHandler

}      // namespace qtuio
#endif // DCC_QTUIOTOUCH_HEAD
