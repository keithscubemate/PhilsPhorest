#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>

#include "json.hpp"
#include "Scaler.h"
#include "Sample.h"
#include "Forest.h"

using json = nlohmann::json;
using namespace std;

vector<Sample> csv_to_samples(ifstream& fin);

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("usage: %s <model_json> <sample_csv>\n", argv[0]);
        return -1;
    }

    // get the samples
    auto sample_file = argv[2];
    ifstream sample_fin(sample_file);
    auto samples = csv_to_samples(sample_fin);

    // get the model
    auto model_file = argv[1];
    ifstream model_fin(model_file);
    json data = json::parse(model_fin);

    auto scaler = data.at("scaler").get<Scaler>();
    auto forest = data.at("model").get<Forest>();

    for (auto sample: samples) {
        cout << sample.to_string() << endl;
    }
    return -1;

    // run the forest
    int he = 0;
    auto sample = samples[0];

    auto svec = sample.to_vec();

    // TODO(austin.jones): get this from forest
    scaler.transform(svec, 13);

    auto val = forest.predict(svec);

    he += val;

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
