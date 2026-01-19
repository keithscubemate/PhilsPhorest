#pragma once

#include <vector>
#include <string>
#include "json.hpp"
#include "Scaler.h"
#include "Forest.h"

class Predictor
{
private:
    Scaler scaler;
    Forest forest;

public:
    Predictor() = default;

    int predict(std::vector<double>& features) const;
    static Predictor LoadEmbedded();
    friend void to_json(nlohmann::json& j, const Predictor& p);
    friend void from_json(const nlohmann::json& j, Predictor& p);
};
