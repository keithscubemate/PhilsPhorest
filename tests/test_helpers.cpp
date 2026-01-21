#include "test_helpers.hpp"
#include <json.hpp>
#include "../include/FeatureArray.h"

using json = nlohmann::json;
using namespace std;

// Create simple 2-node tree for basic traversal testing
// Root node (index 0) splits on feature[0] at threshold 5.0
// Left child (index 1): value = (10.0, 5.0) - leaf
// Right child (index 2): value = (2.0, 15.0) - leaf
Tree create_simple_tree() {
    json tree_json = {
        {"feature", {0, -2, -2}},
        {"threshold", {5.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{10.0, 5.0}}, {{2.0, 15.0}}}},
        {"n_node_samples", {100, 60, 40}}
    };

    Tree tree;
    from_json(tree_json, tree);
    return tree;
}

// Create 3-level tree for multi-level traversal testing
// Root (0) splits on feature[2] at threshold 10.0
// Left branch (1) splits on feature[0] at threshold 50.0
//   Left leaf (3): (25.0, 5.0)
//   Right leaf (4): (15.0, 20.0)
// Right branch (2) is a leaf: (5.0, 30.0)
Tree create_multilevel_tree() {
    json tree_json = {
        {"feature", {2, 0, -2, -2, -2}},
        {"threshold", {10.0, 50.0, 0.0, 0.0, 0.0}},
        {"children_left", {1, 3, -1, -1, -1}},
        {"children_right", {2, 4, -1, -1, -1}},
        {"value", {
            {{0.0, 0.0}},
            {{0.0, 0.0}},
            {{5.0, 30.0}},
            {{25.0, 5.0}},
            {{15.0, 20.0}}
        }},
        {"n_node_samples", {100, 60, 40, 30, 30}}
    };

    Tree tree;
    from_json(tree_json, tree);
    return tree;
}

// Create tree where different feature indices are used at each level
// Root splits on feature[2], left child splits on feature[0]
Tree create_feature_indexed_tree() {
    json tree_json = {
        {"feature", {2, 0, -2, -2, -2}},
        {"threshold", {15.0, 100.0, 0.0, 0.0, 0.0}},
        {"children_left", {1, 3, -1, -1, -1}},
        {"children_right", {2, 4, -1, -1, -1}},
        {"value", {
            {{0.0, 0.0}},
            {{0.0, 0.0}},
            {{5.0, 25.0}},
            {{30.0, 10.0}},
            {{10.0, 15.0}}
        }},
        {"n_node_samples", {100, 70, 30, 50, 20}}
    };

    Tree tree;
    from_json(tree_json, tree);
    return tree;
}

// Create a forest with a single tree for basic testing
Forest create_single_tree_forest() {
    json forest_json = {
        {"n_estimators", 1},
        {"n_features", 13},
        {"n_classes", 2},
        {"classes", {0, 1}},
        {"trees", {
            {
                {"feature", {0, -2, -2}},
                {"threshold", {5.0, 0.0, 0.0}},
                {"children_left", {1, -1, -1}},
                {"children_right", {2, -1, -1}},
                {"value", {{{0.0, 0.0}}, {{50.0, 10.0}}, {{10.0, 50.0}}}},
                {"n_node_samples", {100, 60, 40}}
            }
        }}
    };

    Forest forest;
    forest = forest_json.get<Forest>();
    return forest;
}

// Create a forest with a single tree where all votes are for class 0
Forest create_unanimous_class0_forest() {
    json forest_json = {
        {"n_estimators", 1},
        {"n_features", 13},
        {"n_classes", 2},
        {"classes", {0, 1}},
        {"trees", {
            {
                {"feature", {0, -2, -2}},
                {"threshold", {5.0, 0.0, 0.0}},
                {"children_left", {1, -1, -1}},
                {"children_right", {2, -1, -1}},
                {"value", {{{0.0, 0.0}}, {{100.0, 0.0}}, {{50.0, 0.0}}}},
                {"n_node_samples", {150, 100, 50}}
            }
        }}
    };

    Forest forest;
    forest = forest_json.get<Forest>();
    return forest;
}

// Create a forest with a single tree where all votes are for class 1
Forest create_unanimous_class1_forest() {
    json forest_json = {
        {"n_estimators", 1},
        {"n_features", 13},
        {"n_classes", 2},
        {"classes", {0, 1}},
        {"trees", {
            {
                {"feature", {0, -2, -2}},
                {"threshold", {5.0, 0.0, 0.0}},
                {"children_left", {1, -1, -1}},
                {"children_right", {2, -1, -1}},
                {"value", {{{0.0, 0.0}}, {{0.0, 100.0}}, {{0.0, 50.0}}}},
                {"n_node_samples", {150, 100, 50}}
            }
        }}
    };

    Forest forest;
    forest = forest_json.get<Forest>();
    return forest;
}

// Create a forest where majority voting determines the result
// 3 trees, 2 vote for class 0, 1 votes for class 1
Forest create_majority_vote_forest() {
    json tree1 = {
        {"feature", {0, -2, -2}},
        {"threshold", {5.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{50.0, 25.0}}, {{0.0, 75.0}}}},
        {"n_node_samples", {100, 50, 50}}
    };

    json tree2 = {
        {"feature", {1, -2, -2}},
        {"threshold", {10.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{60.0, 20.0}}, {{0.0, 80.0}}}},
        {"n_node_samples", {100, 50, 50}}
    };

    json tree3 = {
        {"feature", {2, -2, -2}},
        {"threshold", {15.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{0.0, 90.0}}, {{0.0, 60.0}}}},
        {"n_node_samples", {100, 50, 50}}
    };

    json forest_json = {
        {"n_estimators", 3},
        {"n_features", 13},
        {"n_classes", 2},
        {"classes", {0, 1}},
        {"trees", {tree1, tree2, tree3}}
    };

    Forest forest;
    forest = forest_json.get<Forest>();
    return forest;
}

// Create a forest where votes are exactly tied
// 2 trees with equal votes for both classes
Forest create_tie_forest() {
    json tree1 = {
        {"feature", {0, -2, -2}},
        {"threshold", {5.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{100.0, 100.0}}, {{100.0, 100.0}}}},
        {"n_node_samples", {200, 100, 100}}
    };

    json tree2 = {
        {"feature", {1, -2, -2}},
        {"threshold", {10.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{100.0, 100.0}}, {{100.0, 100.0}}}},
        {"n_node_samples", {200, 100, 100}}
    };

    json forest_json = {
        {"n_estimators", 2},
        {"n_features", 13},
        {"n_classes", 2},
        {"classes", {0, 1}},
        {"trees", {tree1, tree2}}
    };

    Forest forest;
    forest = forest_json.get<Forest>();
    return forest;
}

// Create a simple scaler with known transformation values
Scaler create_test_scaler() {
    json scaler_json = {
        {"scale", std::vector<double>(13, 1.0)},
        {"mean", std::vector<double>(13, 0.0)}
    };
    return scaler_json.get<Scaler>();
}

// Create a scaler with zero mean
Scaler create_zero_mean_scaler() {
    json scaler_json = {
        {"scale", std::vector<double>(13, 1.0)},
        {"mean", std::vector<double>(13, 0.0)}
    };
    return scaler_json.get<Scaler>();
}
