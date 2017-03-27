// Copyright 2017 olderor (Yevchenko Bohdan)

#include <iostream>
#include <vector>

// Used for solving the problem.
struct wall_builder {
public:
    // Initialize with field and block prices.
    wall_builder(
        std::vector< std::vector<char> > &data,
        const int priceDouble,
        const int priceSimple);

    // Get answer to the problem - minimum price to fix wall.
    const int get_min_price();

private:
    // Used for storing data about each cell in the field.
    struct point {
        // Determine whether cell is free (has to be fixed).
        bool free;
        // Index of the cell in the bigraph.
        int index;
    };

    // Height and width of the field.
    int height, width;
    // Block prices.
    int price_a, price_b;
    // Number of elements that have to be fixed.
    int free_count = 0;

    // Stores the input data - given state of field. 
    std::vector< std::vector<char> > data;
    // Stores bigraph. Each vertex stores their own number of connected vertex.
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
    int first_size = 0;
    // Number of vertex in the second part of bigraph.
    int second_size = 0;

    // Parse the data (given state of field). Split field into two parts of the bigraph.
    const void process_field();
    // Kuhn algorithm to improve matching for the particular vertex.
    const bool find_maximum_matching_for_vertex(const int vertex);
    // Builds bigraph from the current field state.
    const void build_graph();
    // Set matching randomly.
    const void set_random_matching();
    // Try to improve matching from the current state.
    const void find_maximum_matching();
    // Get matching size.
    const int get_connections_count() const;
};

// Reading data from the input.
const void read_data(
    std::istream &_Istr,
    int &height,
    int &width,
    int &price_double,
    int &price_simple,
    std::vector< std::vector<char> > &data);

// Printing data to the output.
const void print_data(std::ostream &_Ostr, const int data);

// Solve problem with the given data.
// Return answer to the problem.
const int solve(
    const int height,
    const int width,
    const int price_double,
    const int price_simple,
    std::vector< std::vector<char> > &data);

// Entry point.
int main() {

    int height, width, price_double, price_simple;
    std::vector< std::vector<char> > data;

    read_data(std::cin, height, width, price_double, price_simple, data);
    const int answer = solve(height, width, price_double, price_simple, data);
    print_data(std::cout, answer);

    return 0;
}

wall_builder::wall_builder(
    std::vector< std::vector<char> > &data,
    const int price_double,
    const int price_simple) {

    price_a = price_double;
    price_b = price_simple;
    this->data = data;
}

const void wall_builder::process_field() {
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
                field[heighti][widthj].index = first_size++;
            } else {
                field[heighti][widthj].index = second_size++;
            }
        }
    }
}

const int wall_builder::get_min_price() {
    process_field();

    if (2 * price_b <= price_a) {
        return price_b * free_count;
    }

    build_graph();
    set_random_matching();
    find_maximum_matching();
    const int connections = get_connections_count();

    return connections * price_a + (free_count - connections * 2) * price_b;
}

const bool wall_builder::find_maximum_matching_for_vertex(const int vertex) {
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
        if (find_maximum_matching_for_vertex(independent_edge_set[to])) {
            independent_edge_set[to] = vertex;
            return true;
        }
    }

    return false;
}

const void wall_builder::build_graph() {
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

const void wall_builder::set_random_matching() {
    independent_edge_set = std::vector<int>(second_size, -1);
    used_temp = std::vector<bool>(first_size);
    for (int i = 0; i < first_size; ++i) {
        for (int j = 0; j < bigraph[i].size(); ++j) {
            if (independent_edge_set[bigraph[i][j]] == -1) {
                independent_edge_set[bigraph[i][j]] = i;
                used_temp[i] = true;
                break;
            }
        }
    }
}

const void wall_builder::find_maximum_matching() {
    for (int i = 0; i < first_size; ++i) {
        if (used_temp[i]) {
            continue;
        }
        used = std::vector<bool>(first_size, false);
        find_maximum_matching_for_vertex(i);
    }
}

const int wall_builder::get_connections_count() const {
    int connections_count = 0;
    for (int i = 0; i < second_size; ++i) {
        if (independent_edge_set[i] != -1) {
            ++connections_count;
        }
    }
    return connections_count;
}

const void read_data(
    std::istream &_Istr,
    int &height,
    int &width,
    int &price_double,
    int &price_simple,
    std::vector< std::vector<char> > &data) {

    _Istr >> height >> width >> price_double >> price_simple;

    data.resize(height, std::vector<char>(width));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            _Istr >> data[i][j];
        }
    }
}

const void print_data(std::ostream &_Ostr, const int data) {
    _Ostr << data << "\n";
}

const int solve(
    const int height,
    const int width,
    const int price_double,
    const int price_simple,
    std::vector< std::vector<char> > &data) {

    wall_builder wb = wall_builder(data, price_double, price_simple);

    const int answer = wb.get_min_price();

    return answer;
}
