#pragma once

#include <vector>
#include <tuple>
#include <memory>
#include "../include/Tree.h"
#include "../include/Forest.h"
#include "../include/Scaler.h"
#include "../include/Sample.h"

// Helper functions to create test objects

Tree create_simple_tree();
Tree create_multilevel_tree();
Tree create_feature_indexed_tree();

Forest create_single_tree_forest();
Forest create_unanimous_class0_forest();
Forest create_unanimous_class1_forest();
Forest create_majority_vote_forest();
Forest create_tie_forest();

Scaler create_test_scaler();
Scaler create_zero_mean_scaler();
