#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "carlastreamthread.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    thread.start();
    connect(&thread, SIGNAL(renderedImage(QImage)), this, SLOT(updatePixmap(QImage)));
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


static int checked =0;


int MainWindow::on_checkBox_stateChanged(int arg1)
{

checked = arg1;
std::cout<<"checked_box clicked"<<"\n";
 return arg1;
}


void MainWindow::mousePressEvent(QMouseEvent* event) {
 if(checked)
 {
    try {
        thread.CarOperations(event->x(), event->y());
    } catch(...) {
        std::cout << "\nFailed to spawn actor !\n";
    }
}
 else if (checked ==0) {
     try {
         thread.select_path(event->x(), event->y());
     } catch(...) {
         std::cout << "\nFailed to spawn actor !\n";
     }

 }
}

void MainWindow::showMousePosition(QPoint &pos)
{
    ui->label->setText("x: "+QString::number(pos.x()-320) + "y: " + QString::number(pos.y()-240));
}


void MainWindow::on_GenerateWayPoints_clicked()
{
    CarlaStreamThread c;
    c.GenerateWaypoints();
}



void MainWindow::on_show_path_clicked()
{
    CarlaStreamThread c;
    c.ShowPath();
}

void MainWindow::on_SetTransform_clicked()
{
    CarlaStreamThread c;
    c.Settransform();
}




//void MainWindow::on_radioButton_clicked(bool checked ,QMouseEvent* event)
//{


//   std::cout<<"inside radio buttton"<<"\n";
////    QMouseEvent* event;
//    try {
//            thread.CarOperations(event->x(), event->y());
//        } catch(...) {
//            std::cout << "\nFailed to spawn actor !\n";
//        }

//}


void MainWindow::on_DesiredShowPath_clicked()
{
    CarlaStreamThread c;
    c.DesiredShowPath();
}

void MainWindow::on_DesiredSetTransform_clicked()
{
    CarlaStreamThread c;
    c.DesiredSetTransform();
}
