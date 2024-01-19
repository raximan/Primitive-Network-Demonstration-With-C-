#include <iostream>
using namespace std;
#include "adhoc.h"
#include <vector>



int main()
{
    // Initialize the network with a specified corruption rate (e.g., 0.1 for 10%)
    Network network(0.5);

    // Add nodes and their neighbors according to the provided topology
    std::vector<int> connections1 = {2, 3, 4};
    std::vector<int> connections2 = {1, 4, 5};
    std::vector<int> connections3 = {1, 2, 4};
    std::vector<int> connections4 = {2, 3, 6};
    std::vector<int> connections5 = {2, 6, 7};
    std::vector<int> connections6 = {4, 5, 7};
    std::vector<int> connections7 = {5, 6};
    network.addNode(1, connections1);
    network.addNode(2, connections1);
    network.addNode(3, connections3);
    network.addNode(4, connections4);
    network.addNode(5, connections5);
    network.addNode(6, connections6);
    network.addNode(7, connections7);

    // Create a packet with the source ID (1), destination ID (6), and the message "Hello world!"

    Packet packet(1, 6, "Hello world!");

    // Start the simulation
    network.simulate(packet);

    return 0;
}
Packet::Packet() : sourceId(0), destId(0), message(""), hash(0) {}

Packet::Packet(int sourceId, int destId, string message)
    : sourceId(sourceId), destId(destId), message(message) {
    size_t h = 5381;  // Start with a large prime number

    // Include the sourceId and destId in the hash
    h = ((h << 5) + h) + static_cast<size_t>(sourceId); // h * 33 + sourceId
    h = ((h << 5) + h) + static_cast<size_t>(destId);   // h * 33 + destId

    // Iterate over each character in the message
    for (char c : message) {
        h = ((h << 5) + h) + static_cast<size_t>(static_cast<unsigned char>(c)); // h * 33 + c
    }

    size_t hashResult = h;
    hash=h;
}

int Packet::getSourceId()  { return sourceId; }

int Packet::getDestId()  { return destId; }

string Packet::getMessage()  { return message; }


bool Packet::checkIntegrity() {
    //HASH FUNCTION///////
    size_t h = 5381;  // Start with a large prime number
    // Include the sourceId and destId in the hash
    h = ((h << 5) + h) + static_cast<size_t>(sourceId); // h * 33 + sourceId
    h = ((h << 5) + h) + static_cast<size_t>(destId);   // h * 33 + destId

    // Iterate over each character in the message
    for (char c : message) {
        h = ((h << 5) + h) + static_cast<size_t>(static_cast<unsigned char>(c)); // h * 33 + c
    }

    size_t hashResult = h;
    size_t current_hash=h;
    ////////////
    if(current_hash==hash){return true;}
    else{return false;}
}

void Packet::corrupt() {
    int randIndex = std::rand() % message.length();
    char randChar = static_cast<char>(32 + std::rand() % 95); // ASCII range 32-126
    message[randIndex] = randChar;

    }

// MacPacket member functions' implementations
MacPacket::MacPacket()
    : type(MacPacketType::Empty), macSourceId(0), macDestId(0), path(), packet() {}

MacPacket::MacPacket(MacPacketType type, int macSourceId, int macDestId, std::vector<int>& path, Packet& packet)
    : type(type), macSourceId(macSourceId), macDestId(macDestId), path(path), packet(packet) {}

MacPacket MacPacket::createMessagePacket(int macSourceId, int macDestId, std::vector<int>& path, Packet& packet) {
    return MacPacket(MacPacketType::Message, macSourceId, macDestId, path, packet);
}

MacPacket MacPacket::createSuccessPacket(int macSourceId, int macDestId, std::vector<int>& path) {
    Packet dummyPacket; // Assuming a default or dummy Packet
    return MacPacket(MacPacketType::Success, macSourceId, macDestId, path, dummyPacket);
}

MacPacket MacPacket::createFailurePacket(int macSourceId, int macDestId, std::vector<int>& path) {
    Packet dummyPacket; // Assuming a default or dummy Packet
    return MacPacket(MacPacketType::Failure, macSourceId, macDestId, path, dummyPacket);
}

