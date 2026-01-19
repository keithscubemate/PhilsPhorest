#include <catch.hpp>
#include <fstream>
#include <json.hpp>
#include <filesystem>
#include "../include/Sample.h"
#include "../include/Scaler.h"
#include "../include/Forest.h"
#include "../include/Predictor.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

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

// Integration tests for loading real data files from data folder
TEST_CASE("Load TrainDataTest10s.csv from data folder", "[integration][data_folder]") {
    std::string data_file = "data/TrainDataTest10s.csv";

    // Verify file exists
    REQUIRE(fs::exists(data_file));

    // Load CSV samples
    std::ifstream csv_file(data_file);
    REQUIRE(csv_file.is_open());

    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();

    // Frozen value: TrainDataTest10s.csv contains 397 samples
    REQUIRE(samples.size() == 397);

    // Verify samples have valid data (no NaN or inf)
    for (const auto& sample : samples) {
        REQUIRE(!std::isnan(sample.Nep_Tb));
        REQUIRE(!std::isinf(sample.Nep_Tb));
        REQUIRE(!std::isnan(sample.AF));
        REQUIRE(!std::isinf(sample.AF));
    }
}

TEST_CASE("Make predictions on TrainDataTest10s.csv", "[integration][data_folder][predictions]") {
    std::string data_file = "data/TrainDataTest10s.csv";

    // Load samples
    std::ifstream csv_file(data_file);
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();

    REQUIRE(samples.size() == 397);

    // Load embedded predictor
    Predictor predictor = Predictor::LoadEmbedded();

    // Make predictions on first 100 samples
    int predictions_count = 0;
    int class_0_count = 0;
    int class_1_count = 0;

    for (size_t i = 0; i < std::min((size_t)100, samples.size()); ++i) {
        auto features = samples[i].to_vec();
        int prediction = predictor.predict(features);

        REQUIRE((prediction == 0 || prediction == 1));
        predictions_count++;

        if (prediction == 0) class_0_count++;
        else class_1_count++;
    }

    // Frozen values: predictions on first 100 samples
    REQUIRE(predictions_count == 100);
    REQUIRE(class_0_count == 78);
    REQUIRE(class_1_count == 22);
}

// Individual tests for each Test_full CSV file with frozen sample and prediction counts
TEST_CASE("Predictions on Test_full_12_5_Alpha1_T3_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_5_Alpha1_T3_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 9);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 4);
    REQUIRE(class_1 == 5);
}

TEST_CASE("Predictions on Test_full_12_2_Alpha5_T3_rep2_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_2_Alpha5_T3_rep2_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 116);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 102);
    REQUIRE(class_1 == 14);
}

TEST_CASE("Predictions on Test_full_12_5_Alpha1_T1_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_5_Alpha1_T1_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 24);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 16);
    REQUIRE(class_1 == 8);
}

TEST_CASE("Predictions on Test_full_12_5_Alpha1_lowYE_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_5_Alpha1_lowYE_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 3);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 0);
    REQUIRE(class_1 == 3);
}

TEST_CASE("Predictions on Test_full_12_5_Alpha1_mediumYE_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_5_Alpha1_mediumYE_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 65);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 12);
    REQUIRE(class_1 == 53);
}

TEST_CASE("Predictions on Test_full_12_2_Alpha5_T2_rep2_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_2_Alpha5_T2_rep2_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 12);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 9);
    REQUIRE(class_1 == 3);
}

TEST_CASE("Predictions on Test_full_12_2_Alpha5_mediumYE_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_2_Alpha5_mediumYE_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 95);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 23);
    REQUIRE(class_1 == 72);
}

TEST_CASE("Predictions on Test_full_12_2_Alpha5_T1_rep1_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_2_Alpha5_T1_rep1_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 159);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 138);
    REQUIRE(class_1 == 21);
}

TEST_CASE("Predictions on Test_full_12_5_Alpha1_highYE_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_5_Alpha1_highYE_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 171);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 4);
    REQUIRE(class_1 == 167);
}

TEST_CASE("Predictions on Test_full_12_2_Alpha5_lowYE_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_2_Alpha5_lowYE_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 30);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 12);
    REQUIRE(class_1 == 18);
}

TEST_CASE("Predictions on Test_full_12_2_Alpha5_highYE_sorted.csv", "[integration][data_folder][test_full]") {
    std::ifstream csv_file("data/Test_full_12_2_Alpha5_highYE_sorted.csv");
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();
    REQUIRE(samples.size() == 228);

    Predictor predictor = Predictor::LoadEmbedded();
    int class_0 = 0, class_1 = 0;
    for (const auto& sample : samples) {
        auto features = sample.to_vec();
        int pred = predictor.predict(features);
        if (pred == 0) class_0++;
        else class_1++;
    }
    REQUIRE(class_0 == 52);
    REQUIRE(class_1 == 176);
}

TEST_CASE("End-to-end batch prediction pipeline with real data", "[integration][data_folder][pipeline]") {
    std::string data_file = "data/TrainDataTest10s.csv";

    // Load CSV
    std::ifstream csv_file(data_file);
    std::vector<Sample> samples = csv_to_samples(csv_file);
    csv_file.close();

    REQUIRE(samples.size() == 397);

    // Load predictor
    Predictor predictor = Predictor::LoadEmbedded();

    // Process all samples and aggregate predictions
    int he = 0;  // Count of class 1 predictions (same as main.cpp)
    for (const auto& sample : samples) {
        auto svec = sample.to_vec();
        he += predictor.predict(svec);
    }

    // Frozen values: predictions on all 397 samples from TrainDataTest10s.csv
    REQUIRE(he == 178);                    // 178 class 1 predictions
    REQUIRE((int)samples.size() - he == 219);  // 219 class 0 predictions
}
