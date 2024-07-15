#include "libcsv.h"

int main() {
    const char *csvFilePath = "data.csv";
    const char *selectedColumns = "header3,header1";
    const char *rowFilterDefinitions = "header1=1\nheader1=4\nheader2>3\nheader3>4";

    processCsv("header1,header2,header3\n1,2,3\n4,5,6\n7,8,9", selectedColumns, rowFilterDefinitions);
    processCsvFile(csvFilePath, selectedColumns, rowFilterDefinitions);

    return 0;
}