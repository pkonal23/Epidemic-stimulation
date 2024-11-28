#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>  // For file handling
#include <cmath>    // For distance calculation
#include <sstream>  // For parsing the file

using namespace std;

// Define possible states for an agent
enum State { Susceptible, Infected, Recovered, Vaccinated, Quarantined };

// Agent class representing individuals
class Agent {
public:
    State state;            // State of the agent
    int x, y;               // Position on the grid
    int days_infected;      // Days the agent has been infected or quarantined
    int quarantine_duration; // Duration in steps for quarantine

    // Constructor
    Agent(int x, int y, State state) : x(x), y(y), state(state), days_infected(0), quarantine_duration(5) {}

    // Random movement (up, down, left, right)
    void move(int grid_size) {
        if (state != Quarantined) {  // Quarantined agents don't move
            int direction = rand() % 4;
            if (direction == 0 && x > 0) x--;
            else if (direction == 1 && x < grid_size - 1) x++;
            else if (direction == 2 && y > 0) y--;
            else if (direction == 3 && y < grid_size - 1) y++;
        }
    }

    // Update infected days
    void update_days() {
        if (state == Infected || state == Quarantined) {
            days_infected++;
        }
    }
};

// Check if two agents are in contact (within a certain distance)
bool is_in_contact(const Agent& a1, const Agent& a2, int infection_radius) {
    int dx = a1.x - a2.x;
    int dy = a1.y - a2.y;
    return (dx * dx + dy * dy <= infection_radius * infection_radius);  // Euclidean distance check
}

// Save results to CSV
void save_to_csv(int step, int susceptible_count, int infected_count, int recovered_count, int vaccinated_count, int quarantined_count, ofstream& file) {
    file << step << "," << susceptible_count << "," << infected_count << "," << recovered_count << "," << vaccinated_count << "," << quarantined_count << endl;
}

// Function to read parameters from a file
void read_parameters(const string& filename, double& infection_prob, double& recovery_prob, double& vaccination_prob, double& quarantine_prob) {
    ifstream file(filename);
    string line;
    
    while (getline(file, line)) {
        stringstream ss(line);
        string param;
        double value;
        
        getline(ss, param, '=');
        ss >> value;
        
        if (param == "infection_prob") infection_prob = value;
        else if (param == "recovery_prob") recovery_prob = value;
        else if (param == "vaccination_prob") vaccination_prob = value;
        else if (param == "quarantine_prob") quarantine_prob = value;
    }
}

// Simulation function
void abm_simulation(int num_agents, int grid_size, double infection_prob, double recovery_prob, double vaccination_prob, double quarantine_prob, int infection_radius, int total_steps) {
    vector<Agent> agents;  // Store all agents
    srand(static_cast<unsigned>(time(0)));  // Random seed

    // Open file to write results
    ofstream file("ABM_simulation_results.csv");
    file << "Step,Susceptible,Infected,Recovered,Vaccinated,Quarantined" << endl;

    // Initialize agents
    for (int i = 0; i < num_agents; ++i) {
        int x = rand() % grid_size;
        int y = rand() % grid_size;
        State state = (i == 0) ? Infected : (static_cast<double>(rand()) / RAND_MAX < vaccination_prob ? Vaccinated : Susceptible);
        agents.push_back(Agent(x, y, state));
    }

    // Simulation loop
    for (int step = 0; step < total_steps; ++step) {
        // Move agents
        for (size_t i = 0; i < agents.size(); ++i) {
            agents[i].move(grid_size);
            agents[i].update_days(); // Update the number of days infected or quarantined
        }

        // Infection spread and recovery logic
        for (size_t i = 0; i < agents.size(); ++i) {
            Agent& infector = agents[i];
            if (infector.state == Infected) {
                for (size_t j = 0; j < agents.size(); ++j) {
                    Agent& susceptible = agents[j];
                    if (susceptible.state == Susceptible && is_in_contact(infector, susceptible, infection_radius)) {
                        double r = static_cast<double>(rand()) / RAND_MAX;
                        if (r < infection_prob) {
                            susceptible.state = Infected;
                        }
                    }
                }
            }
        }

        // Recovery and quarantine logic
        for (size_t i = 0; i < agents.size(); ++i) {
            Agent& agent = agents[i];
            if (agent.state == Infected) {
                double r = static_cast<double>(rand()) / RAND_MAX;
                if (r < recovery_prob) {
                    agent.state = Recovered;
                } else if (r < quarantine_prob) {
                    agent.state = Quarantined;
                    agent.days_infected = 0; // Reset days in infected status when quarantined
                }
            } else if (agent.state == Quarantined) {
                // Check if the quarantine duration has passed
                if (agent.days_infected >= agent.quarantine_duration) {
                    agent.state = Susceptible; // Return to susceptible after quarantine
                }
            } else if (agent.state == Vaccinated) {
                // Allow a small probability of breakthrough infection
                double r = static_cast<double>(rand()) / RAND_MAX;
                if (r < 0.05) { // 5% chance of breakthrough infection
                    agent.state = Infected;
                }
            }
        }

        // Count each state
        int susceptible_count = 0, infected_count = 0, recovered_count = 0, vaccinated_count = 0, quarantined_count = 0;
        for (size_t i = 0; i < agents.size(); ++i) {
            const Agent& agent = agents[i];  // Use explicit type instead of auto
            switch (agent.state) {
                case Susceptible: susceptible_count++; break;
                case Infected: infected_count++; break;
                case Recovered: recovered_count++; break;
                case Vaccinated: vaccinated_count++; break;
                case Quarantined: quarantined_count++; break;
            }
        }

        // Save the population counts for this step
        save_to_csv(step, susceptible_count, infected_count, recovered_count, vaccinated_count, quarantined_count, file);

        // Display population counts (optional)
        cout << "Step " << step << ": Susceptible = " << susceptible_count
             << ", Infected = " << infected_count
             << ", Recovered = " << recovered_count
             << ", Vaccinated = " << vaccinated_count
             << ", Quarantined = " << quarantined_count << endl;
    }

    file.close();
}

int main() {
    // Simulation parameters
    int num_agents = 100;
    int grid_size = 20;

    // Default probabilities, to be overwritten by file input
    double infection_prob = 0.15;
    double recovery_prob = 0.03;
    double vaccination_prob = 0.02;
    double quarantine_prob = 0.01;

    int infection_radius = 2;  // Agents can infect within 2 unit distance
    int total_steps = 100;

    // Read parameters from file
    read_parameters("ABM_params.txt", infection_prob, recovery_prob, vaccination_prob, quarantine_prob);

    // Run the simulation
    abm_simulation(num_agents, grid_size, infection_prob, recovery_prob, vaccination_prob, quarantine_prob, infection_radius, total_steps);

    return 0;
}
