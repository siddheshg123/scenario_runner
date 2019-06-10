
#ifndef CARLASTREAMTHREAD_H
#define CARLASTREAMTHREAD_H

#include <random>
#include <typeinfo>
#include <QThread>
#include <QImage>
#include <carla/client/Client.h>
#include <carla/client/ActorBlueprint.h>
#include <carla/client/BlueprintLibrary.h>
#include <carla/client/Sensor.h>
#include <carla/Memory.h>
#include <carla/sensor/data/Image.h>
#include <carla/geom/Location.h>
#include <carla/client/Actor.h>
#include <carla/client/Waypoint.h>
#include <carla/geom/Vector3D.h>
#include <carla/geom/Vector2D.h>
#include <carla/client/Map.h>
#include <traffic_manager/include/InMemoryMap.hpp>
#include <carla/client/DebugHelper.h>
#include "carla/client/detail/EpisodeProxy.h"
#include "carla/client/Vehicle.h"
#include "carla/road/Road.h"

class QImage;

class CarlaStreamThread : public QThread
{
    Q_OBJECT

public:
    static std::vector<carla::SharedPtr<carla::client::Actor>> actor_list_set;
    static std::vector<std::vector<boost::shared_ptr<carla::client::Waypoint>>> all_waypoints;
    static std::vector<std::vector<carla::geom::Transform>> vector_of_transforms;
    CarlaStreamThread(QObject *parent = nullptr);
    ~CarlaStreamThread();
    int image_callback (carla::SharedPtr<carla::sensor::SensorData> data);
    std::vector<float> InterensicParametes(int x,int y);
    void CarOperations(int x, int y);
    void select_path(int x, int y);
    void Settransform();
    void DesiredSetTransform();
    //void Settransform(std::vector<boost::shared_ptr<carla::client::Waypoint> > result, boost::shared_ptr<carla::client::Actor> actor);
    void ShowPath();
    void DesiredShowPath();
    int GenerateWaypoints();

signals:

    void renderedImage(const QImage &image);

protected:

    void run() Q_DECL_OVERRIDE;

private:
   std::vector<carla::SharedPtr<carla::client::Actor>> actor_list;
   carla::client::Client client_connection;
   carla::SharedPtr<carla::client::Sensor> camera;
   //std::vector<carla::SharedPtr<carla::client::Actor>> actor_list;
   carla::SharedPtr<carla::client::Map> waypoint;

};

#endif // CARLASTREAMTHREAD_H
