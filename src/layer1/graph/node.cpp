#include "pch/pch.h"

#include "node.h"

namespace Graph {

	Node *Node::findChild(std::string Name)
	{
		for (auto &i : this->Children) {
			if (i->getName() == Name)
				return i;
		}

		return nullptr;
	}

	void Node::addChild(Node *child)
	{
		if (!child)
			return;

		Children.push_back(child);
	}

	void Node::removeChild(Node *child)
	{
		if (!child)
			return;

		Children.erase(std::remove(Children.begin(), Children.end(), child), Children.end());
	}

	void Node::deleteChild(Node *child)
	{
		if (!child)
			return;

		removeChild(child);
		free(child);
	}

	void Node::moveChild(Node *child, Node *parent)
	{
		this->removeChild(child);
		parent->addChild(child);
	}

	void Node::__debug_printChildren()
	{
		for (auto &i : this->Children) {
			std::cout << Name << "/" << i->getName() << std::endl;
		}
	}

	void Node::__debug_printAllChildren()
	{
		for (auto &i : this->Children) {
			std::cout << Name << "/" << std::flush;

			if (i->getChildren().size())
				i->__debug_printChildren();
			else
				std::cout << i->getName() << std::endl;
		}
	}

}
