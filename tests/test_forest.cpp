#include <catch.hpp>
#include "../include/Forest.h"
#include "test_helpers.hpp"

TEST_CASE("Forest returns 0 when all trees vote for class 0", "[forest][voting]") {
    Forest forest = create_unanimous_class0_forest();

    std::vector<double> features(13, 0.0);
    int prediction = forest.predict(features);

    REQUIRE(prediction == 0);
}

TEST_CASE("Forest returns 1 when all trees vote for class 1", "[forest][voting]") {
    Forest forest = create_unanimous_class1_forest();

    std::vector<double> features(13, 0.0);
    int prediction = forest.predict(features);

    REQUIRE(prediction == 1);
}

TEST_CASE("Forest breaks ties in favor of class 1", "[forest][tiebreak]") {
    Forest forest = create_tie_forest();

    // All nodes vote (100, 100), so total is (200, 200) - exactly tied
    std::vector<double> features(13, 0.0);
    int prediction = forest.predict(features);

    // yes_votes >= no_votes means ties go to class 1
    REQUIRE(prediction == 1);
}

TEST_CASE("Forest throws on incorrect feature count - too few", "[forest][validation]") {
    Forest forest = create_single_tree_forest();

    std::vector<double> features(10, 0.0);  // Only 10 features, needs 13

    REQUIRE_THROWS_AS(forest.predict(features), std::invalid_argument);
}

TEST_CASE("Forest throws on incorrect feature count - too many", "[forest][validation]") {
    Forest forest = create_single_tree_forest();

    std::vector<double> features(15, 0.0);  // 15 features, needs 13

    REQUIRE_THROWS_AS(forest.predict(features), std::invalid_argument);
}

TEST_CASE("Forest accepts correct feature count", "[forest][validation]") {
    Forest forest = create_single_tree_forest();

    std::vector<double> features(13, 0.0);  // Exactly 13 features

    int prediction = forest.predict(features);
    REQUIRE((prediction == 0 || prediction == 1));
}

TEST_CASE("Forest deterministic predictions", "[forest][behavior]") {
    Forest forest = create_single_tree_forest();

    std::vector<double> features = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0};

    int prediction1 = forest.predict(features);
    int prediction2 = forest.predict(features);

    REQUIRE(prediction1 == prediction2);
}

TEST_CASE("Forest negative feature values", "[forest][values]") {
    Forest forest = create_single_tree_forest();

    std::vector<double> features = {-10.0, -20.0, -30.0, -40.0, -50.0, -60.0, -70.0, -80.0, -90.0, -100.0, -110.0, -120.0, -130.0};
    int prediction = forest.predict(features);

    REQUIRE((prediction == 0 || prediction == 1));
}

TEST_CASE("Forest large feature values", "[forest][values]") {
    Forest forest = create_single_tree_forest();

    std::vector<double> features = {1e6, 1e6, 1e6, 1e6, 1e6, 1e6, 1e6, 1e6, 1e6, 1e6, 1e6, 1e6, 1e6};
    int prediction = forest.predict(features);

    REQUIRE((prediction == 0 || prediction == 1));
}
