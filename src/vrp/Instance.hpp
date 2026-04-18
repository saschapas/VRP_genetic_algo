#pragma once

#include <string>
#include <vector>

#include "util/Logger.hpp"
#include "util/Matrix.hpp"

namespace ko::vrp {
/**
 * A VRP instance containing the number of vehicles as well as the vehicle capacity, the nodes with their demands and
 * coordinates, and also the travel costs between the nodes.
 */
class Instance : public Logger {
public:
    /// Ids for the nodes
    using NodeId = std::uint32_t;
    /// Ids for vehicles
    using VehicleId = std::uint32_t;
    /// Node demand units
    using Demand = std::uint32_t;
    /// Node coordinate values
    using Coordinate = std::uint32_t;
    /// Travel costs values
    using TravelCost = std::int32_t;

    /// Struct representing a node
    struct Node {
        /// The node id
        const NodeId m_nodeId;
        /// Demand
        const Demand m_demand;
        /// Horizontal coordinate
        const Coordinate m_x;
        /// Vertical coordinate
        const Coordinate m_y;
        /// Constructor
        Node(NodeId nodeId, Demand demand, Coordinate x, Coordinate y)
          : m_nodeId(nodeId)
          , m_demand(demand)
          , m_x(x)
          , m_y(y){};
    };

    /**
     * Constructs an instance from a given instance file.
     *
     * @param filename Path to the instance file.
     */
    explicit Instance(const std::string& filename);

    /**
     * Destructor for the instance.
     */
    ~Instance() override = default;

    /**
     * Get the name of the instance.
     *
     * @return The filename of the instance
     */
    [[nodiscard]] std::string getFilename() const { return m_filename; }

    /**
     * Get the number of nodes.
     *
     * @return The number of nodes
     */
    [[nodiscard]] NodeId getNumberNodes() const { return m_numberNodes; }

    /**
     * Get the number of vehicles
     *
     * @return The number of vehicles
     */
    [[nodiscard]] VehicleId getNumberVehicles() const { return m_numberVehicles; }

    /**
     * Get the vehicle capacity.
     *
     * @return The vehicle capacity
     */
    [[nodiscard]] Demand getVehicleCapacity() const { return m_vehicleCapacity; }

    /**
     * Get the depot node id.
     *
     * @return The depot node id
     */
    [[nodiscard]] NodeId getDepotNodeId() const { return m_depotNodeId; }

    /**
     * Get a reference to the vector with all nodes.
     *
     * @return Vector containing all nodes
     */
    [[nodiscard]] const std::vector<Node>& getAllNodes() const { return m_nodes; }

    /**
     * Get a reference to a specific node.
     *
     * @param nodeId The id of the node
     * @return The node
     */
    [[nodiscard]] const Node& getNode(NodeId nodeId) const { return m_nodes[nodeId]; }

    /**
     * Get the travel costs between two given nodes.
     *
     * @param nodeFirstId The first node's id
     * @param nodeSecondId The second node's id
     * @return The travel cost between the given two nodes
     */
    [[nodiscard]] TravelCost getTravelCost(NodeId nodeFirstId, NodeId nodeSecondId) const {
        return m_travelCosts(nodeFirstId, nodeSecondId);
    }

private:
    /// Filename of the instance file
    std::string m_filename;

    /// Number of nodes
    NodeId m_numberNodes;

    /// Number of vehicles
    VehicleId m_numberVehicles;

    /// Vehicle capacity
    Demand m_vehicleCapacity;

    /// The depot node id
    NodeId m_depotNodeId;

    /// Vector with all nodes
    std::vector<Node> m_nodes;

    /// Matrix with travel costs between all nodes
    Matrix<TravelCost> m_travelCosts;
};
} // namespace ko::vrp