#include <catch.hpp>
#include <cmath>
#include "../include/Tree.h"
#include "../include/FeatureArray.h"
#include "test_helpers.hpp"

TEST_CASE("Tree selects left branch when feature <= threshold", "[tree][traversal]") {
    Tree tree = create_simple_tree();

    FeatureArray features = {3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto result = tree.predict(features);

    // Verify left child value returned
    REQUIRE(std::get<0>(result) == Approx(10.0));
    REQUIRE(std::get<1>(result) == Approx(5.0));
}

TEST_CASE("Tree selects right branch when feature > threshold", "[tree][traversal]") {
    Tree tree = create_simple_tree();

    FeatureArray features = {7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto result = tree.predict(features);

    // Verify right child value returned
    REQUIRE(std::get<0>(result) == Approx(2.0));
    REQUIRE(std::get<1>(result) == Approx(15.0));
}

TEST_CASE("Tree applies epsilon tolerance for threshold comparison", "[tree][epsilon]") {
    Tree tree = create_simple_tree();

    SECTION("Exact threshold match goes left") {
        FeatureArray features = {5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        auto result = tree.predict(features);
        REQUIRE(std::get<0>(result) == Approx(10.0));  // Left branch
    }

    SECTION("Within epsilon tolerance goes left") {
        FeatureArray features = {5.0 + 5e-6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        auto result = tree.predict(features);
        REQUIRE(std::get<0>(result) == Approx(10.0));  // Left branch
    }

    SECTION("Outside epsilon tolerance goes right") {
        FeatureArray features = {5.0 + 2e-5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        auto result = tree.predict(features);
        REQUIRE(std::get<0>(result) == Approx(2.0));  // Right branch
    }
}

TEST_CASE("Tree traverses multiple levels correctly", "[tree][traversal]") {
    Tree tree = create_multilevel_tree();

    // Test path: feature[2] (15.0) > 10.0 -> right child (leaf)
    FeatureArray features = {100.0, 200.0, 15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto result = tree.predict(features);

    // Should reach right leaf with value (5.0, 30.0)
    REQUIRE(std::get<0>(result) == Approx(5.0));
    REQUIRE(std::get<1>(result) == Approx(30.0));
}

TEST_CASE("Tree multilevel traversal - left then right", "[tree][traversal]") {
    Tree tree = create_multilevel_tree();

    // Test path: feature[2] (5.0) <= 10.0 -> left child, then feature[0] (100.0) > 50.0 -> right leaf
    FeatureArray features = {100.0, 200.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto result = tree.predict(features);

    // Should reach right leaf of left subtree with value (15.0, 20.0)
    REQUIRE(std::get<0>(result) == Approx(15.0));
    REQUIRE(std::get<1>(result) == Approx(20.0));
}

TEST_CASE("Tree multilevel traversal - left then left", "[tree][traversal]") {
    Tree tree = create_multilevel_tree();

    // Test path: feature[2] (5.0) <= 10.0 -> left child, then feature[0] (30.0) <= 50.0 -> left leaf
    FeatureArray features = {30.0, 200.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto result = tree.predict(features);

    // Should reach left leaf of left subtree with value (25.0, 5.0)
    REQUIRE(std::get<0>(result) == Approx(25.0));
    REQUIRE(std::get<1>(result) == Approx(5.0));
}

TEST_CASE("Tree uses correct feature index at each node", "[tree][features]") {
    Tree tree = create_feature_indexed_tree();

    // Root splits on feature[2] at 15.0, left child splits on feature[0] at 100.0
    // features[2] = 15.0, exactly at root threshold -> left
    // features[0] = 50.0, <= 100.0 at second level -> left
    FeatureArray features = {50.0, 200.0, 15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto result = tree.predict(features);

    REQUIRE(std::get<0>(result) == Approx(30.0));
    REQUIRE(std::get<1>(result) == Approx(10.0));
}

TEST_CASE("Tree deterministic predictions", "[tree][behavior]") {
    Tree tree = create_simple_tree();
    FeatureArray features = {3.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    auto result1 = tree.predict(features);
    auto result2 = tree.predict(features);

    REQUIRE(std::get<0>(result1) == std::get<0>(result2));
    REQUIRE(std::get<1>(result1) == std::get<1>(result2));
}

TEST_CASE("Tree negative feature values", "[tree][values]") {
    Tree tree = create_simple_tree();

    FeatureArray features = {-10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto result = tree.predict(features);

    // -10.0 <= 5.0 -> left branch
    REQUIRE(std::get<0>(result) == Approx(10.0));
    REQUIRE(std::get<1>(result) == Approx(5.0));
}

TEST_CASE("Tree large feature values", "[tree][values]") {
    Tree tree = create_simple_tree();

    FeatureArray features = {1000000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto result = tree.predict(features);

    // 1000000.0 > 5.0 -> right branch
    REQUIRE(std::get<0>(result) == Approx(2.0));
    REQUIRE(std::get<1>(result) == Approx(15.0));
}

TEST_CASE("Tree returns tuple with both classes", "[tree][structure]") {
    Tree tree = create_simple_tree();

    FeatureArray features = {3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    auto result = tree.predict(features);

    // Result should be a tuple with both class votes
    double class0_votes = std::get<0>(result);
    double class1_votes = std::get<1>(result);

    REQUIRE(class0_votes >= 0.0);
    REQUIRE(class1_votes >= 0.0);
}
