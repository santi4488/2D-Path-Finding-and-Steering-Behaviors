

#pragma once
#include"GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
/**
 * 
 */


class SCROLLER2D_API Graph
{
private:
	class vertex;
	class edge;

	class vertex
	{
	public:
		vertex* parent;
		int32 gCost;
		int32 heuristic;
		FVector coordinate;
		int32 fCost;
		uint32 RectangleID;
		bool open;
		bool bIsValidPathPoint;

		TArray<edge*> adjList;
		vertex(FVector c)
		{
			parent = NULL;
			gCost = 0;
			fCost = 0;
			heuristic = 0;
			coordinate = c;
			open = false;
			bIsValidPathPoint = false;
		}

	};

	class edge
	{
	public:
		vertex* start;
		vertex* end;
		float gCost;
		float heuristic;
		FString moveType;

		edge(vertex* v, vertex* u, float w, FString m)
		{
			start = v;
			end = u;
			gCost = w;
			heuristic = 0;
			//coordinate = c;
			moveType = m;
		}
	};

	//FString graphType;

	TArray<vertex*> vertexList;

public:
	Graph()
	{

	}
	
	//Graph(FString s);

	~Graph()
	{
		vertexList.Empty();
	}

	//function to add vertex to the vertex list
	void addVertex(FVector pos)
	{
		if (!findVertex(pos))
		{
			vertexList.Add(new vertex(pos));
		}
		
	}
	
	

	void addEdge(FVector s, FVector e, FString move, float width)
	{
		addEdge(s, e, 1, move, width);
	}

	
	bool IsConnected(FVector Loc)
	{
		vertex* vert = findVertex(Loc);

		if (vert)
		{
			if (vert->adjList.Num() > 0)
			{
				return true;
			}
			return false;
		}
		return false;
	}

	void addEdge(FVector s, FVector e, float w, FString m, float width)
	{
		vertex* vertA = findVertex(s);
		vertex* vertB = findVertex(e);

		if (vertA != NULL && vertB != NULL && vertA != vertB)
		{
			edge* theEdge = new edge(vertA, vertB, w, m);

			vertA->adjList.Add(theEdge);
		}
	}

	int32 NumberOfVertices()
	{
		return vertexList.Num();
	}

	vertex* findVertex(FVector p)
	{

		for (int32 i = 0; i < vertexList.Num(); i++)
		{
			if (vertexList[i]->coordinate == p)
			{
				return vertexList[i];
			}
		}
		//UE_LOG(MyLog, Warning, TEXT("Vertex %s not found!!!"), *p.ToString());


		return NULL;
	}

	void DrawNodes(UWorld * w, FColor c)
	{
		UE_LOG(MyLog, Log, TEXT("Drawing %d nodes!!!"), vertexList.Num());

		if (vertexList.Num() > 0)
		{
			UWorld *World = w;
			if (World)
			{
				for (int i = 0; i < vertexList.Num(); i++)
				{
					DrawDebugPoint(World, vertexList[i]->coordinate + FVector(0, 1, 0), 5, c, false, 120.0f, 0.03);
				}

			}
		}
	}

	void DrawEdges(UWorld * w, FColor c)
	{
		if (vertexList.Num() > 0)
		{
			UE_LOG(MyLog, Log, TEXT("*********************DRAW EDGES CALLED*************************"));

			UWorld* World = w;
			if (World)
			{
				//UE_LOG(MyLog, Log, TEXT("*********************DRAW EDGES CALLED*************************"));

				for (vertex* & v : vertexList)
				{
					//UE_LOG(MyLog, Log, TEXT("*********************DRAW EDGES CALLED*************************"));

					if (v->adjList.Num() > 0)
					{
						//UE_LOG(MyLog, Log, TEXT("*********************DRAW EDGES CALLED*************************"));

						for (edge* & e : v->adjList)
						{
							//UE_LOG(MyLog, Log, TEXT("*********************DRAW EDGES CALLED*************************"));

							FVector start = e->start->coordinate;
							FVector end = e->end->coordinate;
							//UE_LOG(MyLog, Log, TEXT("*******************DRAWING EDGE START***************"));
							//UE_LOG(MyLog, Warning, TEXT("Edge start:  %s"), *start.ToString());
							//UE_LOG(MyLog, Warning, TEXT("Edge end:  %s"), *end.ToString());
							DrawDebugLine(World, start + FVector(0, 1, 0), end + FVector(0, 1, 0), c, false, 0.1f, 0.03, 0.5f);
							//UE_LOG(MyLog, Log, TEXT("*******************DRAWING EDGE END*****************"));
						}
					}
				}
			}
		}
	}

