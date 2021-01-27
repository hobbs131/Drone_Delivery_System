#include <EntityProject/ANVIL/drone_delivery_system.h>
#include <EntityProject/graph.h>
#include <vector>
#include <string>
#include "src/json_helper.h"
#include "src/drone.h"
#include "src/package.h"
#include "src/customer.h"
#include "src/entity_factory.h"
#include "src/transport_package.h"
#include "src/drone_simulation.h"
#include "src/delivery.h"

namespace csci3081 {

  entity_project::Entity* DroneSimulation::CreateEntity(const picojson::object& val) {
    entity_project::Entity* entity = nullptr;
    if (JsonHelper::ContainsKey(val, "type")) {
      if (JsonHelper::GetString(val, "type") == "drone") {
        entity = factory.CreateDrone(val);
      } else if (JsonHelper::GetString(val, "type") == "customer") {
        entity = factory.CreateCustomer(val);
      } else if (JsonHelper::GetString(val, "type") == "package") {
        entity = factory.CreatePackage(val);
      } else {
        entity = factory.CreateDefault(val);
      }
    }
    return entity;
  }

  void DroneSimulation::Recharge(csci3081::Drone* idleDrone) {
      Drone* rechargeDrone;
      for (int i = 0; i < entities_.size(); i++) {
        Drone* drone = entities_.at(i)->AsType<csci3081::Drone>();
        if (drone && drone->GetMaxBattery() == 1000) {
          // std::cout << "in if statement" << std::endl;
          rechargeDrone = entities_.at(i)->AsType<csci3081::Drone>();
          rechargeDrone->SetDrone(idleDrone);
          break;
        }
      }
      //td::cout << "before setting route" << std::endl;
      //transport.SetRouteToPackage(idleDrone, idleDrone->GetTarget(), graph_);
      transport1.SetRouteToDrone(rechargeDrone, idleDrone, graph_);
      //std::cout << "after setting route" << std::endl;
  }
  void DroneSimulation::DeletePackage() {
    for (int j = 0; j < entities_.size(); j++) {
      Package* package = entities_.at(j)->AsType<Package>();
      if (package) {
        if (package->isDelivered()) {
          delete package;
          entities_.erase(entities_.begin() + j);
        }
      }
    }
  }

  void DroneSimulation::ScheduleDelivery(
          entity_project::Package* package,
          entity_project::Customer* dest,
          const picojson::object& details) {
      Package* p = package->AsType<Package>();
      Customer* c = dest->AsType<Customer>();
      if (c && p) {
        Delivery newDelivery(package, dest, details);
        deliveryQueue_.push_back(newDelivery);
        // this will be done once every update() cycle as well
        for (int i = 0; i < deliveryQueue_.size(); i++) {
          bool beganDelivery = BeginDelivery(deliveryQueue_.at(i));
          if (beganDelivery) {
            // remove assigned deliveries, and revert index to prev. position
            deliveryQueue_.erase(deliveryQueue_.begin() + i);
            i--;
          }
        }
      }
    }

  bool DroneSimulation::BeginDelivery(Delivery delivery) {
    bool beganDelivery = false;
    if ((delivery.package_)->nearCustomer(delivery.customer_)) {
      this->DeletePackage();
      return beganDelivery = true;
    } else if (!((delivery.package_)->nearCustomer(delivery.customer_))) {
      (delivery.customer_)->SetShouldReceive(true);
      (delivery.package_)->SetDest(delivery.customer_);
      (delivery.package_)->setScheduled(true);
      for (int i = 0; i < entities_.size(); i++) {
        Drone* drone = entities_.at(i)->AsType<Drone>();
        if (drone) {
          // drone->NotifyDrone("idle", drone, {0, 0, 0}, {0, 0, 0});
          if (drone->isDelivering() == false && drone->GetHeadedToPackage() == false) {
            beganDelivery = true;
            drone->Notify("scheduled", delivery.package_);
            drone->SetHeadedToPackage(true);
            transport.SetRouteToPackage(drone, delivery.package_, graph_);
            drone->SetTarget(delivery.package_);
            break;
          }
        }
      }
      return beganDelivery;
    }
  }

