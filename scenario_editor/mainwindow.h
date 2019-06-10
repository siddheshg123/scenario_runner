#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <iostream>
#include <QImage>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QMouseEvent>

#include "carlastreamthread.h"

using namespace std::literals::chrono_literals;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private slots:
    int updatePixmap(const QImage &image);
    void showMousePosition(QPoint& pos);
    void on_GenerateWayPoints_clicked();
    void on_show_path_clicked();
    void on_SetTransform_clicked();
    int on_checkBox_stateChanged(int arg1);

    void on_DesiredShowPath_clicked();

    void on_DesiredSetTransform_clicked();

private:
    Ui::MainWindow *ui;

    CarlaStreamThread thread;

    QPixmap image_map;
    QImage  *imageObject;
    QGraphicsScene *scene;

protected:
    // void on_radioButton_clicked(bool checked,QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
};

#endif // MAINWINDOW_H
