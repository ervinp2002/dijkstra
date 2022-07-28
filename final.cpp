#include <iostream>
#include <climits>
#include <fstream>
#include <string>
#include <algorithm>
#include <stack>
#include "queue.h"
#include "graph.h"
using namespace std;

struct Location {
    string vertex;
    bool marked;
    int distance;
    string previous;

    // Useful for sorting and searching functions implemented. 
    bool operator == (Location otherPlace) const {
        return vertex == otherPlace.vertex;
    }

    bool operator > (Location otherPlace) const {
        return vertex > otherPlace.vertex;
    }

    bool operator < (Location otherPlace) const {
        return vertex < otherPlace.vertex;
    }

    bool operator <= (Location otherPlace) const {
        return vertex <= otherPlace.vertex;
    }

    bool operator >= (Location otherPlace) const {
        return vertex >= otherPlace.vertex;
    }
};

// I am not writing out these templates everytime lmao
typedef Graph<string> Map;
typedef Queue<string> Path;
typedef vector<Location> NodeList;

// Pre-Dijkstra
void checkFiles(fstream &input, int argc, char *argv[]);
bool inList(Location vertex, const NodeList &list);
void createGraph(Map &map, NodeList &list, fstream &input);
void startMenu(NodeList &list);
void startPrompt(NodeList &list, string &startVertex, int &startIndex);

// DRIVER FUNCTION FOR DIJKSTRA'S ALGORITHM
void dijkstra(Map &map, Path &q, NodeList &list, NodeList &marked);

// Intermediate Steps in Dijkstra's Algorithm
void setStartingPoint(Location &point, Map &map); 
int getNextVertex(Map &map, NodeList &list, NodeList &marked, Path &q, string currentVertex);
int updateAdjacent(Map &map, NodeList &list, NodeList &marked, Path &q, string currentVertex);
void printVertex(Location &vertex, NodeList &list);

// Cycle Check
bool cycleCheck(const Map &graph, Path &q, string start, vector<string> &cycle);

// USEFUL FUNCTIONS
int binarySearch(string vertex, NodeList &list);
void quickSort(NodeList &list, int first, int last); 
void clearScreen();

// To be used with quickSort
void pincer(NodeList &list, int first, int last, int &split);
void swap(Location &value1, Location &value2); 

int main(int argc, char *argv[]) {
    // PRE: File name for the graph is specified on the command line.
    // POST: Outputs a table showing the shortest path to each node, given a vertex.
    Map myGraph;
    Path myPath(50);
    NodeList nodes;
    NodeList markedSequence;
    fstream infile;

    // Step 1: Check that the file exists. 
    checkFiles(infile, argc, argv);

    // Step 2: Parse input to create vertices, edges, and add to Graph.
    createGraph(myGraph, nodes, infile);

    // Steps 3 & 4: Dijkstra's Algorithm + Cycle Check
    dijkstra(myGraph, myPath, nodes, markedSequence);

    return 0;
}

void checkFiles(fstream &input, int argc, char *argv[]) {
    // PRE: File name has been specfied on the command line. 
    // POST: Validates that the file does exist in the same directory. 
    if (argc == 1) {
        cerr << "Input file not specified..." << endl;
        exit(1);
    } else {
        input.open(argv[1]);
        if (!input) {
            cerr << "File does not exist..." << endl;
            exit(1);
        }
    }
}

int binarySearch(string vertex, NodeList &list) {
    // PRE: List has already been sorted and a vertex to search for has been specified. 
    // POST: Returns the index of where the vertex is in the vector or notFound index if not found. 
    int left = 0;
    int right = list.size() - 1;
    int middle;
    bool found = false;

    while (!found && left <= right) {
        middle = (left + right) / 2;
        if (vertex == list[middle].vertex) found = true;
        else if (vertex > list[middle].vertex) left++;
        else right--;
    }

    return found ? middle : list.size() - 1;
}

void clearScreen() {
    // POST: Clears the screen.
    cout << "\033[2J\033[1;1H";
}

bool inList(Location vertex, const NodeList &list) {
    // PRE: Created Location struct while parsing. 
    // POST: Checks if the vertex specified in the passed struct already exists in the list.
    int limit = list.size();
    for (int i = 0; i < limit; i++) {
        if (vertex == list[i]) return true;
    }
    return false;
}

