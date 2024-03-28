#README

Objective:
This C program performs a Caesar cipher encryption on the contents of a specified file or directory.
The 'caesarCipher' function performs the actual encryption using the Caesar cipher algorithm, 
shifting characters in the input text by the specified amount.

The program handles various error conditions, such as file opening failures, 
memory allocation failures, invalid user input, and file writing errors.

Run with Google Cloud Sheel example file structure Caesar.c
In the commandline terminal (ensure that you are on the directory of that file located) and type of each commands below:
gcc Caesar.c
./a.out (or ./Caesar.out) filename => e.g. ./a.out encrypt.txt

Here's a brief description of its functionality:

This C program performs a Caesar cipher encryption on the contents of a specified file or directory. Here's a brief description of its functionality:

1. Command-Line Argument Handling:
  * The program expects exactly two command-line arguments: the program name itself (argv[0]) and the name of the file to be processed (argv[1]).
  * If the number of arguments is not two, it prints a usage message and exits with an error code.

2. File Handling:
  * Attempts to open the specified file in binary read mode ("rb").
  * If the file opening fails, it prints an error message and exits with an error code.
  * Determines the size of the file by seeking to the end and then rewinding.
  * Checks if the file size is within a supported limit. If not, it prints an error message and exits with an error code.

3. Memory Allocation:
  * Dynamically allocates memory for a buffer to store the file content, with additional space for a null terminator (file_size + 1).
  * If memory allocation fails, it prints an error message, closes the file, and exits with an error code.

4.File Content Reading:
  * Reads the content of the file into the dynamically allocated buffer.
  * Null-terminates the buffer to make it a valid C string.

5.User Input:
  * Prompts the user to enter a shifting number for the Caesar cipher.
  * Validates the user input to ensure it is an integer within a specified range. If not, it prints an error message, frees the buffer, and exits with an error code.

6. Caesar Cipher Encryption:
  * Applies the Caesar cipher encryption to the content of the buffer based on the user-provided shifting number.

7. File Writing:
  * Reopens the file in binary write mode ("wb").
  * Writes the modified content back to the file.

8. Memory Deallocation and File Closure:
  * Frees the dynamically allocated memory.
  * Closes the file.

9. Successful Exit:
  * Exits the program with a success code.
