#ifndef DRONE_SIMULATION_H_
#define DRONE_SIMULATION_H_

#include <EntityProject/ANVIL2/drone_delivery_system.h>
#include <EntityProject/entity_console_logger.h>
#include <vector>
#include <string>
#include "src/json_helper.h"
#include "src/drone.h"
#include "src/package.h"
#include "src/customer.h"
#include "src/entity_factory.h"
#include "src/transport_package.h"
#include "src/delivery.h"

namespace csci3081 {


/**
 * @brief DroneSimulation is a facade that ties together subsystems required for a DroneDeliverySystem simulation.
 * 
 * DroneSimulation has a vector data structure, called entities_, that holds elements of type Entity*. These
 * Entity pointers point to Drone, Customer, and Package objects used in the simulation. CreateEntity gives
 * an entryway into the EntityFactory subsystem, and ScheduleDelivery and Update are entryways into the
 * TransportPackage subsystem.
 */
class DroneSimulation : public entity_project::DroneDeliverySystem {
 public:
  /// Returns a string reference of the project team's name.
  const std::string& GetTeamName() const { return teamName_; }

  /// CreateEntity interacts with EntityFactory. The picojson variable val is passed into this
  /// function is what tells the factory which type of object to create.
  entity_project::Entity* CreateEntity(const picojson::object& val);

  /// Adds an element of type Entity* to the entities_ data structure. **UPDATE DOXYGEN HERE**
  void AddEntity(entity_project::Entity* entity) {
    entities_.push_back(entity);
    // Console Observer
    static entity_project::EntityConsoleLogger logger;
    AddObserver(entity, &logger);
  }

#ifdef ANVIL2
  /// TODO: Add documentation.
  void SetGraph(const entity_project::IGraph* graph) override { graph_ = graph; }
#endif

  void DeletePackage();

/// ScheduleDelivery schedules a new Package object for delivery (by giving it to a "source"
/// Customer and setting its scheduled private boolean variable) and then passes it onto the
/// TransportPackage subsystem to handle the actual delivery.

  /**
   * @brief ScheduleDelivery schedules a new Package object for delivery
   * 
   * @param[in] package The package being delivered
   * @param[in] dest The destination customer
   * @param[in] details The details of the order
   */
  void ScheduleDelivery(entity_project::Package* package,
    entity_project::Customer* dest, const picojson::object& details);

  bool BeginDelivery(Delivery delivery);

  void Recharge(csci3081::Drone* idleDrone);

  // Added for the sake of compiling
  void AddObserver(entity_project::Entity* entity, entity_project::EntityObserver* observer);

  void RemoveObserver(entity_project::Entity* entity, entity_project::EntityObserver* observer);

  /// Returns the entities_ data structure.
  const std::vector<entity_project::Entity*>& GetEntities() const { return entities_; }

  /// Allows Entity objects (like Drone and Package) to have
  /// their position and direction updated over
  /// time. This function utilizes the TransportPackage subsystem also.
  void Update(float dt);

  /**
   * @brief Drone Simulation destructor. Destroys all created entities.
   * 
   */
  ~DroneSimulation() {
    for (int i = 0; i < entities_.size(); i++) {
      delete entities_.at(i);
    }
  }

 private:
  std::string teamName_ = "Group 17";
  /// A vector containing the pointers of every created object
  std::vector<entity_project::Entity*> entities_;
  /// A vector containing the details of every queued delivery
  std::vector<Delivery> deliveryQueue_;
  EntityFactory factory;
  TransportPackage transport;
  TransportPackage transport1;
  /// The set IGraph graph for pathfinding
  const entity_project::IGraph* graph_;
};

}  // namespace csci3081


#endif  // DRONE_SIMULATION_H_
