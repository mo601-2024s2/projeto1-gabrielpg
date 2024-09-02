import os

# Get the current directory
current_directory = os.getcwd()

# Get the parent directory
parent_directory = os.path.dirname(current_directory)

print("Current Directory:", current_directory)
print("Parent Directory:", parent_directory)
