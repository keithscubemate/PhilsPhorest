#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

#include "Sample.h"
#include "Predictor.h"

using namespace std;

vector<Sample> csv_to_samples(ifstream& fin);

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: %s <sample_csv>\n", argv[0]);
        return -1;
    }

    // get the samples
    auto sample_file = argv[1];
    ifstream sample_fin(sample_file);
    auto samples = csv_to_samples(sample_fin);

    auto predictor = Predictor::LoadEmbedded();

    int he = 0;
    // run the forest
    for (const auto& sample : samples) {
        auto sarr = sample.to_array();

        he += predictor.predict(sarr);
    }

    cout << he << endl;

    return 0;
}

vector<Sample> csv_to_samples(ifstream& fin) {
    string line = "";
    vector<Sample> rv = {};

    getline(fin, line);

    while(getline(fin, line)) {
        auto sample = Sample::from_line(line);
        rv.push_back(sample);
    }

    return rv;
}
