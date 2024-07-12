#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libcsv.h"

#define MAX_COLUMNS 256
#define MAX_LINE_LENGTH 1024

typedef struct {
    char *header;
    int columnIndex;
    char comparator;
    char *value;
} Filter;

void parseFilters(const char *filterDefs, Filter *filters, int *filterCount);
int parseSelectedColumns(const char *selectedCols, char **columns);
void filterAndPrintCsv(char *csv, char **selectedColumns, int selectedColumnCount, Filter *filters, int filterCount);
int applyFilter(char **row, Filter *filters, int filterCount, char **headers, int headerCount);
int compare(const char *a, const char *b, char comparator);

void processCsv(const char csv[], const char selectedColumns[], const char filterDefs[]) {
    char *csvCopy = strdup(csv);
    if (!csvCopy) {
        perror("Falha ao alocar memória");
        return;
    }

    Filter filters[MAX_COLUMNS];
    int filterCount = 0;
    parseFilters(filterDefs, filters, &filterCount);

    char *selectedColumnsArray[MAX_COLUMNS];
    int selectedColumnCount = parseSelectedColumns(selectedColumns, selectedColumnsArray);
    filterAndPrintCsv(csvCopy, selectedColumnsArray, selectedColumnCount, filters, filterCount);

    free(csvCopy);
}

void parseFilters(const char *filterDefs, Filter *filters, int *filterCount) {
    char *defsCopy = strdup(filterDefs);
    if (!defsCopy) {
        perror("Falha ao alocar memória");
        return;
    }

    char *currentPos = defsCopy;
    char *nextLine = strchr(currentPos, '\n');
    while (currentPos) {
        if (nextLine) {
            *nextLine = '\0';
        }

        char *headerEnd = strpbrk(currentPos, "><=");
        if (!headerEnd) {
            fprintf(stderr, "Filtro inválido: '%s'\n", currentPos);
            currentPos = nextLine ? nextLine + 1 : NULL;
            nextLine = currentPos ? strchr(currentPos, '\n') : NULL;
            continue;
        }

        char comparator = *headerEnd;
        if (comparator != '>' && comparator != '<' && comparator != '=') {
            fprintf(stderr, "Filtro inválido: '%s'\n", currentPos);
            currentPos = nextLine ? nextLine + 1 : NULL;
            nextLine = currentPos ? strchr(currentPos, '\n') : NULL;
            continue;
        }

        size_t headerLen = headerEnd - currentPos;
        char *header = (char *)malloc(headerLen + 1);
        if (!header) {
            perror("Falha ao alocar memória para o cabeçalho");
            break;
        }
        strncpy(header, currentPos, headerLen);
        header[headerLen] = '\0';

        char *value = headerEnd + 1;
        if (!value || *value == '\0') {
            fprintf(stderr, "Filtro inválido: '%s'\n", currentPos);
            free(header);
            currentPos = nextLine ? nextLine + 1 : NULL;
            nextLine = currentPos ? strchr(currentPos, '\n') : NULL;
            continue;
        }

        filters[*filterCount].header = header;
        filters[*filterCount].comparator = comparator;
        filters[*filterCount].value = strdup(value);
        if (!filters[*filterCount].value) {
            fprintf(stderr, "Falha na alocação de memória\n");
            free(header);
            break;
        }

        (*filterCount)++;
        currentPos = nextLine ? nextLine + 1 : NULL;
        nextLine = currentPos ? strchr(currentPos, '\n') : NULL;
    }

    free(defsCopy);
}

int parseSelectedColumns(const char *selectedCols, char **columns) {
    if (!selectedCols || *selectedCols == '\0') {
        return -1;
    }

    int count = 0;
    char *colsCopy = strdup(selectedCols);
    if (!colsCopy) {
        perror("Falha ao alocar memória");
        return 0;
    }

    char *start = colsCopy;
    char *end = strchr(start, ',');
    while (start) {
        if (end) {
            *end = '\0';
        }

        columns[count] = (char *)malloc(strlen(start) + 1);
        if (!columns[count]) {
            perror("Falha ao alocar memória para coluna");
            free(colsCopy);
            return count;
        }
        strcpy(columns[count], start);
        count++;

        start = end ? end + 1 : NULL;
        end = start ? strchr(start, ',') : NULL;
    }

    free(colsCopy);
    return count;
}

