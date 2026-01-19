#pragma once
#include <vector>

#include "Forest.h"
#include "Scaler.h"
#include "json.hpp"

class Predictor {
    Forest forest;
    Scaler scaler;

    friend void from_json(const nlohmann::json& j, Predictor& p);
public:
    int predict(std::vector<double>& features) const;
};
