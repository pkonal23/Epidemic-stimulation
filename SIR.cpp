#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <unordered_map>
#include <string>

using namespace std;

// Function to update values of S, I, R using Euler's method
void sir_model(double &S, double &I, double &R, double beta, double gamma, double vaccination_rate, double dt)
{
    double dS = -beta * S * I - vaccination_rate * S;
    double dI = beta * S * I - gamma * I;
    double dR = gamma * I + vaccination_rate * S;

    S += dS * dt;
    I += dI * dt;
    R += dR * dt;
}

// Function to log data to a file
void log_data(ofstream &file, double time, double S, double I, double R)
{
    file << fixed << setprecision(4) << time << "," << S << "," << I << "," << R << endl;
}

// Function to load parameters from a file
unordered_map<string, double> load_parameters(const string& filename)
{
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

int main()
{
    // Load parameters from the file
    unordered_map<string, double> params = load_parameters("SIR_params.txt");

    // Retrieve the parameters
    double S = 0.99;               // Initial Susceptible
    double I = 0.01;               // Initial Infectious
    double R = 0.0;                // Initial Recovered

    double beta = 0.4;             // Initial transmission rate
    double gamma = 0.1;            // Recovery rate
    double dt = 0.01;              // Time step for smoother transitions
    int total_steps = 20000;       // Simulate for 20 days

    // Retrieve protective measures parameters
    double reduced_beta = 0.25;    // Reduced transmission rate after quarantine
    double quarantine_time = params["quarantine_time"]; // Read from file
    double vaccination_rate = params["vaccination_rate"]; // Read from file
    double vaccination_start = params["vaccination_start"]; // Read from file
    double vaccination_speed = params["vaccination_speed"]; // Read from file

    // Open a file to log the results
    ofstream data_file("SIR_simulation_results.csv");
    data_file << "Time,Susceptible,Infectious,Recovered\n";

    // Simulation loop to iterate over time
    for (int t = 0; t <= total_steps; ++t)
    {
        double current_time = t * dt;

        // Implement quarantine (reduce transmission rate after certain time)
        if (current_time >= quarantine_time && current_time < quarantine_time + 0.1)
        {
            beta = reduced_beta; // Gradually reduce beta
        }

        // Implement vaccination after a certain time
        if (current_time >= vaccination_start)
        {
            vaccination_rate = vaccination_speed;
        }

        // Log data to the file
        log_data(data_file, current_time, S, I, R);

        // Update the values using the SIR model
        sir_model(S, I, R, beta, gamma, vaccination_rate, dt);

        // Print to console for real-time monitoring
        if (t % 100 == 0)
        {
            cout << "Time: " << current_time << " S: " << S << " I: " << I << " R: " << R << endl;
        }
    }

    // Close the file
    data_file.close();
    cout << "Simulation complete. Data saved to sir_simulation_results.csv" << endl;

    return 0;
}
