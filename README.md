
This C++ project demonstrates a basic ad hoc networking model and simulates packet flow between nodes. It implements:
- **Packet integrity checks** using a custom hash function  
- **Pathfinding** with a breadth-first search (BFS)-style traversal  
- **Random message corruption** to mimic real-world transmission errors  
- **Multiple node classes** (`Node`, `Network`, `Packet`, `MacPacket`) to structure the flow of messages

Nodes pass `Packet` objects along a calculated path until the destination is reached or an error occurs, showcasing how data might be relayed in a simplified network. It uses a **probabilistic corruption rate** to randomly alter message payloads and demonstrates how to handle successes or failures at each node.

