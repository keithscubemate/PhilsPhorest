#include <catch.hpp>
#include "../include/Sample.h"

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
