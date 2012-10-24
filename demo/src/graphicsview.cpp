#include "graphicsview.hpp"
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtGui/QTouchEvent>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QApplication>
#include <QtOpenGL/QGLWidget>

#include <QtWidgets/QGesture>
#include <QtWidgets/QPanGesture>
#include <QtWidgets/QGestureEvent>

static const QPointF swipeTextPos(125.0, 125.0);
static const QPointF panTextPos(125.0, 150.0);
static const QPointF tapTextPos(125.0, 175.0);
static const QPointF tapAndHoldTextPos(125.0, 300.0);
static const QPointF pinchTextPos(125.0, 325.0);

static const QString gestureTemplate("%1 Gesture @ (%2, %3)!");

class MyQGraphicsObject : public QGraphicsObject {
public:
        MyQGraphicsObject(QGraphicsItem* parent = 0);
        virtual ~MyQGraphicsObject();

        virtual QRectF boundingRect() const;
        virtual QPainterPath shape() const;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        virtual bool sceneEvent(QEvent *event);
private:
    QBrush brush_;
};

MyQGraphicsObject::MyQGraphicsObject(QGraphicsItem* parent)
        : QGraphicsObject(parent)
        , brush_(Qt::blue)
{
        grabGesture(Qt::PinchGesture);
        grabGesture(Qt::PanGesture);
        grabGesture(Qt::SwipeGesture);
        grabGesture(Qt::TapAndHoldGesture);
        grabGesture(Qt::TapGesture);
        setAcceptTouchEvents(true);
}

MyQGraphicsObject::~MyQGraphicsObject()
{
}

QRectF
MyQGraphicsObject::boundingRect() const
{
        return QRectF(0, 0, 2560, 1440);
}

QPainterPath
MyQGraphicsObject::shape() const
{
        QPainterPath path;
        path.addEllipse(boundingRect());
        return path;
}

void
MyQGraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setBrush(brush_);
        painter->drawEllipse(boundingRect());
}

bool
MyQGraphicsObject::sceneEvent(QEvent *event)
{
        switch (event->type()) {
        case QEvent::Gesture:
                if (QGestureEvent* pGestureEvent = static_cast<QGestureEvent*>(event)) {
                        if (QPinchGesture* pinch = dynamic_cast<QPinchGesture*>(pGestureEvent->gesture(Qt::PinchGesture))) {
                                const qreal mySF = this->scale() * pinch->scaleFactor();
                                if (mySF > 0.05 && mySF < 10.0) {
                                        this->setScale(mySF);
                                }
                                this->setRotation(pinch->rotationAngle());
                        }
                        if (QPanGesture* pan = dynamic_cast<QPanGesture*>(pGestureEvent->gesture(Qt::PanGesture))) {
                                const QPointF myDelta = pan->delta();
                                this->moveBy(myDelta.x(), myDelta.y());
                        }
//                         if (QGesture* swipe = pGestureEvent->gesture(Qt::SwipeGesture)) {
//                         }
                        if (QGesture* tap = pGestureEvent->gesture(Qt::TapGesture)) {
                                if (tap->state() == Qt::GestureFinished) {
                                        brush_ = QBrush(Qt::blue);
                                        update();
                                } else if (tap->state() == Qt::GestureStarted) {
                                        brush_ = QBrush(Qt::green);
                                        update();
                                } else if (tap->state() == Qt::GestureCanceled) {
                                        brush_ = QBrush(Qt::red);
                                        update();
                                }
                        }
                        // if (QGesture* tapAndHold = pGestureEvent->gesture(Qt::TapAndHoldGesture)) {
                        //         brush_ = QBrush(Qt::cyan);
                        //         update();
                        // }
                }
                return true;
                break;
        default:
                break;
        }
        return QGraphicsObject::sceneEvent(event);
}

class FullScreenButton : public QGraphicsObject {
public:
        FullScreenButton(QGraphicsItem* parent = 0);
        virtual ~FullScreenButton();

        virtual QRectF boundingRect() const;
        virtual QPainterPath shape() const;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        virtual bool sceneEvent(QEvent *event);
private:
        bool windowed_;
};

