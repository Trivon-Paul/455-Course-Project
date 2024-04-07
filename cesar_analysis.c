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

#define MAXPATH 50
#define Userinput_INT_MIN -25
#define Userinput_INT_MAX 25

int length = 100;

void caesarCipher(char *text, int shift);
int directoryLength(char *dirName);
int getFilenames(char *dirName, char **dirArray);
int fileWrite(char *filename);
int isFile(char *filename);
int backupFile(char *filename, char *dir, char *path);

int main(int argc, char *argv[]){
     // if the user doesn't provide any arguments exit program
     if (argc < 2) {
        fprintf(stderr, "Usage: %s [-b] <file_or_directory>, -h for more information\n", argv[0]);
        return 1; // Exit with an error code
    }
    // flags set by the user and if a flag was set
    int flagB = 0, flagH = 0;

     // holds the options/arguments provided by the user
    char option;
    while((option = getopt(argc, argv, "bh")) != -1){
        switch(option){
            case 'b':
                flagB = 1;
                break;
            case 'h':
                flagH = 1;
                break;
        }
    }

     // if the user sets the H flag then print a help message then exit
    if(flagH == 1){
        printf("Usage: %s [-b] <file_or_directory>\n\n"
                "Program name: Caesar\n"
                "Arguments:\n"
                "    -b     Backs up the given file or directory before encryption\n\n"
                "Description:\n"
                "    This program takes the given file name or directory and encrypts it using a Caesar cipher.\n"
                "    If given a directory, it will recursively go through the directory and encrypt all the files\n"
                "    found within it. The user will be prompted to enter the number of shifts for encryption.\n"
                "Example:\n"
                "    Encrypt a single file:\n"
                "        %s example.txt\n\n"
                "    Encrypt a directory (backing up files before encryption):\n"
                "        %s -b /path/to/directory\n", argv[0],argv[0],argv[0]);
        return 0;
    }

    // name of file or directory provided by the user
    char *dirName = argv[1];
    if(flagB == 1) dirName = argv[2];
    
    // if dirName is not a file go through the directory
    if(isFile(dirName) == 0){
        // get the number of files in the directory
        if(directoryLength(dirName) == -1) return 1; // Exit with an error code

        // holds the names of all files and directories
        char **dirArray;
        dirArray = (char **)malloc(length * sizeof(char *));

         // if memory wasn't allocated exit program with error code
        if (dirArray == NULL) {
            printf("Memory allocation failed for the array of strings.\n");
            return 1;
        }

        // Allocate space for each string
        for (int i = 0; i < length; i++) {
            dirArray[i] = (char *)malloc(MAXPATH * sizeof(char));
             
             // if memory wasn't allocated exit program with error code
            if (dirArray[i] == NULL) {
                printf("Memory allocation failed for string %d.\n", i);
                return 1;
            }
        }
        
        // get every file name to then encrypt
        if(getFilenames(dirName, dirArray) == -1) return 1; // Exit with an error code

        char *forwardSlash = "/";
        
        for(int i = 0; i < length; i++){
            // If the filename is the same as the filename of the program go to next file
            if(strcmp(argv[0], dirArray[i]) == 0) continue;

           // build a full path name
           char path[100];

           size_t n = strlen(path);
           size_t remaining = sizeof(path) - n - 1;
           strncat(path, dirName, remaining);
           
           n = strlen(path);
           remaining = sizeof(path) - n - 1;
           strncat(path, forwardSlash, remaining);
           
           n = strlen(path);
           remaining = sizeof(path) - n - 1;
           strncat(path, dirArray[i], remaining);
           

           // if the given path name is a file then encrypt the file
           if(isFile(path) == 1){ 
               printf("File: %s\n\n", path);

                //if the user set a B flag then backup the file
               if(flagB == 1) if(backupFile(dirArray[i], dirName, path) == -1) return 1; // Exit with an error code

                // encrypt the file using caesar cypher
               if(fileWrite(path) == 1) return 1; // Exit with an error code
               printf("*******************************************\n");
           }

            // Free the dynamically allocated memory
           free(dirArray[i]);

            // Clear path variable
           for(int e = 0; e < 100; e++) path[e] = '\0';
        }

         // Free the dynamically allocated memory
        free(dirArray);
    } else {
         //if the user set a B flag then backup the file
        if(flagB == 1) if(backupFile(NULL, NULL, dirName) == -1) return 1; // Exit with an error code

         // encrypt the file using caesar cypher
        if(fileWrite(dirName) == 1) return 1; // Exit with an error code
    }

    return 0;
}

