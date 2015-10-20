#ifndef __HOUSE_H__
#define __HOUSE_H__

#include <vector>

using namespace std;



class Node
{
public:
  Node(int id, std::string& name, std::string markFileName);
  int distanceFromStart;

public:
	int id;
  std::string name;
  std::string markFileName;

	Node* previous;
  Node* next;
};

class Edge
{
public:
  Edge(Node* node1, Node* node2, int distance);
	bool Connects(Node* node1, Node* node2);

public:
	Node* node1;
	Node* node2;
	int distance;
};

class House
{
public:
  House();
  void search();
  void clean();
  void addNode(int id, std::string name, std::string markerName);
  void addPath(int id1, int id2, int weight);  
  bool setStartPoint(int id);
  Node* getStartPoint();
  bool setEndPoint(int id);
  Node* getNextNode(Node* curr);
void PrintShortestRouteTo();
  Node* destination;
  ~House();

private:
  

  vector<Node*> nodes;
  vector<Edge*> edges;

  Node* _startPoint;


  vector<Node*> path;

  bool Contains(vector<Node*>& nodes, Node* node);
  Node* ExtractSmallest(vector<Node*>& nodes);
  vector<Node*>* AdjacentRemainingNodes(Node* node);
  int Distance(Node* node1, Node* node2);
  void getShortestPath();
  
  
  Node* getNodeById(int id);
  
  void test();

};

#endif