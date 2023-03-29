#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 4096

void print_usage(char* argv) {
    printf("Usage: %s -d <directory> -p <permissions string> [-h]\n",argv);
}
void print_usage_error(char* argv) {
    fprintf(stderr,"Usage: %s -d <directory> -p <permissions string> [-h]\n",argv);
}

bool validate_permissions_string(const char* perm_str) {
    const int len = strlen(perm_str);
    if (len != 9) {
        return false;
    }

    const char* valid_chars = "-rwx";
    for (int i = 0; i < len; i++) {
        if (strchr(valid_chars, perm_str[i]) == NULL) {
            return false;
        }
    }
    return true;
}
int perm_string_to_int(const char* perm_str) {
    int perm = 0;
    for (int i = 0; i < 9; i++) {
        if (i == 0 || i == 3 || i == 6) {
            // Check for read permission
            if (perm_str[i] == 'r') {
                perm |= (1 << (8 - i));
            }
        } else if (i == 1 || i == 4 || i == 7) {
            // Check for write permission
            if (perm_str[i] == 'w') {
                perm |= (1 << (8 - i));
            }
        } else {
            // Check for execute permission
            if (perm_str[i] == 'x') {
                perm |= (1 << (8 - i));
            }
        }
    }
    return perm;
}
void pfind(const char* dir_path, const char* perm_str,int start) {
    struct stat dir_info;
    DIR* dir;
    struct dirent* entry;
    struct stat entry_info;
    char entry_path[MAX_PATH_LENGTH];
    if (lstat(dir_path, &dir_info) != 0) {
    	fprintf(stderr, "Error: %s\n", "Cannot stat file.");
    	exit(EXIT_FAILURE);
    } 
    dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Error: Cannot open directory '%s'. Permission denied.\n",dir_path);
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
	if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(entry_path, MAX_PATH_LENGTH, "%s/%s", dir_path, entry->d_name);

        if (lstat(entry_path, &entry_info) != 0) {
            fprintf(stderr, "Error: %s\n", "Cannot stat file.");
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(entry_info.st_mode)) {
            if ((entry_info.st_mode & 0777) == (perm_string_to_int(perm_str) & 0777)) {
                    printf("%s\n", entry_path);
            }
            if (entry_info.st_mode & S_IXUSR) { // recurse only if directory is executable
                pfind(entry_path, perm_str,0);
            }
        } else if (S_ISREG(entry_info.st_mode)) {
            if ((entry_info.st_mode & 0777) == (perm_string_to_int(perm_str) & 0777)) {
                    printf("%s\n", entry_path);
	    }
        }
    }

    closedir(dir);
}

int main(int argc, char** argv) {
    char* dir_path = NULL;
    char* perm_str = NULL;
    int opt;
    if(argc == 1) {
	print_usage_error(argv[0]);
	exit(EXIT_FAILURE);
    }
    while ((opt = getopt(argc, argv, ":hd:p:")) != -1) {
        switch (opt) {
        case 'h':
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        case 'd':
            dir_path = optarg;
	    break;
        case 'p':
            perm_str = optarg;
            break;
        case '?':
            fprintf(stderr, "Error: Unknown option '%c' received.\n",optopt);
	    exit(EXIT_FAILURE);
	}
	}
	if (dir_path == NULL) {
            fprintf(stderr, "Error: Required argument -d <directory> not found.\n");
            exit(EXIT_FAILURE);
        }
    	if (perm_str == NULL) {
       	    fprintf(stderr, "Error: Required argument -p <permissions string> not found.\n");
	    exit(EXIT_FAILURE);
    }
    if (access(dir_path, F_OK)!= 0) {
       fprintf(stderr, "Error: Cannot stat '%s'. No such file or directory.\n", dir_path);
         exit(EXIT_FAILURE);
    }
    if (!validate_permissions_string(perm_str)) {
        fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", perm_str);
	    exit(EXIT_FAILURE);
    }

    pfind(dir_path, perm_str,1);

    return EXIT_SUCCESS;
}

