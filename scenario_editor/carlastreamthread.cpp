#include "carlastreamthread.h"
#include "helper.h"
#include<QMouseEvent>
#include <QMoveEvent>
#include <QWidget>


template <typename RangeT, typename RNG>

static auto &RandomChoice(const RangeT &range, RNG &&generator)
{
  EXPECT_TRUE(range.size() > 0u);
  std::uniform_int_distribution<size_t> dist{0u, range.size() - 1u};
  return range[dist(std::forward<RNG>(generator))];
}


//std::vector<boost::shared_ptr<carla::client::Waypoint>> list_of_waypoints;


CarlaStreamThread::CarlaStreamThread(QObject* parent):QThread(parent),client_connection("localhost", 2000)
{
    auto world = client_connection.GetWorld();
    auto blueprint_library = world.GetBlueprintLibrary();
    auto camera_bp = (carla::client::ActorBlueprint*)blueprint_library->Find("sensor.camera.rgb");
    camera_bp->SetAttribute("image_size_x", "500");
    camera_bp->SetAttribute("image_size_y", "500");
    camera_bp->SetAttribute("fov", "120");


    auto camera_transform = carla::geom::Transform{

        carla::geom::Location{0.0f, 0.0f, 50.0f},
        carla::geom::Rotation{-90.0f, 0.0f, -90.0f}
    };


    auto cam_actor = world.SpawnActor(*camera_bp, camera_transform);
    camera = boost::static_pointer_cast<carla::client::Sensor>(cam_actor);

    camera->Listen([this](auto data)
    {
        this->image_callback(data);
        auto info = camera->GetLocation();
//    std::cout<< "Camera location is ------>" <<info.x<<'\t'<<info.y<< '\t'<<info.z<<'\n';
      }
    );
}

CarlaStreamThread::~CarlaStreamThread(){
    camera->Destroy();
    for(auto actor: actor_list) {
        actor->Destroy();
    }
}

void CarlaStreamThread::run() {
    QThread::exec();
}

int CarlaStreamThread::image_callback (carla::SharedPtr<carla::sensor::SensorData> data) {

    auto image_data = boost::static_pointer_cast<carla::sensor::data::Image>(data);
    uchar* image_buffer = (uchar*) image_data->data();
    QImage q_image(image_buffer, 500, 500, QImage::Format_ARGB32);
    emit renderedImage(q_image);

    return 0;
}


//carlastreamthread.cpp:89:11: error: no viable conversion from 'std::vector<SharedPtr<Waypoint> >' (aka 'vector<boost::shared_ptr<carla::client::Waypoint> >') to 'float'



std::vector<boost::shared_ptr<carla::client::Waypoint>> CarlaStreamThread::makeATesla(int x, int y)
{
    auto world = client_connection.GetWorld();
    auto map = world.GetMap();

    auto blueprint_library = world.GetBlueprintLibrary();
    auto bp = blueprint_library->Find("vehicle.tesla.model3");

    auto delay = carla::time_duration::seconds(8);


    auto new_x = (x-250) * (2* tan(3.14/3)*50)/500;
    auto new_y = (y-250) * (2* tan(3.14/3)*50)/500;
//    std::cout<<"Value of new x:"<<new_x<<"\t"<< "Value of new y:"<<new_y<<"\n";

    auto transform = carla::geom::Transform
    {
        carla::geom::Location{(float)new_x, (float)new_y, 8.0f},
        carla::geom::Rotation{0.0f, 0.0f, 0.0f}
     };

    auto waypoint_map = map->GetWaypoint(transform.location);
    std::vector<boost::shared_ptr<carla::client::Waypoint> > waypoints_for_a_distance = waypoint_map->GetNext(1.0);
    std::vector<boost::shared_ptr<carla::client::Waypoint> > list_of_next_n_waypoints;
    //boost::shared_ptr<carla::client::Waypoint> next = waypoints_for_a_distance[0];
    for(int i =0 ;i<20;i++)
    {
       list_of_next_n_waypoints.push_back(waypoints_for_a_distance[0]);
       waypoints_for_a_distance = (waypoints_for_a_distance[0])->GetNext(1.0);
    }

//    std::cout<<typeid (waypoints_for_a_distance).name()<<"\n";
    std::cout<<"SIZE OF WAYPOINT_FAR A DISTANCE::"<<waypoints_for_a_distance.size()<<"\n";
//    list_of_waypoints = waypoints_for_a_distance;
    //std::cout<<"SIZE OF LIST OF WAYPOINTS::"<<list_of_waypoints.size()<<"\n";
    for (auto i =list_of_next_n_waypoints.begin();i<list_of_next_n_waypoints.end();i++)
    {

        world.MakeDebugHelper().DrawPoint((*i)->GetTransform().location,0.05,{255u,0u,0u},15.0f,true);
        std::cout<<"------------------------------------------"<<"\n";
        std::cout<<"Value of X coordinate: "<< (*i)->GetTransform().location.x<<"\n";
        std::cout<<"Value of Y coordinate: "<<(*i)->GetTransform().location.y<<"\n";
        std::cout<<"Value of Z coordinate: "<<(*i)->GetTransform().location.z<<"\n";
        std::cout<<"Value of PITCH coordinate: "<<(*i)->GetTransform().rotation.pitch<<"\n";
        std::cout<<"Value of YAW coordinate: "<<(*i)->GetTransform().rotation.yaw<<"\n";
        std::cout<<"Value of ROLL coordinate: "<<(*i)->GetTransform().rotation.roll<<"\n";

    }  

    auto new_transform = carla::geom::Transform
    {
            carla::geom::Location{waypoint_map->GetTransform().location.x, waypoint_map->GetTransform().location.y, waypoint_map->GetTransform().location.z},
            carla::geom::Rotation{waypoint_map->GetTransform().rotation.pitch, waypoint_map->GetTransform().rotation.yaw,waypoint_map->GetTransform().rotation.roll}
    };
    auto actor = world.SpawnActor(*bp, new_transform);
    //carla::client::Vehicle vehicle(actor);
    world.WaitForTick(delay);
    actor_list.push_back(actor);
    std::cout<<"debug check1"<<"\n";
    return waypoints_for_a_distance;
}




 int CarlaStreamThread::generate_waypoints()
 {
     auto world = client_connection.GetWorld();
     auto map = world.GetMap();
     auto list_of_waypoints = map->GenerateWaypoints(1.0);
//     std::cout<<"Type of waypoints::"<<typeid(list_of_waypoints).name();
     //std::cout<<list_of_waypoints.size()<<"\n";

     for (auto i =list_of_waypoints.begin();i<list_of_waypoints.end();i++)
     {
         world.MakeDebugHelper().DrawPoint((*i)->GetTransform().location,0.05,{255u,255u,255u},10.0f,true);
         std::cout<<"------------------------------------------"<<"\n";
         std::cout<<"Value of X coordinate: "<< (*i)->GetTransform().location.x<<"\n";
         std::cout<<"Value of Y coordinate: "<<(*i)->GetTransform().location.y<<"\n";
         std::cout<<"Value of Z coordinate: "<<(*i)->GetTransform().location.z<<"\n";
         std::cout<<"Value of PITCH coordinate: "<<(*i)->GetTransform().rotation.pitch<<"\n";
         std::cout<<"Value of YAW coordinate: "<<(*i)->GetTransform().rotation.yaw<<"\n";
         std::cout<<"Value of ROLL coordinate: "<<(*i)->GetTransform().rotation.roll<<"\n";

     }
     return 0;
 }
