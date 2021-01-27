#include "gtest/gtest.h"
#include "src/customer.h"
#include "src/package.h"
#include "src/drone.h"
#include "src/json_helper.h"
#include "EntityProject/ANVIL/drone_delivery_system.h"
#include <EntityProject/osm_graph_parser.h>
#include <EntityProject/graph.h>
#include <EntityProject/project_settings.h>
#include <EntityProject/entity_console_logger.h>
#include <iostream>

namespace csci3081 {
using entity_project::DroneDeliverySystem;
using entity_project::Entity;

class Iter2Test : public ::testing::Test {
 protected:
  virtual void SetUp() {
    customer = new Customer();
    drone = new Drone(details);
    package = new Package();
    droneSimulation = GetDroneDeliverySystem("default");
    graph = parser.CreateGraph("data/umn.osm", "data/umn-height.csv");
    droneSimulation->SetGraph(graph);
  }
  virtual void TearDown() {
    delete customer;
    delete drone;
    delete package;
    delete droneSimulation;
  }

 protected:
  Customer* customer;
  Drone* drone;
  Package* package;
  DroneDeliverySystem* droneSimulation;
  const picojson::object details;
  entity_project::OSMGraphParser parser;
  const entity_project::IGraph* graph;
};


TEST_F(Iter2Test, AssignPackageToDrone) {
  drone->assignPackage(package);
  EXPECT_EQ(drone->isDelivering(), true);
}

TEST_F(Iter2Test, DropoffPackage) {
  drone->assignPackage(package);
  EXPECT_EQ(drone->isDelivering(), true);
  drone->Dropoff();
  EXPECT_EQ(drone->isDelivering(), false);
}

TEST_F(Iter2Test, SetDronePosition) {
  float array[3] = {1.0, 2.0, 1.0};
  drone->setPosition(array);
  std::vector<float> getPos = drone->GetPosition();
  for (int i = 0; i < 3; i++) {
    EXPECT_EQ(array[i], getPos.at(i));
  }
}

TEST_F(Iter2Test, IdleDrone) {
  drone->SetIdle(true);
  EXPECT_EQ(drone->GetIdle(), true);
  EXPECT_EQ(drone->GetSpeed(), 0.0);
}


TEST_F(Iter2Test, UpdateIsZero) {
  float array[3] = {498.292, 253.883, -228.623};
  std::vector<float> array1{498.292, 253.883, -228.623};
  std::vector<float> array2 {-951.412, 254.665, 298.271};

  std::vector<picojson::value> dronePos;
  std::vector<picojson::value> packagePos;
  for (int i = 0; i < 3; i++) {
    dronePos.push_back(picojson::value(array1[i]));
    packagePos.push_back(picojson::value(array2[i]));
  }

  picojson::object obj1;
  obj1["type"] = picojson::value("drone");
  obj1["position"] = picojson::value(dronePos);
  Drone* drone1 = new Drone(obj1);

  picojson::object obj2;
  obj2["type"] = picojson::value("package");
  obj2["position"] = picojson::value(packagePos);
  Package* package1 = new Package(obj2);

  picojson::object obj3;
  obj3["type"] = picojson::value("customer");
  Customer* customer1 = new Customer(obj3);

  droneSimulation->AddEntity(package1);
  droneSimulation->AddEntity(customer1);
  droneSimulation->AddEntity(drone1);
  droneSimulation->ScheduleDelivery(package1, customer1, details);
  EXPECT_EQ(drone1->GetPosition(), array1);
  droneSimulation->Update(0);
  EXPECT_EQ(drone1->GetPosition(), array1);
  droneSimulation->Update(0.2);
  EXPECT_NE(drone1->GetPosition(), array1);
}


TEST_F(Iter2Test, ObserverNotAdded) {
  float array1[3] = {498.292, 253.883, -228.623};
  float array2[3] = {-951.412, 254.665, 298.271};
  float array3[3] = {-255.994, 257.798, -296.859};
  float array4[4] = {1.0, 0.0, 0.0};

  std::vector<picojson::value> dronePos;
  std::vector<picojson::value> packagePos;
  std::vector<picojson::value> customerPos;
  std::vector<picojson::value> droneDir;
  std::vector<picojson::value> packageDir;
  std::vector<picojson::value> customerDir;

  for (int i = 0; i < 3; i++) {
    dronePos.push_back(picojson::value(array1[i]));
    packagePos.push_back(picojson::value(array2[i]));
    customerPos.push_back(picojson::value(array3[i]));
    droneDir.push_back(picojson::value(array4[i]));
    packageDir.push_back(picojson::value(array4[i]));
    customerDir.push_back(picojson::value(array4[i]));
  }
  picojson::object obj1;
  obj1["type"] = picojson::value("drone");
  obj1["position"] = picojson::value(dronePos);
  obj1["direction"] = picojson::value(droneDir);
  Drone* drone1 = new Drone(obj1);

  picojson::object obj2;
  obj2["type"] = picojson::value("package");
  obj2["position"] = picojson::value(packagePos);
  obj2["direction"] = picojson::value(packageDir);
  Package* package1 = new Package(obj2);

  picojson::object obj3;
  obj3["type"] = picojson::value("customer");
  obj3["position"] = picojson::value(customerPos);
  obj3["direction"] = picojson::value(customerDir);
  Customer* customer1 = new Customer(obj3);
  EXPECT_EQ(package1->hasObserver(), false);

  droneSimulation->AddEntity(package1);
  droneSimulation->AddEntity(customer1);
  droneSimulation->AddEntity(drone1);
  droneSimulation->ScheduleDelivery(package1, customer1, details);
  EXPECT_EQ(package1->hasObserver(), true);
}

TEST_F(Iter2Test, BatterySet) {
  float batteryBefore = drone->GetRemainingBattery();
  drone->SetBattery(100.0);
  float batteryAfter = drone->GetRemainingBattery();
  EXPECT_EQ(batteryBefore - 100.0, batteryAfter);
}
TEST_F(Iter2Test, EntityId) {
  picojson::object obj1;
  obj1["type"] = picojson::value("drone");
  Entity* entity = droneSimulation->CreateEntity(obj1);

  picojson::object obj2;
  obj2["type"] = picojson::value("package");
  Entity* entity1 = droneSimulation->CreateEntity(obj2);

  picojson::object obj3;
  obj3["type"] = picojson::value("customer");
  Entity* entity2 = droneSimulation->CreateEntity(obj3);

  EXPECT_NE(entity->GetId(), entity1->GetId());
  EXPECT_NE(entity->GetId(), entity2->GetId());
  EXPECT_NE(entity1->GetId(), entity->GetId());
}


TEST_F(Iter2Test, ScheduleWrongCustomer) {
  float array1[3] = {498.292, 253.883, -228.623};
  float array2[3] = {-951.412, 254.665, 298.271};
  float array3[3] = {-255.994, 257.798, -296.859};
  float array4[4] = {1.0, 0.0, 0.0};

  std::vector<picojson::value> dronePos;
  std::vector<picojson::value> packagePos;
  std::vector<picojson::value> customerPos;
  std::vector<picojson::value> droneDir;
  std::vector<picojson::value> packageDir;
  std::vector<picojson::value> customerDir;

  for (int i = 0; i < 3; i++) {
    dronePos.push_back(picojson::value(array1[i]));
    packagePos.push_back(picojson::value(array2[i]));
    customerPos.push_back(picojson::value(array3[i]));
    droneDir.push_back(picojson::value(array4[i]));
    packageDir.push_back(picojson::value(array4[i]));
    customerDir.push_back(picojson::value(array4[i]));
  }
  picojson::object obj1;
  obj1["type"] = picojson::value("drone");
  obj1["position"] = picojson::value(dronePos);
  obj1["direction"] = picojson::value(droneDir);
  Drone* drone1 = new Drone(obj1);

  picojson::object obj2;
  obj2["type"] = picojson::value("package");
  obj2["position"] = picojson::value(packagePos);
  obj2["direction"] = picojson::value(packageDir);
  Package* package1 = new Package(obj2);

  picojson::object obj3;
  obj3["type"] = picojson::value("customer");
  obj3["position"] = picojson::value(customerPos);
  obj3["direction"] = picojson::value(customerDir);
  Customer* customer1 = new Customer(obj3);

  droneSimulation->AddEntity(package1);
  droneSimulation->AddEntity(customer1);
  droneSimulation->AddEntity(drone1);

  droneSimulation->ScheduleDelivery(package1, customer1, details);
  for (int i = 1; i < 10; i++) {
    droneSimulation->Update(0.1);
  }

  EXPECT_EQ(drone1->isDelivering(), false);
}

TEST_F(Iter2Test, PackageScheduled) {
  float array1[3] = {498.292, 253.883, -228.623};
  float array2[3] = {-951.412, 254.665, 298.271};
  float array3[3] = {-255.994, 257.798, -296.859};
  float array4[4] = {1.0, 0.0, 0.0};

  std::vector<picojson::value> dronePos;
  std::vector<picojson::value> packagePos;
  std::vector<picojson::value> customerPos;
  std::vector<picojson::value> droneDir;
  std::vector<picojson::value> packageDir;
  std::vector<picojson::value> customerDir;

  for (int i = 0; i < 3; i++) {
    dronePos.push_back(picojson::value(array1[i]));
    packagePos.push_back(picojson::value(array2[i]));
    customerPos.push_back(picojson::value(array3[i]));
    droneDir.push_back(picojson::value(array4[i]));
    packageDir.push_back(picojson::value(array4[i]));
    customerDir.push_back(picojson::value(array4[i]));
  }
  picojson::object obj1;
  obj1["type"] = picojson::value("drone");
  obj1["position"] = picojson::value(dronePos);
  obj1["direction"] = picojson::value(droneDir);
  Drone* drone1 = new Drone(obj1);

  picojson::object obj2;
  obj2["type"] = picojson::value("package");
  obj2["position"] = picojson::value(packagePos);
  obj2["direction"] = picojson::value(packageDir);
  Package* package1 = new Package(obj2);

  picojson::object obj3;
  obj3["type"] = picojson::value("customer");
  obj3["position"] = picojson::value(customerPos);
  obj3["direction"] = picojson::value(customerDir);
  Customer* customer1 = new Customer(obj3);

  droneSimulation->AddEntity(package1);
  droneSimulation->AddEntity(customer1);
  droneSimulation->AddEntity(drone1);
  droneSimulation->ScheduleDelivery(package1, customer1, details);
  ASSERT_EQ(package1->isScheduled(), true);
}

TEST_F(Iter2Test, EntityRadius) {
  picojson::object obj1;
  obj1["type"] = picojson::value("drone");
  obj1["radius"] = picojson::value(10.0);
  Entity* entity = droneSimulation->CreateEntity(obj1);
  EXPECT_EQ(entity->GetRadius(), 10.0);

  picojson::object obj2;
  obj2["type"] = picojson::value("package");
  obj2["radius"] = picojson::value(15.0);
  Entity* entity1 = droneSimulation->CreateEntity(obj2);
  EXPECT_EQ(entity1->GetRadius(), 15.0);

  picojson::object obj3;
  obj3["type"] = picojson::value("customer");
  obj3["radius"] = picojson::value(20.0);
  Entity* entity2 = droneSimulation->CreateEntity(obj3);
  EXPECT_EQ(entity2->GetRadius(), 20.0);
}

TEST_F(Iter2Test, DronePool) {
  picojson::object obj1;
  obj1["type"] = picojson::value("drone");
  obj1["model"] = picojson::value("Q-36-01");
  Drone* drone1 = new Drone(obj1);
  EXPECT_EQ(drone1->GetMaxSpeed(), 60);

  picojson::object obj2;
  obj2["type"] = picojson::value("drone");
  obj2["model"] = picojson::value("Q-36-02");
  Drone* drone2 = new Drone(obj2);
  EXPECT_EQ(drone2->GetMaxSpeed(), 55);

  picojson::object obj3;
  obj3["type"] = picojson::value("drone");
  obj3["model"] = picojson::value("Q-36-03");
  Drone* drone3 = new Drone(obj3);
  EXPECT_EQ(drone3->GetMaxSpeed(), 40);

  picojson::object obj4;
  obj4["type"] = picojson::value("drone");
  obj4["model"] = picojson::value("Q-36-04");
  Drone* drone4 = new Drone(obj4);
  EXPECT_EQ(drone4->GetMaxSpeed(), 30);

  picojson::object obj5;
  obj5["type"] = picojson::value("drone");
  obj5["model"] = picojson::value("Q-36-05");
  Drone* drone5 = new Drone(obj5);
  EXPECT_EQ(drone5->GetMaxSpeed(), 60);
}

}  // namespace csci3081
