#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <string>

using namespace std;

// Function to update values of S (Susceptible) and I (Infectious) using Euler's method
void sis_model(double& S, double& I, double beta, double gamma, double dt, 
               double avoid_contact_factor, double health_checkup_factor, 
               double vaccination_factor, double safe_practices_factor) {
    
    // Adjust beta and gamma using the behavioral factors
    double effective_beta = beta * (1.0 - avoid_contact_factor) * (1.0 - safe_practices_factor); // Reduce beta for Avoid Contact and Safe Practices
    double adjusted_gamma = gamma * (1.0 + health_checkup_factor);  // Increase gamma for Health Check-ups
    S = S * (1.0 - vaccination_factor);  // Directly reduce the susceptible population based on vaccination
    
    // Compute changes in S and I
    double dS = -effective_beta * S * I + adjusted_gamma * I;
    double dI = effective_beta * S * I - adjusted_gamma * I;

    // Update S and I based on the time step
    S += dS * dt;
    I += dI * dt;

    // Ensure S and I remain within [0, 1]
    if (S < 0) S = 0;
    if (I < 0) I = 0;
    if (S > 1) S = 1;
    if (I > 1) I = 1;
}

// Function to load parameters from a file
unordered_map<string, double> load_params_from_file(const string& filename) {
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

// Function to run the simulation and store results
void run_simulation(double beta, double gamma, double dt, int total_steps, 
                    double avoid_contact_factor, double health_checkup_factor, 
                    double vaccination_factor, double safe_practices_factor, 
                    vector<pair<double, double>>& results) {
    
    double S = 0.99;  // Initial Susceptible population
    double I = 0.01;  // Initial Infectious population

    for (int t = 0; t <= total_steps; ++t) {
        results.push_back(make_pair(S, I));  // Store results at each time step
        sis_model(S, I, beta, gamma, dt, avoid_contact_factor, health_checkup_factor, vaccination_factor, safe_practices_factor);  // Update S and I using the SIS model

        // Debug: Print values for verification
        cout << "Step " << t << ": S = " << S << ", I = " << I << endl;
    }
}

// Function to save results to a file for further analysis
void save_results_to_file(const vector<pair<double, double>>& results, const string& filename) {
    ofstream file(filename.c_str());  // Use c_str() for compatibility
    if (file.is_open()) {
        file << "Time,Susceptible,Infectious\n";
        for (size_t i = 0; i < results.size(); ++i) {
            file << i * 0.1 << "," << results[i].first << "," << results[i].second << "\n";
        }
        file.close();
        cout << "Results saved to " << filename << endl;
    } else {
        cout << "Unable to open file for writing.\n";
    }
}

int main() {
    // Load parameters from the file
    unordered_map<string, double> params = load_params_from_file("SIS_params.txt");

    // Set default model parameters
    double beta = 0.3;  // Default infection rate
    double gamma = 0.1; // Default recovery rate
    double dt = 0.1;    // Default time step
    int total_steps = 100;  // Default number of iterations

    // Retrieve behavioral factors from the file
    double avoid_contact_factor = params["avoid_contact_factor"];
    double health_checkup_factor = params["health_checkup_factor"];
    double vaccination_factor = params["vaccination_factor"];
    double safe_practices_factor = params["safe_practices_factor"];

    // Debug: Print parameters to verify
    cout << "Parameters loaded:\n";
    cout << "beta: " << beta << ", gamma: " << gamma << ", dt: " << dt << ", total_steps: " << total_steps << endl;
    cout << "avoid_contact_factor: " << avoid_contact_factor 
         << ", health_checkup_factor: " << health_checkup_factor 
         << ", vaccination_factor: " << vaccination_factor 
         << ", safe_practices_factor: " << safe_practices_factor << endl;

    // Vector to store results
    vector<pair<double, double>> results;

    // Run the simulation
    run_simulation(beta, gamma, dt, total_steps, avoid_contact_factor, health_checkup_factor, vaccination_factor, safe_practices_factor, results);

    // Save results to a file
    save_results_to_file(results, "SIS_simulation_results.csv");

    return 0;
}
