#include "carlastreamthread.h"
#include "matplotlibcpp.h"
#include <typeinfo>
#include <cmath>
#include <QWidget>
namespace plt = matplotlibcpp;
//function out of class
std::vector<int>mouselocate();


//constructor

CarlaStreamThread::CarlaStreamThread(QObject* parent):
    QThread(parent), client_connection("localhost", 2000)
    {
       // this->setMouseTracking(true);
        std::vector<int> result = mouselocate();
        std::cout<<result[0]<<"\n";
        std::cout<<result[1]<<"\n";


        auto world = client_connection.GetWorld();
        auto blueprint_library = world.GetBlueprintLibrary();
        auto camera_bp = blueprint_library->Find("sensor.camera.rgb");
        auto world_map = world.GetMap();
        auto topology = world_map->GetTopology();

        auto camera_transform = carla::geom::Transform
        {
                carla::geom::Location{(float)result[0],(float)result[1],(float)result[2]},   // x, y, z.
                carla::geom::Rotation{-90.0f, 0.0f, 0.0f}       // pitch, yaw, roll.
                 };


    auto cam_actor = world.SpawnActor(*camera_bp, camera_transform);
    camera = boost::static_pointer_cast<carla::client::Sensor>(cam_actor);
    camera->Listen([this](auto data)
    {
        this->image_callback(data);
      });
}
//constructor end



//destructor
CarlaStreamThread::~CarlaStreamThread(){
    camera->Destroy();
    for(auto actor: actor_list) {
        actor->Destroy();
    }
}
//destructor end






//class function run()
void CarlaStreamThread::run() {
    QThread::exec();
}

//end run

//class function image_callback
int CarlaStreamThread::image_callback (carla::SharedPtr<carla::sensor::SensorData> data) {

    auto image_data = boost::static_pointer_cast<carla::sensor::data::Image>(data);
    uchar* image_buffer = (uchar*) image_data->data();
    QImage q_image(image_buffer, 800, 600, QImage::Format_ARGB32);
    emit renderedImage(q_image);

    return 0;
}
//end image_callback


//class function makeATesla
int CarlaStreamThread::makeATesla(int x, int y) {
    float k = 0.05f;
    auto world = client_connection.GetWorld();
    auto blueprint_library = world.GetBlueprintLibrary();
    auto bp = blueprint_library->Find("vehicle.tesla.model3");
    auto transform = carla::geom::Transform{
        carla::geom::Location{-1*float(y-400)*k, float(x-300)*k, 5.0f},
        carla::geom::Rotation{0.0f, 0.0f, 0.0f}};
    auto actor = world.SpawnActor(*bp, transform);
    actor_list.push_back(actor);

    return 0;
}

//end makeATesla

std::vector<int>mouselocate()
{
    QMouseEvent *mouse_event;
    QWidget w;


    w.setMouseTracking(true);
    std::vector<int> result={-1,-1,50};

    return result;
}



