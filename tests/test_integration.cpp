#include <catch.hpp>
#include <fstream>
#include <json.hpp>
#include "../include/Sample.h"
#include "../include/Scaler.h"
#include "../include/Forest.h"

using json = nlohmann::json;

// Helper function to load CSV samples (same as main.cpp)
std::vector<Sample> csv_to_samples(std::ifstream& fin) {
    std::string line = "";
    std::vector<Sample> rv = {};

    getline(fin, line);  // Skip header

    while(getline(fin, line)) {
        auto sample = Sample::from_line(line);
        rv.push_back(sample);
    }

    return rv;
}

TEST_CASE("Complete prediction pipeline", "[integration][pipeline]") {
    // Create test data
    json scaler_json = {
        {"scale", std::vector<double>(13, 1.0)},
        {"mean", std::vector<double>(13, 0.0)}
    };

    json tree_json = {
        {"feature", {0, -2, -2}},
        {"threshold", {5.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{10.0, 5.0}}, {{2.0, 15.0}}}},
        {"n_node_samples", {100, 60, 40}}
    };

    json forest_json = {
        {"n_estimators", 1},
        {"n_features", 13},
        {"n_classes", 2},
        {"classes", {0, 1}},
        {"trees", {tree_json}}
    };

    // Load scaler and forest
    Scaler scaler = scaler_json.get<Scaler>();
    Forest forest = forest_json.get<Forest>();

    // Create test sample
    Sample sample;
    sample.Nep_Tb = 3.0;
    sample.Nep_TOF = 0.0;
    sample.NepSumArray = 0.0;
    sample.NepPeakArray = 0.0;
    sample.NepDArray = 0.0;
    sample.YE_TOF = 0.0;
    sample.YE_Size = 0.0;
    sample.YE_Mean = 0.0;
    sample.YE_Median = 0.0;
    sample.YE_V = 0.0;
    sample.YE_Te = 0.0;
    sample.YE_Tc = 0.0;
    sample.AF = 0.0;

    // Transform features
    std::vector<double> features = sample.to_vec();
    scaler.transform(features, 13);

    // Predict
    int prediction = forest.predict(features);

    // Verify result is valid
    REQUIRE(prediction == 0);
}

TEST_CASE("CSV batch processing", "[integration][csv]") {
    // Create temporary CSV file
    std::ofstream temp_csv("tests/fixtures/test_samples_temp.csv");
    temp_csv << "Nep_index,YE,Nep_Tb,Nep_TOF,NepSumArray,NepPeakArray,NepDArray,YE_TOF,YE_Size,YE_Mean,YE_Median,YE_V,YE_Te,YE_Tc,AF\n";
    temp_csv << "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14\n";
    temp_csv << "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15\n";
    temp_csv << "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16\n";
    temp_csv.close();

    // Read samples
    std::ifstream csv_file("tests/fixtures/test_samples_temp.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();

    REQUIRE(samples.size() == 3);

    // Verify first sample
    REQUIRE(samples[0].Nep_index == Approx(0.0));
    REQUIRE(samples[0].YE == Approx(1.0));
    REQUIRE(samples[0].AF == Approx(14.0));

    // Verify second sample
    REQUIRE(samples[1].Nep_index == Approx(1.0));
    REQUIRE(samples[1].YE == Approx(2.0));

    // Verify third sample
    REQUIRE(samples[2].Nep_index == Approx(2.0));
    REQUIRE(samples[2].AF == Approx(16.0));

    // Clean up
    std::remove("tests/fixtures/test_samples_temp.csv");
}

TEST_CASE("Forest with multiple trees aggregation", "[integration][voting]") {
    // Create forest with 3 trees
    json tree1 = {
        {"feature", {0, -2, -2}},
        {"threshold", {5.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{100.0, 0.0}}, {{0.0, 100.0}}}},
        {"n_node_samples", {100, 50, 50}}
    };

    json tree2 = {
        {"feature", {0, -2, -2}},
        {"threshold", {5.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{100.0, 0.0}}, {{0.0, 100.0}}}},
        {"n_node_samples", {100, 50, 50}}
    };

    json tree3 = {
        {"feature", {0, -2, -2}},
        {"threshold", {5.0, 0.0, 0.0}},
        {"children_left", {1, -1, -1}},
        {"children_right", {2, -1, -1}},
        {"value", {{{0.0, 0.0}}, {{100.0, 0.0}}, {{0.0, 100.0}}}},
        {"n_node_samples", {100, 50, 50}}
    };

    json forest_json = {
        {"n_estimators", 3},
        {"n_features", 13},
        {"n_classes", 2},
        {"classes", {0, 1}},
        {"trees", {tree1, tree2, tree3}}
    };

    Forest forest = forest_json.get<Forest>();

    std::vector<double> features(13, 3.0);  // All features = 3.0, so 3.0 < 5.0 -> goes left
    int prediction = forest.predict(features);

    // All trees vote for class 0, so result should be 0
    REQUIRE(prediction == 0);
}
