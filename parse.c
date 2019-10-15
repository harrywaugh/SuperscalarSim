#include "parse.h"

#define MAX_PROGRAM_LEN 50
#define MAX_PROGRAM_LINE_LEN 100

int read_program_file(char *filename, char ***program_str)  {
    FILE *fp;
    char str[MAX_PROGRAM_LINE_LEN];
    int count = 0;
    char **trimmed_program;
    (*program_str) = (char **)malloc(MAX_PROGRAM_LEN * sizeof(char *));

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s", filename);
        exit(0);
    }

    while (fgets(str, MAX_PROGRAM_LINE_LEN, fp) != NULL)  {
        (*program_str)[count] = (char *)malloc(strlen(str) * sizeof(char));
        strcpy((*program_str)[count], str);

        count++;
    }
    printf("\n");
    fclose(fp);

    trimmed_program = (char **)realloc((*program_str), count*sizeof(char*));
    (*program_str) = trimmed_program;

    printf("Contents:\n", count);

    for (int j = 0; j < count; j++) {
        printf("\t\t%s", (*program_str)[j]);
    }
    printf("\n");


    return count;
}