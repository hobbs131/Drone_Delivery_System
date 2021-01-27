#include "gtest/gtest.h"
#include "EntityProject/ANVIL/drone_delivery_system.h"
#include "EntityProject/ANVIL/drone.h"
#include "EntityProject/ANVIL/package.h"
#include "EntityProject/ANVIL/customer.h"
#include <EntityProject/project_settings.h>

#include <iostream>

namespace csci3081 {

using entity_project::DroneDeliverySystem;
using entity_project::Entity;
using entity_project::Drone;
using entity_project::Package;
using entity_project::Customer;

class FactoryTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    system = GetDroneDeliverySystem("default");
  }
  virtual void TearDown() {}

  DroneDeliverySystem* system;
};
/*******************************************************************************
 * Test Cases
 ******************************************************************************/

TEST_F(FactoryTest, DroneCreated) {
  picojson::object obj;
  obj["type"] = picojson::value("drone");
  Entity* entity = system->CreateEntity(obj);
  ASSERT_NE(entity, nullptr);
  Drone* drone = entity->AsType<Drone>();
  ASSERT_NE(drone, nullptr);
  ASSERT_EQ(system->GetEntities().size(), 0);
  system->AddEntity(drone);
  ASSERT_EQ(system->GetEntities().size(), 1);
  ASSERT_EQ(picojson::value(system->GetEntities()[0]->GetDetails()).serialize(),
  picojson::value(obj).serialize());
}

TEST_F(FactoryTest, PackageCreated) {
  picojson::object obj;
  obj["type"] = picojson::value("package");
  Entity* entity = system->CreateEntity(obj);
  ASSERT_NE(entity, nullptr);
  Package* package = entity->AsType<Package>();
  ASSERT_NE(package, nullptr);
  ASSERT_EQ(system->GetEntities().size(), 0);
  system->AddEntity(package);
  ASSERT_EQ(system->GetEntities().size(), 1);
  ASSERT_EQ(picojson::value(system->GetEntities()[0]->GetDetails()).serialize(),
  picojson::value(obj).serialize());
}

TEST_F(FactoryTest, CustomerCreated) {
  picojson::object obj;
  obj["type"] = picojson::value("customer");
  Entity* entity = system->CreateEntity(obj);
  ASSERT_NE(entity, nullptr);
  Customer* customer = entity->AsType<Customer>();
  ASSERT_NE(customer, nullptr);
  ASSERT_EQ(system->GetEntities().size(), 0);
  system->AddEntity(customer);
  ASSERT_EQ(system->GetEntities().size(), 1);
  ASSERT_EQ(picojson::value(system->GetEntities()[0]->GetDetails()).serialize(),
  picojson::value(obj).serialize());
}


}  // namespace csci3081
