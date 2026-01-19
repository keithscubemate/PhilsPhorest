#pragma once
#include <vector>

#include "json.hpp"

class Scaler {
private:
    std::vector<double> scale;
    std::vector<double> mean;
public:
    void transform(std::vector<double>& data, int num_features) const;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Scaler, scale, mean)
};
