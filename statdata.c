#include "statdata.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int CompareById(const void* a, const void* b)
{
    long id_a = ((const StatData*)a)->id;
    long id_b = ((const StatData*)b)->id;
    if (id_a < id_b) return -1;
    if (id_a > id_b) return 1;
    return 0;
}

static int CompareByCost(const void* a, const void* b)
{
    float cost_a = ((const StatData*)a)->cost;
    float cost_b = ((const StatData*)b)->cost;
    if (cost_a < cost_b) return -1;
    if (cost_a > cost_b) return 1;
    return 0;
}

void StoreDump(const char* filename, const StatData* data, size_t count)
{
    if (!filename || !data)
    {
        fprintf(stderr, "Invalid arguments to StoreDump\n");
        return;
    }

    FILE* file = fopen(filename, "wb");
    if (!file)
    {
        perror("Failed to open file for writing");
        return;
    }

    size_t written = fwrite(data, sizeof(StatData), count, file);
    if (written != count)
    {
        perror("Failed to write all data");
    }

    fclose(file);
}

StatData* LoadDump(const char* filename, size_t* count)
{
    if (!filename || !count)
    {
        fprintf(stderr, "Invalid arguments to LoadDump\n");
        return NULL;
    }

    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        perror("Failed to open file for reading");
        return NULL;
    }

    // Определяем размер файла
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size % sizeof(StatData) != 0)
    {
        fprintf(stderr, "File size is not a multiple of StatData size\n");
        fclose(file);
        return NULL;
    }

    *count = file_size / sizeof(StatData);
    StatData* data = malloc(file_size);
    if (!data)
    {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    size_t read = fread(data, sizeof(StatData), *count, file);
    if (read != *count)
    {
        perror("Failed to read all data");
        free(data);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return data;
}

StatData* JoinDump(const StatData* data1, size_t count1, 
                  const StatData* data2, size_t count2, size_t* result_count)
{
    if (!data1 || !data2 || !result_count)
    {
        fprintf(stderr, "Invalid arguments to JoinDump\n");
        return NULL;
    }

    size_t total_count = count1 + count2;
    StatData* temp = malloc(total_count * sizeof(StatData));
    if (!temp)
    {
        perror("Failed to allocate memory");
        return NULL;
    }

    memcpy(temp, data1, count1 * sizeof(StatData));
    memcpy(temp + count1, data2, count2 * sizeof(StatData));

    qsort(temp, total_count, sizeof(StatData), CompareById);

    StatData* result = malloc(total_count * sizeof(StatData));
    if (!result)
    {
        perror("Failed to allocate memory");
        free(temp);
        return NULL;
    }

    size_t result_idx = 0;
    for (size_t i = 0; i < total_count; )
    {
        result[result_idx] = temp[i];
        size_t j = i + 1;

        while (j < total_count && temp[j].id == temp[i].id)
        {

            result[result_idx].count += temp[j].count;
            result[result_idx].cost += temp[j].cost;

            result[result_idx].primary = result[result_idx].primary && temp[j].primary;
            
            if (temp[j].mode > result[result_idx].mode)
            {
                result[result_idx].mode = temp[j].mode;
            }
            j++;
        }
        
        result_idx++;
        i = j;
    }

    free(temp);

    StatData* final_result = realloc(result, result_idx * sizeof(StatData));
    *result_count = result_idx;
    
    return final_result ? final_result : result;
}

void SortDump(StatData* data, size_t count)
{
    if (!data || count == 0) return;
    qsort(data, count, sizeof(StatData), CompareByCost);
}

void PrintStatData(const StatData* data, size_t count, size_t limit)
{
    if (!data || count == 0) return;
    
    if (limit > count) limit = count;
    
    printf("ID         Count      Cost            Primary  Mode\n");
    printf("----------------------------------------------------\n");
    
    for (size_t i = 0; i < limit; i++)
    {
        char mode_bin[4] = 
        {
            (data[i].mode & 4) ? '1' : '0',
            (data[i].mode & 2) ? '1' : '0',
            (data[i].mode & 1) ? '1' : '0',
            '\0'
        };
        printf("0x%-8lx %-10d %-15.3e %-8c %s\n", 
               data[i].id, 
               data[i].count, 
               data[i].cost, 
               data[i].primary ? 'y' : 'n',
               mode_bin);
    }
}

void ProcessFiles(const char* file1, const char* file2, const char* output_file)
{
    size_t count1, count2, result_count;
    
    StatData* data1 = LoadDump(file1, &count1);
    if (!data1)
    {
        fprintf(stderr, "Failed to load %s\n", file1);
        return;
    }
    
    StatData* data2 = LoadDump(file2, &count2);
    if (!data2)
    {
        fprintf(stderr, "Failed to load %s\n", file2);
        free(data1);
        return;
    }
    
    StatData* result = JoinDump(data1, count1, data2, count2, &result_count);
    if (!result)
    {
        fprintf(stderr, "Failed to join data\n");
        free(data1);
        free(data2);
        return;
    }
    
    SortDump(result, result_count);
    
    int limit = 10;
    printf("First %d records:\n",limit);
    PrintStatData(result, result_count, limit);
    
    StoreDump(output_file, result, result_count);
    
    free(data1);
    free(data2);
    free(result);
}