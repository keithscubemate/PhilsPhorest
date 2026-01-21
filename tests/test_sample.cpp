#include <catch.hpp>
#include "../include/Sample.h"
#include "../include/FeatureArray.h"

TEST_CASE("Sample parses valid CSV line", "[sample][parsing]") {
    // Order: Nep_index, YE, Nep_Tb, Nep_TOF, NepSumArray, NepPeakArray, NepDArray,
    //        YE_TOF, YE_Size, YE_Mean, YE_Median, YE_V, YE_Te, YE_Tc, AF
    std::string line = "0,2.069,38.409,40190,1474,1046.349,180,18345710,"
                       "101920.61,83937.5,0.5,2.65E+12,4.216,3.331,0.490";

    Sample sample = Sample::from_line(line);

    REQUIRE(sample.Nep_index == Approx(0.0));
    REQUIRE(sample.YE == Approx(2.069));
    REQUIRE(sample.Nep_Tb == Approx(38.409));
    REQUIRE(sample.Nep_TOF == Approx(40190.0));
    REQUIRE(sample.NepSumArray == Approx(1474.0));
    REQUIRE(sample.NepPeakArray == Approx(1046.349));
    REQUIRE(sample.NepDArray == Approx(180.0));
    REQUIRE(sample.YE_TOF == Approx(18345710.0));
    REQUIRE(sample.YE_Size == Approx(101920.61));
    REQUIRE(sample.YE_Mean == Approx(83937.5));
    REQUIRE(sample.YE_V == Approx(2.65e12));
    REQUIRE(sample.YE_Te == Approx(4.216));
    REQUIRE(sample.YE_Tc == Approx(3.331));
    REQUIRE(sample.AF == Approx(0.490));
}

TEST_CASE("Sample handles scientific notation", "[sample][parsing]") {
    // YE_V is at position 11 (0-indexed)
    std::string line = "0,0,0,0,0,0,0,0,0,0,0,2.65E+12,0,0,0";

    Sample sample = Sample::from_line(line);

    REQUIRE(sample.YE_V == Approx(2.65e12));
}

TEST_CASE("Sample handles small scientific notation", "[sample][parsing]") {
    // YE_V is at position 11 (0-indexed)
    std::string line = "0,0,0,0,0,0,0,0,0,0,0,1.5E-6,0,0,0";

    Sample sample = Sample::from_line(line);

    REQUIRE(sample.YE_V == Approx(1.5e-6));
}

TEST_CASE("Sample to_vec excludes Nep_index and YE", "[sample][conversion]") {
    Sample sample;
    sample.Nep_index = 999.0;  // Should be excluded
    sample.YE = 888.0;         // Should be excluded
    sample.Nep_Tb = 1.0;       // Should be first in vector
    sample.Nep_TOF = 2.0;
    sample.NepSumArray = 3.0;
    sample.NepPeakArray = 4.0;
    sample.NepDArray = 5.0;
    sample.YE_TOF = 6.0;
    sample.YE_Size = 7.0;
    sample.YE_Mean = 8.0;
    sample.YE_Median = 9.0;
    sample.YE_V = 10.0;
    sample.YE_Te = 11.0;
    sample.YE_Tc = 12.0;
    sample.AF = 13.0;           // Should be last in vector

    std::vector<double> vec = sample.to_vec();

    REQUIRE(vec.size() == 13);  // Not 15
    REQUIRE(vec[0] == Approx(1.0));   // Nep_Tb first
    REQUIRE(vec[1] == Approx(2.0));   // Nep_TOF second
    REQUIRE(vec[12] == Approx(13.0)); // AF last

    // Verify 999 and 888 not in vector
    for (double val : vec) {
        REQUIRE(val != Approx(999.0));
        REQUIRE(val != Approx(888.0));
    }
}

TEST_CASE("Sample to_vec returns features in correct order", "[sample][conversion]") {
    Sample sample;
    sample.Nep_Tb = 1.0;
    sample.Nep_TOF = 2.0;
    sample.NepSumArray = 3.0;
    sample.NepPeakArray = 4.0;
    sample.NepDArray = 5.0;
    sample.YE_TOF = 6.0;
    sample.YE_Size = 7.0;
    sample.YE_Mean = 8.0;
    sample.YE_Median = 9.0;
    sample.YE_V = 10.0;
    sample.YE_Te = 11.0;
    sample.YE_Tc = 12.0;
    sample.AF = 13.0;

    std::vector<double> vec = sample.to_vec();

    for (int i = 0; i < 13; i++) {
        REQUIRE(vec[i] == Approx(static_cast<double>(i + 1)));
    }
}

TEST_CASE("Sample round-trip preserves values", "[sample][roundtrip]") {
    std::string original = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15";

    Sample sample1 = Sample::from_line(original);
    std::string serialized = sample1.to_string();
    Sample sample2 = Sample::from_line(serialized);

    // Allow small precision loss due to serialization
    REQUIRE(sample1.Nep_index == Approx(sample2.Nep_index).epsilon(1e-10));
    REQUIRE(sample1.YE == Approx(sample2.YE).epsilon(1e-10));
    REQUIRE(sample1.Nep_Tb == Approx(sample2.Nep_Tb).epsilon(1e-10));
    REQUIRE(sample1.Nep_TOF == Approx(sample2.Nep_TOF).epsilon(1e-10));
    REQUIRE(sample1.AF == Approx(sample2.AF).epsilon(1e-10));
}

