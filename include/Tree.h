#pragma once
#include <vector>

#include "json.hpp"

class Tree
{
    std::vector<int> feature;
    std::vector<double> threshold;
    std::vector<int> children_left;
    std::vector<int> children_right;
    std::vector<std::tuple<double, double>> value;
    std::vector<int> n_node_samples;

    friend void to_json(nlohmann::json& j, const Tree& t);
    friend void from_json(const nlohmann::json& j, Tree& t);
public:
    std::tuple<double, double> predict(const std::vector<double>& features) const;
};
