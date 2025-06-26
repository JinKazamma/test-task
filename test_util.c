#include "statdata.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

const StatData case_1_in_a[2] = {
    {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3},
    {.id = 90089, .count = 1, .cost = 88.90, .primary = 1, .mode = 0}
};

const StatData case_1_in_b[2] = {
    {.id = 90089, .count = 13, .cost = 0.011, .primary = 0, .mode = 2},
    {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2}
};

const StatData case_1_out[3] =  
{{.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2 }, 
{.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3 }, 
{.id = 90089, .count = 14, .cost = 88.911, .primary = 0, .mode = 2 }};

void CreateTestFiles() {
    printf("Creating test files...\n");
    StoreDump("file1.bin", case_1_in_a, 2);
    StoreDump("file2.bin", case_1_in_b, 2);
}

int main() {

    CreateTestFiles();
    
    printf("Running main utility...\n");

    struct timeval start, end;
    gettimeofday(&start, NULL);

    system("./statdata_util file1.bin file2.bin output.bin");

    size_t count;
    StatData* result = LoadDump("output.bin", &count);
    
    for (size_t i = 0; i < count; i++)
    {
        if (result[i].id == case_1_out[i].id &&
            result[i].count == case_1_out[i].count &&
            result[i].cost == case_1_out[i].cost &&
            result[i].primary == case_1_out[i].primary &&
            result[i].mode == case_1_out[i].mode)
        {
            if (i == count-1)
            {
                printf("\ntest passed, result:\n");
                PrintStatData(result,count,count);
                PrintStatData(case_1_out,count,count);
                free(result);
                gettimeofday(&end, NULL);
                double time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
                time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6;
                printf("Test execution time: %.6f seconds\n", time_taken);
            }
        }
        else
        {
            printf("Error: results are not equal\n");
            free(result);
            break;
        }
    }

    return 0;
}