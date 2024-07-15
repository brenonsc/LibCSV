#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libcsv.h"

#define MAX_COLUMNS 256
#define MAX_LINE_LENGTH 1024
#define MAX_FILTERS_PER_COLUMN 10

typedef struct {
    char *header;
    int filterCount;
    struct {
        char comparator;
        char nextComparator;
        char *value;
    } filters[MAX_FILTERS_PER_COLUMN];
} FilterGroup;


void parseFilters(const char *filterDefs, FilterGroup *filterGroups, int *filterGroupCount);
int parseSelectedColumns(const char *selectedCols, char **columns);
void filterAndPrintCsv(char *csv, char **selectedColumns, int selectedColumnCount, FilterGroup *filterGroups, int filterGroupCount);
int applyFilter(char **row, FilterGroup *filterGroups, int filterGroupCount, char **headers, int headerCount);
int compare(const char *a, const char *b, char comparator, char nextComparator);

void processCsv(const char csv[], const char selectedColumns[], const char filterDefs[]) {
    char *csvCopy = strdup(csv);
    if (!csvCopy) {
        perror("Falha ao alocar memória");
        return;
    }

    FilterGroup filterGroups[MAX_COLUMNS];
    int filterGroupCount = 0;

    if (filterDefs && *filterDefs != '\0') {
        parseFilters(filterDefs, filterGroups, &filterGroupCount);
    }

    char *selectedColumnsArray[MAX_COLUMNS];
    int selectedColumnCount = parseSelectedColumns(selectedColumns, selectedColumnsArray);
    filterAndPrintCsv(csvCopy, selectedColumnsArray, selectedColumnCount, filterGroups, filterGroupCount);

    free(csvCopy);
}

void parseFilters(const char *filterDefs, FilterGroup *filterGroups, int *filterGroupCount) {
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

        char *headerEnd = strpbrk(currentPos, "><=!");
        if (!headerEnd) {
            fprintf(stderr, "Filtro inválido: '%s'\n", currentPos);
            currentPos = nextLine ? nextLine + 1 : NULL;
            nextLine = currentPos ? strchr(currentPos, '\n') : NULL;
            continue;
        }

        char comparator = *headerEnd;
        char nextComparator = (headerEnd[1] == '=' ? '=' : '\0');

        size_t headerLen = headerEnd - currentPos;
        char *header = (char *)malloc(headerLen + 1);
        if (!header) {
            perror("Falha ao alocar memória para o cabeçalho");
            break;
        }
        strncpy(header, currentPos, headerLen);
        header[headerLen] = '\0';

        char *value = headerEnd + (nextComparator == '=' ? 2 : 1);
        if (!value || *value == '\0') {
            fprintf(stderr, "Filtro inválido: '%s'\n", currentPos);
            free(header);
            currentPos = nextLine ? nextLine + 1 : NULL;
            nextLine = currentPos ? strchr(currentPos, '\n') : NULL;
            continue;
        }

        int found = 0;
        for (int i = 0; i < *filterGroupCount; i++) {
            if (strcmp(filterGroups[i].header, header) == 0) {
                filterGroups[i].filters[filterGroups[i].filterCount].comparator = comparator;
                filterGroups[i].filters[filterGroups[i].filterCount].nextComparator = nextComparator;
                filterGroups[i].filters[filterGroups[i].filterCount].value = strdup(value);
                filterGroups[i].filterCount++;
                found = 1;
                break;
            }
        }

        if (!found) {
            filterGroups[*filterGroupCount].header = header;
            filterGroups[*filterGroupCount].filters[0].comparator = comparator;
            filterGroups[*filterGroupCount].filters[0].nextComparator = nextComparator;
            filterGroups[*filterGroupCount].filters[0].value = strdup(value);
            filterGroups[*filterGroupCount].filterCount = 1;
            (*filterGroupCount)++;
        }

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

int applyFilter(char **row, FilterGroup *filterGroups, int filterGroupCount, char **headers, int headerCount) {
    for (int i = 0; i < filterGroupCount; i++) {
        int colIndex = -1;

        for (int j = 0; j < headerCount; j++) {
            if (strcmp(headers[j], filterGroups[i].header) == 0) {
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

        int match = 0;
        for (int k = 0; k < filterGroups[i].filterCount; k++) {
            char comparator = filterGroups[i].filters[k].comparator;
            char nextComparator = filterGroups[i].filters[k].nextComparator;
            if (compare(cellValue, filterGroups[i].filters[k].value, comparator, nextComparator)) {
                match = 1;
                break;
            }
        }

        if (!match) {
            return 0;
        }
    }
    return 1;
}

void filterAndPrintCsv(char *csv, char **selectedColumns, int selectedColumnCount, FilterGroup *filterGroups, int filterGroupCount) {
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

    for (int i = 0; i < headerCount; i++) {
        for (int j = 0; j < selectedColumnCount; j++) {
            if (strcmp(headers[i], selectedColumns[j]) == 0) {
                selectedIndices[j] = i;
                selectedColumnsOrder[i] = j;
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

        if (applyFilter(row, filterGroups, filterGroupCount, headers, headerCount)) {
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

int compare(const char *a, const char *b, char comparator, char nextComparator) {
    int comparison;
    
    double valA, valB;
    int numeric = sscanf(a, "%lf", &valA) == 1 && sscanf(b, "%lf", &valB) == 1;
    
    if (numeric) {
        comparison = (valA > valB) - (valA < valB);
    } else {
        comparison = strcmp(a, b);
    }
    
    switch (comparator) {
        case '=':
            return comparison == 0;
        case '>':
            if (nextComparator == '=') {
                return comparison >= 0;
            }
            return comparison > 0;
        case '<':
            if (nextComparator == '=') {
                return comparison <= 0;
            }
            return comparison < 0;
        case '!':
            if (nextComparator == '=') {
                return comparison != 0;
            }
            return 0;
        default:
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