/// UNCLASSIFIED
#ifndef QASIO_HEAD
#define QASIO_HEAD

#include <QObject>
#include <QTimer>

#include <boost/asio/io_service.hpp>

namespace qtuio {

// wraps the boost::asio::io_service into the Qt main loop
class QAsioIOService : public QObject
{
        Q_OBJECT;
public:
        QAsioIOService(QObject* parent = 0);
        virtual ~QAsioIOService();

        boost::asio::io_service& get_io_service() { return service_; }

public Q_SLOTS:
        void start();
        void stop();

private:
        boost::asio::io_service service_;
        QScopedPointer<boost::asio::io_service::work> work_;
        QScopedPointer<QTimer> timer_; // used to service the io_service work

private Q_SLOTS:
        void pollIOService();
};     // class QAsioIOService
}      // namespace qtuio
#endif // QASIO_HEAD
