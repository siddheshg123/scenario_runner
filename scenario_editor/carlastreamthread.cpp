#include "carlastreamthread.h"
#include"mainwindow.h"
#include "helper.h"
#include<QMouseEvent>
#include <QMoveEvent>
#include <QWidget>




//========================================================================================================================================================================================================
// ******************************************************************CarlaStreamThread Constructor********************************************************************************************************
//========================================================================================================================================================================================================

CarlaStreamThread::CarlaStreamThread(QObject* parent):QThread(parent),client_connection("localhost", 2000)
{
    auto blueprint_library = client_connection.GetWorld().GetBlueprintLibrary();
    auto camera_bp = (carla::client::ActorBlueprint*)blueprint_library->Find("sensor.camera.rgb");
    camera_bp->SetAttribute("image_size_x", "500");
    camera_bp->SetAttribute("image_size_y", "500");
    camera_bp->SetAttribute("fov", "120");


    auto camera_transform = carla::geom::Transform{

        carla::geom::Location{0.0f, 0.0f, 50.0f},
        carla::geom::Rotation{-90.0f, 0.0f, -90.0f}
    };


    auto cam_actor = client_connection.GetWorld().SpawnActor(*camera_bp, camera_transform);
    camera = boost::static_pointer_cast<carla::client::Sensor>(cam_actor);

    camera->Listen([this](auto data)
    {
        this->image_callback(data);
    });
}


//========================================================================================================================================================================================================
// ******************************************************************CarlaStreamThread Destructor********************************************************************************************************
//========================================================================================================================================================================================================


CarlaStreamThread::~CarlaStreamThread(){
    camera->Destroy();
    for(auto actor: actor_list) {
        actor->Destroy();
    }
}


//========================================================================================================================================================================================================
// ******************************************************************CarlaStreamThread :: run()********************************************************************************************************
//========================================================================================================================================================================================================


void CarlaStreamThread::run() {
    QThread::exec();
}

//========================================================================================================================================================================================================
// ******************************************************************CarlaStreamThread :: image callback()********************************************************************************************************
//========================================================================================================================================================================================================


int CarlaStreamThread::image_callback (carla::SharedPtr<carla::sensor::SensorData> data) {

    auto image_data = boost::static_pointer_cast<carla::sensor::data::Image>(data);
    uchar* image_buffer = (uchar*) image_data->data();
    QImage q_image(image_buffer, 500, 500, QImage::Format_ARGB32);
    emit renderedImage(q_image);

    return 0;
}

//========================================================================================================================================================================================================
// ******************************************************************CarlaStreamThread Interensic Parameters********************************************************************************************************
//========================================================================================================================================================================================================

std::vector<float> CarlaStreamThread::InterensicParametes(int x,int y)
{
    std::vector<float> output;
    float xp = ((x-250.0f) * (2.0f* tan(3.14f/3.0f)*50.0f)/500.0f);
    output.push_back(xp);
    float yp = ((y-250.0f) * (2.0f* tan(3.14f/3.0f)*50.0f)/500.0f);
    output.push_back(yp);

    return output;
}

//========================================================================================================================================================================================================
// ******************************************************************CarlaStreamThread CarOperations********************************************************************************************************
//========================================================================================================================================================================================================

