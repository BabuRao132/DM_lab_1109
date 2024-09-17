// Function to print an FP-Tree (for debugging purposes)
void printTree(FPTreeNode* node, int depth = 0) {
    if (node == nullptr) return;

    // Indent to represent tree levels
    for (int i = 0; i < depth; ++i) {
        cout << "  ";
    }
    cout << node->item << " (" << node->count << ")" << endl;

    // Recur for each child
    for (const auto& child : node->children) {
        printTree(child.second, depth + 1);
    }
}

// Traverse the tree to find all paths leading to the given item (conditional pattern base)
vector<pair<vector<string>, int>> findConditionalPatternBase(FPTree& tree, const string& item) {
    vector<pair<vector<string>, int>> conditionalPatternBase;
    FPTreeNode* currentNode = tree.headerTable[item].node;

    cout << "Conditional Pattern Base for item '" << item << "':\n";

    while (currentNode != nullptr) {
        vector<string> path;
        FPTreeNode* traverseNode = currentNode->parent;

        while (traverseNode != nullptr && traverseNode->item != "") {
            path.push_back(traverseNode->item);
            traverseNode = traverseNode->parent;
        }

        if (!path.empty()) {
            conditionalPatternBase.push_back({path, currentNode->count});
            cout << "Path: ";
            for (const auto& p : path) {
                cout << p << " ";
            }
            cout << "| Count: " << currentNode->count << endl;
        }

        currentNode = currentNode->next;
    }

    cout << "End of Conditional Pattern Base for item '" << item << "'\n\n";
    return conditionalPatternBase;
}

// Build and print the conditional FP-Tree for a given item
void buildConditionalFPTree(FPTree& originalTree, const string& item) {
    vector<pair<vector<string>, int>> conditionalPatternBase = findConditionalPatternBase(originalTree, item);

    if (conditionalPatternBase.empty()) return;

    FPTree conditionalTree;

    for (const auto& pattern : conditionalPatternBase) {
        conditionalTree.insertTransaction(pattern.first, pattern.second);
    }

    // Print the conditional FP-Tree
    cout << "Conditional FP-Tree for item '" << item << "':\n";
    printTree(conditionalTree.root);
    cout << "End of Conditional FP-Tree for item '" << item << "'\n\n";

    // Now, recursively mine the conditional FP-Tree
    mineFPTree(conditionalTree);
}

// Mine the frequent itemsets from the FP-Tree
void mineFPTree(FPTree& tree) {
    for (const auto& entry : tree.headerTable) {
        const string& item = entry.first;
        int support = entry.second.support;

        // Frequent itemset for the single item
        cout << "Frequent itemset: " << item << " | Support: " << support << endl;

        // Build and mine the conditional FP-Tree for the current item
        buildConditionalFPTree(tree, item);
    }
}
