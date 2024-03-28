#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h> // For SIZE_MAX
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAXWORD 20

#define Userinput_INT_MIN -25
#define Userinput_INT_MAX 25

int length = 100;

void caesarCipher(char *text, int shift);
int dirLengthI(char *dirName);
int do_ls(char *dirName, char **dirArray);
int fileWrite(char *filename);
int isFile(char *filename);

// Author: Trivon Paul
int main(int argc, char *argv[]){
    // the state of the overall program if 1 an error has occured
    int state = 0; 

    // flags set by the user and if a flag was set
    int flagA = 0, flagS = 0, flagR = 0;
    int flagSet = 0; 
    char option;
    while((option = getopt(argc, argv, "asr")) != -1){
        switch(option){
            case 'a':
                flagSet = 1; 
                flagA = 1;
                break;
            case 's':
                flagSet = 1; 
                flagS = 1;
                break;
            case 'r':
                flagSet = 1; 
                flagR = 1;
                break;
        }
    }
    
    // name of file or directory provided by the user
    char *dirName = argv[1];
    
    // if dirName is not a file go through the directory
    if(isFile(dirName) == 0){
        // get the number of files in the directory
        state = dirLengthI(dirName);

        // holds the names of all files and directories
        char **dirArray;
        dirArray = (char **)malloc(length * sizeof(char *));

        if (dirArray == NULL) {
            printf("Memory allocation failed for the array of strings.\n");
            return 1;
        }

        // Allocate space for each string
        for (int i = 0; i < length; i++) {
            dirArray[i] = (char *)malloc(MAXWORD * sizeof(char));
            if (dirArray[i] == NULL) {
                printf("Memory allocation failed for string %d.\n", i);
                return 1;
            }
        }
        
        // If no flag is set then get every file name to then encrypt
        if(flagSet == 0) state = do_ls(dirName, dirArray);

        char *forwardSlash = "/";
        
        
        for(int i = 0; i < length; i++){
            if(strcmp(argv[0], dirArray[i]) != 0){
                // build a full path name
                char temp[100];

                char *forwardSlash = "/";
                size_t n = strlen(temp);
                size_t remaining = sizeof(temp) - n - 1;
                strncat(temp, dirName, remaining);
                
                n = strlen(temp);
                remaining = sizeof(temp) - n - 1;
                strncat(temp, forwardSlash, remaining);
                
                n = strlen(temp);
                remaining = sizeof(temp) - n - 1;
                strncat(temp, dirArray[i], remaining);
                
                printf("File: %s\n\n", temp);

                // if the given path name is a file then encrypt the file
                if(isFile(temp) == 1) state = fileWrite(temp);

                free(dirArray[i]);
                printf("*******************************************\n");
                for(int e = 0; e < 100; e++) temp[e] = '\0';
            }
        }
        
        free(dirArray);
        return state;
    } else {
        return fileWrite(dirName);
    }
}
// Author: Minh Tram
void caesarCipher(char *text, int shift)
{
    for (int i = 0; text[i] != '\0'; ++i)
    {
        if (text[i] >= 'A' && text[i] <= 'Z')
        {
            text[i] = toupper(((text[i] - 'A' + shift + 26) % 26) + 'A');
        }
        else if (text[i] >= 'a' && text[i] <= 'z')
        {
            text[i] = toupper(((text[i] - 'A' + shift + 26) % 26) + 'A');
        }
    }
}

