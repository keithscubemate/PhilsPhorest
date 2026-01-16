#include "Forest.h"
#include "Scaler.h"
#include "Tree.h"

using json = nlohmann::json;
using namespace std;

int Forest::predict(const std::vector<double>& features) {
    tuple<double, double> class_votes = make_tuple(0.0, 0.0);

    // TODO(austin.jones): mayber convert this to a result throw?
    if (static_cast<int>(features.size()) != this->n_features) {
        printf("features vec wrong size\n");
        throw;
    }

    for (auto& tree: this->trees) {
        auto vote = tree.predict(features);

        get<0>(class_votes) += get<0>(vote);
        get<1>(class_votes) += get<1>(vote);
    }

    auto no_votes = get<0>(class_votes);
    auto yes_votes = get<1>(class_votes);

    if (yes_votes >= no_votes) {
        return 1;
    } else {
        return 0;
    }
}

Forest Forest::from_json(const json& d_info) {
    Forest forest;

    forest.n_classes  = d_info["model"]["n_classes"];
    forest.n_features = d_info["model"]["n_features"];

    auto forest_size = d_info["model"]["trees"].size();

    for (size_t i = 0; i < forest_size; i++) {
        auto tree = d_info.at("model").at("trees").at(i).get<Tree>();
        forest.trees.push_back(tree);
    }

    return forest;
}
