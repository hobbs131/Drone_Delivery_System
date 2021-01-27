#ifndef DRONE_H_
#define DRONE_H_

#include <EntityProject/ANVIL2/drone.h>
#include "src/package.h"
#include "src/customer.h"
#include "src/json_helper.h"
#include <math.h>
#include <vector>
#include <utility>
#include <fstream>
#include <string>
#include <sstream>
#include "src/iter2_route_manager.h"
#include <EntityProject/entity_observer.h>


namespace csci3081 {

/**
 * @brief The Drone class is a concrete class for objects responsible for transporting packages.
 * 
 * This class interacts with EntityFactory (which makes Entity objects and is a concrete factory),
 * DroneSimulation, and TransportPackage classes. It has methods and private variables related to its speed, whether
 * or not it is currently carrying a Package, and whether or not it is in range of a Customer for picking up/dropping off.
 * It also knows of both the Package class (which is a private member variable in this class) and Customer class (which is
 * a parameter to a function in this class) because Drones interact with both directly.
 */
class Drone : public entity_project::Drone {
 private:
    Package* package = nullptr;
    Package* targetPackage = nullptr;
    bool hasPackage = false;
    float speed_ = 30.0;
    bool headedToPackage = false;
    std::vector<Iter2RoutePoint> currRoute;
    std::vector<std::string> nameList;
    // New variables for iteration 2 (and their default initializations)
    float mass = 50;
    float MaxSpeed = 60;
    float BaseAccel = 4;
    float WeightCapacity = 10;
    float BatteryCapacity = 300;
    float MaxBattery = 0;
    std::string physModel;
    float currSpeed = 0;
    // FEATURE 2
    bool idle = true;
    std::vector<std::pair<std::string, std::vector<std::string>>> planetX;
    std::vector<entity_project::EntityObserver*> observers;
    csci3081::Drone* idleDrone_ = nullptr;
    bool recharged = false;


 public:
    /// Used to give each Drone a unique id_
    static int droneID;

    /**
     * @brief The default constructor for Drone
     * 
     * It adds type Drone to TypedObject's type system.
     */
    Drone() { AddType<Drone>(); }

    /// An additional constructor for Drone. It allows the details
    /// variable to be set upon initialization.

    /**
     * @brief A constructor for Drone that allows for details to be set upon initialization.
     * 
     * @param[in] val A picojson containing the details of the Drone
     */
    explicit Drone(const picojson::object& val) {
      AddType<Drone>();
      details_ = val;
      id_ = droneID;
      droneID++;
      if (JsonHelper::ContainsKey(val, "name")) {
        name_ = JsonHelper::GetString(val, "name");
      }
      if (JsonHelper::ContainsKey(val, "position")) {
        auto ValArray1 = JsonHelper::GetArray(val, "position");
        int arraySize = ValArray1.size();
        for (int i = 0; i < arraySize; i++) {
          position_.at(i) = ValArray1[i].get<double>();
        }
        if (arraySize < 3) {
          for (int i = arraySize; i < 3; i++) {
            position_.at(i) = 0.0;
          }
        }
      }
      if (JsonHelper::ContainsKey(val, "direction")) {
        auto ValArray2 = JsonHelper::GetArray(val, "direction");
        int arraySize1 = ValArray2.size();
        for (int i = 0; i < arraySize1; i++) {
          direction_.at(i) = ValArray2[i].get<double>();
        }
        if (arraySize1 < 3) {
          for (int i = arraySize1; i < 3; i++) {
            position_.at(i) = 0.0;
          }
        }
      }
      if (JsonHelper::ContainsKey(val, "speed")) {
        speed_ = JsonHelper::GetDouble(val, "speed");
      }
      if (JsonHelper::ContainsKey(val, "radius")) {
        radius_ = JsonHelper::GetDouble(val, "radius");
      }
      if (JsonHelper::ContainsKey(val, "model")) {
        planetX = Drone_csv("data/planet-x.csv");
        PlanetXParser(JsonHelper::GetString(val, "model"));
      }
      if (JsonHelper::ContainsKey(val, "physics-model")) {
        physModel = JsonHelper::GetString(val, "physics-model");
      }
    }

