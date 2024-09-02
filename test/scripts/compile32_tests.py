import os
import subprocess

# Define the directory containing the .c files
# Replace with the path to your folder
source_directory = './test/acstone'

# Define the output directory for compiled files (optional)
# Replace with the path to your output folder
output_directory = './test_bin/acstone'

# Ensure the output directory exists
os.makedirs(output_directory, exist_ok=True)

# Iterate over all files in the source directory
for filename in os.listdir(source_directory):
    # Check if the file is a .c file
    if filename.endswith('.c'):
        # Construct the full path to the source file
        source_file = os.path.join(source_directory, filename)
        aux_file = os.path.join(source_directory, 'crt.S')

        # Construct the output file name by replacing .c with .o
        # There are some .c in the middle of the file names
        filename = filename + '.elf'
        output_file = os.path.join(
            output_directory, filename.replace('.c.', '.'))

        # Compile the .c file using riscv64-linux-gnu-gcc
        compile_command = ['riscv32-unknown-elf-gcc', '-g', '-march=rv32im', '-std=gnu99', '-mabi=ilp32',
                           aux_file, source_file, '-o', output_file,
                           '-g', '-march=rv32im', '-std=gnu99', '-mabi=ilp32', '-nostartfiles', '-nostdinc', '-nostdlib']

        # Execute the compile command
        try:
            print(f"Compiling {source_file}...")
            subprocess.run(compile_command, check=True)
            print(f"Successfully compiled {filename} to {output_file}")
        except subprocess.CalledProcessError as e:
            print(f"Error compiling {filename}: {e}")

print("Compilation process completed.")
