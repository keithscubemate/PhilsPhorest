# PhilsPhorest

A high-performance C++ inference engine for Decision Forest (Random Forest) models. This tool loads pre-trained decision tree models and makes fast predictions on new data without requiring Python or machine learning libraries.

## Overview

PhilsPhorest is designed to run pre-trained machine learning models efficiently in C++ environments. It reads model definitions from JSON (typically exported from scikit-learn) and sample data from CSV files, applies the forest's predictions, and outputs binary classification results.

### Key Features

- Fast inference without Python/ML framework dependencies
- JSON-based model loading
- Feature normalization/scaling
- Binary classification via majority voting
- Minimal external dependencies

## Open Source Tools Used

### nlohmann/json
- **Description**: Modern C++ JSON library for parsing and serialization
- **Version**: 3.11+ (included as `include/json.hpp`)
- **License**: MIT
- **Repository**: https://github.com/nlohmann/json
- **Usage**: Model deserialization and JSON parsing

This is the only external dependency and is included as a single-header library.

## Building

### Requirements

- C++17 compatible compiler (g++ recommended)
- Make build system

### Build Commands

```bash
# Build the executable
make

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild

# Display all available targets
make help
```

The executable will be created as `pp` in the project root.

## Usage

### Basic Usage

```bash
./pp <model_json> <sample_csv>
```

**Arguments:**
- `model_json`: Path to JSON file containing the trained forest model and scaler
- `sample_csv`: Path to CSV file with sample data (15 numeric features)

**Example:**
```bash
./pp model.json data.csv
```

### Input File Formats

#### model.json
JSON file containing:
- `model`: Serialized forest structure with decision trees
- `scaler`: Feature normalization parameters (mean and scale vectors)

Example structure:
```json
{
  "model": {
    "n_estimators": 100,
    "n_features": 13,
    "n_classes": 2,
    "estimators": [...]
  },
  "scaler": {
    "mean": [...],
    "scale": [...]
  }
}
```

#### data.csv
CSV file with 15 columns representing sample features:
- Nep measurements: Nep_index, Nep_Tb, Nep_TOF, NepSumArray, NepPeakArray, NepDArray
- YE measurements: YE, YE_TOF, YE_Size, YE_Mean, YE_Median, YE_V, YE_Te, YE_Tc
- AF: Derived feature

The first row should contain headers and will be skipped during processing.

### Output

The program outputs a single integer representing the sum of all predictions (0s and 1s) across all samples in the CSV file.

## Converting model.json to model_data.h

For deployment scenarios where you want to embed the model directly into the binary (avoiding runtime JSON parsing), you can convert `model.json` into a C++ header file `model_data.h`.

### Benefits of Embedding

- Faster startup (no JSON parsing at runtime)
- Single self-contained executable
- No external model file dependencies
- Reduced deployment complexity

### Conversion Process

#### Option 1: Manual Conversion

1. **Parse the JSON structure**: Extract the model and scaler data from `model.json`

2. **Create C++ data structures**: Convert JSON arrays to C++ `std::vector` initializers

3. **Write header file**: Create `include/model_data.h` with the following structure:

```cpp
#pragma once
#include <vector>
#include <tuple>

namespace EmbeddedModel {
    // Scaler data
    const std::vector<double> scaler_mean = {
        /* copy mean values from JSON */
    };

    const std::vector<double> scaler_scale = {
        /* copy scale values from JSON */
    };

    // Forest structure
    const int n_estimators = /* value */;
    const int n_features = /* value */;
    const int n_classes = /* value */;

    // Tree data - each tree needs:
    // - feature indices
    // - thresholds
    // - children_left/right
    // - values (tuples)
    // - n_node_samples

    // Example for first tree:
    const std::vector<int> tree_0_feature = {/* ... */};
    const std::vector<double> tree_0_threshold = {/* ... */};
    const std::vector<int> tree_0_children_left = {/* ... */};
    const std::vector<int> tree_0_children_right = {/* ... */};
    const std::vector<std::tuple<double, double>> tree_0_value = {/* ... */};
    const std::vector<int> tree_0_n_node_samples = {/* ... */};

    // Repeat for all trees...
}
```

4. **Modify main.cpp**: Replace JSON loading code with direct initialization from embedded data:

