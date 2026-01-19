#include "Predictor.h"
#include "model_data.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int Predictor::predict(vector<double>& features) const {
    this->scaler.transform(features, this->forest.get_n_features());

    return forest.predict(features);
}

void from_json(const nlohmann::json& data, Predictor& p) {
    p.scaler = data.at("scaler").get<Scaler>();
    p.forest = data.at("model").get<Forest>();
}

Predictor Predictor::LoadEmbedded() {
    json data = json::parse(string(MODEL_DATA));

    return data.get<Predictor>();
}
