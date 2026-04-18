#include <fstream>

#include "Instance.hpp"

namespace ko::vrp {

Instance::Instance(const std::string& filename)
  : Logger("Instance")
  , m_filename(filename)
  , m_numberNodes()
  , m_numberVehicles()
  , m_vehicleCapacity()
  , m_depotNodeId(0)
  , m_nodes()
  , m_travelCosts() {
    // Open the file for reading
    std::ifstream ifs(filename);

    // Check if the input stream was created properly
    if (!ifs) {
        ERROR("Error when opening the file {}.", filename);
        std::exit(EXIT_FAILURE);
    }

    // Reads the number n of nodes
    if (!(ifs >> m_numberNodes)) {
        ERROR("Error while reading the number of nodes.");
        std::exit(EXIT_FAILURE);
    }

    // Reads the number m of vehicles
    if (!(ifs >> m_numberVehicles)) {
        ERROR("Error while reading the number of vehicles.");
        std::exit(EXIT_FAILURE);
    }

    // Reads the vehicle capacity C
    if (!(ifs >> m_vehicleCapacity)) {
        ERROR("Error while reading the vehicle capacity.");
        std::exit(EXIT_FAILURE);
    }

    // Reads the demand and two-dimensional coordinates for each node
    m_nodes.reserve(m_numberNodes);
    for (auto nodeId = NodeId(0); nodeId < m_numberNodes; nodeId++) {
        // Variables to store the values read in
        auto demand = Demand(0);
        auto coordinateX = Coordinate(0);
        auto coordinateY = Coordinate(0);
        if (!(ifs >> demand >> coordinateX >> coordinateY)) {
            ERROR("Error while reading the demands and coordinates.");
            std::exit(EXIT_FAILURE);
        }
        // Store the node coordinates
        m_nodes.emplace_back(nodeId, demand, coordinateX, coordinateY);
    }

    // Calculate the travel cost matrix
    m_travelCosts = Matrix<TravelCost>(m_numberNodes, m_numberNodes);
    for (auto nodeFirstId = NodeId(0); nodeFirstId < m_numberNodes; nodeFirstId++) {
        for (auto nodeSecondId = NodeId(nodeFirstId); nodeSecondId < m_numberNodes; nodeSecondId++) {
            // Retrieve the relevant nodes
            const auto& nodeFirst = m_nodes[nodeFirstId];
            const auto& nodeSecond = m_nodes[nodeSecondId];
            // Calculate the Euclidean distance
            const auto deltaX = double(nodeFirst.m_x) - double(nodeSecond.m_x);
            const auto deltaY = double(nodeFirst.m_y) - double(nodeSecond.m_y);
            const auto distance = TravelCost(std::sqrt(deltaX * deltaX + deltaY * deltaY) + double(0.5));
            // Set the travel costs in the matrix
            m_travelCosts(nodeFirstId, nodeSecondId) = distance;
            m_travelCosts(nodeSecondId, nodeFirstId) = distance;
        }
    }
}

} // namespace ko::vrp