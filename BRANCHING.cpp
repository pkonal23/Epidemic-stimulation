#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <unordered_map>
#include <string>

using namespace std;

// Function to generate the number of new infections for each individual using Poisson distribution
int generate_new_infections(double reproduction_rate, mt19937& gen) {
    poisson_distribution<> poisson(reproduction_rate);
    return poisson(gen);
}

// Function to apply protective measures and adjust reproduction rate
double apply_protective_measures(double reproduction_rate, double vaccination_prob, double quarantine_prob, double safe_practices_prob, mt19937& gen) {
    uniform_real_distribution<> dist(0.0, 1.0);
    
    if (dist(gen) < vaccination_prob) {
        reproduction_rate *= 0.1;
    }
    if (dist(gen) < quarantine_prob) {
        reproduction_rate *= 0.5;
    }
    if (dist(gen) < safe_practices_prob) {
        reproduction_rate *= 0.7;
    }

    return reproduction_rate;
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

// Function to simulate a branching process and write results to a CSV file
void branching_process(double reproduction_rate, int initial_infected, int max_generations, const string& output_file, 
                       double vaccination_prob, double quarantine_prob, double safe_practices_prob) {
    vector<int> generations(max_generations, 0);
    vector<int> cumulative_infections(max_generations, 0);

    generations[0] = initial_infected;
    cumulative_infections[0] = initial_infected;

    random_device rd;
    mt19937 gen(rd());

    ofstream file(output_file);
    file << "Generation,New Infections,Cumulative Infections\n";
    file << "0," << initial_infected << "," << initial_infected << "\n";

    cout << "Generation 0: " << initial_infected << " infected individuals" << endl;

    for (int gen_idx = 1; gen_idx < max_generations; ++gen_idx) {
        int new_infections = 0;

        for (int i = 0; i < generations[gen_idx - 1]; ++i) {
            double adjusted_reproduction_rate = apply_protective_measures(reproduction_rate, vaccination_prob, quarantine_prob, safe_practices_prob, gen);
            new_infections += generate_new_infections(adjusted_reproduction_rate, gen);
        }

        generations[gen_idx] = new_infections;
        cumulative_infections[gen_idx] = cumulative_infections[gen_idx - 1] + new_infections;

        file << gen_idx << "," << new_infections << "," << cumulative_infections[gen_idx] << "\n";
        cout << "Generation " << gen_idx << ": " << new_infections << " infected individuals" << endl;

        if (new_infections == 0) {
            cout << "The epidemic has died out after " << gen_idx << " generations." << endl;
            break;
        }
    }

    file.close();
}

int main() {
    double reproduction_rate = 2.0;
    int initial_infected = 5;
    int max_generations = 20;
    string output_file = "Branching_simulation_results.csv";

    // Load parameters from the file
    unordered_map<string, double> params = load_parameters("Branching_params.txt");

    // Retrieve the parameters
    double vaccination_prob = params["vaccination_prob"];
    double quarantine_prob = params["quarantine_prob"];
    double safe_practices_prob = params["safe_practices_prob"];

    // Run the branching process simulation with loaded protective measures
    branching_process(reproduction_rate, initial_infected, max_generations, output_file, 
                      vaccination_prob, quarantine_prob, safe_practices_prob);

    cout << "Simulation results have been saved to " << output_file << endl;

    return 0;
}
