#include "gtest/gtest.h"
#include "src/customer.h"
#include "src/package.h"
#include "src/drone.h"
#include "src/json_helper.h"

namespace csci3081 {


class EntityTest : public ::testing::Test {
 public:
  void SetUp() {
    customer = new Customer();
    drone = new Drone();
    package = new Package();
  }
 protected:
  Customer* customer;
  Drone* drone;
  Package* package;
};

TEST_F(EntityTest, IsDelivering) {
  drone->SetDelivering(true);
  EXPECT_EQ(drone->isDelivering(), true);
}

TEST_F(EntityTest, assignPackage) {
  drone->assignPackage(package);
  EXPECT_EQ(drone->isDelivering(), true);
}

TEST_F(EntityTest, Dropoff) {
  drone->assignPackage(package);
  EXPECT_EQ(drone->isDelivering(), true);
  drone->Dropoff();
  EXPECT_EQ(drone->isDelivering(), false);
}

TEST_F(EntityTest, setPos) {
  float array[3] = {1.0, 2.0, 1.0};
  drone->setPosition(array);
  std::vector<float> getPos = drone->GetPosition();
  for (int i = 0; i < 3; i++) {
    EXPECT_EQ(array[i], getPos.at(i));
  }
}

TEST_F(EntityTest, Customer) {
  customer->SetShouldReceive(true);
  EXPECT_EQ(customer->ShouldReceive(), true);
}

TEST_F(EntityTest, IsDynamic) {
  package->setScheduled(true);
  EXPECT_EQ(package->IsDynamic(), true);
}

TEST_F(EntityTest, EnRoute) {
  package->setEnRoute(true);
  EXPECT_EQ(package->isEnRoute(), true);
}

TEST_F(EntityTest, Delivered) {
  package->setDelivered(true);
  EXPECT_EQ(package->isDelivered(), true);
}

TEST_F(EntityTest, CustDynamic) {
  EXPECT_EQ(customer->IsDynamic(), false);
}




}  // namespace csci3081
