#include "Scaler.h"
#include <stdexcept>

using json = nlohmann::json;

void Scaler::transform(std::vector<double>& data, int num_features) {
    if (data.size() != static_cast<size_t>(num_features)) {
        throw std::invalid_argument(
            "Data vector size " + std::to_string(data.size()) +
            " does not match expected size " + std::to_string(num_features)
        );
    }

    for (auto i = 0; i < num_features; i++) {
        data[i] = (data[i] - this->mean[i]) / this->scale[i];
    }
}
