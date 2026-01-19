#pragma once

#include <stdio.h>
#include <vector>
#include <string>

struct Sample {
    double Nep_index;
    double YE;
    double Nep_Tb;
    double Nep_TOF;
    double NepSumArray;
    double NepPeakArray;
    double NepDArray;
    double YE_TOF;
    double YE_Size;
    double YE_Mean;
    double YE_Median;
    double YE_V;
    double YE_Te;
    double YE_Tc;
    double AF;

    static Sample from_line(const std::string& line);
    std::string to_string() const;
    std::vector<double> to_vec() const;
};