  void DroneSimulation::Update(float dt) {
    // scheduling any leftover deliveries for any now-free drones
    for (int i = 0; i < deliveryQueue_.size(); i++) {
      bool beganDelivery = BeginDelivery(deliveryQueue_.at(i));
      if (beganDelivery) {
        // remove assigned deliveries, and revert index to prev. position
        deliveryQueue_.erase(deliveryQueue_.begin() + i);
        i--;
      }
    }

    // currently there is no way to discriminate based on order priority, will be implemented
    // in iteration 3
    for (int i = 0; i < entities_.size(); i++) {
      Drone* drone = entities_.at(i)->AsType<csci3081::Drone>();
      if (drone) {
        if (drone->GetHeadedToPackage() && !(drone->nearPackage(drone->GetTarget()))) {
          if (!drone->GetIdle()) {
            transport.FlyDrone(drone, dt);
          } else {
            if (drone->GetTarget() != NULL && !drone->GetRecharged()) {
              drone->SetRecharged(true);
              Recharge(drone);
              drone->NotifyDrone("idle", drone, {0, 0, 0}, {0, 0, 0});
            }
          }
        } else if (drone->GetHeadedToPackage() && drone->nearPackage(drone->GetTarget())) {
          transport.PickupPackage(drone, drone->GetTarget());
          transport.SetRouteToCustomer(drone, drone->GetPackage()->GetDest(), graph_);
        } else if (drone->isDelivering() &&
        !(drone->nearCustomer(drone->GetPackage()->GetDest()))) {
          if (!drone->GetIdle()) {
            transport.FlyDrone(drone, dt);
          } else {
            if (drone->GetPackage() != NULL && drone->GetRecharged() == false) {
              drone->SetRecharged(true);
              Recharge(drone);
            }
          }
        } else if (drone->isDelivering() &&
         drone->nearCustomer(drone->GetPackage()->GetDest())) {
          transport.DropoffPackage(drone, drone->GetPackage()->GetDest());
          this->DeletePackage();
        } else if (drone->GetMaxBattery() == 1000 && drone->GetDrone() != nullptr) {
            if (drone->nearDrone(drone->GetDrone())) {
              drone->GetDrone()->AddBattery(dt);
              drone->SetBattery(dt);
            } else if ((!drone->nearDrone(drone->GetDrone())) && drone->GetDrone()->GetIdle()) {
                transport1.FlyDrone(drone, dt);
            }
        }
      }
    }
  }

  void DroneSimulation::AddObserver(entity_project::Entity* entity,
    entity_project::EntityObserver* observer) {
    picojson::object val = entity->GetDetails();
    csci3081::Package* pkg = nullptr;
    csci3081::Drone* drone = nullptr;
    if (JsonHelper::ContainsKey(val, "type")) {
      if (JsonHelper::GetString(val, "type") == "package") {
        if (entity != nullptr) {
          pkg = entity->AsType<csci3081::Package>();
          pkg->Attach(observer);
        }
      } else if (JsonHelper::GetString(val, "type") == "drone") {
        if (entity != nullptr) {
          drone = entity->AsType<csci3081::Drone>();
          drone->Attach(observer);
        }
      }
    }
  }

  void DroneSimulation::RemoveObserver(entity_project::Entity* entity,
    entity_project::EntityObserver* observer) {
      picojson::object val = entity->GetDetails();
      csci3081::Package* pkg = nullptr;
      csci3081::Drone* drone = nullptr;
      if (JsonHelper::ContainsKey(val, "type")) {
        if (JsonHelper::GetString(val, "type") == "package") {
          pkg = entity->AsType<csci3081::Package>();
          pkg->Detach(observer);
        } else if (JsonHelper::GetString(val, "type") == "drone") {
          drone = entity->AsType<csci3081::Drone>();
          drone->Detach(observer);
        }
      }
  }
}  // namespace csci3081
