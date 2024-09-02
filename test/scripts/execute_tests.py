import os
import subprocess


def run_main_with_elf_files(input_directory, output_directory):
    # Ensure the output directory exists
    os.makedirs(output_directory, exist_ok=True)

    # Loop through all files in the specified input directory
    for filename in os.listdir(input_directory):
        # Check if the file has a .elf extension
        if filename.endswith(".elf"):
            elf_file_path = os.path.join(input_directory, filename)

            # Generate the corresponding .txt output file name in the output directory
            txt_output_file_name = os.path.splitext(filename)[0] + ".log"
            txt_output_file_path = os.path.join(
                output_directory, txt_output_file_name)

            # Command to run the main.elf program with the ELF file as input and .txt file as output
            command = ['./main.elf', elf_file_path, txt_output_file_path]

            try:
                # Run the command and capture the output
                result = subprocess.run(
                    command, check=True, text=True, capture_output=True)

                # Print the standard output and error from the command
                print(f"Processed {elf_file_path}:\nOutput written to {
                      txt_output_file_path}")
                print(f"Command Output:\n{result.stdout}")

            except subprocess.CalledProcessError as e:
                print(f"An error occurred while processing {
                      elf_file_path}:\n{e.stderr}")
            except Exception as e:
                print(f"An unexpected error occurred: {e}")


# Specify the input directory containing ELF files
input_directory_path = "../test_bin/acstone"

# Specify the output directory for .txt files
output_directory_path = "../test_log/acstone"

# Call the function to run main.elf with all ELF files in the input directory
run_main_with_elf_files(input_directory_path, output_directory_path)
