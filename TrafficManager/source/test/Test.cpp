#include <atomic>
#include <iostream>
#include <signal.h>

#include "boost/stacktrace.hpp"
#include "carla/client/ActorList.h"
#include "carla/client/Client.h"
#include "carla/Logging.h"

#include "BatchControlStage.h"
#include "CollisionStage.h"
#include "CarlaDataAccessLayer.h"
#include "InMemoryMap.h"
#include "LocalizationStage.h"
#include "MotionPlannerStage.h"
#include "Pipeline.h"
#include "TrafficLightStage.h"

void test_dense_topology(const carla::client::World &);

void test_in_memory_map(carla::SharedPtr<carla::client::Map>);

void test_lane_change(const carla::client::World &world);

void test_pipeline_stages(
    carla::SharedPtr<carla::client::ActorList> actor_list,
    carla::SharedPtr<carla::client::Map> world_map,
    carla::client::Client &client_conn,
    carla::client::World &world);

void test_pipeline(
    carla::client::World &world,
    carla::client::Client &client_conn,
    int target_traffic_amount);

std::atomic<bool> quit(false);
void got_signal(int) {
  quit.store(true);
}

std::vector<carla::SharedPtr<carla::client::Actor>>* global_actor_list;
void handler() {

  if (!quit.load()) {
     carla::log_error("TrafficManager encountered a problem!\n");
     carla::log_info("Destorying all actors spawned\n");
    for (auto actor: *global_actor_list) {
      if (actor != nullptr && actor->IsAlive()) {
        actor->Destroy();
      }
    }
    exit(1);
  }
} 

int main(int argc, char *argv[]) {
  std::set_terminate(handler);

  auto client_conn = carla::client::Client("localhost", 2000);
  std::cout << "Connected with client object : " << client_conn.GetClientVersion() << std::endl;
  auto world = client_conn.GetWorld();
  auto world_map = world.GetMap();
  auto actorList = world.GetActors();
  auto vehicle_list = actorList->Filter("vehicle.*");

  // test_dense_topology(world);
  // test_in_memory_map(world_map);
  // test_pipeline_stages(vehicle_list, world_map, client_conn, world);
  // test_lane_change(world);
  test_pipeline(world, client_conn, 0);

  return 0;
}

void test_pipeline(
    carla::client::World &world,
    carla::client::Client &client_conn,
    int target_traffic_amount) {

  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = got_signal;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);

  auto world_map = world.GetMap();
  auto debug_helper = client_conn.GetWorld().MakeDebugHelper();
  auto dao = traffic_manager::CarlaDataAccessLayer(world_map);
  auto topology = dao.GetTopology();
  auto local_map = std::make_shared<traffic_manager::InMemoryMap>(topology);
  local_map->SetUp(1.0);

  auto core_count = traffic_manager::read_core_count();
  std::cout << "Found " << core_count << " CPU cores" << std::endl;
  auto registered_actors = traffic_manager::spawn_traffic(world, core_count, target_traffic_amount);
  global_actor_list = &registered_actors;

  traffic_manager::Pipeline pipeline(
    {0.1f, 0.15f, 0.01f},
    {5.0f, 0.0f, 0.1f},
    {10.0f, 0.01f, 0.1f},
    25/3.6,
    50/3.6,
    registered_actors,
    *local_map.get(),
    client_conn,
    world,
    debug_helper,
    std::ceil(core_count / 5)
  );
  pipeline.Start();

  std::cout << "Started " << 2 + 4 * std::ceil(core_count / 4) << " pipeline threads" << std::endl;

  while (!quit.load()) {
    sleep(1);
  }

  pipeline.Stop();

  for (auto actor: registered_actors) {
    if (actor != nullptr && actor->IsAlive()) {
      actor->Destroy();
    }
  }

  std::cout << "TrafficManager stopped by user" << std::endl;
}

