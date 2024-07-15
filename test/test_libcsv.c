#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "libcsv.h"

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

void test_processCsv(void) {
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header1,header3";
    const char filterDefs[] = "header1=4\nheader2>3";

    FILE *output = freopen("output_processCsv.txt", "w", stdout);
    if (!output) {
        CU_FAIL("Failed to redirect stdout");
        return;
    }

    processCsv(csv, selectedColumns, filterDefs);

    freopen("/dev/tty", "w", stdout);
    fclose(output);

    FILE *result = fopen("output_processCsv.txt", "r");
    if (!result) {
        CU_FAIL("Failed to open output file");
        return;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), result); // Headers
    CU_ASSERT_STRING_EQUAL(buffer, "header1,header3\n");

    fgets(buffer, sizeof(buffer), result); // First data row
    CU_ASSERT_STRING_EQUAL(buffer, "4,6\n");

    fclose(result);
}

void test_processCsvFile(void) {
    const char csvFilePath[] = "test.csv";
    const char selectedColumns[] = "header1,header3";
    const char filterDefs[] = "header1=4\nheader2>3";

    FILE *csvFile = fopen(csvFilePath, "w");
    fprintf(csvFile, "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9");
    fclose(csvFile);

    FILE *output = freopen("output_processCsvFile.txt", "w", stdout);
    if (!output) {
        CU_FAIL("Failed to redirect stdout");
        return;
    }

    processCsvFile(csvFilePath, selectedColumns, filterDefs);

    freopen("/dev/tty", "w", stdout);
    fclose(output);

    FILE *result = fopen("output_processCsvFile.txt", "r");
    if (!result) {
        CU_FAIL("Failed to open output file");
        return;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), result); // Headers
    CU_ASSERT_STRING_EQUAL(buffer, "header1,header3\n");

    fgets(buffer, sizeof(buffer), result); // First data row
    CU_ASSERT_STRING_EQUAL(buffer, "4,6\n");

    fclose(result);
}

void test_processCsv_ignore_quotes(void) {
    const char csv[] = "hea\"der1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "hea\"der1,header3";
    const char filterDefs[] = "hea\"der1=4\nheader2>3";

    FILE *output = freopen("output_processCsv_ignore_quotes.txt", "w", stdout);
    if (!output) {
        CU_FAIL("Failed to redirect stdout");
        return;
    }

    processCsv(csv, selectedColumns, filterDefs);

    freopen("/dev/tty", "w", stdout);
    fclose(output);

    FILE *result = fopen("output_processCsv_ignore_quotes.txt", "r");
    if (!result) {
        CU_FAIL("Failed to open output file");
        return;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "hea\"der1,header3\n");

    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "4,6\n");

    fclose(result);
}

void test_processCsv_selectedColumns_arbitrary_order(void) {
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header3,header1";
    const char filterDefs[] = "";

    FILE *output = freopen("output_processCsv_selectedColumns_arbitrary_order.txt", "w", stdout);
    if (!output) {
        CU_FAIL("Failed to redirect stdout");
        return;
    }

    processCsv(csv, selectedColumns, filterDefs);

    freopen("/dev/tty", "w", stdout);
    fclose(output);

    FILE *result = fopen("output_processCsv_selectedColumns_arbitrary_order.txt", "r");
    if (!result) {
        CU_FAIL("Failed to open output file");
        return;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "header1,header3\n");

    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "1,3\n");
    
    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "4,6\n");
    
    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "7,9\n");

    fclose(result);
}

void test_processCsv_selectedFilters_arbitrary_order(void) {
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header1,header3";
    const char filterDefs[] = "header3>3\nheader1=4";

    FILE *output = freopen("output_processCsv_selectedFilters_arbitrary_order.txt", "w", stdout);
    if (!output) {
        CU_FAIL("Failed to redirect stdout");
        return;
    }

    processCsv(csv, selectedColumns, filterDefs);

    freopen("/dev/tty", "w", stdout);
    fclose(output);

    FILE *result = fopen("output_processCsv_selectedFilters_arbitrary_order.txt", "r");
    if (!result) {
        CU_FAIL("Failed to open output file");
        return;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "header1,header3\n");

    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "4,6\n");

    fclose(result);
}

void test_processCsv_accepts_more_than_one_filter_per_column(void) {
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "";
    const char filterDefs[] = "header1=1\nheader1=4\nheader2>3\nheader3>4";

    FILE *output = freopen("output_processCsv_accepts_more_than_one_filter_per_column.txt", "w", stdout);
    if (!output) {
        CU_FAIL("Failed to redirect stdout");
        return;
    }

    processCsv(csv, selectedColumns, filterDefs);

    freopen("/dev/tty", "w", stdout);
    fclose(output);

    FILE *result = fopen("output_processCsv_accepts_more_than_one_filter_per_column.txt", "r");
    if (!result) {
        CU_FAIL("Failed to open output file");
        return;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "header1,header2,header3\n");

    fgets(buffer, sizeof(buffer), result);
    CU_ASSERT_STRING_EQUAL(buffer, "4,5,6\n");

    fclose(result);
}

int main() {
    CU_pSuite pSuite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    pSuite = CU_add_suite("Suite de Testes para libcsv", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(pSuite, "Teste de processCsv", test_processCsv)) ||
        (NULL == CU_add_test(pSuite, "Teste de processCsvFile", test_processCsvFile)) ||
        (NULL == CU_add_test(pSuite, "Teste ignorando aspas", test_processCsv_ignore_quotes)) ||
        (NULL == CU_add_test(pSuite, "Teste de seleção de colunas em ordem arbitrária", test_processCsv_selectedColumns_arbitrary_order)) ||
        (NULL == CU_add_test(pSuite, "Teste de seleção de filtros em ordem arbitrária", test_processCsv_selectedFilters_arbitrary_order)) ||
        (NULL == CU_add_test(pSuite, "Teste de aceitação de mais de um filtro por coluna", test_processCsv_accepts_more_than_one_filter_per_column))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}