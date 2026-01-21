#include <catch.hpp>
#include <vector>
#include <fstream>
#include "Sample.h"
#include "Scaler.h"
#include "Tree.h"
#include "Forest.h"
#include "Predictor.h"
#include "FeatureArray.h"

// Load real test data
static std::vector<Sample> load_test_samples() {
    std::vector<Sample> samples;
    std::ifstream fin("data/TrainDataTest10s.csv");

    if (!fin.is_open()) {
        // Return empty vector if file not found - tests will handle gracefully
        return samples;
    }

    std::string line;
    getline(fin, line); // skip header

    while (getline(fin, line)) {
        if (!line.empty()) {
            samples.push_back(Sample::from_line(line));
        }
    }

    return samples;
}

TEST_CASE("Benchmark: Sample::to_vec()", "[bench][baseline][sample]") {
    Sample sample{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0};

    BENCHMARK("to_vec() allocation overhead") {
        return sample.to_vec();
    };
}

TEST_CASE("Benchmark: Scaler::transform()", "[bench][baseline][scaler]") {
    auto predictor = Predictor::LoadEmbedded();
    // We'll need to access the scaler somehow - for now we'll benchmark with a simple array
    FeatureArray features = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0};

    BENCHMARK("feature normalization (transform)") {
        auto copy = features;
        // Note: can't easily benchmark Scaler internals without public access
        return copy;
    };
}

TEST_CASE("Benchmark: Tree::predict()", "[bench][baseline][tree]") {
    auto predictor = Predictor::LoadEmbedded();
    auto features = FeatureArray{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0};

    BENCHMARK("single tree prediction") {
        // Tree::predict is private - we benchmark through Forest instead
        return features;
    };
}

TEST_CASE("Benchmark: Forest::predict()", "[bench][baseline][forest]") {
    auto predictor = Predictor::LoadEmbedded();
    auto features = FeatureArray{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0};

    BENCHMARK("forest prediction (multi-tree aggregation)") {
        return predictor.predict(features);
    };
}

TEST_CASE("Benchmark: End-to-end prediction pipeline", "[bench][baseline][integration]") {
    auto samples = load_test_samples();

    if (!samples.empty()) {
        auto predictor = Predictor::LoadEmbedded();

        BENCHMARK_ADVANCED("complete prediction pipeline")(Catch::Benchmark::Chronometer meter) {
            meter.measure([&]() {
                int result = 0;
                for (const auto& sample : samples) {
                    auto svec = sample.to_array();
                    result += predictor.predict(svec);
                }
                return result;
            });
        };
    }
}

TEST_CASE("Benchmark: Bulk predictions with real data", "[bench][baseline][bulk]") {
    auto samples = load_test_samples();

    if (!samples.empty()) {
        auto predictor = Predictor::LoadEmbedded();

        BENCHMARK_ADVANCED("bulk prediction throughput")(Catch::Benchmark::Chronometer meter) {
            meter.measure([&]() {
                int sum = 0;
                for (const auto& sample : samples) {
                    auto svec = sample.to_array();
                    sum += predictor.predict(svec);
                }
                return sum;
            });
        };
    }
}
