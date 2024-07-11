#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libcsv.h"

char **splitString(const char *str, const char *delimiter, int *count) {
    char *strCopy = strdup(str);
    if (strCopy == NULL) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }

    int capacity = 10;
    char **result = malloc(capacity * sizeof(char *));
    if (result == NULL) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }

    *count = 0;
    char *token = strtok(strCopy, delimiter);
    while (token != NULL) {
        if (*count >= capacity) {
            capacity *= 2;
            char **temp = realloc(result, capacity * sizeof(char *));
            if (temp == NULL) {
                perror("Erro de realocação de memória");
                exit(EXIT_FAILURE);
            }
            result = temp;
        }
        result[*count] = strdup(token);
        if (result[*count] == NULL) {
            perror("Erro de alocação de memória");
            exit(EXIT_FAILURE);
        }
        (*count)++;
        token = strtok(NULL, delimiter);
    }
    free(strCopy);
    return result;
}

int applyFilters(char **headers, char **row, int columnCount, const char *rowFilterDefinitions) {
    int filterCount;
    char **filters = splitString(rowFilterDefinitions, "\n", &filterCount);

    for (int i = 0; i < filterCount; i++) {
        char *filter = strdup(filters[i]);
        if (filter == NULL) {
            perror("Erro de alocação de memória");
            exit(EXIT_FAILURE);
        }

        char *header = strtok(filter, "=><");
        char *operator = filter + strlen(header);
        while (*operator != '=' && *operator != '>' && *operator != '<') {
            operator++;
        }
        char op = *operator;
        char *value = operator + 1;
        *operator = '\0';

        int columnIndex = -1;
        for (int j = 0; j < columnCount; j++) {
            if (strcmp(headers[j], header) == 0) {
                columnIndex = j;
                break;
            }
        }
        if (columnIndex == -1) {
            free(filter);
            continue;
        }

        int comparison = strcmp(row[columnIndex], value);
        if ((op == '=' && comparison != 0) ||
            (op == '>' && comparison <= 0) ||
            (op == '<' && comparison >= 0)) {
            free(filter);
            return 0;
        }
        free(filter);
    }

    free(filters);
    return 1;
}

void processCsv(const char csv[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    int lineCount;
    char **lines = splitString(csv, "\n", &lineCount);

    if (lineCount == 0) {
        return;
    }

    int columnCount;
    char **headers = splitString(lines[0], ",", &columnCount);

    int selectedColumnCount;
    char **selectedColumnsArray = splitString(selectedColumns, ",", &selectedColumnCount);

    if (selectedColumnCount == 1 && selectedColumnsArray[0][0] == '\0') {
        selectedColumnCount = columnCount;
        selectedColumnsArray = headers;
    }

    for (int i = 0; i < selectedColumnCount; i++) {
        printf("%s", selectedColumnsArray[i]);
        if (i < selectedColumnCount - 1) {
            printf(",");
        }
    }
    printf("\n");

    for (int i = 1; i < lineCount; i++) {
        char **row = splitString(lines[i], ",", &columnCount);
        if (applyFilters(headers, row, columnCount, rowFilterDefinitions)) {
            for (int j = 0; j < selectedColumnCount; j++) {
                for (int k = 0; k < columnCount; k++) {
                    if (strcmp(headers[k], selectedColumnsArray[j]) == 0) {
                        printf("%s", row[k]);
                        if (j < selectedColumnCount - 1) {
                            printf(",");
                        }
                        break;
                    }
                }
            }
            printf("\n");
        }
        for (int j = 0; j < columnCount; j++) {
            free(row[j]);
        }
        free(row);
    }

    for (int i = 0; i < lineCount; i++) {
        free(lines[i]);
    }
    free(lines);

    if (selectedColumnsArray != headers) {
        for (int i = 0; i < selectedColumnCount; i++) {
            free(selectedColumnsArray[i]);
        }
    }
    free(selectedColumnsArray);

    for (int i = 0; i < columnCount; i++) {
        free(headers[i]);
    }
    free(headers);
}

void processCsvFile(const char csvFilePath[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    FILE *file = fopen(csvFilePath, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo CSV");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char *csvData = (char *)malloc(fileSize + 1);
    if (csvData == NULL) {
        perror("Erro de alocação de memória");
        fclose(file);
        return;
    }

    fread(csvData, 1, fileSize, file);
    csvData[fileSize] = '\0';

    fclose(file);
    
    processCsv(csvData, selectedColumns, rowFilterDefinitions);

    free(csvData);
}