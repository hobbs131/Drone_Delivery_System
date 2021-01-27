#ifndef PACKAGE_H_
#define PACKAGE_H_

#include <EntityProject/ANVIL2/package.h>
#include "src/customer.h"
#include "src/json_helper.h"
#include <vector>
#include <EntityProject/entity_observer.h>


namespace csci3081 {


/**
 * @brief The Package class is a concrete class for objects that get delivered via Drone.
 * 
 * This class interacts with its factory, EntityFactory, as well as the DroneSimulation and TransportPackage
 * classes. Because it is important for the simulation to know the status of a Package (whether it needs to be
 * scheduled, is in transit, etc.) the variables and methods of this pertain to that. EntityFactory will
 * create Package objects, and DroneSimulation/TransportPackage will interact with the variables and methods
 * here to facilitate delivery.
 */
class Package : public entity_project::Package {
 private:
  bool scheduled;
  bool enRoute;
  bool delivered;
  bool hasObserver_ = false;
  Customer* dest;
  float weight = 0;
  std::vector<entity_project::EntityObserver*> observers;

 public:
    /// Gives each individual Package a unique id_ attribute
    static int packageID;

    /**
     * @brief This is the default constructor for Package.
     * 
     * It adds type Drone to TypedObject's type system.
     */
    Package() { AddType<Package>(); }

    /**
     * @brief An additional constructor for Package that allows the details to be set upon initalization
     * 
     * @param[in] val The picojson containing all of the details of the Package
     */
    explicit Package(const picojson::object& val) {
      AddType<Package>();
      details_ = val;
      id_ = packageID;
      packageID++;
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
      if (JsonHelper::ContainsKey(val, "radius")) {
        radius_ = JsonHelper::GetDouble(val, "radius");
      }
      if (JsonHelper::ContainsKey(val, "weight")) {
        weight = JsonHelper::GetDouble(val, "weight");
      }
    }

    /**
     * @brief Returns if the Package has been scheduled yet
     * 
     * @return true The package has been scheduled for delivery
     * @return false The package has not been scheduled for delivery
     */
    bool isScheduled() { return scheduled; }

    /**
     * @brief Returns if the Package is currently in transit
     * 
     * @return true The Package is currently in transit
     * @return false The Package is not currently in transit
     */
    bool isEnRoute() { return enRoute; }

    /**
     * @brief Returns if the package has been delivered yet
     * 
     * If it has been delivered, it can safely be deleted fron the entities_ vector in DroneSimulation
     * 
     * @return true The Package has been delivered
     * @return false The Package has not been delivered
     */
    bool isDelivered() { return delivered; }

    /**
     * @brief Sets the position of the Package
     * 
     * @param[in] array The new position coordinates of the Package
     */
    void setPosition(float array[3]) {
      for (int i = 0; i < 3; i++) {
        position_.at(i) = array[i];
      }
    }

    /**
     * @brief Sets the direction of the Package
     * 
     * @param[in] array The new direction coordinates of the Package
     */
    void setDirection(float array[3]) {
      for (int i = 0; i < 3; i++) {
        direction_.at(i) = array[i];
      }
    }

    /**
     * @brief Sets the Package as scheduled for delivery
     * 
     * @param[in] x The new status of the Package
     */
    void setScheduled(bool x) { scheduled = x; }

    /**
     * @brief Sets the Package as en route of its destination when it is picked up by a Drone
     * 
     * @param[in] y The new status of the Package
     */
    void setEnRoute(bool y) { enRoute = y; }

    /// Allows the delivered private boolean variable to
    /// be updated when a Drone drops off a Package.

    /**
     * @brief Sets the Package as delivered to its destination
     * 
     * @param[in] z The new status of the Package
     */
    void setDelivered(bool z) { delivered = z; }

    /**
     * @brief Sets the destination Customer of the Package
     * 
     * @param[in] customer The Customer the Package will be delivered to
     */
    void SetDest(Customer* customer) {
      this->dest = customer;
    }

    /**
     * @brief Returns the Customer the Package will be delivered to
     * 
     * @return Customer* A pointer to the Customer that the Package will be delivered to
     */
    Customer* GetDest() {
      return this->dest;
    }

    /**
     * @brief Get the Id object
     * 
     * @return int 
     */
    int GetId() {
      return packageID;
    }
    /// Packages are dynamic after they have been scheduled
    /// for delivery.

    /**
     * @brief Returns the dynamic status of the packafe
     * 
     * Packages are dynamic after they have been scheduled for delivery.
     * 
     * @return true The Package is dynamic (i.e. scheduled for delivery)
     * @return false The Package is static
     */
    bool IsDynamic() const { return scheduled; }


    /**
     * @brief Checks to see if a Package is right next to a Customer, in which case there is immediate delivery
     * 
     * @param[in] customer The customer whose proximity is checked
     * @return true The Package is within range of the Customer
     * @return false The Package is outside the range of the Customer
     */
    bool nearCustomer(Customer* customer) {
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
    /**
     * @brief Checks to see if the package has an observer
     * 
     * @return true 
     * @return false 
     */
    bool hasObserver() {
      return hasObserver_;
    }
    /**
     * @brief Returns the weight of the package
     * 
     * @return float The weight of the package
     */
    float GetWeight() const { return weight; }

    /** @brief return the observers vector
    *
    * @return the vector of observers
    */
    std::vector<entity_project::EntityObserver*> getObservers() {
      return observers;
    }

    /** @brief Adds an observer to observers vector
    *
    * @param[in] observer the observer to be added
    */
    void Attach(entity_project::EntityObserver* observer) {
      observers.push_back(observer);
      hasObserver_ = true;
    }

    /** @brief Removes an observer from the observers vector
    *
    * @param[in] observer the observer to be removed
    */
    void Detach(entity_project::EntityObserver* observer) {
      observers.erase(std::remove(observers.begin(),
      observers.end(), observer), observers.end());
    }
};
}  // namespace csci3081
#endif
