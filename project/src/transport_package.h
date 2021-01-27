#ifndef TRANSPORT_PACKAGE_H
#define TRANSPORT_PACKAGE_H


// #include <EntityProject/ANVIL/simple_UMN_route_manager.h>
#include <src/iter2_route_manager.h>
#include "src/customer.h"
#include "src/package.h"
#include "src/drone.h"
#include "src/json_helper.h"
#include <vector>


namespace csci3081 {

/**
 * @brief The TransportPackage class allows Packages to move from one place to another.
 *
 * After a Package is scheduled for delivery inside DroneSimulation, this class has methods
 * that do the work of picking it up, flying it to its destination, and dropping it off.
 * This class also uses Iter2RouteManager to set the route for the Drone.
 */
class TransportPackage {
 private:
  int vectorIndex;
  int vectorSize;

 public:
  /**
   * @brief Sets the route from the drone to the package
   * 
   * @param[in] src Source node, the location of the drone
   * @param[in] dest Source node, the location of the package
   * @param[in] graph IGraph set holding node data
   */
  void SetRouteToPackage(Drone* src, Package* dest, const entity_project::IGraph* graph) {
    Iter2RouteManager manager;
    const std::vector<float> PackagePos = dest->GetPosition();
    const std::vector<float> DronePos = src->GetPosition();
    float srcX = DronePos.at(0);
    float srcY = DronePos.at(1);
    float srcZ = DronePos.at(2);
    float destX = PackagePos.at(0);
    float destY = PackagePos.at(1);
    float destZ = PackagePos.at(2);
    Iter2RoutePoint src1 = Iter2RoutePoint(srcX, srcY, srcZ);
    Iter2RoutePoint dest1 = Iter2RoutePoint(destX, destY, destZ);
    src1.name = "start";
    dest1.name = "dest";
    manager.SetGraph(graph);
    src->SetRoute(manager.GetRoute(src1, dest1));
    src->NotifyDrone("moving", src, DronePos, PackagePos);
    vectorIndex = 0;
    vectorSize = src->GetRoute().size();
  }

    void SetRouteToDrone(Drone* src, Drone* dest, const entity_project::IGraph* graph) {
    Iter2RouteManager manager;
    const std::vector<float> RechargeDronePos = src->GetPosition();
    const std::vector<float> DronePos = dest->GetPosition();
    float srcX = RechargeDronePos.at(0);
    float srcY = RechargeDronePos.at(1);
    float srcZ = RechargeDronePos.at(2);
    float destX = DronePos.at(0);
    float destY = DronePos.at(1);
    float destZ = DronePos.at(2);
    Iter2RoutePoint src1 = Iter2RoutePoint(srcX, srcY, srcZ);
    Iter2RoutePoint dest1 = Iter2RoutePoint(destX, destY, destZ);
    src1.name = "start";
    dest1.name = "dest";
    manager.SetGraph(graph);
    src->SetRoute(manager.GetRoute(src1, dest1));
    vectorIndex = 0;
    vectorSize = src->GetRoute().size();
  }

  /**
   * @brief Allows the Drone to pick up the Package
   * 
   * @param[in] src The Drone picking up the Package
   * @param[in] dest The Package being picked up
   */
  void PickupPackage(Drone* src, Package* dest) {
    src->assignPackage(dest);
    src->SetHeadedToPackage(false);
    src->SetTarget(nullptr);
    dest->setEnRoute(true);
    src->Notify("en route");
  }

  /**
   * @brief Sets the route from the drone to the customer
   * 
   * @param[in] src Source node, the location of the drone
   * @param[in] dest Source node, the location of the package
   * @param[in] graph IGraph pointer holding the node data
   */
  void SetRouteToCustomer(Drone* src, Customer* dest, const entity_project::IGraph* graph) {
    Iter2RouteManager manager;
    const std::vector<float> CustomerPos = dest->GetPosition();
    const std::vector<float> DronePos = src->GetPosition();
    float srcX = DronePos.at(0);
    float srcY = DronePos.at(1);
    float srcZ = DronePos.at(2);
    float destX = CustomerPos.at(0);
    float destY = CustomerPos.at(1);
    float destZ = CustomerPos.at(2);
    Iter2RoutePoint src1 = Iter2RoutePoint(srcX, srcY, srcZ);
    Iter2RoutePoint dest1 = Iter2RoutePoint(destX, destY, destZ);
    src1.name = "start";
    dest1.name = "dest";
    manager.SetGraph(graph);
    src->SetRoute(manager.GetRoute(src1, dest1));
    src->NotifyDrone("moving", src, DronePos, CustomerPos);
    vectorIndex = 0;
    vectorSize = src->GetRoute().size();
  }

