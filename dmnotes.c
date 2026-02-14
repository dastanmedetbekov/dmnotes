#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define MAX_LINE 2048

void createDir(const char *path) {
    #ifdef _WIN32
        mkdir(path);
    #else
        mkdir(path, 0700);
    #endif
}

int appendToFile(const char *filename, const char *date, const char *text) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) return 1;
    fprintf(file, "[%s] %s\n", date, text);
    fclose(file);
    return 0;
}

void printLastLines(const char *filename, int n) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return;

    char line[MAX_LINE];
    char **lines = calloc(n, sizeof(char*));
    if (!lines) { fclose(file); return; }

    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        if (lines[count % n]) free(lines[count % n]);
        lines[count % n] = strdup(line); 
        count++;
    }
    fclose(file);

    int start = (count > n) ? (count % n) : 0;
    int total = (count > n) ? n : count;

    for (int i = 0; i < total; i++) {
        printf("%s", lines[(start + i) % n]);
        free(lines[(start + i) % n]); 
    }
    free(lines);
}

void searchInFile(const char *filename, const char *query) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, query) != NULL) { 
            printf("%s", line);
        }
    }
    fclose(file);
}

int deleteLineFromFile(const char *filename, const char *query) {
    char tempFilename[PATH_MAX + 10];
    snprintf(tempFilename, sizeof(tempFilename), "%s.tmp", filename);

    FILE *src = fopen(filename, "r");
    if (!src) return 1;

    FILE *tmp = fopen(tempFilename, "w");
    if (!tmp) { fclose(src); return 1; }

    char line[MAX_LINE];
    int found = 0;
    
    while (fgets(line, sizeof(line), src)) {
        if (strstr(line, query) == NULL) {
            fputs(line, tmp);
        } else {
            found = 1;
        }
    }

    fclose(src);
    fclose(tmp);

    if (found) {
        remove(filename);
        rename(tempFilename, filename);
    } else {
        remove(tempFilename);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    char str[MAX_LINE] = ""; 
    const char *homeDir = getenv("HOME");
    char dirpath[PATH_MAX];
    char filepath[PATH_MAX + 64];

    if (homeDir != NULL) {
        snprintf(dirpath, sizeof(dirpath), "%s/.local/share/dmnotes", homeDir);
        createDir(dirpath);
        snprintf(filepath, sizeof(filepath), "%s/notes.txt", dirpath);
    } else {
        strncpy(filepath, "notes.txt", sizeof(filepath));
    }

    if (argc >= 2) {
        if (strcmp(argv[1], "-r") == 0) {
            int lines = (argc == 3) ? atoi(argv[2]) : 10;
            printLastLines(filepath, lines);
            return 0;
        }

        if (strcmp(argv[1], "-s") == 0) {
            if (argc < 3) return 1;
            searchInFile(filepath, argv[2]);
            return 0;
        }

        if (strcmp(argv[1], "-d") == 0) {
            if (argc < 3) return 1;
            deleteLineFromFile(filepath, argv[2]);
            return 0;
        }

        if (strcmp(argv[1], "--reset") == 0) {
            fclose(fopen(filepath, "w"));
            return 0;
        }
    }

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            strncat(str, argv[i], sizeof(str) - strlen(str) - 1);
            if (i < argc - 1) strncat(str, " ", sizeof(str) - strlen(str) - 1);
        }
    } else {
        if (fgets(str, sizeof(str), stdin) == NULL) return 1;
        str[strcspn(str, "\n")] = 0;
    }

    if (strlen(str) == 0) return 0;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%d.%m.%Y %H:%M", t);

    if (appendToFile(filepath, dateStr, str) != 0) {
        fprintf(stderr, "Error writing to file\n");
        return 1;
    }

    return 0;
}