std::vector<std::vector<boost::shared_ptr<carla::client::Waypoint>>> CarlaStreamThread::all_waypoints = (std::vector<std::vector<boost::shared_ptr<carla::client::Waypoint>>>)0;
std::vector<boost::shared_ptr<carla::client::Waypoint>> CarlaStreamThread::CarOperations(int x=0, int y=0)
{
    std::vector<boost::shared_ptr<carla::client::Waypoint> > waypoints_for_a_distance ;
    std::vector<boost::shared_ptr<carla::client::Waypoint> > list_of_next_n_waypoints;
    auto world = client_connection.GetWorld();
    auto blueprint_library = world.GetBlueprintLibrary();
    auto bp = blueprint_library->Find("vehicle.tesla.model3");
    auto transform = carla::geom::Transform
    {
        carla::geom::Location{InterensicParametes(x,y)[0], InterensicParametes(x,y)[1], 8.0f},
        carla::geom::Rotation{0.0f, 0.0f, 0.0f}
     };

    auto map = world.GetMap();
    auto waypoint_map = map->GetWaypoint(transform.location);
    waypoints_for_a_distance = waypoint_map->GetNext(1.0);



    for(int i =0 ;i<50;i++)
    {

       list_of_next_n_waypoints.push_back(waypoints_for_a_distance[0]);
       std::cout<<"x_coordinate: "<<i<<" "<<waypoints_for_a_distance[0]->GetTransform().location.x<<"\n";
       waypoints_for_a_distance = (waypoints_for_a_distance[0])->GetNext(1.0);
    }
    all_waypoints.push_back(list_of_next_n_waypoints);
    std::cout<< all_waypoints.size()<<"\n";
    //ShowPath(all_waypoints);

    auto new_transform = carla::geom::Transform
    {
            carla::geom::Location{waypoint_map->GetTransform().location.x, waypoint_map->GetTransform().location.y, waypoint_map->GetTransform().location.z},
            carla::geom::Rotation{waypoint_map->GetTransform().rotation.pitch, waypoint_map->GetTransform().rotation.yaw,waypoint_map->GetTransform().rotation.roll}
    };

    auto actor = world.SpawnActor(*bp, new_transform);

    //Settransform(list_of_next_n_waypoints,actor);

    actor_list.push_back(actor);
    //std::cout<<"debug check1"<<"\n";
    return list_of_next_n_waypoints;
}

//========================================================================================================================================================================================================
// ******************************************************************CarlaStreamThread Settransform********************************************************************************************************
//========================================================================================================================================================================================================

 void CarlaStreamThread::Settransform(std::vector<boost::shared_ptr<carla::client::Waypoint> > result, boost::shared_ptr<carla::client::Actor> actor)
 {
     auto delay = carla::time_duration::seconds(10);

     auto world = client_connection.GetWorld();

     for (auto i =result.begin();i<result.end();i++)
     {
         world.WaitForTick(delay);
         //std::cout<<(*i)->GetTransform().location.x<<"\n";
         actor->SetTransform((*i)->GetTransform());
         //std::cout<<"debug check0"<<"\n";
     }
 }

//========================================================================================================================================================================================================
// ******************************************************************CarlaStreamThread ShowPath********************************************************************************************************
//========================================================================================================================================================================================================

void CarlaStreamThread::ShowPath()

{
    auto world = client_connection.GetWorld();
    std::cout<<"debug for showpath"<<"\n";
    std::vector<std::vector<boost::shared_ptr<carla::client::Waypoint>>> path_points = all_waypoints;
    for (auto i =path_points.begin();i<path_points.end();i++)
    {
        for(auto j = i->begin();j<i->end();j++)
        {

        world.MakeDebugHelper().DrawPoint((*j)->GetTransform().location,0.05f,{255u,0u,0u},15.0f,true);
    }
    }

}

//========================================================================================================================================================================================================
// ******************************************************************CarlaStreamThread Generate waypoints********************************************************************************************************
//========================================================================================================================================================================================================



 int CarlaStreamThread::GenerateWaypoints()
 {
     auto world = client_connection.GetWorld();
     auto map = world.GetMap();
     auto list_of_waypoints = map->GenerateWaypoints(1.0);

     for (auto i =list_of_waypoints.begin();i<list_of_waypoints.end();i++)
     {

         world.MakeDebugHelper().DrawPoint((*i)->GetTransform().location,0.05,{255u,255u,255u},10.0f,true);
      }
     return 0;
 }

 //========================================================================================================================================================================================================
 // ******************************************************************END********************************************************************************************************
 //========================================================================================================================================================================================================
