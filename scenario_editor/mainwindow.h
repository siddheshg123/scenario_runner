#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <iostream>
#include<QPoint>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QMouseEvent>
#include <QMoveEvent>
#include "carlastreamthread.h"

using namespace std::literals::chrono_literals;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    int updatePixmap(const QImage &image);
    void showMousePosition(QPoint& pos);

private:
    Ui::MainWindow *ui;
    CarlaStreamThread thread;

    QPixmap image_map;
    QImage  *imageObject;
    QGraphicsScene *scene;

protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif // MAINWINDOW_H
