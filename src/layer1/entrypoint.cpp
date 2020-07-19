#include <string>
#include <stdio.h>

#include "graph/node.h"

using namespace Graph;

class Test_Object : public Node {
public:
	Test_Object(std::string Name) : Node(Name) {  }
};

int main()
{
	// testing graph functionality
	
	Node root_node("root");

	Test_Object test_obj("object1");
	Test_Object test_obj2("object2");
	Test_Object test_obj3("object3");

	root_node.addChild(&test_obj);
	root_node.addChild(&test_obj2);
	root_node.addChild(&test_obj3);

	root_node.moveChild(root_node.findChild("object2"), root_node.findChild("object1"));

	root_node.__debug_printAllChildren();
}