FullScreenButton::FullScreenButton(QGraphicsItem* parent)
        : QGraphicsObject(parent)
        , windowed_(false)
{
        grabGesture(Qt::TapGesture);
}

FullScreenButton::~FullScreenButton()
{
}

QRectF
FullScreenButton::boundingRect() const
{
        return QRectF(200, 200, 200, 100);
}

QPainterPath
FullScreenButton::shape() const
{
        QPainterPath path;
        path.addRect(boundingRect());
        return path;
}

void
FullScreenButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setBrush(Qt::yellow);
        painter->drawRect(boundingRect());
        painter->drawText(boundingRect(), Qt::AlignCenter, QString("Toggle Full Screen"));
}

bool
FullScreenButton::sceneEvent(QEvent *event)
{
        switch (event->type()) {
        case QEvent::Gesture:
                if (QGestureEvent* pGestureEvent = static_cast<QGestureEvent*>(event)) {
                        if (QGesture* tap = pGestureEvent->gesture(Qt::TapGesture)) {
                                if (tap->state() == Qt::GestureFinished) {
                                        if (windowed_) {
                                                foreach (QWidget *widget, QApplication::topLevelWidgets()) {
                                                        widget->showFullScreen();
                                                        break;
                                                }
                                        } else {
                                                foreach (QWidget *widget, QApplication::topLevelWidgets()) {
                                                        widget->showNormal();
                                                        break;
                                                }
                                        }
                                        windowed_ = !windowed_;
                                }
                                return true;
                        }
                }
                break;
        default:
                break;
        }
        return QGraphicsObject::sceneEvent(event);
}

class QuitButton : public QGraphicsObject {
public:
        QuitButton(QGraphicsItem* parent = 0);
        virtual ~QuitButton();

        virtual QRectF boundingRect() const;
        virtual QPainterPath shape() const;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        virtual bool sceneEvent(QEvent *event);
};

QuitButton::QuitButton(QGraphicsItem* parent)
        : QGraphicsObject(parent)
{
        grabGesture(Qt::TapGesture);
}

QuitButton::~QuitButton()
{
}

QRectF
QuitButton::boundingRect() const
{
        return QRectF(400, 200, 200, 100);
}

QPainterPath
QuitButton::shape() const
{
        QPainterPath path;
        path.addRect(boundingRect());
        return path;
}

void
QuitButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setBrush(Qt::red);
        painter->drawRect(boundingRect());
        painter->drawText(boundingRect(), Qt::AlignCenter, QString("Quit"));
}

bool
QuitButton::sceneEvent(QEvent *event)
{
        switch (event->type()) {
        case QEvent::Gesture:
                if (QGestureEvent* pGestureEvent = static_cast<QGestureEvent*>(event)) {
                        if (QGesture* tap = pGestureEvent->gesture(Qt::TapGesture)) {
                                if (tap->state() == Qt::GestureFinished) {
                                        QApplication::quit();
                                }
                                return true;
                        }
                }
                break;
        default:
                break;
        }
        return QGraphicsObject::sceneEvent(event);
}

class GraphicsScene : public QGraphicsScene {
public:
        GraphicsScene(QObject* parent = 0);
        virtual ~GraphicsScene();

protected:
        // this draws the trails
        virtual bool event(QEvent* evt);
private:
        QList<QGraphicsItem*> items_;
        QGraphicsTextItem* pinchText_;
        QGraphicsTextItem* panText_;
        QGraphicsTextItem* swipeText_;
        QGraphicsTextItem* tapText_;
        QGraphicsTextItem* tapAndHoldText_;
        bool inEvent_;

        bool handleTouchEvent(QTouchEvent* evt);
        bool gestureEvent(QGestureEvent *event);
        void panTriggered(QGestureEvent* evt, QPanGesture* gesture);
        void pinchTriggered(QGestureEvent* evt, QPinchGesture* gesture);
        void swipeTriggered(QGestureEvent* evt, QSwipeGesture* gesture);
        void tapTriggered(QGestureEvent* evt, QTapGesture* gesture);
        void tapAndHoldTriggered(QGestureEvent* evt, QTapAndHoldGesture* gesture);
};