	edge* findEdge(FVector s, FVector e)
	{
		FVector start = s;
		FVector end = e;

		edge* thisEdge = NULL;

		return thisEdge;

	}

	void RemoveEdge(FVector s, FVector e)
	{

	}

	void resetNodes()
	{
		for (int32 i = 0; i < vertexList.Num(); i++)
		{
			vertexList[i]->parent = NULL;
			vertexList[i]->heuristic = 0;
			vertexList[i]->fCost = 0;
			vertexList[i]->gCost = 0;
			vertexList[i]->open = false;
		}
	}
	
	void FindPath(FVector& start, FVector& end, float radius)
	{
		TArray<vertex*> OpenList;
		TArray<vertex*> ClosedList;
		vertex* neighbor = NULL;

		resetNodes();

		if (findClosestNode(start, radius) && findClosestNode(end, radius))
		{


			vertex* currentNode = findVertex(start);
			vertex* endNode = findVertex(end);



			//UE_LOG(MyLog, Log, TEXT("**************************************************A* START*******************************"));
			//UE_LOG(MyLog, Warning, TEXT("END NODE POS:  %s"), *end.ToString());

			//implement min heap in the future for better performance
			if (currentNode && endNode)
			{
				//UE_LOG(MyLog, Log, TEXT("PROCESSING NODE AT POS:  %s"), *currentNode->coordinate.ToString());
				currentNode->gCost = 0;
				currentNode->heuristic = 0;
				currentNode->fCost = 0;

				FVector endCoordinate = end;

				float hCost = 0;
				int32 gCost = 0;
				const int32 gDiagCost = 45;
				const int32 gVertCost = 32;


				//add the starting point to the open list
				OpenList.Add(currentNode);
				currentNode->open = true;
				//add reachable adjacent vertices to the open list
				//and set the starting vertex as their parent
				while (!IsInList(ClosedList, endNode) && OpenList.Num() > 0)
				{
					//UE_LOG(MyLog, Log, TEXT("PROCESSING NODE AT POS:  %s"), *currentNode->coordinate.ToString());

					gCost = 0;
					hCost = 0;
					currentNode = findLowestFCost(OpenList);
					OpenList.Remove(currentNode);
					ClosedList.Add(currentNode);

					/**JUMP TO POINT ALGORITHM START*/
					if (currentNode == endNode)
						break;

					//identifySuccessors(OpenList, currentNode, endNode, clearance, toClear);
					/**JTP ALGORITHM END**/

					if (currentNode->adjList.Num() > 0)
					{
						for (int32 i = 0; i < currentNode->adjList.Num(); i++)
						{
							//add each adjacent vertex to the open list if not on the closed list
							//and calculate the f cost
							neighbor = currentNode->adjList[i]->end;

							if (IsInList(ClosedList, neighbor))
							{
								continue;
							}

							//calculate gCost based on type of move
							if (currentNode->adjList[i]->moveType == "diagonal")
							{
								gCost = currentNode->gCost + gDiagCost;
							}
							else
							{
								gCost = currentNode->gCost + gVertCost;
							}

							if (!neighbor->open || gCost < neighbor->gCost)
							{
								neighbor->gCost = gCost;
								hCost = CalculateHeuristic(neighbor, endNode);
								neighbor->heuristic = hCost;
								neighbor->parent = currentNode;
								
								neighbor->fCost = neighbor->gCost + neighbor->heuristic;

								if (!neighbor->open)
								{
									OpenList.Add(neighbor);
									neighbor->open = true;

								}
								else
								{
									//update the nodes position in the heap
									//this will be done later if a min heap is implemented
								}
							
							}

						}
					}
				}
			}
			//end a* algorithm

			else
			{
				if (!currentNode)
					UE_LOG(MyLog, Warning, TEXT("CURRENT NODE NOT FOUND!!!"));
				if (!endNode)
					UE_LOG(MyLog, Warning, TEXT("END NODE NOT FOUND!!!"));


			}
		}

		else
		{
			if (!findClosestNode(start, radius))
				UE_LOG(MyLog, Warning, TEXT("A*: NO START NODE!!!"));
			if (!findClosestNode(end, radius))
				UE_LOG(MyLog, Log, TEXT("A*: NO END NODE!!!"));


		}
		
		OpenList.Empty();
		ClosedList.Empty();
		//UE_LOG(MyLog, Log, TEXT("***********************************************A* END***************************************"));

	}



