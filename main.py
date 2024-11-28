import matplotlib.pyplot as plt
import pandas as pd
import os

# Fetch the filename dynamically
filename_path = "selected_model_filename.txt"  # Temporary file written by C++ code
if not os.path.exists(filename_path):
    raise FileNotFoundError(f"{filename_path} not found. Ensure the C++ program executed properly.")

with open(filename_path, 'r') as file:
    filename = file.read().strip()  # Read the file name written by the C++ code

# Load the CSV file into a DataFrame
print(f"Loading data from {filename}...")
data = pd.read_csv(filename)

# Fetch column names dynamically
headers = data.columns.tolist()
print("Columns in the file:", headers)

# First column is assumed to be the x-axis
x_variable = headers[0]
y_variables = headers[1:]  # All other columns for y-axes

# Plot each y-variable against the x-variable
plt.figure(figsize=(10, 6))  # Set figure size for better visualization
for y_variable in y_variables:
    plt.plot(data[x_variable], data[y_variable], label=y_variable)

# Adding title and labels
plt.title(f"Dynamic Plot from {filename}")
plt.xlabel(x_variable)
plt.ylabel("Values")

# Adding a legend (always shown)
plt.legend()

# Display the plot
plt.grid(True)  # Optional: Add grid for better readability
plt.show()