MacPacketType MacPacket::getType()  {
    return type;
}

int MacPacket::getMacSourceId()  {
    return macSourceId;
}

int MacPacket::getMacDestId()  {
    return macDestId;
}

std::vector<int>& MacPacket::getPath() {
    return path;
}

Packet& MacPacket::getPacket() {
    return packet;
}

// Node class method definitions

Node::Node() : id(0) {}

Node::Node(int id, std::vector<int>& neighbors) : id(id), neighbors(neighbors) {}

int Node::getId()  {
    return id;
}

std::vector<int>& Node::getNeighbors() {
    return neighbors;
}

int findNodeIndex(int node,vector<int>pathh){
        int index = -1; // Initialize with -1 to indicate 'not found' initially
        for (size_t i = 0; i < pathh.size(); ++i) {
            if (pathh[i] == node) {
                index = i;
            }
        }
        return index;
}
MacPacket Node::receive(MacPacket packet) {
    int indexofnode=findNodeIndex(id,packet.getPath());
    // Handle different types of packets
    switch (packet.getType()) {
        case MacPacketType::Message: {
        // Check packet integrity
        if (packet.getPacket().checkIntegrity()) {
                // If this node is the final destination
                if (packet.getPath().back() == id) {
                    // Log receiving message
                    std::cout << "[Node " << id << "] Message successfully received : ' " << packet.getPacket().getMessage() <<"'. Sending acknowledgment to Node "<<packet.getMacSourceId()<<"..." << std::endl;

                    // Send a success packet back
                } else {
                        // Not final destination
                       std::cout << "[Node " << id << "] Packet received. Sending to Node " <<packet.getPath()[indexofnode+1]<<"..."<< std::endl;
                       Packet nextpacket=Packet(packet.getPath()[0],packet.getPath().back(),packet.getPacket().getMessage());
                       buffer=MacPacket::createMessagePacket(id, packet.getPath()[indexofnode+1], packet.getPath(),nextpacket);
                       return buffer;
                    }
                } else {
                    std::cout << "[Node " << id << "]"<<" Packet corrupted: '"<<packet.getPacket().getMessage()<<".' Requesting resend from Node " <<packet.getPath()[indexofnode-1]<<"..."<< std::endl;
                    return MacPacket::createFailurePacket(id, packet.getPath()[indexofnode-1], packet.getPath());
                    }

        }
        case MacPacketType::Success: {
            if (packet.getPath()[0] == id) {
                // Log success
                std::cout << "[Node " << id << "] Acknowledgment received. " << std::endl;
                MacPacket emptypacket = MacPacket(MacPacketType::Empty, -2, -2, packet.getPath(), packet.getPacket());
                return emptypacket;
            } else {
                // Forward success packet back through the path
                std::cout << "[Node " << id << "] Acknowledgment received. "<<"Sending to Node "<<packet.getPath()[indexofnode-1]<<"..." << std::endl;
                return MacPacket::createSuccessPacket(id, packet.getPath()[indexofnode-1], packet.getPath());
            }

        }
        case MacPacketType::Failure: {
            // Handle failure packet
            std::cout << "[Node " << id << "] Resending previous packet to Node "<<packet.getMacSourceId()<<"..."<< std::endl;
           Packet nextpacket=Packet(packet.getPath()[0],packet.getPath().back(),packet.getPacket().getMessage());
           return buffer;
        }
    }

    // Return an empty packet as the default action
    return MacPacket();
}

Network::Network(float corruptionRate) : corruptionRate(corruptionRate) {}

Node* Network::getNode(int id) {
    for (Node& node : nodes) {
        if (node.getId() == id) {
            return &node;
        }
    }
    return nullptr; // If no node with the given id is found
}

void Network::addNode(int id, std::vector<int>& neighbors) {
    // Add new node
    nodes.push_back(Node(id, neighbors));

    // Add new node as a neighbor to existing nodes
    for (int neighborId : neighbors) {
        Node* neighborNode = getNode(neighborId);
        if (neighborNode != nullptr) {
            neighborNode->getNeighbors().push_back(id);
        }
    }
}

