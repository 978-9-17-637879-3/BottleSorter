#pragma once
#include <vector>

template<typename _valueType> class Tree {
public:
    _valueType value;
    Tree* parent = nullptr;
    std::vector<Tree<_valueType>> children;

    // Root of tree
    Tree<_valueType>() = default;

    // Root of tree (with value)
    Tree<_valueType>(_valueType value) {
        this->value = value;
    }

    // Branch of tree
    Tree<_valueType>(_valueType value, Tree* parent) {
        this->value = value;
        this->parent = parent;
    }

    Tree<_valueType>* addChild(_valueType childValue) {
        Tree<_valueType> child = Tree(childValue, this);
        children.push_back(child);
        return &children.back();
    }

    long size() {
        long totalSize = 1;
        if (!this->children.empty()) {
            for (long i = 0; i < this->children.size(); i++) {
                totalSize += this->children[i].size();
            }
        }
        return totalSize;
    }

};