```cpp
#include "model_data.h"

// Instead of:
// json data = json::parse(model_fin);
// auto scaler = data.at("scaler").get<Scaler>();
// auto forest = data.at("model").get<Forest>();

// Use:
Scaler scaler(EmbeddedModel::scaler_mean, EmbeddedModel::scaler_scale);
Forest forest = Forest::from_embedded(
    EmbeddedModel::n_estimators,
    EmbeddedModel::n_features,
    EmbeddedModel::n_classes,
    /* tree data arrays */
);
```

#### Option 2: Python Conversion Script

Create a Python script to automate the conversion:

```python
#!/usr/bin/env python3
import json
import sys

def json_to_header(json_file, output_file):
    with open(json_file, 'r') as f:
        data = json.load(f)

    with open(output_file, 'w') as f:
        f.write("#pragma once\n")
        f.write("#include <vector>\n")
        f.write("#include <tuple>\n\n")
        f.write("namespace EmbeddedModel {\n")

        # Write scaler data
        scaler = data['scaler']
        f.write(f"    const std::vector<double> scaler_mean = {{\n")
        f.write(f"        {', '.join(map(str, scaler['mean']))}\n")
        f.write(f"    }};\n\n")

        f.write(f"    const std::vector<double> scaler_scale = {{\n")
        f.write(f"        {', '.join(map(str, scaler['scale']))}\n")
        f.write(f"    }};\n\n")

        # Write forest metadata
        model = data['model']
        f.write(f"    const int n_estimators = {model['n_estimators']};\n")
        f.write(f"    const int n_features = {model['n_features']};\n")
        f.write(f"    const int n_classes = {model['n_classes']};\n\n")

        # Write tree data for each estimator
        for i, tree in enumerate(model['estimators']):
            f.write(f"    // Tree {i}\n")
            # Write feature array
            # Write threshold array
            # Write children arrays
            # Write value tuples
            # Write n_node_samples
            # (implement based on actual JSON structure)

        f.write("}\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input.json> <output.h>")
        sys.exit(1)
    json_to_header(sys.argv[1], sys.argv[2])
```

Usage:
```bash
python3 json_to_header.py model.json include/model_data.h
```

### Implementation Notes

- The conversion requires modifying the `Forest` and `Tree` classes to support construction from embedded data (not just JSON deserialization)
- Consider adding a `from_embedded()` factory method to both classes
- For large models, the generated header file may be substantial (several MB)
- Compilation time will increase with embedded model size
- Consider using `constexpr` where possible for compile-time optimization

## Project Structure

```
PhilsPhorest/
├── Makefile              # Build configuration
├── .gitignore           # Git ignore rules
├── include/             # Header files
│   ├── Forest.h         # Decision forest ensemble class
│   ├── Tree.h           # Individual decision tree class
│   ├── Sample.h         # Data sample structure
│   ├── Scaler.h         # Feature normalization
│   └── json.hpp         # JSON parsing library
└── src/                 # Implementation files
    ├── main.cpp         # Entry point and CLI
    ├── Forest.cpp       # Forest prediction logic
    ├── Tree.cpp         # Tree traversal
    ├── Sample.cpp       # CSV parsing
    └── Scaler.cpp       # Data preprocessing
```

## Architecture

### Components

**Forest**: Ensemble classifier that aggregates predictions from multiple trees using majority voting.

**Tree**: Individual decision tree that traverses from root to leaf using feature thresholds and returns vote counts.

**Sample**: Represents a single data observation with 15 numeric fields, converts to feature vector for prediction.

**Scaler**: Normalizes features using pre-computed mean and scale values: `(x - mean) / scale`

### Prediction Pipeline

```
CSV → Parse Samples → Normalize Features → Tree Predictions → Majority Vote → Output
```

## Development

### Recent Changes

- Performance optimization for copy operations
- Fixed float to int casting bug
- Initial implementation with prediction functionality

### Building for Development

```bash
# Build with debug symbols
make CXXFLAGS="-std=c++17 -Wall -Wextra -I./include -g"

# Build with optimizations
make CXXFLAGS="-std=c++17 -Wall -Wextra -I./include -O3"
```

## License

(License information not specified in codebase)

## Contributing

(Contribution guidelines not specified in codebase)