TEST_CASE("Sample parses zero values", "[sample][parsing]") {
    std::string line = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";

    Sample sample = Sample::from_line(line);

    REQUIRE(sample.Nep_index == Approx(0.0));
    REQUIRE(sample.AF == Approx(0.0));
}

TEST_CASE("Sample parses negative values", "[sample][parsing]") {
    std::string line = "-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15";

    Sample sample = Sample::from_line(line);

    REQUIRE(sample.Nep_index == Approx(-1.0));
    REQUIRE(sample.YE == Approx(-2.0));
    REQUIRE(sample.AF == Approx(-15.0));
}

TEST_CASE("Sample parses large values", "[sample][parsing]") {
    std::string line = "1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6";

    Sample sample = Sample::from_line(line);

    REQUIRE(sample.Nep_index == Approx(1e6));
    REQUIRE(sample.AF == Approx(1e6));
}

TEST_CASE("Sample to_string produces valid CSV format", "[sample][format]") {
    Sample sample;
    sample.Nep_index = 1.0;
    sample.YE = 2.0;
    sample.Nep_Tb = 3.0;
    sample.Nep_TOF = 4.0;
    sample.NepSumArray = 5.0;
    sample.NepPeakArray = 6.0;
    sample.NepDArray = 7.0;
    sample.YE_TOF = 8.0;
    sample.YE_Size = 9.0;
    sample.YE_Mean = 10.0;
    sample.YE_Median = 11.0;
    sample.YE_V = 12.0;
    sample.YE_Te = 13.0;
    sample.YE_Tc = 14.0;
    sample.AF = 15.0;

    std::string csv = sample.to_string();

    // String should contain commas
    size_t comma_count = std::count(csv.begin(), csv.end(), ',');
    REQUIRE(comma_count == 14);  // 15 fields = 14 commas
}

TEST_CASE("Sample deterministic parsing", "[sample][behavior]") {
    std::string line = "1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5,11.5,12.5,13.5,14.5,15.5";

    Sample sample1 = Sample::from_line(line);
    Sample sample2 = Sample::from_line(line);

    REQUIRE(sample1.Nep_index == Approx(sample2.Nep_index));
    REQUIRE(sample1.YE == Approx(sample2.YE));
    REQUIRE(sample1.AF == Approx(sample2.AF));
}

TEST_CASE("Sample to_vec size is exactly 13", "[sample][structure]") {
    Sample sample;
    sample.Nep_index = 1.0;
    sample.YE = 2.0;
    sample.Nep_Tb = 3.0;
    sample.Nep_TOF = 4.0;
    sample.NepSumArray = 5.0;
    sample.NepPeakArray = 6.0;
    sample.NepDArray = 7.0;
    sample.YE_TOF = 8.0;
    sample.YE_Size = 9.0;
    sample.YE_Mean = 10.0;
    sample.YE_Median = 11.0;
    sample.YE_V = 12.0;
    sample.YE_Te = 13.0;
    sample.YE_Tc = 14.0;
    sample.AF = 15.0;

    std::vector<double> vec = sample.to_vec();

    REQUIRE(vec.size() == 13);
}

TEST_CASE("Sample to_array returns FeatureArray with correct values", "[sample][conversion]") {
    Sample sample;
    sample.Nep_index = 999.0;  // Should be excluded
    sample.YE = 888.0;         // Should be excluded
    sample.Nep_Tb = 1.0;       // Should be first in array
    sample.Nep_TOF = 2.0;
    sample.NepSumArray = 3.0;
    sample.NepPeakArray = 4.0;
    sample.NepDArray = 5.0;
    sample.YE_TOF = 6.0;
    sample.YE_Size = 7.0;
    sample.YE_Mean = 8.0;
    sample.YE_Median = 9.0;
    sample.YE_V = 10.0;
    sample.YE_Te = 11.0;
    sample.YE_Tc = 12.0;
    sample.AF = 13.0;           // Should be last in array

    FeatureArray arr = sample.to_array();

    REQUIRE(arr.size() == 13);  // FeatureArray has exactly 13 elements
    REQUIRE(arr[0] == Approx(1.0));   // Nep_Tb first
    REQUIRE(arr[1] == Approx(2.0));   // Nep_TOF second
    REQUIRE(arr[12] == Approx(13.0)); // AF last

    // Verify 999 and 888 not in array
    for (double val : arr) {
        REQUIRE(val != Approx(999.0));
        REQUIRE(val != Approx(888.0));
    }

    // Verify all elements match expected order
    for (int i = 0; i < 13; i++) {
        REQUIRE(arr[i] == Approx(static_cast<double>(i + 1)));
    }
}

TEST_CASE("Sample to_array and to_vec produce equivalent values", "[sample][conversion]") {
    Sample sample;
    sample.Nep_Tb = 1.0;
    sample.Nep_TOF = 2.0;
    sample.NepSumArray = 3.0;
    sample.NepPeakArray = 4.0;
    sample.NepDArray = 5.0;
    sample.YE_TOF = 6.0;
    sample.YE_Size = 7.0;
    sample.YE_Mean = 8.0;
    sample.YE_Median = 9.0;
    sample.YE_V = 10.0;
    sample.YE_Te = 11.0;
    sample.YE_Tc = 12.0;
    sample.AF = 13.0;

    FeatureArray arr = sample.to_array();
    std::vector<double> vec = sample.to_vec();

    REQUIRE(arr.size() == vec.size());
    for (size_t i = 0; i < arr.size(); i++) {
        REQUIRE(arr[i] == Approx(vec[i]));
    }
}
