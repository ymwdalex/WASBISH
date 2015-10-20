#include <iostream>
#include "yaml/yaml.h"
#include <ctype.h>
#include <fstream>
#include <assert.h>
#include "House.h"



Node::Node(int id, std::string& name, std::string markFileName)
  : id(id), name(name), markFileName(markFileName), previous(NULL),	next(NULL), distanceFromStart(INT_MAX)
{

}

Edge::Edge(Node* node1, Node* node2, int distance): node1(node1), node2(node2), distance(distance)
{
  
}

bool Edge::Connects(Node* node1, Node* node2)
{
	return (
		(node1 == this->node1 &&
		node2 == this->node2) ||
		(node1 == this->node2 &&
		node2 == this->node1));
}


// Does the 'nodes' vector contain 'node'
bool House::Contains(vector<Node*>& nodes, Node* node)
{
	const int size = nodes.size();
	for(int i=0; i<size; ++i)
	{
		if (node == nodes.at(i))
		{
			return true;
		}
	}
	return false;
}


House::House(){
  nodes.clear();
  edges.clear();
}

House::~House(){
  // Delete all nodes
  std::vector<Node*>::iterator it;
  for (it=nodes.begin(); it!=nodes.end(); it++){
    delete (*it);
  }

  // Delete all edges
  std::vector<Edge*>::iterator edge;
  for (edge=edges.begin(); edge!=edges.end(); edge++){
    delete (*edge);
  }

  nodes.clear();
  edges.clear();
}


// Find the node with the smallest distance,
// remove it, and return it.
Node* House::ExtractSmallest(vector<Node*>& nodes)
{
	int size = nodes.size();
	if (size == 0) return NULL;
	int smallestPosition = 0;
	Node* smallest = nodes.at(0);
	for (int i=1; i<size; ++i)
	{
		Node* current = nodes.at(i);
		if (current->distanceFromStart <
			smallest->distanceFromStart)
		{
			smallest = current;
			smallestPosition = i;
		}
	}
	nodes.erase(nodes.begin() + smallestPosition);
	return smallest;
}
// Return all nodes adjacent to 'node' which are still
// in the 'nodes' collection.
vector<Node*>* House::AdjacentRemainingNodes(Node* node)
{
	vector<Node*>* adjacentNodes = new vector<Node*>();
	const int size = edges.size();
	for(int i=0; i<size; ++i)
	{
		Edge* edge = edges.at(i);
		Node* adjacent = NULL;
		if (edge->node1 == node)
		{
			adjacent = edge->node2;
		}
		else if (edge->node2 == node)
		{
			adjacent = edge->node1;
		}
		if (adjacent && Contains(nodes, adjacent))
		{
			adjacentNodes->push_back(adjacent);
		}
	}
	return adjacentNodes;
}
// Return distance between two connected nodes
int House::Distance(Node* node1, Node* node2)
{
	const int size = edges.size();
	for(int i=0; i<size; ++i)
	{
		Edge* edge = edges.at(i);
		if (edge->Connects(node1, node2))
		{
			return edge->distance;
		}
	}
	return -1; // should never happen
}


void House::PrintShortestRouteTo()
{
	Node* previous = destination;
	cout << "Distance from start: "
		<< destination->distanceFromStart << endl;
	while (previous)
	{
		cout << previous->id << " ";
		previous = previous->previous;
	}
	cout << endl;
}

void House::getShortestPath()
{
  Node* curr = destination;
	Node* pre = destination->previous;

  while (pre)
	{
    pre->next = curr;
    curr = pre;
    pre = pre->previous;
	}
}

void House::search()
{
	while (nodes.size() > 0)
	{
		Node* smallest = ExtractSmallest(nodes);
		vector<Node*>* adjacentNodes =
			AdjacentRemainingNodes(smallest);
		const int size = adjacentNodes->size();
		for (int i=0; i<size; ++i)
		{
			Node* adjacent = adjacentNodes->at(i);
			int distance = Distance(smallest, adjacent) +
				smallest->distanceFromStart;
			if (distance < adjacent->distanceFromStart)
			{
				adjacent->distanceFromStart = distance;
				adjacent->previous = smallest;
			}
		}
		delete adjacentNodes;
	}

  getShortestPath();
}

void House::addNode(int id, std::string name, std::string markerName){
  if (getNodeById(id))
    return;

  Node* a = new Node(id, name, markerName);
  nodes.push_back(a);
}

void House::addPath(int id1, int id2, int weight){
  Node* n1 = getNodeById(id1);
  Node* n2 = getNodeById(id2);
    
  if (!n1 || !n2)
    return;

	Edge* e = new Edge(n1, n2, 1);
  edges.push_back(e);
}

bool House::setStartPoint(int id){
  Node* node = getNodeById(id);

  if (node){
    node->distanceFromStart = 0;
    _startPoint = node;
    return true;
  } else {
    _startPoint = NULL;
    return false;
  }
}

Node* House::getStartPoint(){
  return _startPoint;
}

bool House::setEndPoint(int id){
  Node* node = getNodeById(id);

  if (node){
    destination = node;
    return true;
  } else {
    return false;
  }
}

Node* House::getNodeById(int id){
  int size = nodes.size();
  for (int i=0; i<size; i++){
    if (nodes[i]->id == id)
      return nodes[i];
  }
  
  return NULL;
}

// this method has to be called after getShortestPath
Node* House::getNextNode(Node* curr)
{
  return curr->next;
}


//
//void House::test(){
//	Node* a = new Node('a');
//  nodes.push_back(a);
//	Node* b = new Node('b');
//  nodes.push_back(b);
//	Node* c = new Node('c');
//  nodes.push_back(c);
//	Node* d = new Node('d');
//  nodes.push_back(d);
//	Node* e = new Node('e');
//  nodes.push_back(e);
//	Node* f = new Node('f');
//  nodes.push_back(f);
//	Node* g = new Node('g');
//  nodes.push_back(g);
//	Edge* e1 = new Edge(a, c, 1);
//  edges.push_back(e1);
//	Edge* e2 = new Edge(a, d, 1);
//  edges.push_back(e2);
//	Edge* e3 = new Edge(b, c, 1);
//  edges.push_back(e3);
//	Edge* e4 = new Edge(c, d, 1);
//  edges.push_back(e4);
//	Edge* e5 = new Edge(b, f, 1);
//  edges.push_back(e5);
//	Edge* e6 = new Edge(c, e, 1);
//  edges.push_back(e6);
//	Edge* e7 = new Edge(e, f, 1);
//  edges.push_back(e7);
//	Edge* e8 = new Edge(d, g, 1);
//  edges.push_back(e8);
//	Edge* e9 = new Edge(g, f, 1);
//  edges.push_back(e9);
//	f->distanceFromStart = 0; // set start node
//	search();
//	PrintShortestRouteTo();
//	// Node / Edge memory cleanup snipped
// 	  
//}