  /**
   * @brief Drops off the Package to the Customer
   * 
   * @param[in] src The Drone delivering the Package
   * @param[in] dest The Customer receiving the Package
   */
  void DropoffPackage(Drone* src, Customer* dest) {
    dest->SetShouldReceive(false);
    src->GetPackage()->setEnRoute(false);
    src->GetPackage()->setScheduled(false);
    src->GetPackage()->setDelivered(true);
    src->Notify("delivered");
    src->NotifyDrone("idle", src, {0, 0, 0}, {0, 0, 0});
    src->Dropoff();
  }

  /**
   * @brief Flies the Drone based on acceleration physics
   * 
   * @param[in] drone The Drone being moved
   * @param[in] dt The time in seconds between updates
   */
  void FlyDroneAccel(Drone* drone, float dt) {
    auto PointsVector = drone->GetRoute();
    std::vector<float> DronePos = drone->GetPosition();
    float droneX = DronePos.at(0);
    float droneY = DronePos.at(1);
    float droneZ = DronePos.at(2);
    const Iter2RoutePoint& DronePoint =
    Iter2RoutePoint(droneX, droneY, droneZ);
    if (DronePoint.DistanceBetween(PointsVector.at(vectorIndex)) < drone->GetRadius()) {
      if (vectorIndex+1 < vectorSize) {
        vectorIndex++;
      }
    }
    float vectorBetween[3] = {PointsVector.at(vectorIndex).x - DronePoint.x,
      PointsVector.at(vectorIndex).y - DronePoint.y,
      PointsVector.at(vectorIndex).z - DronePoint.z};
    float magnitude = sqrt(pow(vectorBetween[0], 2)
      + pow(vectorBetween[1], 2) + pow(vectorBetween[2], 2));
    vectorBetween[0] = vectorBetween[0] / magnitude;
    vectorBetween[1] = vectorBetween[1] / magnitude;
    vectorBetween[2] = vectorBetween[2] / magnitude;

    std::vector<float> velocity;
    velocity.push_back(vectorBetween[0] * drone->GetCurrentSpeed());
    velocity.push_back(vectorBetween[1] * drone->GetCurrentSpeed());
    velocity.push_back(vectorBetween[2] * drone->GetCurrentSpeed());

    std::vector<float> updateVelocity;
    updateVelocity.push_back(vectorBetween[0] * drone->GetBaseAcceleration() * dt);
    updateVelocity.push_back(vectorBetween[1] * drone->GetBaseAcceleration() * dt);
    updateVelocity.push_back(vectorBetween[2] * drone->GetBaseAcceleration() * dt);

    velocity.at(0) += updateVelocity.at(0);
    velocity.at(1) += updateVelocity.at(1);
    velocity.at(2) += updateVelocity.at(2);

    float velocityMagnitude = sqrt(pow(velocity.at(0), 2)
     + pow(velocity.at(1), 2) + pow(velocity.at(2), 2));
    drone->SetCurrentSpeed(velocityMagnitude);

    float updateArray[3] = {DronePoint.x + velocity.at(0) * dt,
      DronePoint.y + velocity.at(1) * dt,
      DronePoint.z + velocity.at(2) * dt};

    if (drone->GetRemainingBattery() - dt > 0) {
      drone->setPosition(updateArray);
      drone->setDirection(vectorBetween);
      drone->SetBattery(dt);
      if (drone->isDelivering()) {
        drone->GetPackage()->setPosition(updateArray);
        drone->GetPackage()->setDirection(vectorBetween);
      }
    } else {
      drone->SetBattery(drone->GetRemainingBattery());
      drone->SetIdle(true);
    }
  }

