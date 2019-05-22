#ifndef CARLASTREAMTHREAD_H
#define CARLASTREAMTHREAD_H

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
#include<carla/client/Waypoint.h>
#include <carla/client/Client.h>
#include <carla/client/ActorBlueprint.h>
#include <carla/client/BlueprintLibrary.h>
#include <carla/client/Sensor.h>
#include <carla/Memory.h>
#include <carla/sensor/data/Image.h>
#include <carla/client/Map.h>

class QImage;
class QWidget;



class CarlaStreamThread : public QThread
{
    Q_OBJECT

public:
    CarlaStreamThread(QObject *parent = nullptr);
    ~CarlaStreamThread();
    //std::vector<float> plot_map();
    int makeATesla(int x, int y);
//    std::vector<float> mouselocate();

protected:
     void mouseMoveEvent(QMoveEvent *mouse_event);
     void run() Q_DECL_OVERRIDE;

private:

   carla::client::Client client_connection;
   carla::SharedPtr<carla::client::Sensor> camera;
   std::vector<carla::SharedPtr<carla::client::Actor>> actor_list;
   int image_callback (carla::SharedPtr<carla::sensor::SensorData> data);

signals:
    void renderedImage(const QImage &image);
    void sendMousePosition(QPoint&);
};

#endif // CARLASTREAMTHREAD_H