void test_pipeline_stages(
    carla::SharedPtr<carla::client::ActorList> actor_list,
    carla::SharedPtr<carla::client::Map> world_map,
    carla::client::Client &client_conn,
    carla::client::World &world) {

  auto debug_helper = client_conn.GetWorld().MakeDebugHelper();

  std::vector<carla::SharedPtr<carla::client::Actor>> registered_actors;
  for (auto it = actor_list->begin(); it != actor_list->end(); ++it) {
    registered_actors.push_back(*it);
  }

  std::cout << "Setting up local map cache ... " << std::endl;
  auto dao = traffic_manager::CarlaDataAccessLayer(world_map);
  auto topology = dao.GetTopology();
  auto local_map = traffic_manager::InMemoryMap(topology);
  local_map.SetUp(1.0);
  std::cout << "Map set up !" << std::endl;

  auto localization_collision_messenger = std::make_shared<traffic_manager::LocalizationToCollisionMessenger>();
  auto localization_traffic_light_messenger = std::make_shared<traffic_manager::LocalizationToTrafficLightMessenger>();
  auto collision_planner_messenger = std::make_shared<traffic_manager::CollisionToPlannerMessenger>();
  auto localization_planner_messenger = std::make_shared<traffic_manager::LocalizationToPlannerMessenger>();
  auto traffic_light_planner_messenger = std::make_shared<traffic_manager::TrafficLightToPlannerMessenger>();
  auto planner_control_messenger = std::make_shared<traffic_manager::PlannerToControlMessenger>();

  traffic_manager::LocalizationStage localization_stage(
    localization_planner_messenger, localization_collision_messenger,
    localization_traffic_light_messenger, registered_actors.size(), 1,
    registered_actors, local_map,
    debug_helper
  );

  traffic_manager::CollisionStage collision_stage(
    localization_collision_messenger, collision_planner_messenger,
    registered_actors.size(), 1,
    world, debug_helper
  );

  traffic_manager::TrafficLightStage traffic_light_stage(
    localization_traffic_light_messenger, traffic_light_planner_messenger,
    registered_actors.size(), 1
  );

  traffic_manager::MotionPlannerStage planner_stage(
    localization_planner_messenger,
    collision_planner_messenger,
    traffic_light_planner_messenger,
    planner_control_messenger,
    registered_actors.size(), 1,
    25/3.6, 50/3.6, {0.1f, 0.15f, 0.01f},
    {10.0f, 0.01f, 0.1f}, {10.0f, 0.0f, 0.1f}
  );

  traffic_manager::BatchControlStage control_stage(
    planner_control_messenger, client_conn,
    registered_actors.size(), 1
  );
 
  std::cout << "Starting stages ... " << std::endl;

  localization_stage.Start();
  collision_stage.Start();
  traffic_light_stage.Start();
  planner_stage.Start();
  // control_stage.Start();

  int messenger_state = planner_control_messenger->GetState() -1;
  // int messenger2_state = localization_planner_messenger->GetState() -1;

  while (planner_control_messenger->GetState() == 0);
  std::cout << "Sensed pipeline output !" << std::endl;

  long count = 0;
  auto last_time = std::chrono::system_clock::now();
  while (true) {
    std::this_thread::sleep_for(10ms);
    // sleep(1);

    // std::cout 
    // << "===========================================================================" << std::endl
    // << "Running test receiver"
    // << " with messenger's state "
    // << localization_planner_messenger->GetState()
    // << " previous state "
    // << messenger_state
    // << std::endl;

    auto dummy_1 = planner_control_messenger->ReceiveData(messenger_state);
    messenger_state = dummy_1.id;

    // auto dummy_2 = localization_planner_messenger->ReceiveData(messenger2_state);
    // messenger2_state = dummy_2.id;

    // std::cout 
    // << "Finished test receiver"
    // << " with messenger's state "
    // << localization_planner_messenger->GetState()
    // << " previous state "
    // << messenger_state
    // << std::endl
    // << "===========================================================================" << std::endl;

    auto current_time = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = current_time - last_time;

    ++count;
    if (diff.count() > 1.0) {
      last_time = current_time;
      std::cout << "Updates processed per second " << count * registered_actors.size() << std::endl;
      count = 0;
    }
  }
}

void test_in_memory_map(carla::SharedPtr<carla::client::Map> world_map) {
  auto dao = traffic_manager::CarlaDataAccessLayer(world_map);
  auto topology = dao.GetTopology();
  traffic_manager::InMemoryMap local_map(topology);

  std::cout << "setup starting" << std::endl;
  local_map.SetUp(1.0);
  std::cout << "setup complete" << std::endl;
  int loose_ends_count = 0;
  auto dense_topology = local_map.GetDenseTopology();
  for (auto &swp : dense_topology) {
    if (swp->GetNextWaypoint().size() < 1 || swp->GetNextWaypoint()[0] == 0) {
      loose_ends_count += 1;
      auto loc = swp->GetLocation();
      std::cout << "Loose end at : " << loc.x << " " << loc.y << std::endl;
    }
  }
  std::cout << "Number of loose ends : " << loose_ends_count << std::endl;
}

void test_dense_topology(const carla::client::World &world) {
  auto debug = world.MakeDebugHelper();
  auto dao = traffic_manager::CarlaDataAccessLayer(world.GetMap());
  auto topology = dao.GetTopology();
  traffic_manager::InMemoryMap local_map(topology);
  local_map.SetUp(1.0);
  for (auto point : local_map.GetDenseTopology()) {
    auto location = point->GetLocation();
    debug.DrawPoint(location + carla::geom::Location(0,
        0,
        1), 0.2, carla::client::DebugHelper::Color{225U, 0U, 0U}, 30.0F);
  }
}

void test_lane_change(const carla::client::World &world) {

  auto debug = world.MakeDebugHelper();
  auto dao = traffic_manager::CarlaDataAccessLayer(world.GetMap());
  auto topology = dao.GetTopology();
  traffic_manager::InMemoryMap local_map(topology);
  local_map.SetUp(1.0);

  int missing_left_lane_links = 0;
  int missing_right_lane_links = 0;
  int total_left_lane_links = 0;
  int total_right_lane_links = 0;

  for (auto point : local_map.GetDenseTopology()) {

    auto raw_waypoint = point->GetWaypoint();
    uint8_t lane_change = static_cast<uint8_t>(raw_waypoint->GetLaneChange());
    uint8_t change_right = static_cast<uint8_t>(carla::road::element::LaneMarking::LaneChange::Right);
    uint8_t change_left = static_cast<uint8_t>(carla::road::element::LaneMarking::LaneChange::Left);

    if ((lane_change & change_right) > 0 && !(point->CheckJunction())) {
      ++total_right_lane_links;
      if (point->GetRightWaypoint() == nullptr) {
        ++missing_right_lane_links;
      }
    }

    if ((lane_change & change_left) > 0 && !(point->CheckJunction())) {
      ++total_left_lane_links;
      if (point->GetLeftWaypoint() == nullptr) {
        ++missing_left_lane_links;
      }
    }
  }

  std::cout << "Total right lane changes available : " << total_right_lane_links << std::endl;
  std::cout << "Missed right lane changes : " << missing_right_lane_links << std::endl;
  std::cout << "Total left lane changes available : " << total_left_lane_links << std::endl;
  std::cout << "Missed left lane changes : " << missing_left_lane_links << std::endl;

  sleep(1);
  std::terminate();
}
