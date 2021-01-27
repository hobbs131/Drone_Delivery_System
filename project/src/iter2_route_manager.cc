#include "src/iter2_route_manager.h"

namespace csci3081 {
Iter2RoutePoint::Iter2RoutePoint(float x_, float y_, float z_) {
    x = x_;
    y = y_;
    z = z_;
}

float Iter2RoutePoint::operator[](int index) const {
    switch (index) {
        case 0 :
            return x;
        case 1 :
            return y;
        case 2 :
            return z;
        default :
            return 0;
    }
}

float Iter2RoutePoint::DistanceBetween(const Iter2RoutePoint &other) const {
    float distance = 0;
    distance += pow(other.x - x, 2.0);
    distance += pow(other.y - y, 2.0);
    distance += pow(other.z - z, 2.0);
    return sqrt(distance);
}

std::vector<Iter2RoutePoint> Iter2RouteManager::GetRoute(
                const Iter2RoutePoint& src,
                const Iter2RoutePoint& dest) const {
    // Graph gotten via setter function

    // fringe holds all nodes at fringe of search
    std::vector<SearchNode*> fringe;
    // explored just holds all of the SearchNodes created for easy memory clearing
    std::vector<SearchNode*> explored;

    SearchNode start;
    SearchNode end;
    start.GraphNode = nodeList.front();
    start.distTravelled = 0.0;

    // Determine which node to start at
    // Done by finding node that is closest
    float buf = 0.0;
    float lowestDist = 0.0;
    for (int i = 0; i < 3; i++) {
        // Because the only parts that matter when comparing distances is the proportion,
        // we can skip the sqrt() step.
        lowestDist += pow(((nodeList.front())->GetPosition()).at(i) - src[i], 2.0);
    }
    lowestDist = sqrt(lowestDist);
    for (int i = 0; i < nodeList.size(); i++) {
        buf = 0.0;
        for (int j = 0; j < 3; j++) {
            buf += pow(((nodeList.at(i))->GetPosition()).at(j) - src[j], 2.0);
        }
        if (sqrt(buf) < lowestDist) {
            lowestDist = sqrt(buf);
            start.GraphNode = nodeList.at(i);
        }
    }

    // Determine which node to end
    // Done by finding node that is closest
    lowestDist = 0.0;
    for (int i = 0; i < 3; i++) {
        lowestDist += pow(((nodeList.front())->GetPosition()).at(i) - dest[i], 2.0);
    }
    for (int i = 0; i < nodeList.size(); i++) {
        buf = 0.0;
        for (int j = 0; j < 3; j++) {
            buf += pow(((nodeList.at(i))->GetPosition()).at(j) - dest[j], 2.0);
        }
        if (buf < lowestDist) {
            lowestDist = buf;
            end.GraphNode = nodeList.at(i);
        }
    }

    start.distTravelled = 0.0;
    buf = 0.0;
    for (int i = 0; i < 3; i++) {
        buf += pow(((start.GraphNode)->GetPosition()).at(i) -
            ((end.GraphNode)->GetPosition()).at(i), 2.0);
    }
    start.heuristic = sqrt(buf);

    // std::cout << "start node: " << (start.GraphNode)->GetName() << std::endl;
    // std::cout << "end node:   " << (end.GraphNode)->GetName() << std::endl;

    // vector to hold children of node
    std::vector<entity_project::IGraphNode*> children;

    // add first node to fringe
    fringe.push_back(&start);

    SearchNode* currNode;
    SearchNode* newNode;
    bool foundDest = false;

    // The real A* begins here
    while (!fringe.empty()) {
        // std::cout << ".";
        currNode = fringe.front();
        // pick element in fringe with lowest f-cost (distTravelled + heuristic)
        for (int i = 0; i < fringe.size(); i++) {
            if ((fringe.at(i))->distTravelled +
                    (fringe.at(i))->heuristic <
                currNode->distTravelled +
                    currNode->heuristic) {
                currNode = fringe.at(i);
            }
        }
        // std::cout << "node being searched: " << (currNode->GraphNode)->GetName() << std::endl;
        // std::cout << "distance travelled: " << currNode->distTravelled << std::endl;
        // std::cout << "f-cost: " << currNode->distTravelled + currNode->heuristic << std::endl;

        // check if the node selected is the destination
        if (currNode->GraphNode == end.GraphNode) {
            // std::cout << "found end node" << std::endl;
            foundDest = true;
            break;
        }

        // add children to fringe
        children = (currNode->GraphNode)->GetNeighbors();
        for (int i = 0; i < children.size(); i++) {
            newNode = new SearchNode;
            newNode->GraphNode = children.at(i);

            // set distance travelled
            buf = 0.0;
            for (int j = 0; j < 3; j++) {
                buf += pow(((currNode->GraphNode)->GetPosition()).at(j) -
                    ((newNode->GraphNode)->GetPosition()).at(j), 2.0);
            }
            newNode->distTravelled = currNode->distTravelled + sqrt(buf);

            // set heuristic
            buf = 0.0;
            for (int i = 0; i < 3; i++) {
                buf += pow(((newNode->GraphNode)->GetPosition()).at(i) -
                    ((end.GraphNode)->GetPosition()).at(i), 2.0);
            }
            newNode->heuristic = sqrt(buf);

            // setting parent node;
            newNode->prevNode = currNode;

            // check for duplicates in the children
            bool isDuplicate = false;
            for (int j = 0; j < explored.size(); j++) {
                if (newNode->GraphNode == (explored.at(j))->GraphNode) {
                    // check if we can update it instead
                    if (newNode->distTravelled + newNode->heuristic <
                        (explored.at(j))->distTravelled + (explored.at(j))->heuristic) {
                            // if better, assign node new values
                            (explored.at(j))->GraphNode = newNode->GraphNode;
                            (explored.at(j))->prevNode = newNode->prevNode;
                            (explored.at(j))->distTravelled = newNode->distTravelled;
                            (explored.at(j))->heuristic = newNode->heuristic;
                    }
                    isDuplicate = true;
                }
            }
            explored.push_back(newNode);
            if (isDuplicate) {
                // std::cout << "Skipped duplicate\n";
                continue;
            }

            // if not a duplicate, add to fringe
            fringe.push_back(newNode);
        }
        // remove explored node from fringe
        fringe.erase(std::remove(fringe.begin(), fringe.end(), currNode), fringe.end());
    }

    if (fringe.empty()) {
         // std::cout << "failed to find end node" << std::endl;
    }

    // put found route into vector
    std::vector<Iter2RoutePoint> shortestRoute;
    Iter2RoutePoint point(0.0, 0.0, 0.0);
    if (foundDest) {
        while (currNode->GraphNode != start.GraphNode) {
            point.name = (currNode->GraphNode)->GetName();
            point.x = ((currNode->GraphNode)->GetPosition()).at(0);
            point.y = ((currNode->GraphNode)->GetPosition()).at(1);
            point.z = ((currNode->GraphNode)->GetPosition()).at(2);
            shortestRoute.insert(shortestRoute.begin(), point);
            currNode = currNode->prevNode;
        }
        // inserting start node at front
        point.name = (start.GraphNode)->GetName();
        point.x = ((start.GraphNode)->GetPosition()).at(0);
        point.y = ((start.GraphNode)->GetPosition()).at(1);
        point.z = ((start.GraphNode)->GetPosition()).at(2);
        shortestRoute.insert(shortestRoute.begin(), point);
        // Sometimes the closest node still isn't close enough
        // shortestRoute.insert(shortestRoute.begin(), src);
        shortestRoute.push_back(dest);
    }

    // std::cout << explored.size() << std::endl;
    int counter = 0;
    // clear memory
    for ( ; counter < explored.size(); ++counter) {
        delete explored.at(counter);
    }
    explored.clear();
    // std::cout << counter << std::endl;
    // std::cout << "cleared memory" << std::endl;
    // std::cout << shortestRoute.size() << std::endl;
    return shortestRoute;  // if failed, will return an empty vector!
}

std::string Iter2RouteManager::GetFile(
                const Iter2RoutePoint& src,
                const Iter2RoutePoint& dest) const {
    // Get file (Why are the parameters here? What file?)
}

std::vector<Iter2RoutePoint> Iter2RouteManager::ReadRoute(std::string file_name) const {
    // std::vector<Iter2RoutePoint> buffer;
    // get IGraph from CSV, and then convert to RoutePoints to chuck into buffer
}

}  // namespace csci3081
