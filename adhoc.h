#include <string>
#include <vector>

class Packet
{
private:
    int sourceId;
    int destId;
    std::string message;
    size_t hash;

public:
    Packet();
    Packet(int sourceId, int destId, std::string message);
    int getSourceId();
    int getDestId();
    std::string getMessage();
    bool checkIntegrity();
    void corrupt();
};

enum class MacPacketType
{
    Empty,
    Message,
    Success,
    Failure
};

class MacPacket
{
private:
    MacPacketType type;
    int macSourceId;
    int macDestId;
    std::vector<int> path;
    Packet packet;

public:
    MacPacket();
    MacPacket(MacPacketType type, int macSourceId, int macDestId, std::vector<int>& path, Packet& packet);
    static MacPacket createMessagePacket(int macSourceId, int macDestId, std::vector<int>& path, Packet& packet);
    static MacPacket createSuccessPacket(int macSourceId, int macDestId, std::vector<int>& path);
    static MacPacket createFailurePacket(int macSourceId, int macDestId, std::vector<int>& path);
    MacPacketType getType();
    int getMacSourceId();
    int getMacDestId();
    std::vector<int>& getPath();
    Packet& getPacket();
};

class Node
{
private:
    int id;
    std::vector<int> neighbors;
    MacPacket buffer;

public:
    Node();
    Node(int id, std::vector<int>& neighbors);
    int getId();
    std::vector<int>& getNeighbors();
    MacPacket receive(MacPacket packet);
};

class Network
{
private:
    std::vector<Node> nodes;
    float corruptionRate;

    Node* getNode(int id);
    std::vector<int> calculatePath(int source, int destination);

public:
    Network(float corruptionRate);
    void addNode(int id, std::vector<int>& neighbors);
    void removeNode(int id);
    void simulate(Packet packet);
};
