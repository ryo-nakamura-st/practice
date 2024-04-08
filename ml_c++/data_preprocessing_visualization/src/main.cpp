#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include "matplotlibcpp.h"

enum IrisClass {
    Iris_setosa,     // 0
    Iris_versicolor, // 1
    Iris_virginica,  // 2
    Iris_unknown     // 3
};

// use template to accomadate multiple types(Int, double, fraction)
template <typename T>
double Mean(const std::vector<T>& data)
{
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

template <typename T>
double StDev(const std::vector<T>& data){
    double mean = Mean(data);
    /*
    std::inner_product -> usually it's used for dot product calculation, meaning different vector
    but this case, it's for square calculation, using same data, data.begin for first and third arg
    */
    auto sum_sq = std::inner_product(data.begin(), data.end(), data.begin(), 0.0, std::plus<>(),
    [mean](const T &a, const T &b){ return (a - mean) * (b - mean); });
    return std::sqrt(sum_sq / data.size());
}

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
            temp_iris_class.push_back(iris_class_f);
        }
        Iris_Dataset.push_back(temp_sepal_len);
        Iris_Dataset.push_back(temp_sepal_wid);
        Iris_Dataset.push_back(temp_petal_len);
        Iris_Dataset.push_back(temp_petal_wid);
        Iris_Dataset.push_back(temp_iris_class);
    }
    else
    {
        std::cout << "Unable to open file";
    }
    return Iris_Dataset;
}


template <typename T>
std::vector<std::vector<T>> vector_Transpose(std::vector<std::vector<T>>& input_vector)
{
    if(input_vector.size()>0){
        std::vector<std::vector<T>> output_vector(input_vector[0].size(), std::vector<T>(input_vector.size()));
        for (int i = 0; i < input_vector.size(); i++){
            for (int j = 0; j < input_vector[0].size(); j++)
            {
                output_vector[j][i] = input_vector[i][j];
            }
        }
        return output_vector;
    }
    return input_vector;
}

std::vector<std::vector<float>> split_by_class(std::vector<std::vector<float>>& dataset, float data_class)
{
    std::vector<std::vector<float>> temp_out;
    std::vector<std::vector<float>> out_vector;

    int counter = 0;
    std::for_each(dataset[4].begin(), dataset[4].end(), [&](float &item_class)
                  {
                    if(item_class == data_class){
                        std::vector<float> temp;
                        temp.push_back(dataset[0][counter]);
                        temp.push_back(dataset[1][counter]);
                        temp.push_back(dataset[2][counter]);
                        temp.push_back(dataset[3][counter]);
                        temp.push_back(dataset[4][counter]);
                        temp_out.push_back(temp);
                    }
                    counter++; });
    out_vector = vector_Transpose(temp_out);
    return out_vector;
}

namespace plt = matplotlibcpp;
int main() {
    std::vector<std::vector<float>> dataset =  Read_Iris_Dataset();
    std::vector<std::vector<float>> Iris_setosa_vec = split_by_class(dataset, Iris_setosa);
    std::replace_if(
        dataset[0].begin(), dataset[0].end(), [](float &value)
        { return value >= 5.8; },
        5.8);

    // std::cout << dataset[0] << std::endl;

    double sepal_length_mean = Mean(dataset[0]);
    double sepal_length_stdev = StDev(dataset[0]);
    // easy way to make the seed, out of current time
    std::for_each(dataset[0].begin(), dataset[0].end(), [&](float &x)
                  { x = (x - sepal_length_mean) / sepal_length_stdev; });

    std::cout<< "Before transpose, Data Size was ( " << dataset.size() << " , "  <<dataset[0].size()<<" )" <<std::endl;
    std::random_device rd;
    std::mt19937 g(rd());

    dataset = vector_Transpose(dataset);
    std::shuffle(dataset.begin(), dataset.end(),g);
    std::cout<< "After transpose, Data Size is ( " << dataset.size() << " , "  <<dataset[0].size()<<" )" <<std::endl;

    plt::plot(dataset[0], {{"label", "sepal_length"}});
    plt::plot(dataset[1],{ {"label", "sepal_width"}});
    plt::plot(dataset[2],{ {"label", "petal_length"}});
    plt::plot(dataset[3],{  {"label", "petal_width"}});
    plt::title("Standard usage");
    plt::legend();
    plt::show();
}