void createGraph(Map &map, NodeList &list, fstream &input) {
    // PRE: Input file, Graph, and Vector of nodes have been instantiated. 
    // POST: Creates a graph by parsing the input file and adding the input as vertices.
    string temp;
    string origin;
    string destination;
    string distance;
    int firstSemi;
    int secondSemi;
    int counter;
    bool originFound;
    bool destinationFound;

    while (!input.eof()) {
        // For parsing the entire line
        getline(input, temp);
        counter = 0;

        // Parses origin.
        for (; temp[counter] != ';'; counter++) origin += temp[counter];
        counter++;

        // Parses destination.
        for (; temp[counter] != ';'; counter++) destination += temp[counter];
        counter++;

        // Parses distance.
        for (; counter < temp.length(); counter++) distance += temp[counter];

        // Create structs of the vertices.
        Location point1 = {origin, false, INT_MAX, "N/A"};
        Location point2 = {destination, false, INT_MAX, "N/A"};

        // Check if vertices exist in vector and adds them if it doesn't exists.
        originFound = inList(point1, list);
        if (!originFound || list.size() == 0) {
            map.AddVertex(origin);
            list.push_back(point1);
            originFound = true;  
        }

        destinationFound = inList(point2, list);
        if (!destinationFound || list.size() == 0) {
            map.AddVertex(destination);
            list.push_back(point2);
            destinationFound = true;
        }

        // Adds the edge if both vertices exist. 
        if (originFound && destinationFound) {
            map.AddEdge(origin, destination, atoi(distance.c_str()));
        }

        // Flush all of the strings. 
        origin = "";
        destination = "";
        distance = "";
        temp = "";
    }

    list.pop_back(); // Parsing creates an extra node for some reason
    input.close();

    // Add notFound struct
    Location notFound = {"ZZZZ", false, INT_MAX, "N/A"};
    list.push_back(notFound);
}

void setStartingPoint(Location &point, Map &map, NodeList &marked) {
    // PRE: Vector containing vertices has been created and a valid location is specified.
    // POST: Adjusts the attributes of the specified vertex to mark it as the starting point. 
    point.marked = true;
    point.distance = 0;
    point.previous = "N/A";
    map.MarkVertex(point.vertex);
    marked.push_back(point);
}

void quickSort(NodeList &list, int first, int last) {
    // PRE: Instantiated and filled an unsorted vector of vertices.
    // POST: Recursively sorts the vector by vertex name.
    if (first < last) {
        int splitPoint;
        pincer(list, first, last, splitPoint);
        quickSort(list, first, splitPoint - 1);
        quickSort(list, splitPoint + 1,last);
    }
}

void pincer(NodeList &list, int first, int last, int &split) {
    // PRE: To be used with a quick sort. 
    // POST: Moves all values less than pivot to the left side and larger values to the right side.
    int pivot = first;
    int saveFirst = first;		
    first++;

    while (first <= last) {
        while ((list[first] <= list[pivot]) && (first <= last)) first++;
        while ((list[last] >= list[pivot]) && (first <= last)) last--;  
        if (first < last) {
            swap(list[first], list[last]);
            first++;	
            last--;
        }
    }

    split = last;
    swap(list[saveFirst], list[split]); 
}

void swap(Location &value1, Location &value2) {
    // PRE: Two vertices have been instantiated.
    // POST: Swaps the contents of the two vertices.
    Location temp = value1;
    value1 = value2;
    value2 = temp;
}

void printRow(Location &vertex, NodeList &list) {
    // PRE: Vertex passed has been instantiated. 
    // POST: Outputs the attributes of the vertex in a row. 
    if (vertex.vertex != "ZZZZ") {
        cout << "\t" << vertex.vertex << "\t\t\t";
        if (vertex.distance != INT_MAX) cout << vertex.distance << "\t\t\t";
        else cout << "Not in Path" << "\t\t";
        cout  << vertex.previous << endl;
    } 
}

void startMenu(NodeList &list) {
    // PRE: Graph has already been created, along with vector of vertices. 
    // POST: Outputs the starting table for Dijkstra's Algorithm. 

    // Sort the vector of vertices. 
    quickSort(list, 0, list.size() - 1);

    // Starting menu prompt
    clearScreen();
    cout << "\t";
    for (int i = 0; i < 20; i++) cout << "^";
    cout << "\tDIJKSTRA'S ALGORITHM\t";
    for (int i = 0; i < 20; i++) cout << "^";
    cout << endl;
    cout << "\tA weighted graph has been built for these " << list.size() - 1 <<  " vertices:\n";

    // Print out the vertices. 
    for (int i = 0; i < list.size() - 1; i++) {
        if (i % 3 == 0) cout << endl << "\t\t" << list[i].vertex;
        else cout << "\t\t" << list[i].vertex;
    }
    cout << endl;
}

void startPrompt(NodeList &list, string &startVertex, int &startIndex) {
    // PRE: Vector of vertices has been initialized. 
    // POST: Prompts the user to enter a valid starting vertex.

    // Prompt to input a starting vertex.
    cout << endl << "\tInput a starting vertex: ";
    getline(cin, startVertex);
    startIndex = binarySearch(startVertex, list);

    while (startIndex == list.size() - 1) {
        cout << endl << "\tInput a starting vertex: ";
        getline(cin, startVertex);
        startIndex = binarySearch(startVertex, list);
    }
}

