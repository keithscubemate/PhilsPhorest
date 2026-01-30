#pragma once
#include <vector>
#include <array>

#include "json.hpp"
#include "Sample.h"

class Scaler {
private:
    std::vector<double> scale;
    std::vector<double> mean;
public:
    void transform(FeatureArray& data) const;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Scaler, scale, mean)
};
