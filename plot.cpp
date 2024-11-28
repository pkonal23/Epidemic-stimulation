#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

int main() {
    // Load the CSV file
    std::string filename = "seir_simulation_results.csv";  // Replace with your filename
    std::ifstream file(filename);
    std::string line;

    // Vectors to store data from the CSV
    std::vector<std::string> headers;
    std::vector<std::vector<double>> data;

    // Read the first line for headers
    if (getline(file, line)) {
        std::stringstream ss(line);
        std::string header;
        // Adjust the delimiter based on your file format (use ',' for CSV with commas, '\t' for tab-separated)
        while (getline(ss, header, ',')) {  // Assuming comma as the delimiter
            headers.push_back(header);
        }
    }

    // Debug output to verify headers
    std::cout << "Headers:\n";
    for (const auto& header : headers) {
        std::cout << header << "\n";
    }

    // Read the remaining lines for data
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        std::vector<double> row_data;
        // Split the row by commas (or use '\t' for tab-separated)
        while (getline(ss, value, ',')) {  // Use ',' for CSV, or '\t' for tab-separated
            row_data.push_back(std::stod(value));
        }
        data.push_back(row_data);
    }
    file.close();

    // Debug output to verify data is being read correctly
    std::cout << "\nData:\n";
    for (const auto& row : data) {
        for (const auto& val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

    // Assuming the first column is time and the rest are dynamic columns (e.g., Susceptible, Infected, Recovered, etc.)
    std::vector<double> x_data;  // Time steps
    std::vector<std::vector<double>> y_data(headers.size() - 1);  // Other columns (Susceptible, Infected, etc.)

    // Populate x_data and y_data
    for (size_t i = 0; i < data.size(); ++i) {
        x_data.push_back(data[i][0]);  // Time column
        for (size_t j = 1; j < headers.size(); ++j) {
            y_data[j - 1].push_back(data[i][j]);  // Other columns
        }
    }

    // Debugging output: print a few values from x_data and y_data to verify the sizes and content
    std::cout << "\nFirst few values of x_data (Time):\n";
    for (size_t i = 0; i < std::min(x_data.size(), size_t(5)); ++i) {
        std::cout << "x_data[" << i << "] = " << x_data[i] << "\n";
    }

    std::cout << "\nFirst few values of y_data:\n";
    for (size_t i = 0; i < std::min(y_data.size(), size_t(4)); ++i) {
        std::cout << headers[i + 1] << " data: ";
        for (size_t j = 0; j < std::min(y_data[i].size(), size_t(5)); ++j) {
            std::cout << y_data[i][j] << " ";
        }
        std::cout << "\n";
    }

    // Plot each y-variable (Susceptible, Infected, Recovered, etc.) against time (x-axis)
    plt::figure_size(800, 600);  // Set figure size for better visualization
    for (size_t i = 0; i < y_data.size(); ++i) {
        // Ensure the sizes match before plotting
        if (x_data.size() != y_data[i].size()) {
            std::cerr << "Error: x and y vectors must have the same size!" << std::endl;
            std::cout << "x_data size: " << x_data.size() << ", y_data[" << i << "] size: " << y_data[i].size() << std::endl;
            return -1;
        }

        // Plot each y-variable and set the label for the legend
        plt::plot(x_data, y_data[i], {{"label", headers[i + 1]}});  // Dynamic labeling based on headers
    }

    // Adding title and labels
    plt::title("Simulation Results: " + filename);
    plt::xlabel(headers[0]);  // Dynamic x-axis label (assuming the first column is Time)
    plt::ylabel("Values");

    // Adding a legend (always shown)
    plt::legend();  // Ensure the labels are properly recognized

    // Display the plot
    plt::grid(true);  // Optional: Add grid for better readability
    plt::show();

    return 0;
}