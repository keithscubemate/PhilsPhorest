#include "Forest.h"
#include "Scaler.h"
#include "Tree.h"
#include <stdexcept>

using json = nlohmann::json;
using namespace std;

int Forest::predict(const vector<double>& features) const {
    tuple<double, double> class_votes = make_tuple(0.0, 0.0);

    if (static_cast<int>(features.size()) != this->n_features) {
        throw invalid_argument(
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
