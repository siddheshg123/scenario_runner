#ifndef COORDINATES_H
#define COORDINATES_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
# include<QLabel>

class coordinates: public QGraphicsView
{
    Q_OBJECT
public:
    coordinates(QWidget *parent =nullptr);

protected:
      void mouseMoveEvent(QMouseEvent *mouse_event);

signals:
    void sendMousePosition(QPoint&);
};

#endif // MATDISPLAY_H