	//checks if vertex v is in the list
	bool IsInList(TArray<vertex*> list, vertex* v)
	{
		bool isInClosedList = false;
		
		if (list.Num() > 0)
		{
			for (int32 j = 0; j < list.Num(); j++)
			{
				if (list[j] == v)
				{
					isInClosedList = true;
					break;
				}
				else
					isInClosedList = false;
			}
		}

		return isInClosedList;
	}

	float CalculateHeuristic(vertex* start, vertex* end)
	{
		float f = abs(end->coordinate.X - start->coordinate.X) + abs(end->coordinate.Z - start->coordinate.Z);
		float dx = abs(start->coordinate.X - end->coordinate.X);
		float dy = abs(start->coordinate.Y - end->coordinate.Y);

		f *= (1 + 1 / 1000);
		return f;
	}

	void ShowPath(UWorld* World, FVector end)
	{
		TArray<vertex*> path;
		vertex* currentNode = findVertex(end);

		while (currentNode)
		{
			if (currentNode->parent)
				DrawDebugDirectionalArrow(World, currentNode->parent->coordinate + FVector(0, 1, 0), currentNode->coordinate + FVector(0, 1, 0), 30.f, FColor::Red, false, 120.0f);
			currentNode = currentNode->parent;
		}

		
	}

	void GetPath(FVector end, TArray<FVector> & path, bool trim)
	{
		vertex* currentNode = findVertex(end);
		vertex* prevNode = NULL;

		if (currentNode && trim)
		{
			path.Add(currentNode->coordinate + FVector(0, 1, 0));
			prevNode = currentNode;
			currentNode = currentNode->parent;
		}
			

		while (currentNode)
		{
			//UE_LOG(PathFollowing, Log, TEXT("PATH ARRAY SIZE:  %d"), path.Num());
			if (trim)
			{
				if (currentNode->bIsValidPathPoint)
				{
					path.Add(currentNode->coordinate + FVector(0, 1, 0));
					prevNode = currentNode;
				}
			}
			else
			{
				path.Add(currentNode->coordinate + FVector(0, 1, 0));

			}
			currentNode = currentNode->parent;
		}
		if (trim && prevNode)
			path.Add(prevNode->coordinate + FVector(0, 1, 0));

	}

	vertex* findLowestFCost(TArray<vertex*> list)
	{
		vertex* lowest = NULL;
		if (list.Num() < 1)
			return NULL;
		if (list.Num() > 1)
		{
			lowest = list[0];
			for (int32 i = 1; i < list.Num(); i++)
			{
				if (lowest->fCost > list[i]->fCost)
				{
					lowest = list[i];
				}
			}
		}
		else
			return list[0];

		return lowest;
	}

	bool findClosestNode(FVector & pos, float radius)
	{
		FVector node;
		float x = 0;
		float z = 0;

		//UE_LOG(MyLog, Log, TEXT("NUMBER OF NODES IN VERTEX LIST: %d"), vertexList.Num());


		for (int i = 0; i < vertexList.Num(); i++)
		{
			x = 0;
			z = 0;
			if (pos == vertexList[i]->coordinate)
			{
				return true;
				break;
			}
			x = FMath::Pow(pos.X - vertexList[i]->coordinate.X, 2);
			z = FMath::Pow(pos.Z - vertexList[i]->coordinate.Z, 2);

			if ((x + z) < FMath::Pow(radius, 2))
			{

				pos = vertexList[i]->coordinate;
				return true;
			}

		}

		return false;
	}

	TArray<vertex*> identifySuccessors(TArray<vertex*> &OpenList, vertex * current, vertex* start, vertex * end)
	{
		TArray<vertex*> successors;
		TArray<vertex*> neighbors; 
		findNeighbors(current, neighbors);
		
		for (int i = 0; i < current->adjList.Num(); i++)
		{
			neighbors.Add(current->adjList[i]->end);
		}

		//vertex* jumpPoint = jump()

		return successors;
	}

	void findNeighbors(vertex* current, TArray<vertex*> & neighbors)
	{
		if (!current->parent)
		{
			for (int32 i = 0; i < current->adjList.Num(); i++)
			{
				neighbors.Add(current->adjList[i]->end);
			}
			return;
		}

		
	}

};