void Network::removeNode(int id) {
    // Manually find and erase the node with the specified id
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        if (it->getId() == id) {
            nodes.erase(it);
            break; // Break after erasing to avoid invalidating the iterator
        }
    }

    // Remove the node as a neighbor from other nodes
    for (Node& node : nodes) {
        std::vector<int>& neighbors = node.getNeighbors();
        for (auto it = neighbors.begin(); it != neighbors.end();) {
            if (*it == id) {
                it = neighbors.erase(it);
            } else {
                ++it;
            }
        }
    }
}

std::vector<int> Network::calculatePath(int source, int destination) {
    // Check if source and destination are valid
    if (getNode(source) == nullptr || getNode(destination) == nullptr) {
        return std::vector<int>(); // Return empty path if nodes are not valid
    }

    std::vector<int> path;
    std::vector<int> predecessors(nodes.size(), -1); // Vector to store predecessors of each node
    std::vector<bool> visited(nodes.size(), false);  // Vector to keep track of visited nodes

    std::vector<int> queue; // Vector to act as a queue for BFS

    // Initialize BFS-like traversal
    queue.push_back(source);
    visited[source - 1] = true;  // Assuming node IDs are 1-based

    bool found = false;
    while (!queue.empty() && !found) {
        int current = queue.front();
        queue.erase(queue.begin());

        Node* currentNode = getNode(current);

        // Explore neighbors of the current node
        for (int neighbor : currentNode->getNeighbors()) {
            if (!visited[neighbor - 1]) {
                visited[neighbor - 1] = true;
                predecessors[neighbor - 1] = current;
                queue.push_back(neighbor);

                if (neighbor == destination) {
                    found = true;
                    break;
                }
            }
        }
    }

    // Reconstruct the path if the destination is reachable
    if (predecessors[destination - 1] != -1) {
        int at = destination;
        while (at != -1) {
            path.push_back(at);
            at = predecessors[at - 1];
        }

        // Reverse the path without std::swap
        int size = path.size();
        for (int i = 0; i < size / 2; ++i) {
            int temp = path[i];
            path[i] = path[size - i - 1];
            path[size - i - 1] = temp;
        }
    }

    return path;
}
Packet corruptMessage(MacPacket macpacket,float probability){
    Packet normalpacket=macpacket.getPacket();
    string message=normalpacket.getMessage();
    if (static_cast<double>(rand()) / RAND_MAX < probability) { // Check if we should corrupt
        normalpacket.corrupt();
    }
    return normalpacket;
}
void Network::simulate(Packet packet) {
    // Calculate the path from source to destination
    std::vector<int> path = calculatePath(packet.getSourceId(), packet.getDestId());

    // Assuming the path is valid and contains at least the source and destination nodes
    if (path.empty()) {
        std::cout << "No valid path found." << std::endl;
        return;
    }

    //PRINT PATH
    std::cout << "Path: ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i];
        if (i < path.size() - 1) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
    int currentNodeIndex=0;
    int currentNodeId=path[currentNodeIndex];
    Node* currentNode=getNode(currentNodeId);
    MacPacket currentPacket = MacPacket::createMessagePacket(1, path[0], path, packet);
    // Infinite loop for simulation
    while (true) {
        currentNodeId = currentPacket.getPath()[currentNodeIndex];
        Node* currentNode = getNode(currentNodeId);
        if (!currentNode) break;  // Invalid node, end simulation
        // Send and receive packet
        MacPacket responsePacket = currentNode->receive(currentPacket);
        // Check response packet type
        if (responsePacket.getType() == MacPacketType::Empty) {
            // Empty packet received, end simulation
            break;
        } else if (responsePacket.getType() == MacPacketType::Message) {
            // Corrupt with given probability
            if (static_cast<double>(rand()) / RAND_MAX < corruptionRate) {
            responsePacket.getPacket().corrupt();
            }
            currentPacket=responsePacket;
            currentNodeIndex++;
        } else if (responsePacket.getType() == MacPacketType::Success) {
            // If acknowledgment, move back in the path
            currentNodeIndex--;
            currentPacket = responsePacket;
        }
          else if(responsePacket.getType()==MacPacketType::Failure){
            currentNodeIndex--;
            currentPacket = responsePacket;
          }

    }
}

