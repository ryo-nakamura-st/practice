#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

typedef struct {
    double sum;
    int count;
    double min;
    double max;
    double sumOfSquares; // For variance calculation
} StatData;

StatData initStatData() {
    StatData data;
    data.sum = 0.0;
    data.count = 0;
    data.min = DBL_MAX;
    data.max = DBL_MIN;
    data.sumOfSquares = 0.0;
    return data;
}

void updateStatData(StatData *data, double value) {
    data->sum += value;
    data->sumOfSquares += value * value; // For variance
    data->count++;
    if (value < data->min) data->min = value;
    if (value > data->max) data->max = value;
}

void parseLine(char *line, StatData *salesData, StatData *tempData, StatData *thiData, StatData *staffData) {
    const char *delimiter = ",";
    char *token;
    int column = 0;
    double value;

    token = strtok(line, delimiter);
    while (token != NULL) {
        if (strcmp(token, "") != 0) { // Check if the token is not an empty string
            value = atof(token);
            switch (column) {
                case 1: updateStatData(salesData, value); break;
                case 3: updateStatData(tempData, value); break;
                case 4: updateStatData(thiData, value); break;
                case 5: updateStatData(staffData, value); break;
            }
        }
        column++;
        token = strtok(NULL, delimiter);
    }
}

double calculateMean(StatData data) {
    if (data.count == 0) return 0;
    return data.sum / data.count;
}

double calculateVariance(StatData data) {
    if (data.count < 2) return 0;
    double mean = calculateMean(data);
    return (data.sumOfSquares - 2 * mean * data.sum + data.count * mean * mean) / (data.count - 1);
}

void replaceMissingWithMean(StatData *data) {
    if (data->count == 0) {
        data->sum = data->max = data->min = calculateMean(*data);
        data->count = 1; // To avoid division by zero later
    }
}

double normalize(double value, double min, double max) {
    if (max - min == 0) return 0; // Avoid division by zero
    return (value - min) / (max - min);
}

int main(int argc, char *argv[]) {
    FILE *file;
    char buffer[1024];
    StatData salesData = initStatData(), tempData = initStatData(), 
             thiData = initStatData(), staffData = initStatData();
    int isFirstLine = 1;

    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), file)) {
        if (isFirstLine) {
            isFirstLine = 0; // Skip header
            continue;
        }
        parseLine(buffer, &salesData, &tempData, &thiData, &staffData);
    }

    fclose(file);

    // Output statistics for each column
    printf("Sales: Mean = %f, Variance = %f, Min = %f, Max = %f\n",
           calculateMean(salesData), calculateVariance(salesData), salesData.min, salesData.max);
    printf("Temperature: Mean = %f, Variance = %f, Min = %f, Max = %f\n",
           calculateMean(tempData), calculateVariance(tempData), tempData.min, tempData.max);
    // Similarly for THI and Staff

    return 0;
}


// int main(int argc, char *argv[]) {
//   // start from 1 to ignore script name; argv[0] will be a name of processing file.
//   for (int i = 1; i < argc; i++) {
//     printf ("argv[%i]: %s\n", i, argv[i]);
//   }
//   return 0;
// }
