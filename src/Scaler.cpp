#include "Scaler.h"
#include <stdexcept>

using namespace std;

void Scaler::transform(vector<double>& data, int num_features) const {
    if (data.size() != static_cast<size_t>(num_features)) {
        throw invalid_argument(
            "Data vector size " + to_string(data.size()) +
            " does not match expected size " + to_string(num_features)
        );
    }

    for (auto i = 0; i < num_features; i++) {
        data[i] = (data[i] - this->mean[i]) / this->scale[i];
    }
}
