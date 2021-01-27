#ifndef ITER_TWO_ROUTE_MANAGER
#define ITER_TWO_ROUTE_MANAGER

#include <string>
#include <vector>
#include <iostream>
#include <EntityProject/graph.h>
#include <cmath>
#include <algorithm>

namespace csci3081 {
/** @brief Simple class made to represent map nodes for lookups
 * 
 *  Contains information about search nodes as well.
 *  Location coordinates can be accessed by the [] operator
 */
struct Iter2RoutePoint {
    /// Point coordinate x
    float x;
    /// Point coordinate y
    float y;
    /// Point coordinate z
    float z;

    /// Name of node point from graph
    std::string name = "default";

    /// Constructor that initializes coordinates
    Iter2RoutePoint(float x_, float y_, float z_);
    /// Returns the euclidean distance between two Iter2RoutePoints
    float DistanceBetween(const Iter2RoutePoint &other) const;
    /// Gets the coordinate value via an index instead of x, y, and z
    float operator[](int index) const;
};

/**
 * @brief A class that holds data for the search nodes.
 * 
 * Meant to form a backwards-facing linked list to reconstruct paths
*/
struct SearchNode {
    entity_project::IGraphNode *GraphNode;
    SearchNode *prevNode;
    float distTravelled;
    float heuristic;
};

/**
 *  @brief Iter2RouteManager is a class dedicated to making and returning optimal routes between two points.
 * 
 *  This current version is using A* to make the routes.
 */
class Iter2RouteManager {
 public:
    /**
     * @brief Returns the best route match between two points.
     * 
     * @param[in] src The starting location of the search
     * @param[in] dest The intended destination of the search
     * @return std::vector<Iter2RoutePoint> The vector containing the route
     */
    std::vector<Iter2RoutePoint> GetRoute(
                const Iter2RoutePoint& src,
                const Iter2RoutePoint& dest) const;

    /**
     * @brief Sets the graph pointer for the class
     * 
     * @param[in] graph_ The graph that the route manager will use
     */
    void SetGraph(const entity_project::IGraph* graph_) {
        graph = graph_;
        nodeList = graph->GetNodes();
    }

 private:
    /// Placeholder function. Not needed until Iteration 3
    std::string GetFile(const Iter2RoutePoint& src, const Iter2RoutePoint& dest) const;
    /// Placeholder function. Not needed until Iteration 3
    std::vector<Iter2RoutePoint> ReadRoute(std::string file_name) const;
    /// A pointer to the IGraph node graph usd
    const entity_project::IGraph* graph;
    /// A vector containing all of the IGraphNodes in the IGraph pointer
    std::vector<entity_project::IGraphNode*> nodeList;
};

}  // namespace csci3081

#endif  // ITER_TWO_ROUTE_MANAGER
