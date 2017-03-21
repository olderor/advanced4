// Copyright 2017 olderor (Yevchenko Bohdan)

#include <iostream>
#include <vector>

#pragma warning(disable: 4996)

// Used for solving the problem.
struct wall_builder {
 public:
    // Initialize with field and block prices.
    wall_builder(
        std::vector< std::vector<char> > &data,
        const int priceDouble,
        const int priceSimple);

    // Get answer to the problem - minimum price to fix wall.
    int get_min_price();

 private:
    // Used for storing data about each cell in the field.
    struct point {
        // Determine wether cell is free (has to be fixed).
        bool free;
        // Index of the cell in the Bigraph.
        int index;
    };

    // Height and width of the field.
    int height, width;
    // Block prices.
    int priceA, priceB;
    // Number of elements that have to be fixed.
    int free_count = 0;

    // Storing bigraph. Each vertex stores their own number of connected vertex.
    std::vector < std::vector<int> > bigraph;
    // Matching in the graph theory.
    std::vector<int> independent_edge_set;
    // Used for checking if vertex can be used in the new matching.
    std::vector<bool> used;
    // Define if vertex is used in the random matching filling.
    std::vector<bool> used_temp;
    // Stores current state of field.
    std::vector< std::vector<point> > field;

    // Number of vertex in the first part of bigraph.
    int firstSize = 0;
    // Number of vertex in the second part of bigraph.
    int secondSize = 0;

    // Kuhn algorithm to improve matching for the particular vertex.
    bool kuhn(const int vertex);
    // Builds bigraph from the current field state.
    void build_graph();
    // Set matching randomly.
    void fill_graph();
    // Try to improve matching from the current state.
    void improve_graph();
    // Get matching size.
    int get_connections_count();
};

// Reading data from the input.
void read_data(
    int &height,
    int &width,
    int &priceDouble,
    int &priceSimple,
    std::vector< std::vector<char> > &data);

// Printing data to the output.
void print_data(const int data);

// Entry point.
int main();



wall_builder::wall_builder(
    std::vector< std::vector<char> > &data,
    const int priceDouble,
    const int priceSimple) {
    priceA = priceDouble;
    priceB = priceSimple;

    height = data.size();
    if (height == 0) {
        width = 0;
        return;
    }
    width = data[0].size();

    field = std::vector< std::vector<point> >(height,
        std::vector<point>(width));

    for (int heighti = 0; heighti < height; ++heighti) {
        for (int widthj = 0; widthj < width; ++widthj) {
            field[heighti][widthj].free = data[heighti][widthj] == '*';

            if (!field[heighti][widthj].free) {
                continue;
            }

            ++free_count;

            if ((heighti + widthj) % 2 == 0) {
                field[heighti][widthj].index = firstSize++;
            } else {
                field[heighti][widthj].index = secondSize++;
            }
        }
    }
}

int wall_builder::get_min_price() {
    if (2 * priceB <= priceA) {
        return priceB * free_count;
    }

    build_graph();
    fill_graph();
    improve_graph();
    const int connections = get_connections_count();

    return connections * priceA + (free_count - connections * 2) * priceB;
}

bool wall_builder::kuhn(const int vertex) {
    if (used[vertex]) {
        return false;
    }

    used[vertex] = true;

    for (int i = 0; i < bigraph[vertex].size(); ++i) {
        const int to = bigraph[vertex][i];
        if (independent_edge_set[to] == -1) {
            independent_edge_set[to] = vertex;
            return true;
        }
        if (kuhn(independent_edge_set[to])) {
            independent_edge_set[to] = vertex;
            return true;
        }
    }

    return false;
}

void wall_builder::build_graph() {
    for (int heighti = 0; heighti < height; ++heighti) {
        for (int widthj = 0; widthj < width; ++widthj) {
            if (!field[heighti][widthj].free || (heighti + widthj) % 2 != 0) {
                continue;
            }

            std::vector<int> neighbors;
            if (heighti + 1 != height && field[heighti + 1][widthj].free) {
                neighbors.push_back(field[heighti + 1][widthj].index);
            }
            if (heighti != 0 && field[heighti - 1][widthj].free) {
                neighbors.push_back(field[heighti - 1][widthj].index);
            }
            if (widthj + 1 != width && field[heighti][widthj + 1].free) {
                neighbors.push_back(field[heighti][widthj + 1].index);
            }
            if (widthj != 0 && field[heighti][widthj - 1].free) {
                neighbors.push_back(field[heighti][widthj - 1].index);
            }
            bigraph.push_back(neighbors);
        }
    }
}

void wall_builder::fill_graph() {
    independent_edge_set = std::vector<int>(secondSize, -1);
    used_temp = std::vector<bool>(firstSize);
    for (int i = 0; i < firstSize; ++i) {
        for (int j = 0; j < bigraph[i].size(); ++j) {
            if (independent_edge_set[bigraph[i][j]] == -1) {
                independent_edge_set[bigraph[i][j]] = i;
                used_temp[i] = true;
                break;
            }
        }
    }
}

void wall_builder::improve_graph() {
    for (int i = 0; i < firstSize; ++i) {
        if (used_temp[i]) {
            continue;
        }
        used = std::vector<bool>(firstSize, false);
        kuhn(i);
    }
}

int wall_builder::get_connections_count() {
    int connections_count = 0;
    for (int i = 0; i < secondSize; ++i) {
        if (independent_edge_set[i] != -1) {
            ++connections_count;
        }
    }
    return connections_count;
}

void read_data(
    int &height,
    int &width,
    int &priceDouble,
    int &priceSimple,
    std::vector< std::vector<char> > &data) {
    scanf("%d %d %d %d", &height, &width, &priceDouble, &priceSimple);

    data.resize(height, std::vector<char>(width));

    char new_line;
    for (int i = 0; i < height; ++i) {
        scanf("%c", &new_line);
        for (int j = 0; j < width; ++j) {
            scanf("%c", &data[i][j]);
        }
    }
}

void print_data(const int data) {
    printf("%d", data);
}

int main() {
    int height, width, priceDouble, priceSimple;
    std::vector< std::vector<char> > data;

    read_data(height, width, priceDouble, priceSimple, data);

    wall_builder wb = wall_builder(data, priceDouble, priceSimple);

    print_data(wb.get_min_price());


    return 0;
}


/*

3 10 3 2
.**.**.***
.*..*..*.*
..**.*.***

.<>.<>.<>|
.?..?..|.|
..<>.?.|<>

0123456789
.<>.|?.<>|
.?..|..|.|
..<>.?.|<>
2 * 7 + 3 * 3 = 14 + 9 = 23


*/
