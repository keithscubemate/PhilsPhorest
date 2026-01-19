#include "Predictor.h"

int Predictor::predict(std::vector<double>& features) const {
    this->scaler.transform(features, this->forest.get_n_features());

    return forest.predict(features);
}

void from_json(const nlohmann::json& data, Predictor& p) {
    p.scaler = data.at("scaler").get<Scaler>();
    p.forest = data.at("model").get<Forest>();
}