    /// Returns the Drone's speed.
    float GetSpeed() { return speed_; }

    /**
     * @brief Returns if a Drone is delivering a package
     * 
     * @return true The Drone is delivering a package
     * @return false The Drone is not delivering a package
     */
    bool isDelivering() { return hasPackage; }

    /**
     * @brief A setter for the Drone's package variable
     * 
     * When the Drone has no package to deliver, the pointer is set to NULL
     * 
     * @param[in] package The Package the Drone is assigned to
     */
    void assignPackage(Package* package) {
      this->package = package;
      this->hasPackage = true;
    }

    double GetRadius() {
      return radius_;
    }

    void SetRecharged(bool flag) {
      recharged = flag;
    }

    bool GetRecharged() {
      return recharged;
    }

    /// Based off the Drone's speed and working with the Update(float dt) function,
    /// setPosition updates the
    /// Drone's position variable as it travels.
    /**
     * @brief Sets the position of the Drone based off of its current speed
     * 
     * @param[in] array The new position of the Drone
     */
    void setPosition(float array[3]) {
      for (int i = 0; i < 3; i++) {
        position_.at(i) = array[i];
      }
      // std::cout << position_.at(0) << " " << position_.at(1) << position_.at(2) << std::endl;
    }
    void SetDrone(csci3081::Drone* idleDrone) {
      idleDrone_ = idleDrone;
    }

    csci3081::Drone* GetDrone() {
      return idleDrone_;
    }


    /**
     * @brief Sets the direction of the Drone
     * 
     * @param[in] array The new direction of the Drone
     */
    void setDirection(float array[3]) {
      for (int i = 0; i < 3; i++) {
        direction_.at(i) = array[i];
      }
    }

    /**
     * @brief Get the Id object
     * 
     * @return int 
     */
    int GetId() {
      return id_;
    }

    /// In order to facilitate picking up and dropping off of packages,
    ///  we need to know when the Drone is in
    /// range of a Customer. This method returns a bool based off
    /// whether a Drone is in pickup/dropoff range.
    /**
     * @brief Returns whether or not the Drone is in a dropoff range
     * 
     * @param[in] customer 
     * @return true The Drone is within range of a Customer
     * @return false The Drone is not within range of a Customer
     */
    bool nearCustomer(Customer* customer);

    /**
     * @brief Returns whether or not the Drone is in a pickup range
     * 
     * @param[in] customer 
     * @return true The Drone is within range of a Package
     * @return false The Drone is not within range of a Package
     */
    bool nearPackage(Package* package);

    bool nearDrone(Drone* drone);

    /**
     * @brief Sets the speed of the Drone. Depreciated function.
     * 
     * @param[in] speed New speed of the Drone
     */
    void setSpeed(float speed) {
      this->speed_ = speed;
    }

    /**
     * @brief Returns the pointer of the package the Drone is carrying
     * 
     * If the Drone has no package, it will return NULL
     * 
     * @return Package* The pointer of the package the Drone is carrying
     */
    Package* GetPackage() { return package; }

    /**
     * @brief Sets a new route for the Drone to follow
     * 
     * @param[in] newRoute A vector of route points for the Drone to follow
     */
    void SetRoute(std::vector<Iter2RoutePoint> newRoute) {
      currRoute.clear();
      nameList.clear();
      for (int i = 0; i < newRoute.size(); i++) {
        currRoute.push_back(newRoute.at(i));
      }
      for (int i = 0; i < newRoute.size() - 1; i++) {  // excludes actual end node
        nameList.push_back((newRoute.at(i)).name);
      }
    }

    /**
     * @brief Returns a reference to the current vector of route nodes the Drone is following
     * 
     * @return std::vector<Iter2RoutePoint>& a reference to the current vector of route nodes used
     */
    std::vector<Iter2RoutePoint>& GetRoute() {
      return currRoute;
    }

    /**
     * @brief Returns if the Drone is headed to pick up a Package
     * 
     * @return true The Drone is going to pick up a Package
     * @return false The Drone is not going to pick up a Package
     */
    bool GetHeadedToPackage() { return headedToPackage; }

    /** Sets whether a Drone is headed towards a Package
     * 
     * @param[in] x The new status of the Drone
     * 
     */
    void SetHeadedToPackage(bool x) {
      headedToPackage = x;
      idle = false;
    }