// Author: Minh Tram
int fileWrite(char *filename){
    FILE *file;
    char *buffer;
    size_t file_size;
    int shift;
    // Check if the file exists
    // Command Injection, ID: 3-1 Avoid Constructing Commands with User Input
    /* Failure to Handle Errors Correctly, ID: 4-1 This part correctly checks if the file can be opened and 
       uses perror to print an error message with additional information from the operating system. */
    // Catching Exceptions, ID: 5-1 The code checks if the file can be opened successfully using 'fopen'.
    if ((file = fopen(filename, "rb")) == NULL)
    {
        perror("Error opening file for reading");
        return 1; // Exit with an error code
    }
    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);
    // Check for excessively large files
    /* Integer Overflows, ID: 2-1 This ensures that the total file size still falls within the representable 
       range of size_t and left last space for null terminator */
    if (file_size < 0 || file_size + 1 > SIZE_MAX)
    {
        fprintf(stderr, "File size exceeds the supported limit\n");
        fclose(file);
        return 1; // Exit with an error code
    }
    // Dynamically allocate memory for the file content
    buffer = (char *)malloc(file_size + 1); // +1 for null terminator
    // Failure to Handle Errors Correctly, ID: 4-3 Similarly, this part checks if memory allocation was successful and prints an error message if it fails.
    // Catching Exceptions, ID: 5-3 Memory allocation using malloc is checked, and if it fails, an error message is printed, and the program returns with an error code.
    if (buffer == NULL)
    {
        perror("Memory allocation failed");
        fclose(file);
        return 1; // Exit with an error code
    }
    // Read the file content into the dynamically allocated memory
    // Catching Exceptions, ID: 5-4 The code uses fread to read the file content into the dynamically allocated buffer. If operation fails, an error message is printed, and the program returns with an error code.
    if (fread(buffer, 1, file_size, file) != file_size)
    {
        perror("Error reading file");
        fclose(file);
        free(buffer);
        return 1; // Exit with an error code
    }
    // Null-terminate the buffer to make it a valid C string
    buffer[file_size] = '\0';
    // Display or process the original file content as needed
    printf("Original File content:\n%s\n", buffer);
    // Close the file after reading
    fclose(file);
    // Author: Minh Tram
    // Get the shifting number from the user
    printf("Enter the shifting number: ");
    // Catching Exceptions, ID: 5-5 The code uses scanf to get user input for the shifting number. If the input is not valid (e.g., not an integer), an error message is printed, and the program returns with an error code.
    if (scanf("%d", &shift) != 1 || (shift < Userinput_INT_MIN || shift > Userinput_INT_MAX))
    {
        fprintf(stderr, "Invalid input for shifting number\n");
        free(buffer);
        return 1; // Exit with an error code
    }
    // Apply Caesar cipher to the file content
    caesarCipher(buffer, shift);
    // Display or process the modified file content as needed
    printf("Modified File content:\n%s\n", buffer);
    // Open the file in binary mode for writing
    // Command Injection, ID: 3-2 Avoid Constructing Commands with User Input
    // Failure to Handle Errors Correctly, ID: 4-2 This part correctly checks if the file can be opened and uses perror to print an error message with additional information from the operating system.
    // Catching Exceptions, ID: 5-2 The code checks if the file can be opened successfully using 'fopen'.
    if ((file = fopen(filename, "wb")) == NULL)
    {
        perror("Error opening file for writing");
        free(buffer);
        return 1; // Exit with an error code
    }
    // Write the modified content back to the file
    // Catching Exceptions, ID: 5-6 The code uses fwrite to write the modified content back to the file. If operation fails, an error message is printed, and the program returns with an error code.
    if (fwrite(buffer, 1, file_size, file) != file_size)
    {
        perror("Error writing to file");
        fclose(file);
        free(buffer);
        return 1; // Exit with an error code
    }
    // Free the dynamically allocated memory
    free(buffer);
    // Close the file
    fclose(file);
    return 0; // Exit successfully
}

// Author: Trivon Paul
int isFile(char *filename){
    struct stat fileInfo;
    if (lstat(filename, &fileInfo) == -1) {
        perror("Failed to get file status");
        return -1;
    }

    if (S_ISREG(fileInfo.st_mode)) return 1;
    if (S_ISDIR(fileInfo.st_mode)) return 0;

    return -1;
}

// Author: Trivon Paul
int dirLengthI(char *dirName){
    length = 0;
    DIR *dir_ptr;
    struct dirent *dirent_ptr;
    if((dir_ptr = opendir(dirName)) == 0){
    printf("Directory not found\n");
    return -1; 
    }
    while((dirent_ptr = readdir(dir_ptr)) != 0){
    char *temp = dirent_ptr->d_name;
    if(temp[0] != '.')
        length++;
    }
    closedir(dir_ptr);
    return 0; 
}

// Author: Trivon Paul
int do_ls(char *dirName, char **dirArray){
    DIR *dir_ptr;
    struct dirent *dirent_ptr;
    if((dir_ptr = opendir(dirName)) == 0){
    printf("Directory not found\n");
    return -1; 
    } 
    int i = 0; 
    while((dirent_ptr = readdir(dir_ptr)) != 0){
    char *temp = dirent_ptr->d_name;
    if(temp[0] != '.'){
        strcpy(dirArray[i],temp);
        i++;
    }
    }
    closedir(dir_ptr);
    return 0; 
}
