#ifndef GRAPHICSVIEW_HPP
#define GRAPHICSVIEW_HPP

#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsView>

class GraphicsView : public QGraphicsView
{
        Q_OBJECT;
public:
        explicit GraphicsView(QWidget *parent = 0);
        virtual ~GraphicsView();
};

#endif // GRAPHICSVIEW_HPP
