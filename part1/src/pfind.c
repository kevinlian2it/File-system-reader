#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 4096

void print_usage() {
    printf("Usage: pfind -d <directory> -p <permissions string> [-h]\n");
}
void print_usage_error() {
    fprintf(stderr,"Usage: pfind -d <directory> -p <permissions string> [-h]\n");
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

void pfind(const char* dir_path, const char* perm_str,int start) {
    struct stat dir_info;
    DIR* dir;
    struct dirent* entry;
    struct stat entry_info;
    char entry_path[MAX_PATH_LENGTH];

    if (lstat(dir_path, &dir_info) != 0) {
        fprintf(stderr, "Error: %s\n", "Cannot stat directory.");
        exit(EXIT_FAILURE);
    }
	if(start == 0) {
    printf("%s\n", dir_path); // print directory if it matches
	}
    dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Error: %s\n", "Cannot open directory.");
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
            if ((entry_info.st_mode & 0777) == strtol(perm_str, NULL, 8)) {
                printf("%s\n", entry_path); // print directory if it matches
            }
            if (entry_info.st_mode & S_IXUSR) { // recurse only if directory is executable
                pfind(entry_path, perm_str,0);
            }
        } else if (S_ISREG(entry_info.st_mode)) {
            if ((entry_info.st_mode & 0777) == strtol(perm_str, NULL, 8)) {
                printf("%s\n", entry_path);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char** argv) {
    char* dir_path = NULL;
    char* perm_str = NULL;
	if(argc < 2) {
		print_usage_error();
		exit(EXIT_FAILURE);
	}
    int opt;
    while ((opt = getopt(argc, argv, ":hd:p:")) != -1) {
        switch (opt) {
        case 'h':
            print_usage();
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
	default:
	    print_usage_error();
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
DIR* dir = opendir(dir_path);
    if (dir == NULL) {
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