GraphicsScene::GraphicsScene(QObject* parent)
        : QGraphicsScene(parent)
        , items_()
        , pinchText_(NULL)
        , panText_(NULL)
        , swipeText_(NULL)
        , tapText_(NULL)
        , tapAndHoldText_(NULL)
        , inEvent_(false)
{
        setSceneRect(0, 0, 2560, 1440);
        MyQGraphicsObject *i = new MyQGraphicsObject;
        i->setTransformOriginPoint(2560 / 2, 1440 / 2);
        addItem(i);
        addItem(new FullScreenButton);
        addItem(new QuitButton);
}

GraphicsScene::~GraphicsScene()
{
}

bool
GraphicsScene::event(QEvent* evt)
{
        switch (evt->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
                return handleTouchEvent(static_cast<QTouchEvent*>(evt));
                break;
        case QEvent::TouchEnd:
                Q_FOREACH(QGraphicsItem *item, items_) {
                        removeItem(item);
                }
                items_.clear();
                tapText_ = NULL;
                inEvent_ = false;
                return true;
                break;
        case QEvent::Gesture:
                gestureEvent(static_cast<QGestureEvent*>(evt));
                return QGraphicsScene::event(evt); // passes gesture event down to the QGraphicsObject::sceneEvent
                break;
        default:
                break;
        }
        return QGraphicsScene::event(evt);
}

bool
GraphicsScene::handleTouchEvent(QTouchEvent* evt)
{
        Q_FOREACH(const QTouchEvent::TouchPoint& tp, evt->touchPoints()) {
                QGraphicsRectItem * const i = this->addRect(tp.sceneRect());
                QColor myColor;
                if (tp.state() & Qt::TouchPointPressed) {
                        myColor.setBlue(255);
                        myColor.setGreen(255);
                }
                if (tp.state() & Qt::TouchPointReleased) {
                        myColor.setRed(255);
                }
                if (tp.state() & Qt::TouchPointMoved) {
                        myColor.setGreen(255);
                        myColor.setAlphaF(0.5);
                }
                if (tp.state() & Qt::TouchPointStationary) {
                        myColor = Qt::cyan;
                        myColor.setAlphaF(0.25);
                }
                i->setBrush(myColor);
                items_ << i;
        }
        return true;
}

bool
GraphicsScene::gestureEvent(QGestureEvent *event)
{
        if (QGesture *swipe = event->gesture(Qt::SwipeGesture)) {
                swipeTriggered(event, static_cast<QSwipeGesture *>(swipe));
        }
        if (QGesture *pan = event->gesture(Qt::PanGesture)) {
                panTriggered(event, static_cast<QPanGesture *>(pan));
        }
        if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
                pinchTriggered(event, static_cast<QPinchGesture *>(pinch));
        }
        if (QGesture *tap = event->gesture(Qt::TapGesture)) {
                tapTriggered(event, static_cast<QTapGesture*>(tap));
        }
        if (QGesture *tap = event->gesture(Qt::TapAndHoldGesture)) {
                tapAndHoldTriggered(event, static_cast<QTapAndHoldGesture*>(tap));
        }
        return true;
}

void
GraphicsScene::panTriggered(QGestureEvent* evt, QPanGesture* gesture)
{
        evt->accept(Qt::PanGesture);
        QGraphicsTextItem* myText = panText_;
        if (!myText) {
                myText = panText_ = this->addText(QString("Pan Gesture!"));
                myText->setPos(panTextPos);
        }
        Q_ASSERT(panText_);
        const QPointF gestureSpot = evt->mapToGraphicsScene(gesture->hotSpot());
        myText->setPlainText(gestureTemplate.arg("Pan").arg(gestureSpot.x()).arg(gestureSpot.y()));

        if (gesture->state() == Qt::GestureFinished || gesture->state() == Qt::GestureCanceled) {
                removeItem(panText_);
                panText_ = NULL;
        }
}

