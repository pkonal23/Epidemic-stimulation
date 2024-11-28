#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>  // For better number formatting
#include <sstream>  // For parsing the file

using namespace std;

// Function to update values of S, E, I, R using Euler's method for SEIR model
void seir_model(double& S, double& E, double& I, double& R, double beta, double sigma, double gamma, double vaccination_rate, double dt) {
    double dS = -beta * S * I - vaccination_rate * S;
    double dE = beta * S * I - sigma * E;
    double dI = sigma * E - gamma * I;
    double dR = gamma * I + vaccination_rate * S;

    S += dS * dt;
    E += dE * dt;
    I += dI * dt;
    R += dR * dt;
}

// Function to log data to a file
void log_data(ofstream& file, double time, double S, double E, double I, double R) {
    file << fixed << setprecision(4) << time << "," << S << "," << E << "," << I << "," << R << endl;
}

// Function to read parameters from a file
void read_parameters(const string& filename, double& quarantine_time, double& reduced_beta, double& vaccination_rate, double& vaccination_start, double& vaccination_speed) {
    ifstream file(filename);
    string line;
    
    while (getline(file, line)) {
        stringstream ss(line);
        string param;
        double value;
        
        getline(ss, param, '=');
        ss >> value;
        
        if (param == "quarantine_time") quarantine_time = value;
        if (param == "reduced_beta") reduced_beta = value;
        if (param == "vaccination_rate") vaccination_rate = value;
        if (param == "vaccination_start") vaccination_start = value;
        if (param == "vaccination_speed") vaccination_speed = value;
    }
}

int main() {
    // Adjusted Initial Conditions for the SEIR model
    double S = 0.94;  // Initial Susceptible
    double E = 0.01;  // Initial Exposed
    double I = 0.05;  // Initial Infectious
    double R = 0.0;   // Initial Recovered

    // Parameters (initial values, will be overwritten by file input)
    double beta = 0.6;    // Transmission rate
    double sigma = 0.1;   // Rate at which exposed individuals become infectious
    double gamma = 0.2;   // Recovery rate
    double dt = 0.01;     // Time step
    int total_steps = 20000;  // Simulate for 200 days (20,000 * 0.01 = 200)

    // Protective Measures
    double quarantine_time = 20;     // Quarantine starts after 20 days
    double reduced_beta = 0.35;      // Adjusted reduced transmission rate during quarantine
    double vaccination_rate = 0.0;   // No vaccination initially
    double vaccination_start = 30;   // Vaccination starts at day 30
    double vaccination_speed = 0.002; // Moderate vaccination rollout speed

    // Read the parameters from the file
    read_parameters("parameters.txt", quarantine_time, reduced_beta, vaccination_rate, vaccination_start, vaccination_speed);

    // Open a file to log the results
    ofstream data_file("SEIR_simulation_results.csv");
    data_file << "Time,Susceptible,Exposed,Infectious,Recovered\n";

    // Simulation loop to iterate over time
    for (int t = 0; t <= total_steps; ++t) {
        double current_time = t * dt;

        // Implement quarantine (reduce transmission rate after certain time)
        if (current_time >= quarantine_time) {
            beta = reduced_beta;
        }

        // Implement vaccination after a certain time
        if (current_time >= vaccination_start) {
            vaccination_rate = vaccination_speed;
        }

        // Log data to the file
        log_data(data_file, current_time, S, E, I, R);

        // Update the values using the SEIR model
        seir_model(S, E, I, R, beta, sigma, gamma, vaccination_rate, dt);

        // Print to console for real-time monitoring
        if (t % 100 == 0) {
            cout << "Time: " << current_time << " S: " << S << " E: " << E << " I: " << I << " R: " << R << endl;
        }
    }

    // Close the file
    data_file.close();
    cout << "Simulation complete. Data saved to seir_simulation_results.csv" << endl;

    return 0;
}
