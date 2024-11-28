#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace std;

// Helper function to write parameters to file
void write_parameters_to_file(const string &filename, const vector<pair<string, double>> &params)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Could not open parameter file " << filename << endl;
        return;
    }

    for (const auto &param : params)
    {
        file << param.first << "=" << param.second << endl;
    }

    file.close();
    cout << "Parameters saved to " << filename << endl;
}

void display_gui()
{
    static int model_selection = 0;
    const char *models[] = {"SEIR", "SIR", "SIS", "ABM", "Branching", "MARKONIKOV"};
    ImGui::Combo("Select Model", &model_selection, models, IM_ARRAYSIZE(models));

    // Model parameters as static variables
    static float quarantine_time = 10.0f;
    static float vaccination_rate = 0.5f;
    static float vaccination_start = 20.0f;
    static float vaccination_speed = 0.01f;
    static float avoid_contact_factor = 0.2f;
    static float health_checkup_factor = 0.3f;
    static float safe_practices_factor = 0.4f;
    static float infection_prob = 0.8f;
    static float recovery_prob = 0.7f;
    static float protective_measures_rate = 0.0f;

    vector<pair<string, double>> params; // To store parameter key-value pairs
    string filename;                     // Parameter file name
    string cpp_filename;                 // C++ file name to compile and execute
    string csv_filename; // CSV file for the selected model

    // Display inputs dynamically based on the selected model
    switch (model_selection)
    {
    case 0: // SEIR
        ImGui::SliderFloat("Quarantine Time", &quarantine_time, 1.0f, 100.0f);
        ImGui::SliderFloat("Vaccination Rate", &vaccination_rate, 0.0f, 1.0f);
        ImGui::SliderFloat("Vaccination Start", &vaccination_start, 1.0f, 100.0f);
        ImGui::SliderFloat("Vaccination Speed", &vaccination_speed, 0.0f, 0.01f);
        params = {
            {"quarantine_time", quarantine_time},
            {"vaccination_rate", vaccination_rate},
            {"vaccination_start", vaccination_start},
            {"vaccination_speed", vaccination_speed}};
        filename = "SEIR_params.txt";
        cpp_filename = "SEIR.cpp";
        csv_filename = "SEIR_simulation_results.csv";
        break;

    case 1: // SIR
        ImGui::SliderFloat("Quarantine Time", &quarantine_time, 1.0f, 100.0f);
        ImGui::SliderFloat("Vaccination Rate", &vaccination_rate, 0.0f, 1.0f);
        ImGui::SliderFloat("Vaccination Start", &vaccination_start, 1.0f, 100.0f);
        ImGui::SliderFloat("Vaccination Speed", &vaccination_speed, 0.0f, 0.01f);
        params = {
            {"quarantine_time", quarantine_time},
            {"vaccination_rate", vaccination_rate},
            {"vaccination_start", vaccination_start},
            {"vaccination_speed", vaccination_speed}};
        filename = "SIR_params.txt";
        cpp_filename = "SIR.cpp";
        csv_filename = "SIR_simulation_results.csv";
        break;

    case 2: // SIS
        ImGui::SliderFloat("Avoid Contact Factor", &avoid_contact_factor, 0.0f, 1.0f);
        ImGui::SliderFloat("Health Checkup Factor", &health_checkup_factor, 0.0f, 1.0f);
        ImGui::SliderFloat("Vaccination Factor", &vaccination_rate, 0.0f, 1.0f);
        ImGui::SliderFloat("Safe Practices Factor", &safe_practices_factor, 0.0f, 1.0f);
        params = {
            {"avoid_contact_factor", avoid_contact_factor},
            {"health_checkup_factor", health_checkup_factor},
            {"vaccination_factor", vaccination_rate},
            {"safe_practices_factor", safe_practices_factor}};
        filename = "SIS_params.txt";
        cpp_filename = "SIS.cpp";
        csv_filename = "SIS_simulation_results.csv";
        break;

    case 3: // ABM
        ImGui::SliderFloat("Infection Probability", &infection_prob, 0.0f, 1.0f);
        ImGui::SliderFloat("Recovery Probability", &recovery_prob, 0.0f, 1.0f);
        ImGui::SliderFloat("Vaccination Probability", &vaccination_rate, 0.0f, 1.0f);
        ImGui::SliderFloat("Quarantine Probability", &quarantine_time, 0.0f, 1.0f);
        params = {
            {"infection_prob", infection_prob},
            {"recovery_prob", recovery_prob},
            {"vaccination_prob", vaccination_rate},
            {"quarantine_prob", quarantine_time}};
        filename = "ABM_params.txt";
        cpp_filename = "ABM.cpp";
        csv_filename = "ABM_simulation_results.csv";
        break;

    case 4: // Branching
        ImGui::SliderFloat("Vaccination Probability", &vaccination_rate, 0.0f, 1.0f);
        ImGui::SliderFloat("Quarantine Probability", &quarantine_time, 0.0f, 1.0f);
        ImGui::SliderFloat("Safe Practices Probability", &safe_practices_factor, 0.0f, 1.0f);
        params = {
            {"vaccination_prob", vaccination_rate},
            {"quarantine_prob", quarantine_time},
            {"safe_practices_prob", safe_practices_factor}};
        filename = "Branching_params.txt";
        cpp_filename = "SEIR.cpp";
        csv_filename = "Branching_simulation_results.csv";
        break;

    case 5: // MARKONIKOV
        ImGui::SliderFloat("Vaccination Rate", &vaccination_rate, 0.0f, 1.0f);
        ImGui::SliderFloat("Protective Measures Rate", &protective_measures_rate, 0.0f, 1.0f);
        params = {
            {"vaccination_rate", vaccination_rate},
            {"protective_measures_rate", protective_measures_rate}};
        filename = "MARKONIKOV_params.txt";
        cpp_filename = "MARKOV_PROCESS_SIR.cpp";
        csv_filename = "MARKONIKOV_simulation_results.csv";
        break;
    }

    if (ImGui::Button("Run Simulation"))
    {

        // Write selected model's CSV filename to a temporary file
        ofstream file("selected_model_filename.txt");
        if (file.is_open())
        {
            file << csv_filename;
            file.close();
        }
        else
        {
            cerr << "Error: Could not write to selected_model_filename.txt" << endl;
            return;
        }

        cout << "Selected model's results will be loaded from: " << csv_filename << endl;


        write_parameters_to_file(filename, params);
        cout << "Running " << models[model_selection] << " simulation..." << endl;

        // Compile and execute the model's C++ file, then run the Python script
        string compile_command = "g++ " + cpp_filename + " -o model_exec -std=c++11 && ./model_exec";
        string python_command = "python3 main.py";

        // Execute the commands
        int compile_status = system(compile_command.c_str());
        if (compile_status == 0)
        {
            cout << "Model executed successfully. Generating plots..." << endl;
            system(python_command.c_str());
        }
        else
        {
            cerr << "Error: Compilation or execution of the model failed." << endl;
        }
    }
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        cerr << "Error: Could not initialize GLFW" << endl;
        return -1;
    }

    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(1280, 720, "ImGui Application", NULL, NULL);
    if (window == NULL)
    {
        cerr << "Error: Could not create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 120");

    // Main application loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Display GUI
        display_gui();

        // Render ImGui content
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}