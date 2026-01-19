#include <limits>
#include <iomanip>
#include <sstream>
#include "Sample.h"

using namespace std;

Sample Sample::from_line(const string& line) {
    Sample sample;

    sscanf(
        line.c_str(),
        "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
        &sample.Nep_index,
        &sample.YE,
        &sample.Nep_Tb,
        &sample.Nep_TOF,
        &sample.NepSumArray,
        &sample.NepPeakArray,
        &sample.NepDArray,
        &sample.YE_TOF,
        &sample.YE_Size,
        &sample.YE_Mean,
        &sample.YE_Median,
        &sample.YE_V,
        &sample.YE_Te,
        &sample.YE_Tc,
        &sample.AF
    );

    return sample;
}

string Sample::to_string() const {
    stringstream oss;

    oss << defaultfloat << setprecision(numeric_limits<double>::max_digits10);

    oss <<
        this->Nep_index << "," <<
        this->YE << "," <<
        this->Nep_Tb << "," <<
        this->Nep_TOF << "," <<
        this->NepSumArray << "," <<
        this->NepPeakArray << "," <<
        this->NepDArray << "," <<
        this->YE_TOF << "," <<
        this->YE_Size << "," <<
        this->YE_Mean << "," <<
        this->YE_Median << "," <<
        this->YE_V << "," <<
        this->YE_Te << "," <<
        this->YE_Tc << "," <<
        this->AF;

    return oss.str();
}

vector<double> Sample::to_vec() const {
    return {
        this->Nep_Tb,
        this->Nep_TOF,
        this->NepSumArray,
        this->NepPeakArray,
        this->NepDArray,
        this->YE_TOF,
        this->YE_Size,
        this->YE_Mean,
        this->YE_Median,
        this->YE_V,
        this->YE_Te,
        this->YE_Tc,
        this->AF,
    };
}
