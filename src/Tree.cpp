#include <cmath>

#include "Tree.h"

using namespace std;

tuple<double, double> Tree::predict(const std::vector<double>& features) const {
    auto node = 0;

    while (this->children_left[node] != -1) {
        const auto feature = this->feature[node];
        const auto sample = features[feature];

        const auto threshold = this->threshold[node];

        if (sample <= threshold || abs(sample - threshold) < 1e-5) {
            node = this->children_left[node];
        } else {
            node = this->children_right[node];
        }
    }

    tuple<double, double> value = this->value[node];

    return value;
}

void to_json(nlohmann::json& j, const Tree& t) {
    vector<vector<vector<double>>> value;

    for (auto tup: t.value) {
        vector<vector<double>> vv = { { get<0>(tup), get<1>(tup) } };
        value.push_back(vv);
    }


    j = nlohmann::json{
        {"feature", t.feature},
        {"threshold", t.threshold},
        {"children_left", t.children_left},
        {"children_right", t.children_right},
        {"value", value},
        {"n_node_samples", t.n_node_samples}
    };
}

void from_json(const nlohmann::json& j, Tree& t) {
    j.at("feature").get_to(t.feature);
    j.at("threshold").get_to(t.threshold);
    j.at("children_left").get_to(t.children_left);
    j.at("children_right").get_to(t.children_right);
    j.at("n_node_samples").get_to(t.n_node_samples);

    auto value = j
        .at("value")
        .get<vector<vector<vector<double>>>>();

    for (auto vv: value) {
        tuple<double, double> tup = make_tuple(vv[0][0], vv[0][1]);
        t.value.push_back(tup);
    }
}
