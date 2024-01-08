#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <sstream>

class StatData {
public:
    double sum = 0.0;
    int count = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    double sumOfSquares = 0.0; // For variance calculation

    void update(double value) {
        sum += value;
        sumOfSquares += value * value;
        count++;
        if (value < min) min = value;
        if (value > max) max = value;
    }

    double mean() const {
        return count == 0 ? 0 : sum / count;
    }

    double variance() const {
        if (count < 2) return 0;
        double meanVal = mean();
        return (sumOfSquares - 2 * meanVal * sum + count * meanVal * meanVal) / (count - 1);
    }
};


double normalize(double x, double min, double max) {
    return (x - min) / (max - min);
}


void parseLine(const std::string& line, StatData& salesData, StatData& tempData, StatData& thiData, StatData& staffData) {
    std::stringstream ss(line);
    std::string token;

    std::getline(ss, token, ','); // Skip the date

    // Sales data
    if (std::getline(ss, token, ',') && !token.empty()) {
        salesData.update(std::stod(token));
    }

    // Skip weather since it's categorical and doesn't need statistics
    std::getline(ss, token, ',');

    // Temperature data
    if (std::getline(ss, token, ',') && !token.empty()) {
        tempData.update(std::stod(token));
    }

    // THI data
    if (std::getline(ss, token, ',') && !token.empty()) {
        thiData.update(std::stod(token));
    }

    // Staff data
    if (std::getline(ss, token, ',') && !token.empty()) {
        staffData.update(std::stoi(token));
    }
}

// Modify the function signature to accept double values directly
void replaceMissingValuesWithMeanAndNormalize(std::string& line, double salesMean, double tempMean, double thiMean, double staffMean, double tempMin, double tempMax, double thiMin, double thiMax, double staffMin, double staffMax) {
    std::stringstream ss(line), normalizedSS;
    std::string token;
    std::string newLine;
    std::getline(ss, token, ','); // Get date
    normalizedSS << token << ","; // Start with the date

    // Process other fields
    for (int i = 0; i < 5; ++i) {
        if (!std::getline(ss, token, ',')) {
            std::cerr << "Error: Line format incorrect." << std::endl;
            break;
        }

        if (token.empty()) { // Replace missing values with means
            switch (i) {
                case 0:
                    token = std::to_string(salesMean);
                    break;
                case 1:
                    token = "1"; // Assuming '1' for sunny weather for missing values
                    break;
                case 2:
                    token = std::to_string(tempMean);
                    break;
                case 3:
                    token = std::to_string(thiMean);
                    break;
                case 4:
                    token = std::to_string(staffMean);
                    break;
            }
        }

        if (i == 2 || i == 3 || i == 4) { // Normalize temperature, THI, and staff number
            double value = std::stod(token);
            switch (i) {
                case 2: // Normalize temperature
                    token = std::to_string(normalize(value, tempMin, tempMax));
                    break;
                case 3: // Normalize THI
                    token = std::to_string(normalize(value, thiMin, thiMax));
                    break;
                case 4: // Normalize staff number
                    token = std::to_string(normalize(value, staffMin, staffMax));
                    break;
            }
        }

        normalizedSS << token; // Append the processed token
        if (i < 4) normalizedSS << ","; // Add comma except for the last field
    }

    line = normalizedSS.str(); // Replace the old line with the new line with missing values filled and normalized
}



int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <step> <path_to_csv>" << std::endl;
        return -1;
    }

    std::string step = argv[1];


    if (step == "step1") {
        // Call Step 1 processing function
        std::ifstream file(argv[2]);

        if (!file.is_open()) {
            std::cerr << "Error opening file" << std::endl;
            return -1;
        }

        std::string line;
        StatData salesData, tempData, thiData, staffData;

        std::getline(file, line); // Read the header line

        while (std::getline(file, line)) {
            parseLine(line, salesData, tempData, thiData, staffData);
        }
        std::cout.precision(10); // Set precision to 10 digits after the decimal
        std::cout << salesData.mean() << ", " << salesData.variance() << ", "
                << salesData.min << ", " << salesData.max << std::endl;
        std::cout << tempData.mean() << ", " << tempData.variance() << ", "
                << tempData.min << ", " << tempData.max << std::endl;
        std::cout << thiData.mean() << ", " << thiData.variance() << ", "
                << thiData.min << ", " << thiData.max << std::endl;
        std::cout << staffData.mean() << ", " << staffData.variance() << ", "
                << staffData.min << ", " << staffData.max << std::endl;

                // After calculating statistics, write means and min/max to files
        std::ofstream meansFile("means.txt"), minMaxFile("minmax.txt");
        if (!meansFile.is_open() || !minMaxFile.is_open()) {
            std::cerr << "Error: Could not open file for writing means/minmax." << std::endl;
            return -1;
        }

        meansFile.precision(10);
        minMaxFile.precision(10);

        // Write means
        meansFile << salesData.mean() << " "
                << tempData.mean() << " "
                << thiData.mean() << " "
                << staffData.mean() << std::endl;

        // Write min and max values
        minMaxFile << tempData.min << " "
                << tempData.max << " "
                << thiData.min << " "
                << thiData.max << " "
                << staffData.min << " "
                << staffData.max << std::endl;

        meansFile.close();
        minMaxFile.close();

    } else if (step == "step2") {

        double salesMean, tempMean, thiMean, staffMean;
        double tempMin, tempMax, thiMin, thiMax, staffMin, staffMax;

        std::ifstream meansFile("means.txt"), minMaxFile("minmax.txt");
        if (!meansFile.is_open() || !minMaxFile.is_open()) {
            std::cerr << "Error: Could not open means or minmax file for reading." << std::endl;
            return -1;
        }

        // Read mean values
        meansFile >> salesMean >> tempMean >> thiMean >> staffMean;
        meansFile.close();

        // Read min and max values
        minMaxFile >> tempMin >> tempMax >> thiMin >> thiMax >> staffMin >> staffMax;
        minMaxFile.close();

        std::ifstream file(argv[2]);
        if (!file.is_open()) {
            std::cerr << "Error opening file for step 2" << std::endl;
            return -1;
        }
        std::string line;
        getline(file, line); // Skip the header

            while (getline(file, line)) {
        // Use the replaceMissingValuesWithMean function to replace missing values with the means
        replaceMissingValuesWithMeanAndNormalize(line, salesMean, tempMean, thiMean, staffMean, tempMin, tempMax, thiMin, thiMax, staffMin, staffMax);

        // Output the processed data
        std::cout << line << std::endl;
    }

    } else if (step == "step3") {

    } else {
        std::cerr << "Invalid step" << std::endl;
        return 1;
    }


    return 0;
}

