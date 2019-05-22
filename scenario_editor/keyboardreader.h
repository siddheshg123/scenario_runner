#ifndef KEYBOARDREADER_H
#define KEYBOARDREADER_H
#include <QThread>
#include <QImage>
#include <vector>
#include <QWidget>
#include <QPoint>
#include <QCursor>
#include <QGraphicsView>
#include <QtWidgets>
#include <QGraphicsScene>
#include <QtGui>
#include <QLabel>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QObject>
#include <QLabel>

class keyboardreader : public QThread
{
    Q_OBJECT
   signals:
       void KeyPressed(char ch);
public:
    keyboardreader();
};

#endif // KEYBOARDREADER_H
