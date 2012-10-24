/// UNCLASSIFIED
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "qasioioservice.hpp"
#include <QDebug>

namespace qtuio {

QAsioIOService::QAsioIOService(QObject* parent)
        : QObject(parent)
        , service_()
        , work_()
        , timer_()
{
}

QAsioIOService::~QAsioIOService()
{
        service_.stop();
        if (timer_) {
                timer_->stop();
        }
}

void
QAsioIOService::start()
{
        if (timer_) return;

        QScopedPointer<QTimer>my_timer(new QTimer(this));
        if (my_timer) {
                timer_.swap(my_timer);
                connect(timer_.data(), &QTimer::timeout, this, &QAsioIOService::pollIOService);

                using namespace boost;
                service_.reset();
                QScopedPointer<asio::io_service::work> my_work(new asio::io_service::work(service_));
                my_work.swap(work_);
                /// \note Fire every 10 ms; This is the value used by Perceptive Pixel, according to Bob.
                timer_->start(10);
        }
}

void
QAsioIOService::stop()
{
        if (!timer_) return;
        timer_->stop();
        timer_.reset();

        // now drain the io_service
        using namespace boost;
        work_.reset();
        boost::system::error_code error;
        // Should I use poll here to make sure that the GUI does not lock up?
        // I would have to put some time constraints to verify that all of the handlers
        // were executed... Not trivial
        service_.run(error);
        if (error) {
                qDebug() << "Failed to drain I/O service: " << error.message().c_str() << "\n";
        }
}

void
QAsioIOService::pollIOService()
{
        boost::system::error_code error;
        service_.poll(error);
        if (error) {
                qDebug() << "ASIO serivce poll_one failed: " << error.message().c_str() << "\n";
        }
}

} // namespace qtuio
