/// UNCLASSIFIED
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "qtuiotouch.hpp"
#include "qasioioservice.hpp"
#include "dosc/asio/udp_receiver.hpp"
#include "tuio/tuio.hpp"
#include <string>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/asio.hpp>
#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qwindowsysteminterface.h>
// based on implementation of qevdevtouch.cpp from Qt 5.0 Beta 1

namespace qtuio {
namespace detail {
//static float hw_width() { return 2560.0f; }
//static float hw_height() { return 1440.0f; }

struct tuio_impl {
        QAsioIOService asio;
        tuio::frame_decoder decoder;
        boost::shared_ptr<dosc::udp_receiver> recv;
        QMap<std::string, QTouchDevice*> devices;
        QMap<int, QWindowSystemInterface::TouchPoint> points;

        tuio_impl(QObject* parent, const boost::asio::ip::udp::endpoint& endpoint);

        void register_device(const std::string& name);
        void handle_frame(const tuio::frame& frm);
        void report_points(const tuio::frame& frm);

        QRectF get_screen_rect();
        QRectF get_window_rect();
        QWindowSystemInterface::TouchPoint& fill_qws_touch_point(const tuio::touch_point& tuio_tp, QWindowSystemInterface::TouchPoint& qws_tp);
        QWindowSystemInterface::TouchPoint make_qws_touch_point(const tuio::touch_point& tp);
        QWindowSystemInterface::TouchPoint& update_qws_touch_point(const tuio::touch_point&tp, QWindowSystemInterface::TouchPoint& existing);