int getNextVertex(Map &map, NodeList &list, NodeList &marked, Path &q, string currentVertex) {
    // PRE: Graph has been instantiated and a starting vertex was made.
    // POST: Finds the index of closest vertex at a given vertex.

    // Find adjacent vertices
    map.GetToVertices(currentVertex, q);

    int currentWeight;
    string top;
    int curr = binarySearch(currentVertex, list);
    int tempTop;
    int min = list.size() - 1;

    while (!q.isEmpty()) {
        top = q.getFront();
        tempTop = binarySearch(top, list);
        currentWeight = map.WeightIs(currentVertex, top);

        // Check if smaller than what's currently stored as distance for it. 
        if (!list[tempTop].marked && !map.IsMarked(top) && 
                list[tempTop].distance > currentWeight + list[curr].distance) {
            list[tempTop].distance = currentWeight + list[curr].distance;
            list[tempTop].previous = currentVertex;
        }

        // Set as minimum if it's smaller than the current minimum weight;
        if (list[tempTop].distance < list[min].distance && !list[tempTop].marked) {
            min = tempTop;
        }

        q.dequeue();
    }

    return binarySearch(list[min].vertex, list);
}

int updateAdjacent(Map &map, NodeList &list, NodeList &marked, Path &q, string currentVertex) {
    // PRE: Graph has been initalized.  
    // POST: Finds the vector index of the overall closest unmarked vertex from all marked vertices. 

    int temp;
    int min = binarySearch("ZZZZ", list);
    string top;
    q.makeEmpty();

    for (int i = 0; i < marked.size(); i++) {
        temp = getNextVertex(map, list, marked, q, marked[i].vertex);
        if (list[temp].distance < list[min].distance && !list[temp].marked) min = temp;
    }
    
    // Mark the minimum value. 
    return binarySearch(list[min].vertex, list);
}

bool cycleCheck(const Map &graph, Path &q, string start, vector<string> &cycle) {
    // PRE: All vertices from the graph have been recorded.
    // POST: Returns true if it can identify a cycle. 

    string searchPoint = start;
    vector<string> visited;
    stack<string> s;

    // Push starting vertex onto stack
    s.push(searchPoint);

    while (!s.empty()) {
        // Pop and mark top vertex as visited
        visited.push_back(s.top());
        s.pop();

        // Push adjacent vertices onto stack from queue. 
        graph.GetToVertices(searchPoint, q); 
        while (!q.isEmpty()) {
            if (binary_search(visited.begin(), visited.end(), q.getFront())) {
                cycle.assign(visited.begin(), visited.end());
                return true;
            } else {
                s.push(q.getFront());
                q.dequeue();
            }
        }

        if (!s.empty()) searchPoint = s.top();
        // New top vertex that will be popped and get adjacent vertices of  
    }
    
    return false;
}

void dijkstra(Map &map, Path &q, NodeList &list, NodeList &marked) {
    // PRE: Graph and vector of vertices has been filled. 
    // POST: Outputs a table showing the shortest path to each vertex.

    // Local variables used within function.
    string start;
    int index;
    int next;
    vector<string> cycle;
    
    startMenu(list);
    startPrompt(list, start, index);
    
    // Mark the inputted vertex as the starting point. 
    setStartingPoint(list[index], map, marked);
    start = list[index].vertex;

    // Start making the table
    cout << "\t";
    for (int i = 0; i < 68; i++) cout << "-";
    cout << endl;
    cout << "\tVertex\t\t\tDistance\t\tPrevious" << endl;
    printRow(list[index], list);

    do {
        next = updateAdjacent(map, list, marked, q, list[index].vertex);
        list[next].marked = true;
        map.MarkVertex(list[next].vertex);
        marked.push_back(list[next]);
        printRow(list[next], list);
    } while (list[next].vertex != "ZZZZ");

    for (int i = 0; i < list.size(); i++) {
        if (!list[i].marked) {
            printRow(list[i], list);
        }
    }

    cout << "\t";
    for (int i = 0; i < 68; i++) cout << "-";
    cout << endl;

    if (cycleCheck(map, q, start, cycle)) {
        cout << "\t\t\tThe graph contains a cycle." << endl;
        cout << "\tThe cycle is:" << endl;  
        cout << "\t\t\t" << cycle[cycle.size() - 1] << "    \t->\t" << cycle[cycle.size() - 2] << endl;  
        cout << "\t\t\t" << cycle[cycle.size() - 2] << "    \t->\t" << cycle[cycle.size() - 1] << endl;
        cout << endl;   
    } else {
        cout << "\t\t\tThe path does not contain a cycle." << endl << endl;
    }
}