void
GraphicsScene::pinchTriggered(QGestureEvent* evt, QPinchGesture* gesture)
{
        evt->accept(Qt::PinchGesture);
        QGraphicsTextItem* myText = pinchText_;
        if (!myText) {
                myText = pinchText_ = this->addText(QString("Pinch Gesture!"));
                myText->setPos(pinchTextPos);
        }
        Q_ASSERT(pinchText_);
        const QPointF gestureSpot = evt->mapToGraphicsScene(gesture->hotSpot());
        myText->setPlainText(gestureTemplate.arg("Pinch").arg(gestureSpot.x()).arg(gestureSpot.y()));

        if (gesture->state() == Qt::GestureFinished || gesture->state() == Qt::GestureCanceled) {
                removeItem(pinchText_);
                pinchText_ = NULL;
        }
}

void
GraphicsScene::swipeTriggered(QGestureEvent* evt, QSwipeGesture* gesture)
{
        evt->accept(Qt::SwipeGesture);
        QGraphicsTextItem* myText = swipeText_;
        if (!myText) {
                myText = swipeText_ = this->addText(QString("Swipe Gesture!"));
                myText->setPos(swipeTextPos);
        }
        Q_ASSERT(swipeText_);
        const QPointF gestureSpot = evt->mapToGraphicsScene(gesture->hotSpot());
        myText->setPlainText(gestureTemplate.arg("Swipe").arg(gestureSpot.x()).arg(gestureSpot.y()));

        if (gesture->state() == Qt::GestureFinished || gesture->state() == Qt::GestureCanceled) {
                removeItem(swipeText_);
                swipeText_ = NULL;
        }
}

void
GraphicsScene::tapTriggered(QGestureEvent* evt, QTapGesture* gesture)
{
        evt->accept(Qt::TapGesture);
        QGraphicsTextItem* myText = tapText_;
        if (!myText) {
                myText = tapText_ = this->addText(QString("tap Gesture!"));
                myText->setPos(tapTextPos);
        }
        Q_ASSERT(tapText_);
        const QPointF gestureSpot = evt->mapToGraphicsScene(gesture->hotSpot());
        myText->setPlainText(gestureTemplate.arg("Tap").arg(gestureSpot.x()).arg(gestureSpot.y()));

        if (gesture->state() == Qt::GestureFinished || gesture->state() == Qt::GestureCanceled) {
                removeItem(tapText_);
                tapText_ = NULL;
        }
}

void
GraphicsScene::tapAndHoldTriggered(QGestureEvent* evt, QTapAndHoldGesture* gesture)
{
        evt->accept(Qt::TapAndHoldGesture);
        QGraphicsTextItem* myText = tapAndHoldText_;
        if (!myText) {
                myText = tapAndHoldText_ = this->addText(QString("tapAndHold Gesture!"));
                myText->setPos(tapAndHoldTextPos);
        }
        Q_ASSERT(tapAndHoldText_);
        const QPointF gestureSpot = evt->mapToGraphicsScene(gesture->hotSpot());
        myText->setPlainText(gestureTemplate.arg("Tap & Hold").arg(gestureSpot.x()).arg(gestureSpot.y()));

        if (gesture->state() == Qt::GestureFinished || gesture->state() == Qt::GestureCanceled) {
                removeItem(tapAndHoldText_);
                tapAndHoldText_ = NULL;
        }
}

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
        qDebug() << "The QTapAndHoldGesture timeout is " << QTapAndHoldGesture::timeout();
        setInteractive(true);

        // QScopedPointer<QGLWidget> glWidget(new QGLWidget(QGLFormat(QGL::SampleBuffers), this));
        // Q_CHECK_PTR(glWidget);
        // setupViewport(glWidget.take());
        // setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

        QScopedPointer<QGraphicsScene> scene(new GraphicsScene(this));
        Q_CHECK_PTR(scene);
        // a blue background
        //scene->setBackgroundBrush(Qt::blue);

        // a gradient background
        // QRadialGradient gradient(0, 0, 10);
        // gradient.setSpread(QGradient::RepeatSpread);
        // scene->setBackgroundBrush(gradient);

        setScene(scene.take());
        setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

GraphicsView::~GraphicsView()
{
}
