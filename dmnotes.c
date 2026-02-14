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

int appendToFile(const char *filename, const char *date, const char *text) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) return 1;
    fprintf(file, "[%s] %s\n", date, text);
    fclose(file);
    return 0;
}

int main(int argc, char *argv[]) {
    char str[2048] = ""; 
    const char *homeDir = getenv("HOME");
    char dirpath[PATH_MAX];
    char filepath[PATH_MAX + 64];
    char command[PATH_MAX + 256];

    if (homeDir != NULL) {
        snprintf(dirpath, sizeof(dirpath), "%s/.local/share/dmnotes", homeDir);
        mkdir(dirpath, 0700);
        snprintf(filepath, sizeof(filepath), "%s/notes.txt", dirpath);
    } else {
        strncpy(filepath, "notes.txt", sizeof(filepath));
    }

    if (argc >= 2 && strcmp(argv[1], "-r") == 0) {
        int lines = (argc == 3) ? atoi(argv[2]) : 10;
        snprintf(command, sizeof(command), "tail -n %d \"%s\"", lines, filepath);
        system(command);
        return 0;
    }

    if (argc >= 2 && strcmp(argv[1], "-s") == 0) {
        if (argc < 3) return 1;
        snprintf(command, sizeof(command), "grep -i \"%s\" \"%s\"", argv[2], filepath);
        system(command);
        return 0;
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

    if (argc >= 3 && strcmp(argv[1], "-d") == 0) {
    snprintf(command, sizeof(command), "sed -i '/%s/d' \"%s\"", argv[2], filepath);
    system(command);
    return 0;
    }

    if (argc >= 2 && strcmp(argv[1], "--reset") == 0) {
    FILE *f = fopen(filepath, "w");
    if (f) {
        fclose(f);
    }
    return 0;
    }

    if (appendToFile(filepath, dateStr, str) != 0) {
        fprintf(stderr, "Error\n");
        return 1;
    }

    return 0;
}
