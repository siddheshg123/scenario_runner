#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    thread.start();
    connect(&thread, SIGNAL(renderedImage(QImage)), this, SLOT(updatePixmap(QImage)));
    //connect(&thread,SIGNAL(sendMousePosition(QPoint&)),this,SLOT(showMousePosition(QPoint&)));
   connect(ui->graphicsView,SIGNAL(sendMousePosition(QPoint&)),this,SLOT(showMousePosition(QPoint&)));
}
MainWindow::~MainWindow() {
    delete ui;
    thread.quit();
}

int MainWindow::updatePixmap(const QImage &image) {

    image_map = QPixmap::fromImage(image);
    scene = new QGraphicsScene(this);
    scene->addPixmap(image_map);
    scene->setSceneRect(image_map.rect());
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();

    return 0;
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    try {
        thread.makeATesla(event->x(), event->y());
    } catch(...) {
        std::cout << "\nFailed to spawn actor !\n";
    }
}

void MainWindow::showMousePosition(QPoint &pos)
{
    ui->label->setText("x: "+QString::number(pos.x()) + "y: " + QString::number(pos.y()));
}
