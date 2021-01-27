#ifndef DELIVERY_H_
#define DELIVERY_H_

#include <vector>
#include <string>
#include "src/json_helper.h"
#include "src/drone.h"
#include "src/package.h"
#include "src/customer.h"
#include "src/transport_package.h"
#include "src/drone_simulation.h"

namespace csci3081 {

/**
 * @brief A class dedicated to holding delivery details.
 * 
 * Holds all required elements of the delivery, and
 * will eventually return the details inside of the given picojson.
 * 
 */
struct Delivery {
    /// The package being delivered
    Package* package_;
    /// The customer being delivered to
    Customer* customer_;
    /// The details of the delivery contained in a picojson
    picojson::object details_;
    /**
     * @brief Delivery constructor
     * 
     * @param[in] package The package being delivered
     * @param[in] dest The customer being delivered to
     * @param[in] details The details of the delivery contained in a picojson
     */
    Delivery(entity_project::Package* package,
        entity_project::Customer* dest, const picojson::object& details) {
        package_ = package->AsType<Package>();
        customer_ = dest->AsType<Customer>();
        details_ = details;
    }
};

}  // namespace csci3081

#endif  // DELIVERY_H_