  /**
   * @brief Flies the Drone based on force physics
   * 
   * @param[in] drone The Drone being moved
   * @param[in] dt The time in seconds between updates
   */
  void FlyDroneForce(Drone* drone, float dt) {
    auto PointsVector = drone->GetRoute();
    const std::vector<float> DronePos = drone->GetPosition();
    float droneX = DronePos.at(0);
    float droneY = DronePos.at(1);
    float droneZ = DronePos.at(2);
    const Iter2RoutePoint& DronePoint =
    Iter2RoutePoint(droneX, droneY, droneZ);
    if (DronePoint.DistanceBetween(PointsVector.at(vectorIndex)) < drone->GetRadius()) {
      if (vectorIndex+1 < vectorSize) {
        vectorIndex++;
      }
    }
    float vectorBetween[3] = {PointsVector.at(vectorIndex).x - DronePoint.x,
      PointsVector.at(vectorIndex).y - DronePoint.y,
      PointsVector.at(vectorIndex).z - DronePoint.z};
    float magnitude = sqrt(pow(vectorBetween[0], 2)
      + pow(vectorBetween[1], 2) + pow(vectorBetween[2], 2));
    vectorBetween[0] = vectorBetween[0] / magnitude;
    vectorBetween[1] = vectorBetween[1] / magnitude;
    vectorBetween[2] = vectorBetween[2] / magnitude;

    float F_drone = drone->GetMass() * drone->GetBaseAcceleration();
    float M_total = drone->GetMass();
    if (drone->isDelivering()) {
      M_total += drone->GetPackage()->GetWeight();
    }
    float accel = F_drone / M_total;

    std::vector<float> velocity;
    velocity.push_back(vectorBetween[0] * drone->GetCurrentSpeed());
    velocity.push_back(vectorBetween[1] * drone->GetCurrentSpeed());
    velocity.push_back(vectorBetween[2] * drone->GetCurrentSpeed());

    std::vector<float> updateVelocity;
    updateVelocity.push_back(vectorBetween[0] * accel * dt);
    updateVelocity.push_back(vectorBetween[1] * accel * dt);
    updateVelocity.push_back(vectorBetween[2] * accel * dt);

    velocity.at(0) += updateVelocity.at(0);
    velocity.at(1) += updateVelocity.at(1);
    velocity.at(2) += updateVelocity.at(2);

    float velocityMagnitude = sqrt(pow(velocity.at(0), 2)
     + pow(velocity.at(1), 2) + pow(velocity.at(2), 2));
    drone->SetCurrentSpeed(velocityMagnitude);

    float updateArray[3] = {DronePoint.x + velocity.at(0) * dt,
      DronePoint.y + velocity.at(1) * dt,
      DronePoint.z + velocity.at(2) * dt};

    if (drone->GetRemainingBattery() - dt > 0) {
      drone->setPosition(updateArray);
      drone->setDirection(vectorBetween);
      drone->SetBattery(dt);
      if (drone->isDelivering()) {
        drone->GetPackage()->setPosition(updateArray);
        drone->GetPackage()->setDirection(vectorBetween);
      }
    } else {
      drone->SetBattery(dt);
      drone->SetIdle(true);
    }
  }

  /**
   * @brief This is the function Update(float dt) uses to fly the Drone.
   * 
   * @param[in] drone Drone being moved
   * @param[in] dt Time between updates in seconds
   */
  void FlyDrone(Drone* drone, float dt) {
    if (drone->GetPhysicsModel().compare("acceleration") == 0) {
      FlyDroneAccel(drone, dt);
    } else if (drone->GetPhysicsModel().compare("force") == 0) {
      FlyDroneForce(drone, dt);
    } else {
      drone->SetCurrentSpeed(drone->GetMaxSpeed());
      auto PointsVector = drone->GetRoute();
      std::vector<float> DronePos = drone->GetPosition();
      float droneX = DronePos.at(0);
      float droneY = DronePos.at(1);
      float droneZ = DronePos.at(2);
      const Iter2RoutePoint& DronePoint =
      Iter2RoutePoint(droneX, droneY, droneZ);
      if (DronePoint.DistanceBetween(PointsVector.at(vectorIndex)) < drone->GetRadius()) {
        if (vectorIndex+1 < vectorSize) {
          vectorIndex++;
        }
      }
      float vectorBetween[3] = {PointsVector.at(vectorIndex).x - DronePoint.x,
        PointsVector.at(vectorIndex).y - DronePoint.y,
        PointsVector.at(vectorIndex).z - DronePoint.z};
      float magnitude = sqrt(pow(vectorBetween[0], 2)
        + pow(vectorBetween[1], 2) + pow(vectorBetween[2], 2));
      vectorBetween[0] = vectorBetween[0] / magnitude;
      vectorBetween[1] = vectorBetween[1] / magnitude;
      vectorBetween[2] = vectorBetween[2] / magnitude;
      float updateArray[3] = {DronePoint.x + vectorBetween[0] * drone->GetMaxSpeed() * dt,
        DronePoint.y + vectorBetween[1] * drone->GetMaxSpeed() * dt,
        DronePoint.z + vectorBetween[2] * drone->GetMaxSpeed() * dt};
      if (drone->GetRemainingBattery() - dt > 0) {
        drone->setPosition(updateArray);
        drone->setDirection(vectorBetween);
        drone->SetBattery(dt);
        if (drone->isDelivering()) {
          drone->GetPackage()->setPosition(updateArray);
          drone->GetPackage()->setDirection(vectorBetween);
        }
      } else {
        drone->SetBattery(dt);
        drone->SetIdle(true);
      }
    }
  }
};

}  // namespace csci3081


#endif  // TRANSPORT_PACKAGE_H