        void add_touch_point(const tuio::touch_point& tp);
        void update_touch_point(const tuio::touch_point& tp);
        void remove_touch_point(const tuio::touch_point& tp);
        void drop_touch_point(const tuio::touch_point& tp);
};

tuio_impl::tuio_impl(QObject* parent, const boost::asio::ip::udp::endpoint& endpoint)
        : asio(parent)
        , decoder()
        , recv(boost::make_shared<dosc::udp_receiver>(boost::ref(asio.get_io_service()), endpoint))
        , devices()
        , points()
{
        decoder.set_frame_callback(boost::bind(&tuio_impl::handle_frame, this, _1));
        /// \todo Register explicitly for the tuio 2d profile
        dosc::add_method(*recv, dosc::decoder::all, boost::ref(decoder));
}

// this is hard coded for the perceptive pixel hardware (27" touch screen)
// I don't think that the TUIO protocol advertises the hardware capabilities
void
tuio_impl::register_device(const std::string& name)
{
        /// calling QWindowSystemInterface::registerTouchDevice() takes ownership of the pointer
        QScopedPointer<QTouchDevice> device(new QTouchDevice);
        if (device) {
                device->setName(QString::fromStdString(name));
                device->setType(QTouchDevice::TouchScreen);
                device->setCapabilities(QTouchDevice::Position |
                                        QTouchDevice::Area |
                                        QTouchDevice::Pressure |
                                        QTouchDevice::Velocity |
                                        QTouchDevice::RawPositions |
                                        QTouchDevice::NormalizedPosition);
                devices.insert(name, device.data());
                QWindowSystemInterface::registerTouchDevice(device.take());
                qDebug() << "Registered TUIO device " << QString::fromStdString(name);
        } else {
                /// \todo Throw!
        }
        Q_ASSERT(devices.contains(name));
}

void
tuio_impl::handle_frame(const tuio::frame& frm)
{
        // check if the device has been registered...
        if (!devices.contains(frm.source)) {
                register_device(frm.source);
        }
        boost::for_each(frm.added, boost::bind(&tuio_impl::add_touch_point, this, _1));
        boost::for_each(frm.updated, boost::bind(&tuio_impl::update_touch_point, this, _1));
        boost::for_each(frm.removed, boost::bind(&tuio_impl::remove_touch_point, this, _1));
        report_points(frm);
        boost::for_each(frm.removed, boost::bind(&tuio_impl::drop_touch_point, this, _1));
}

void
tuio_impl::drop_touch_point(const tuio::touch_point& tp)
{
        points.remove(tp.session_id);
}

QRectF
tuio_impl::get_screen_rect()
{
        /// \todo add support for multiple screens...
        return QGuiApplication::primaryScreen()->geometry();
}

QRectF
tuio_impl::get_window_rect()
{
#if 0
        // qevdevtouch has this "m_forceToActiveWindow" member; not sure why...
        if (m_forceToActiveWindow) {
                if (QWindow *win = QGuiApplication::focusWindow()) {
                        return win->geometry();
                }
        }
#endif
        return get_screen_rect();
}

QWindowSystemInterface::TouchPoint&
tuio_impl::fill_qws_touch_point(const tuio::touch_point& tuio_tp, QWindowSystemInterface::TouchPoint& qws_tp)
{
        qws_tp.id = tuio_tp.session_id;

        /// The TUIO touch point arrives in normalized hardware coordinates
        const QRectF screenRect = get_screen_rect();
        //const QRectF winRect = get_window_rect();
        // Generate a screen position that is always within the active window
        // const qreal
        //         xScaleFactor = screenRect.width()  / winRect.width(),
        //         yScaleFactor = screenRect.height() / winRect.height();
        const QPointF screenPos(tuio_tp.position.x * screenRect.width(), tuio_tp.position.y * screenRect.height());

        // QWindowSystemInterface::TouchPoint wants the normalPosition in normalized touch device coordinates (same as TUIO)
        qws_tp.normalPosition = QPointF(tuio_tp.position.x, tuio_tp.position.y);
        // QWindowSystemInterface::TouchPoint wants the area in screen coordinates
        qws_tp.area = QRectF(0, 0, tuio_tp.dims.width * screenRect.width(), tuio_tp.dims.height * screenRect.height());
        qws_tp.area.moveCenter(screenPos);
        qws_tp.pressure = tuio_tp.area;
        // again, the velocity is reported in the screen coordinate system (pixels per second)
        qws_tp.velocity = QVector2D(tuio_tp.velocity.X * screenRect.width(), tuio_tp.velocity.Y * screenRect.height());
        if (tuio_tp.type == tuio::token_touch) {
                qws_tp.flags = QTouchEvent::TouchPoint::Pen;
        }
        QVector<QPointF> myRawPositions = qws_tp.rawPositions;
        myRawPositions << screenPos;
        qws_tp.rawPositions = myRawPositions;
        return qws_tp;
}

QWindowSystemInterface::TouchPoint
tuio_impl::make_qws_touch_point(const tuio::touch_point& tp)
{
        QWindowSystemInterface::TouchPoint touch;
        fill_qws_touch_point(tp, touch);
        touch.state = Qt::TouchPointPressed;
        return touch;
}

QWindowSystemInterface::TouchPoint&
tuio_impl::update_qws_touch_point(const tuio::touch_point& tp, QWindowSystemInterface::TouchPoint& existing)
{
        Q_ASSERT(tp.session_id == existing.id);

        const QPointF normPos(tp.position.x, tp.position.y);
        if (normPos == existing.normalPosition) {
                fill_qws_touch_point(tp, existing);
                existing.state = Qt::TouchPointStationary;
                existing.velocity = QVector2D();
                return existing;
        }
        fill_qws_touch_point(tp, existing);
        existing.state = Qt::TouchPointMoved;
        return existing;
}

void
tuio_impl::add_touch_point(const tuio::touch_point& tp)
{
    if (points.contains(tp.session_id)) {
        qWarning() << "Ignoring ADD command for existing session ID" << tp.session_id;
        return;
    }
    QWindowSystemInterface::TouchPoint touch = make_qws_touch_point(tp);
    points.insert(touch.id, touch);
}

void
tuio_impl::update_touch_point(const tuio::touch_point& tp)
{
    if (!points.contains(tp.session_id)) {
        qWarning() << "Ignoring UPDATE command for missiong session ID" << tp.session_id;
        return;
    }
    update_qws_touch_point(tp, points[tp.session_id]);
}

void
tuio_impl::remove_touch_point(const tuio::touch_point& tp)
{
    if (!points.contains(tp.session_id)) {
        qWarning() << "Ignoring REMOVE command for missing session ID" << tp.session_id;
        return;
    }
    QWindowSystemInterface::TouchPoint& existing = update_qws_touch_point(tp, points[tp.session_id]);
    existing.state = Qt::TouchPointReleased;
}

void
tuio_impl::report_points(const tuio::frame& frm)
{
        QWindowSystemInterface::handleTouchEvent(0, devices.value(frm.source), points.values());
}

} // namespace detail

QTuioTouchScreenHandler::QTuioTouchScreenHandler(const QString& spec, QObject* parent)
        : QObject(parent)
        , spec_(spec)
        , tuio_()
{
        setObjectName(QLatin1String("TUIO Touch Handler"));
}

QTuioTouchScreenHandler::~QTuioTouchScreenHandler()
{
}

void
QTuioTouchScreenHandler::start()
{
        {
                /// \todo read the desired address out of spec_ and resolve it with ASIO;
                const boost::asio::ip::udp::endpoint my_endpoint(boost::asio::ip::udp::v4(), 3333);
                QScopedPointer<detail::tuio_impl> my_tuio(new detail::tuio_impl(this, my_endpoint));
                if (!my_tuio) {
                        // \todo throw!
                }
                tuio_.swap(my_tuio);
        }
        Q_ASSERT(tuio_);

        tuio_->recv->start();
        tuio_->asio.start();
}

void
QTuioTouchScreenHandler::stop()
{
        if (tuio_) {
                tuio_->recv->stop();
                tuio_->asio.stop();
        }
}

} // namespace qtuio
