#include "libcsv.h"

int main() {
    const char *csvFilePath = "data.csv";
    const char *selectedColumns = "header1,header3,header4";
    const char *rowFilterDefinitions = "header1>1\nheader3<10";

    processCsvFile(csvFilePath, selectedColumns, rowFilterDefinitions);

    return 0;
}