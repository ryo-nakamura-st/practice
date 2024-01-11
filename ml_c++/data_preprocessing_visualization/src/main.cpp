#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include "matplotlibcpp.h"

enum IrisClass {
    Iris_setosa,     // 0
    Iris_versicolor, // 1
    Iris_virginica,  // 2
    Iris_unknown     // 3
};

std::vector<std::vector<float>> Read_Iris_Dataset(void)
{
    std::ifstream myfile("../data/in/iris.data");
    std::string line;
    std::vector<std::vector<float>> Iris_Dataset;
    std::vector<float> temp_sepal_len;
    std::vector<float> temp_sepal_wid;
    std::vector<float> temp_petal_len;
    std::vector<float> temp_petal_wid;
    std::vector<float> temp_iris_class;

    float sepal_len_f, sepal_wid_f, petal_len_f, petal_wid_f;
    float iris_class_f;

    std::string temp_string;
    int count = 0;
    if (myfile.is_open())
    {
        std::cout << "file opened successfully" << std::endl;
        while (std::getline(myfile, line))
        {
            std::replace(line.begin(), line.end(), '-', '_');
            std::replace(line.begin(), line.end(), ',', ' ');

            std::istringstream iss(line);
            count++;
            iss >> sepal_len_f >> sepal_wid_f >> petal_len_f >> petal_wid_f >> temp_string;

            temp_sepal_len.push_back(sepal_len_f);
            temp_sepal_wid.push_back(sepal_wid_f);
            temp_petal_len.push_back(petal_len_f);
            temp_petal_wid.push_back(petal_wid_f);
            if(temp_string.compare("Iris_setosa")==0){
                iris_class_f = Iris_setosa;
            }
            else if(temp_string.compare("Iris_versicolor")==0){
                iris_class_f = Iris_versicolor;
            }
            else if(temp_string.compare("Iris_virginica")==0){
                iris_class_f = Iris_virginica;
            }else
            {
                iris_class_f = Iris_unknown;
            }
        }
        Iris_Dataset.push_back(temp_sepal_len);
        Iris_Dataset.push_back(temp_sepal_wid);
        Iris_Dataset.push_back(temp_petal_len);
        Iris_Dataset.push_back(temp_petal_wid);
    }else
    {
        std::cout << "Unable to open file";
    }
    return Iris_Dataset;
}

namespace plt = matplotlibcpp;
int main() {
    std::vector<std::vector<float>> dataset =  Read_Iris_Dataset();
    std::replace_if(
        dataset[0].begin(), dataset[0].end(), [](float &value)
        { return value >= 5.8; },
        5.8);
    plt::plot(dataset[0], {{"label", "sepal_length"}});
    plt::plot(dataset[1],{ {"label", "sepal_width"}});
    plt::plot(dataset[2],{ {"label", "petal_length"}});
    plt::plot(dataset[3],{  {"label", "petal_width"}});
    plt::title("Standard usage");
    plt::legend();
    plt::show();
}