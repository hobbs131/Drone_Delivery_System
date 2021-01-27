#include <EntityProject/ANVIL2/drone.h>
#include "src/package.h"
#include "src/customer.h"
#include "src/json_helper.h"
#include "src/drone.h"
#include <math.h>
#include <string>
#include <vector>
#include <iostream>

namespace csci3081 {

  // std::vector<std::pair<std::string, std::vector<std::string>>> Drone::planetX =
  //  Drone_csv("data/planet-x.csv");
  int Drone::droneID = 1;
  bool Drone::nearCustomer(Customer* customer) {
    std::vector<float> CustomerPos = customer->GetPosition();
    float XDistance = this->position_.at(0) - (CustomerPos.at(0));
    float YDistance = this->position_.at(1) - (CustomerPos.at(1));
    float ZDistance = this->position_.at(2) - (CustomerPos.at(2));
    float dist = sqrt(pow(XDistance, 2) + pow(YDistance, 2) + pow(ZDistance, 2));
    if (dist < customer->GetRadius() + this->GetRadius()) {
      return true;
    }
    return false;
  }
  bool Drone::nearDrone(Drone* drone) {
    std::vector<float> dronePos = drone->GetPosition();
    float XDistance = this->position_.at(0) - (dronePos.at(0));
    float YDistance = this->position_.at(1) - (dronePos.at(1));
    float ZDistance = this->position_.at(2) - (dronePos.at(2));
    float dist = sqrt(pow(XDistance, 2) + pow(YDistance, 2) + pow(ZDistance, 2));
    if (dist < drone->GetRadius() + this->GetRadius()) {
      return true;
    }
    return false;
  }
  bool Drone::nearPackage(Package* package) {
    std::vector<float> PackagePos = package->GetPosition();
    float XDistance = this->position_.at(0) - (PackagePos.at(0));
    float YDistance = this->position_.at(1) - (PackagePos.at(1));
    float ZDistance = this->position_.at(2) - (PackagePos.at(2));
    float dist = sqrt(pow(XDistance, 2) + pow(YDistance, 2) + pow(ZDistance, 2));
    if (dist < package->GetRadius() + this->GetRadius()) {
      return true;
    }
    return false;
  }

  void Drone::Notify(std::string value, Package* package1) {
    if (package1 == nullptr) {
      package1 = package;
    }
    picojson::object temp;
    temp[static_cast<std::string>("type")] = picojson::value(static_cast<std::string>("notify"));
    temp[static_cast<std::string>("value")] = picojson::value(static_cast<std::string>(value));
    picojson::value event(temp);
    std::vector<entity_project::EntityObserver*> obs = package1->getObservers();
    for (int i = 0; i < obs.size(); i++) {
      obs.at(i)->OnEvent(event, *package1);
    }
  }

  void Drone::NotifyDrone(std::string value, Drone* drone,
    std::vector<float> src, std::vector<float> dest) {
    picojson::object temp;
    temp[static_cast<std::string>("type")] = picojson::value(static_cast<std::string>("notify"));
    temp[static_cast<std::string>("value")] = picojson::value(static_cast<std::string>(value));
    if (value == "moving") {
      vector<vector<float>> arr;
      arr.push_back(dest);
      arr.push_back(src);
      temp[static_cast<std::string>("path")] = JsonHelper::EncodeArray(arr);
    }
    picojson::value event(temp);
    for (int i = 0; i < observers.size(); i++) {
      observers.at(i)->OnEvent(event, *drone);
    }
  }

}  // namespace csci3081