// Perform Ceasar Cipher using the shift on the text given
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

// With the filename given open the file to read then write to
int fileWrite(char *filename){
    FILE *file;
    char *buffer;
    size_t file_size;
    int shift;
    // Check if the file exists
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
    if (file_size < 0 || file_size + 1 > SIZE_MAX)
    {
        fprintf(stderr, "File size exceeds the supported limit\n");
        fclose(file);
        return 1; // Exit with an error code
    }
    // Dynamically allocate memory for the file content
    buffer = (char *)malloc(file_size + 1); // +1 for null terminator
    if (buffer == NULL)
    {
        perror("Memory allocation failed");
        fclose(file);
        return 1; // Exit with an error code
    }
    // Read the file content into the dynamically allocated memory
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
    // Get the shifting number from the user
    printf("Enter the shifting number: ");
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
    if ((file = fopen(filename, "wb")) == NULL)
    {
        perror("Error opening file for writing");
        free(buffer);
        return 1; // Exit with an error code
    }
    // Write the modified content back to the file
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

// Check to see if the given filename is a file or directory
int isFile(char *filename){
    struct stat fileInfo;
    // end with error if lstat can't access the filename given
    if (lstat(filename, &fileInfo) == -1) {
        perror("Failed to get file status");
        return -1;
    }

    // return 1 if filename is a file
    if (S_ISREG(fileInfo.st_mode)) return 1;

    // return 0 if filename is a directory
    if (S_ISDIR(fileInfo.st_mode)) return 0;

    return -1;
}

// check to see the length of a given directory
int directoryLength(char *dirName){
     // Set the length to 0
    length = 0;
    DIR *dir_ptr;
    struct dirent *dirent_ptr;
     
    if((dir_ptr = opendir(dirName)) == 0){
        printf("Directory not found\n");
        return -1; 
    }

    // For every file or directory encountered add 1 to the length
    while((dirent_ptr = readdir(dir_ptr)) != 0){
        char *temp = dirent_ptr->d_name;
        if(temp[0] != '.')
            length++;
    }

    // Close the directory
    closedir(dir_ptr);
    return 0; 
}

// Get all the filenames within a directory
int getFilenames(char *dirName, char **dirArray){
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

    // Close the directory
    closedir(dir_ptr);
    return 0; 
}

// Create a directory called backup to copy files to
int backupFile(char *filename, char *dir, char *path) {
    char backupDir[MAXPATH];
    char backupFile[MAXPATH];

    // Create backup directory if it doesn't exist
    if(dir == NULL){
        strcpy(backupDir, "backup");
    } else {
        strcpy(backupDir, dir);
        strcat(backupDir, "/backup");
    }
    mkdir(backupDir, 0777);

    // Form the backup file path
    if(dir == NULL)
    snprintf(backupFile, sizeof(backupFile), "%s/%s", backupDir, path);
    else
    snprintf(backupFile, sizeof(backupFile), "%s/%s", backupDir, filename);

    // Open original file for reading
    FILE *originalFile = fopen(path, "rb");
    if (originalFile == NULL) {
        perror("Error opening original file");
        return 1;
    }

    // Open backup file for writing
    FILE *backup_file_ptr = fopen(backupFile, "wb");
    if (backup_file_ptr == NULL) {
        perror("Error creating backup file");
        fclose(originalFile);
        return 1;
    }

    // Copy contents from original file to backup file
    int ch;
    while ((ch = fgetc(originalFile)) != EOF) {
        fputc(ch, backup_file_ptr);
    }

    // Close files
    fclose(originalFile);
    fclose(backup_file_ptr);

    printf("Backup created: %s\n\n", backupFile);

    return 0;
}
