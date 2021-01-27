#include <EntityProject/project_settings.h>
#include <string>
#include "../drone_simulation.h"

extern "C" {
  entity_project::DroneDeliverySystem* GetDroneDeliverySystem(const std::string& id) {
        return new csci3081::DroneSimulation();
  }
}
