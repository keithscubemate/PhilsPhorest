#include "Scaler.h"

void Scaler::transform(FeatureArray& data) const {
    for (size_t i = 0; i < N_FEATURES; i++) {
        data[i] = (data[i] - this->mean[i]) / this->scale[i];
    }
}
