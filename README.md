# Dijkstra's Algorithm

Designed by: Ervin Pangilinan

Semester: Spring 2022 (v1.0) May 4, 2022

Course: CSC 245 - Data Structures & Algorithm Analysis 

Language Used: C++

## Objective:

The goal of this program is to create a client program using a predefined 
graph and queue classes that will create a graph and output a summary table of the
shortest paths from a vertex using Dijkstra's Algorithm. Additionally, the
program will check if cycles exist in the graph. 

## Command Line Syntax:

    ./RELEASE [input file]

*NOTE: The input file must be in the same directory as the program.

## Sample Input:

The specified input will have each line in the following order:

    origin;destination;trip-length

An example input file is shown below:

    UC;Tarver;40
    Tarver;Willet;50
    [blank line]

## Sample Output:
The output is a summary table from the result of implementing Dijkstra's
Algorithm. Using the same input file, the summary table is shown below:

        ^^^^^^^^^^^^^^^^^^^^    DIJKSTRA'S ALGORITHM    ^^^^^^^^^^^^^^^^^^^^
        A weighted graph has been built for these 3 vertices:

                Tarver          UC              Willet

        Input a starting vertex: Tarver
        --------------------------------------------------------------------
        Vertex                  Distance                Previous
        Tarver                  0                       N/A
        Willet                  50                      Tarver
        UC                      Not in Path             N/A
        --------------------------------------------------------------------
                        The path does not contain a cycle.

## Graph Class:

The predefined graph class produces an adjacency matrix made from the vertices
and edges that the user inputs. It also comes with 2 parallel one-dimensional
arrays to keep track of vertices and if a specific vertex was already marked.
This graph class also utilizes a queue from a predefined queue class (non-STL)
for gathering all adjacent vertices to a specific vertex.

## OOP Design:
Along with the implementation of a Graph and Queue class, the client program
also includes the following:

    - iostream
    - climits
    - fstream
    - string
    - algorithm
    - stack

## Client Algorithm:

The client algorithm can be divided into 4 steps:

    - Step 1: File Validation
    - Step 2: Input Parsing & Graph Creation
    - Step 3: Dijkstra's Algorithm
    - Step 4: Cycle Check

## File Validation:

The program checks if there was a command-line argument. If there wasn't 
a command line argument, the program will exit. If there was, the program
will check if the file exists. If the file does exist, the program opens the
file, but if the file does not exist, the program will exit.

## Input Parsing & Graph Creation:

The program takes a line from the input file and concatenates to a string until
a semicolon is reached. Since each line will always have 2 semicolons, the 
concatenated string up until the first semicolon produces the origin. Then, the 
string produced between the first and second semicolons creates the destination.
From the second semicolon up until the end of the line, the string produced is the
weight of the edge. The origin and destination will be treated as structs to add to
a vector holding all of the vertices of the graph. To account for the possibility of
duplicate structs being parsed, every origin and destination string will be searched
in the vector of structs to determine if it's already present. If it's already present
in the vector, that string will not be added as a struct, but if not, then that string
will be added as a struct. For every struct that has been added, the Graph class's
function for adding a vertex is also called, resulting in that vertex being incorporated
into the adjacency matrix. Edges are also added with the Graph class's function for
adding edges.

## Dijkstra's Algorithm:

Dijkstra's Algorithm seeks to create the shortest path to traverse from a starting
vertex to another vertex on the graph. After the user inputs the starting vertex,
the algorithm starts by finding adjacent vertices to the starting vertex. These 
vertices are then put into a queue to determine the the next vertex to traverse to
by comparing the distances of the edges and finding the vertex with the smallest edge.
The struct that corresponds to the specific vertex will have its attributes changed
to reflect the distance travelled and the vertex the program traversed from. This 
vertex/struct is then marked as visited. Afterwards, the program outputs the attributes of
the struct onto the table. The program then repeats the same steps with the newly traversed-to
vertex instead of the starting vertex. Additionally, the program will now check for adjacent,
unmarked vertices of other marked vertices. The distance attribute stored in each struct/vertex
will continue to be updated as traversal of the graph continues. The algorithm will continue to
search for adjacent vertices until all vertices have been marked or an adjacent vertex is not
found. 

## Cycle Check:

Cycle checks are done as a modified depth-first search. Using the starting vertex, the program
pushes the starting vertex onto a stack. Next, the program pops the starting vertex from the stack
and pushes all adjacent vertices onto the stack. The popped starting vertex is added to a vector
that keeps track of traversed vertices. The new top vertex is popped and added to the vector and 
its adjacent vertices are pushed onto the stack. This will continue to happen until either the stack
becomes empty or if a vertex that is already in the vector is pushed onto the stack. The contents
from the vector become outputted which in turn shows the cycle.

## Optimization:

Throughout the program, a special binary search function is called to search for the index of the
corresponding vertex/struct. This binary search function returns the index of a "NOT FOUND" struct
to denote that there are no vertex with that specific name.
