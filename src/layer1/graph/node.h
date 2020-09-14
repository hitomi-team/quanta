#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

namespace Graph {

	class Node {
	public:
		Node(std::string Name) { this->Name = Name; Parent = nullptr; }
		virtual ~Node() {  }

		inline std::string getName() { return Name; }
		inline Node *getParent() { return Parent; }
		inline std::vector<Node *>getChildren() { return Children; }

		Node *findChild(std::string Name); // finds child with Name
		void addChild(Node *child);	   // adds child to Node
		void removeChild(Node *child);	   // removes child from Node
		void deleteChild(Node *child);	   // removes and deletes child from Node. MUST ONLY BE USED WITH HEAP ALLOCATED OBJECTS!
		void moveChild(Node *child, Node *parent); // moves child from Node to parent

		virtual void Destroy() {  } // Destroy function must be overriden and implemented

		void __debug_printChildren();
		void __debug_printAllChildren();

	protected:
		Node *Parent;
		std::vector<Node *> Children;
		std::string Name;
	};

}

#endif
