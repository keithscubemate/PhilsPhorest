#include "Forest.h"
#include "Scaler.h"
#include "Tree.h"
#include <stdexcept>

using json = nlohmann::json;
using namespace std;

int Forest::predict(const std::vector<double>& features) const {
    tuple<double, double> class_votes = make_tuple(0.0, 0.0);

    if (static_cast<int>(features.size()) != this->n_features) {
        throw std::invalid_argument(
            "Feature vector size " + to_string(features.size()) +
            " does not match expected size " + to_string(this->n_features)
        );
    }

    for (auto& tree: this->trees) {
        auto vote = tree.predict(features);

        get<0>(class_votes) += get<0>(vote);
        get<1>(class_votes) += get<1>(vote);
    }

    auto no_votes = get<0>(class_votes);
    auto yes_votes = get<1>(class_votes);

    if (yes_votes >= no_votes) {
        return this->classes[1];
    } else {
        return this->classes[0];
    }
}

Forest Forest::from_json(const json& d_info) {
    Forest forest;

    forest.n_classes  = d_info["model"]["n_classes"];
    forest.n_features = d_info["model"]["n_features"];

    for (const auto& tree_json : d_info["model"]["trees"]) {
        forest.trees.push_back(tree_json.get<Tree>());
    }

    return forest;
}
