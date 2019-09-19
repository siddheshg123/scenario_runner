#pragma once

#include <chrono>
#include <memory>

#include "carla/client/Client.h"
#include "carla/rpc/Command.h"
#include "carla/rpc/VehicleControl.h"
#include "carla/Logging.h"

#include "MessengerAndDataTypes.h"
#include "PipelineStage.h"

namespace traffic_manager {

  namespace chr = std::chrono;

  /// This class receives actuation signals (throttle, brake, steer)
  /// from MotionPlannerStage class and communicates these signals to
  /// the simulator in batches to control vehicles' movement.
  class BatchControlStage : public PipelineStage {

  private:

    /// Variable to remember messenger state
    int messenger_state;
    /// Pointer to frame received from MotionPlanner
    std::shared_ptr<PlannerToControlFrame> data_frame;
    /// Pointer to messenger from MotionPlanner
    std::shared_ptr<PlannerToControlMessenger> messenger;
    /// Variable used to measure system throughput
    int frame_count = 0;
    /// Feference to carla client connection object
    carla::client::Client &carla_client;
    /// Array to hold command batch
    std::shared_ptr<std::vector<carla::rpc::Command>> commands;
    /// Object used to track time for measuring throughput
    chr::time_point<chr::_V2::system_clock, chr::nanoseconds> last_update_instance;

  public:

    BatchControlStage(
        std::shared_ptr<PlannerToControlMessenger> messenger,
        carla::client::Client &carla_client,
        int number_of_vehicles,
        int pool_size);
    ~BatchControlStage();

    void DataReceiver() override;

    void Action(const int start_index, const int end_index) override;

    void DataSender() override;

  };

}