int applyFilter(char **row, Filter *filters, int filterCount, char **headers, int headerCount) {
    for (int i = 0; i < filterCount; i++) {
        int colIndex = -1;

        for (int j = 0; j < headerCount; j++) {
            if (strcmp(headers[j], filters[i].header) == 0) {
                colIndex = j;
                break;
            }
        }

        if (colIndex < 0) {
            fprintf(stderr, "Coluna especificada no filtro inválida.");
            exit(EXIT_FAILURE);
        }

        char *cellValue = row[colIndex];
        if (!cellValue) {
            fprintf(stderr, "Valor nulo na linha do índice de coluna %d\n", colIndex);
            return 0;
        }

        if (!compare(cellValue, filters[i].value, filters[i].comparator)) {
            return 0;
        }
    }
    return 1;
}

void filterAndPrintCsv(char *csv, char **selectedColumns, int selectedColumnCount, Filter *filters, int filterCount) {
    char *lines[MAX_LINE_LENGTH];
    int lineCount = 0;

    char *line = strtok(csv, "\n");
    while (line) {
        lines[lineCount++] = line;
        line = strtok(NULL, "\n");
    }

    char *headers[MAX_COLUMNS];
    int headerCount = 0;
    char *headerLine = strdup(lines[0]);
    char *header = strtok(headerLine, ",");
    while (header) {
        headers[headerCount++] = header;
        header = strtok(NULL, ",");
    }

    if (selectedColumnCount == -1) {
        selectedColumnCount = headerCount;
        for (int i = 0; i < headerCount; i++) {
            selectedColumns[i] = headers[i];
        }
    }

    for (int i = 0; i < selectedColumnCount; i++) {
        int exists = 0;
        for (int j = 0; j < headerCount; j++) {
            if (strcmp(selectedColumns[i], headers[j]) == 0) {
                exists = 1;
                break;
            }
        }
        if (!exists) {
            fprintf(stderr, "A coluna selecionada '%s' não existe no CSV.\n", selectedColumns[i]);
            free(headerLine);
            exit(EXIT_FAILURE);
        }
    }

    int selectedIndices[MAX_COLUMNS];
    int selectedColumnsOrder[MAX_COLUMNS];
    memset(selectedColumnsOrder, -1, sizeof(selectedColumnsOrder));

    for (int i = 0; i < selectedColumnCount; i++) {
        for (int j = 0; j < headerCount; j++) {
            if (strcmp(selectedColumns[i], headers[j]) == 0) {
                selectedIndices[i] = j;
                selectedColumnsOrder[j] = i;
                break;
            }
        }
    }

    for (int i = 0; i < headerCount; i++) {
        if (selectedColumnsOrder[i] != -1) {
            printf("%s", headers[i]);
            if (i < headerCount - 1) {
                printf(",");
            }
        }
    }
    printf("\n");

    for (int i = 1; i < lineCount; i++) {
        char *row[MAX_COLUMNS];
        int colIndex = 0;
        char *token = strtok(lines[i], ",");
        while (token) {
            row[colIndex++] = token;
            token = strtok(NULL, ",");
        }

        if (applyFilter(row, filters, filterCount, headers, headerCount)) {
            for (int j = 0; j < headerCount; j++) {
                if (selectedColumnsOrder[j] != -1) {
                    printf("%s", row[j]);
                    if (j < headerCount - 1) {
                        printf(",");
                    }
                }
            }
            printf("\n");
        }
    }

    free(headerLine);
}

int compare(const char *a, const char *b, char comparator) {
    if (!a || !b) {
        fprintf(stderr, "Uma das strings é nula.\n");
        return 0;
    }

    int cmp = strcmp(a, b);
    switch (comparator) {
        case '>': return cmp > 0;
        case '<': return cmp < 0;
        case '=': return cmp == 0;
        default:
            fprintf(stderr, "Comparador inválido '%c'.\n", comparator);
            return 0;
    }
}

void processCsvFile(const char csvFilePath[], const char selectedColumns[], const char filterDefs[]) {
    FILE *file = fopen(csvFilePath, "r");
    if (!file) {
        perror("Falha ao abrir arquivo");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *csvData = (char *)malloc(fileSize + 1);
    if (!csvData) {
        perror("Falha ao alocar memória");
        fclose(file);
        return;
    }

    fread(csvData, 1, fileSize, file);
    csvData[fileSize] = '\0';
    fclose(file);

    processCsv(csvData, selectedColumns, filterDefs);
    free(csvData);
}