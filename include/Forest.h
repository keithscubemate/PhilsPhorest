#pragma once
#include <vector>

#include "Scaler.h"
#include "Tree.h"
#include "json.hpp"

class Forest
{
    int n_estimators;
    int n_classes;
    int n_features;
    std::vector<int> classes;
    std::vector<Tree> trees;
public:
    int predict(const std::vector<double>& features);
    static Forest from_json(const nlohmann::json& d_info);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Forest, n_estimators, n_features, n_classes, classes, trees)
};
