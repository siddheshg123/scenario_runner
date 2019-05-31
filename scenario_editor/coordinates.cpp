#include "coordinates.h"
#include <QMessageBox>
#include<iostream>
#include <QMouseEvent>
coordinates::coordinates(QWidget* parent):QGraphicsView (parent)
{
    this->setMouseTracking(true);

}

void coordinates::mouseMoveEvent(QMouseEvent *mouse_event)
{
    QPoint mouse_pos = mouse_event->pos();

    if (mouse_pos.x()<=this->size().width() && mouse_pos.y()<=this->size().height())
    {
        if(mouse_pos.x()>=0 && mouse_pos.y()>=0)
        {
//            std::cout<<"x:";
//            std::cout<<mouse_pos.x()<<"\t";
//            std::cout<<"y:";
//            std::cout<<mouse_pos.y()<<"\n";
            emit sendMousePosition(mouse_pos);

        }
    }
}


