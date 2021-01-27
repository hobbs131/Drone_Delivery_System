#include "src/customer.h"
#include "src/package.h"
#include "src/drone.h"
#include "src/entity_factory.h"


namespace csci3081 {


  entity_project::Entity* EntityFactory::CreateDrone(const picojson::object& val) {
    entity_project::Entity* MyDrone = new Drone(val);
    return MyDrone;
  }

  entity_project::Entity* EntityFactory::CreatePackage(const picojson::object& val) {
    entity_project::Entity* MyPackage = new Package(val);
    return MyPackage;
  }

  entity_project::Entity* EntityFactory::CreateCustomer(const picojson::object& val) {
    entity_project::Entity* MyCustomer = new Customer(val);
    return MyCustomer;
  }

  entity_project::Entity* EntityFactory::CreateDefault(const picojson::object& val) {
    entity_project::Entity* MyEntity = new entity_project::Entity();
    return MyEntity;
  }
}  // namespace csci3081