    /**
     * @brief Returns the dynamic status of the Drone
     * 
     * Since Drones regularly move, they are treated as dynamic.
     * 
     * @return true Since Drones regularly move, they are treated as dynamic.
     */
    bool IsDynamic() const { return true; }

    /** Sets whether a Drone is delivering a Package
     * 
     * @param[in] x The new status of the Drone
     * 
     */
    void SetDelivering(bool x) { hasPackage = x; }

    /**
     * @brief Sets which Package the Drone is headed to pickup
     * 
     * @param[in] package The package the Drone is picking up.
     */
     // Comment for tests
    void SetTarget(Package* package) { targetPackage = package; }
    /**
     * @brief Returns the Package a Drone is headed to pickup
     * 
     * @return Package* The package the Drone is picking up.
     */
    Package* GetTarget() { return targetPackage; }

    /// Updates Drone during a Package dropoff to a Customer
    void Dropoff() {
      package = nullptr;
      hasPackage = false;
    }

    //  Iteration 2 added methods!

    /// Returns a vector of each node's name on the current route.
    const std::vector<std::string>& GetCurrentRoute() const {
      return nameList;
    }

    /// Returns the amount of battery left in the drone
    float GetRemainingBattery() const { return BatteryCapacity; }

    /// Returns a drone's mass
    float GetMass() { return mass; }

    /// Returns a drone's current speed
    float GetCurrentSpeed() const { return currSpeed; }

    /// Returns a drone's max speed
    float GetMaxSpeed() const { return MaxSpeed; }

    /// Returns a Drone's maximum Weight Capacity
    float GetMaxCapacity() const { return WeightCapacity; }

    /// Returns the remaining WeightCapacity available
    float GetRemainingCapacity() const { return WeightCapacity; }

    /// Returns base acceleration (used to calculate movement in FlyDrone)
    float GetBaseAcceleration() const { return BaseAccel; }

    /// Returns the physics model to be used for flying the drone
    std::string GetPhysicsModel() { return physModel; }

    /** @brief Notifies all observers attached to package1 of a change
    *
    * @param[in] value, a string representing the state of package1
    * @param[in] package1, the package being observerd
    */
    void Notify(std::string value, Package* package1 = nullptr);

    /** @brief Notifies all observers attached to drone of a change
    *
    * @param[in] value, a string representing the state of the drone
    * @param[in] drone, the drone being observerd
    * @param[in] src, position vector representing start of path
    * @param[in] dest, position vector representing end of path
    */
    void NotifyDrone(std::string value, Drone* drone,
       std::vector<float> src, std::vector<float> dest);

    /** @brief Adds an observer to observers vector
    *
    * @param[in] the observer to be added
    */
    void Attach(entity_project::EntityObserver* observer) {
      observers.push_back(observer);
    }

    /** @brief Removes an observer from the observers vector
    *
    * @param[in] the observer to be removed
    */
    void Detach(entity_project::EntityObserver* observer) {
      observers.erase(std::remove(observers.begin(),
      observers.end(), observer), observers.end());
    }

    /**
     * @brief Sets the details picojson of the Drone
     * 
     * @param[in] details The new details picojson
     */
    void SetDetails(const picojson::object& details) {
      details_ = details;
    }

    /**
     * @brief Returns the details picojson of the Drone
     * 
     * @return const picojson::object& A reference to the details picojson
     */
    const picojson::object& GetDetails() {
      return details_;
    }

    /**
     * @brief Sets the battery life after updating the Drone's position
     * 
     * @param[in] dt Time in seconds between updates
     */
    void SetBattery(float dt) {
      if (dt >= BatteryCapacity) {
        BatteryCapacity = 0;
      } else {
        BatteryCapacity-= dt;
      }
    }

    void AddBattery(float dt) {
      if (this->GetRemainingBattery() + dt >= MaxBattery) {
        this->SetIdle(false);
        BatteryCapacity = MaxBattery;
      } else {
        BatteryCapacity += dt;
      }
      //std::cout << BatteryCapacity << std::endl;
    }

    void SetMaxBattery(float battery) {
      MaxBattery = battery;
    }

