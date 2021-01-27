#ifndef CUSTOMER_H_
#define CUSTOMER_H_

#include <EntityProject/ANVIL2/customer.h>
#include "src/json_helper.h"

namespace csci3081 {
/**
 * @brief The Customer class is a concrete class for objects that receive Packages from Drones (and also give Packages to Drones).
 * 
 * Customer is related to its factory, EntityFactory, and also to DroneSimulation and TransportPackage.
 * The position of a Customer (which is an inherited attribute from the Entity class) is important to know
 * so that Drones can be made to fly to them. Other than that, it is just necessary to know if a Customer
 * wants to receieve a Package from a Drone or if it wants to give a Package to a Drone. The Customer class
 * knows of the Package class, and because Customers must be allowed to have Packages, this class has a
 * Package* private variable.
 */
class Customer : public entity_project::Customer {
 private:
  bool receivesPackage = false;

 public:
    /// Used to give each Customer a unique id_
    static int customerID;

    /**
     * @brief The default constructor for Customer.
     * 
     * It adds type Customer to TypedObject's type system.
     */
    Customer() { AddType<Customer>(); }

    /**
     * @brief A constructor for Customer that allows details to be set upon initialization
     * 
     * @param[in] val A picojson containing entity details
     */
    explicit Customer(const picojson::object& val) {
      AddType<Customer>();
      details_ = val;
      id_ = customerID;
      customerID++;
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
    }
    /**
     * @brief Set the Position object of the customer
     * 
     * @param array 
     */
    void setPosition(float array[3]) {
      for (int i = 0; i < 3; i++) {
        position_.at(i) = array[i];
      }
    }
    /**
     * @brief Get the Id object
     * 
     * @return int 
     */
    int GetId() {
      return customerID;
    }

    /**
     * @brief Returns if the Customer can receive a package from a Drone
     * 
     * @return true The Customer can receive packages from Drones
     * @return false The Customer can not receive packages from Drones
     */
    bool ShouldReceive() { return receivesPackage; }

    /**
     * @brief Sets true and false whether a Customer has "ordered" a Package
     * 
     * Part of the scheduling process
     * 
     * @param[in] x The new status of the Customer
     */
    void SetShouldReceive(bool x) { receivesPackage = x; }

    /**
     * @brief Tells the program if the Customer is dynamic
     * 
     * Customers do not move, so they are not dynamic.
     * 
     * @return false Customers are never dynamic
     */
    bool IsDynamic() { return false; }
};

}  // namespace csci3081

#endif
