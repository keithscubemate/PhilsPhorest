#include "Scaler.h"

using json = nlohmann::json;

void Scaler::transform(std::vector<double>& data, int num_features) {
    // TODO(austin.jones): mayber convert this to a result throw?
    if (data.size() != static_cast<size_t>(num_features)) {
        printf("data didn't match scaler\n");
        throw;
    }

    for (auto i = 0; i < num_features; i++) {
        data[i] = (data[i] - this->mean[i]) / this->scale[i];
    }
}
