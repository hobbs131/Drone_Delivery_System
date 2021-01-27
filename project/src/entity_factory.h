#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "src/customer.h"
#include "src/package.h"
#include "src/drone.h"


namespace csci3081 {


/**
 * @brief EntityFactory is the singular, concrete factory for creating Drone, Customer, and Package objects with type Entity*.
 * 
 * This class is a subsystem of the DroneDeliverySystem facade implemented
 * in DroneSimulation. It creates the objects
 * necessary for the simulation system.
 */
class EntityFactory {
 public:
  /// Creates a Drone object from a picojson::object reference

  /**
   * @brief Creates a Drone object
   * 
   * @param[in] val A picojson containing the details of the created object
   * @return entity_project::Entity* A pointer to the created object
   */
  entity_project::Entity* CreateDrone(const picojson::object& val);

  /**
   * @brief Creates a Package object
   * 
   * @param[in] val A picojson containing the details of the created object
   * @return entity_project::Entity* A pointer to the created object
   */
  entity_project::Entity* CreatePackage(const picojson::object& val);

  /**
   * @brief Creates a Customer object
   * 
   * @param[in] val A picojson containing the details of the created object
   * @return entity_project::Entity* A pointer to the created object
   */
  entity_project::Entity* CreateCustomer(const picojson::object& val);

  /**
   * @brief Creates a default entity object
   * 
   * @param[in] val A picojson containing the details of the created object
   * @return entity_project::Entity* A pointer to the created object
   */
  entity_project::Entity* CreateDefault(const picojson::object& val);
};

}  // namespace csci3081


#endif  // ENTITY_FACTORY_H