    float GetMaxBattery() {
      return MaxBattery;
    }

    /// FEATURE 2: set drone to idle mode if there is no battery remaining
    void SetIdle(bool idle_) {
      this->setSpeed(0.0);
      idle = idle_;
      this->SetRecharged(false);
    }

    /**
     * @brief Returns if the drone is idle or not
     * 
     * @return true The Drone is idle
     * @return false The Drone is not idle
     */
    bool GetIdle() {
      return idle;
    }

    std::vector<std::pair<std::string, std::vector<std::string>>>

    /**
     * @brief This function parses the planet-x.csv file for Drone model info and puts this info into a data structure
     * 
     * References:
     * - gormanalysis.com/blog/reading-and-writing-csv-files-with-cpp/
     * - geeksforgeeks.org/csv-file-management-using-c/
     * 
     * @param[in] FileName The filename of the CSV file used
     * @return std::vector<std::pair<std::string, std::vector<std::string>>> The returned data struture
     */
    Drone_csv(std::string FileName) {
      std::vector<std::pair<std::string, std::vector<std::string>>> result;  // return this

      std::ifstream fin;  // declare input file stream
      fin.open(FileName);  // open the file

      std::string headerLine, headerWord;  // set the headers as the first in all the pairs
      getline(fin, headerLine);  // also, initialize the vectors as the second in all the pairs
      std::stringstream headerStream(headerLine);
      while (getline(headerStream, headerWord, ',')) {
        std::vector<std::string> newVector;
        std::pair<std::string, std::vector<std::string>> headerPair(headerWord, newVector);
        result.push_back(headerPair);
      }

      std::string line, word;  // now fill the vectors according to the data in columns
      while (!fin.eof()) {
        line.clear();
        word.clear();
        getline(fin, line);
        std::stringstream s(line);
        int count = 0;
        while (getline(s, word, ',')) {
          result.at(count).second.push_back(word);
          count++;
        }
      }
      fin.close();
      return result;
    }

    /**
     * @brief Parses the data structure made from data in planet-x.csv
     * 
     * Initializes the attributes of the Drone object with said data
     * 
     * @param[in] model The name of the physics model used
     */
    void PlanetXParser(std::string model) {
      int column = -1;
      int row = -1;
      for (int i = 0; i < planetX.size(); i++) {
        if (planetX.at(i).first.compare("Model #") == 0) {
          column = i;
          for (int j = 0; j < planetX.at(i).second.size(); j++) {
            if (planetX.at(i).second.at(j).compare(model) == 0) {
              row = j;
              break;
            }
          }
        }
      }
      if (!(row == -1)) {
        std::string array[5] = { "Mass (kg)", "Max Speed (km/h)", "Base Acceleration (m/s^2)",
         "WeightCapacity (kg)", "Base Battery Capacity (seconds)" };
        for (int i = 0; i < planetX.size(); i++) {
          if (planetX.at(i).first.compare(array[0]) == 0) {
            this->mass = stof(planetX.at(i).second.at(row));
          } else if (planetX.at(i).first.compare(array[1]) == 0) {
            this->MaxSpeed = stof(planetX.at(i).second.at(row));
          } else if (planetX.at(i).first.compare(array[2]) == 0) {
            this->BaseAccel = stof(planetX.at(i).second.at(row));
          } else if (planetX.at(i).first.compare(array[3]) == 0) {
            this->WeightCapacity = stof(planetX.at(i).second.at(row));
          } else if (planetX.at(i).first.compare(array[4]) == 0) {
           this->BatteryCapacity = stof(planetX.at(i).second.at(row));
           this->MaxBattery = stof(planetX.at(i).second.at(row));
           this->SetMaxBattery(this->MaxBattery);
          }
        }
      }
    }

    /**
     * @brief Sets the current speed of the Drone.
     * 
     * The speed will cap at the drone's maximum speed.
     * Used in the functions that move the Drone in the TransportPackage class
     * 
     * @param[in] spd The new speed of the Drone
     */
    void SetCurrentSpeed(float spd) {
      if (spd > MaxSpeed) {
        currSpeed = MaxSpeed;
      } else {
      currSpeed = spd;
      }
    }
};

}  // namespace csci3081
#endif
