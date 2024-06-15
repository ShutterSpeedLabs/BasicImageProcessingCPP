#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

struct YoloDataset {
    std::string train_path;
    std::string val_path;
    std::string test_path;
    std::vector<std::string> class_names;
    int num_classes;
};

YoloDataset readYoloDataYaml(const std::string& file_path) {
    YAML::Node config = YAML::LoadFile(file_path);
    
    YoloDataset dataset;
    dataset.train_path = config["train"].as<std::string>();
    dataset.val_path = config["val"].as<std::string>();
    
    if (config["test"]) {
        dataset.test_path = config["test"].as<std::string>();
    } else {
        dataset.test_path = "";
    }
    
    if (config["names"]) {
        for (const auto& name : config["names"]) {
            dataset.class_names.push_back(name.as<std::string>());
        }
    }
    
    if (config["nc"]) {
        dataset.num_classes = config["nc"].as<int>();
    } else {
        dataset.num_classes = dataset.class_names.size();
    }

    return dataset;
}

int main() {
    std::string file_path = "/media/parashuram/dataset/YOLOv8_/datasets/data.yaml";
    YoloDataset dataset = readYoloDataYaml(file_path);
    
    std::cout << "Training data path: " << dataset.train_path << std::endl;
    std::cout << "Validation data path: " << dataset.val_path << std::endl;
    std::cout << "Testing data path: " << dataset.test_path << std::endl;
    std::cout << "Number of classes: " << dataset.num_classes << std::endl;
    std::cout << "Class names: " << std::endl;
    for (const auto& name : dataset.class_names) {
        std::cout << " - " << name << std::endl;
    }
    
    return 0;
}
