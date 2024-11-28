#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <unordered_map>  // For storing parameters

using namespace std;

// Define the states for individuals in the population
enum State { Susceptible, Infected, Recovered, Vaccinated };

// Function to generate a random number between 0 and 1
double get_random() {
    static random_device rd;
    static mt19937 generator(rd());
    static uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(generator);
}

// Function to simulate Markov Chain transitions for an individual
State next_state(State current_state, double p_si, double p_ir, bool protective_measures) {
    double r = get_random();

    if (protective_measures) {
        p_si *= 0.5;
    }

    if (current_state == Susceptible && r < p_si) {
        return Infected;
    } else if (current_state == Infected && r < p_ir) {
        return Recovered;
    }
    return current_state;
}

// Function to save the simulation results to a CSV file
void save_to_csv(int step, int susceptible_count, int infected_count, int recovered_count, int vaccinated_count, ofstream& file) {
    file << step << "," << susceptible_count << "," << infected_count << "," << recovered_count << "," << vaccinated_count << endl;
}

// Function to load parameters from a file
unordered_map<string, double> load_parameters(const string& filename) {
    unordered_map<string, double> params;
    ifstream file(filename);

    if (file.is_open()) {
        string key;
        double value;
        while (file >> key >> value) {
            params[key] = value;
        }
        file.close();
    } else {
        cerr << "Error: Could not open the file " << filename << endl;
    }

    return params;
}

// Simulation function for the Markov Chain SIR model with protective measures
void markov_chain_sir(int population_size, double p_si, double p_ir, int total_steps, double vaccination_rate, double protective_measures_rate) {
    vector<State> population(population_size, Susceptible);

    int vaccinated_count = population_size * vaccination_rate;
    for (int i = 0; i < vaccinated_count; ++i) {
        population[i] = Vaccinated;
    }

    population[vaccinated_count] = Infected;

    ofstream file("MARKONIKOV_simulation_results.csv");
    file << "Step,Susceptible,Infected,Recovered,Vaccinated" << endl;

    for (int step = 0; step < total_steps; ++step) {
        int susceptible_count = 0, infected_count = 0, recovered_count = 0, current_vaccinated_count = 0;

        for (int i = 0; i < population_size; ++i) {
            if (population[i] != Vaccinated) {
                bool uses_protection = get_random() < protective_measures_rate;
                population[i] = next_state(population[i], p_si, p_ir, uses_protection);
            }

            if (population[i] == Susceptible) susceptible_count++;
            else if (population[i] == Infected) infected_count++;
            else if (population[i] == Recovered) recovered_count++;
            else if (population[i] == Vaccinated) current_vaccinated_count++;
        }

        save_to_csv(step, susceptible_count, infected_count, recovered_count, current_vaccinated_count, file);

        cout << "Step " << step << ": Susceptible = " << susceptible_count
             << ", Infected = " << infected_count
             << ", Recovered = " << recovered_count
             << ", Vaccinated = " << current_vaccinated_count << endl;
    }

    file.close();
}

int main() {
    int population_size = 100;
    double p_si = 0.05;
    double p_ir = 0.01;
    int total_steps = 100;

    // Load parameters from the file
    unordered_map<string, double> params = load_parameters("MARKONIKOV_params.txt");

    // Retrieve the parameters
    double vaccination_rate = params["vaccination_rate"];
    double protective_measures_rate = params["protective_measures_rate"];

    markov_chain_sir(population_size, p_si, p_ir, total_steps, vaccination_rate, protective_measures_rate);

    return 0;
}
