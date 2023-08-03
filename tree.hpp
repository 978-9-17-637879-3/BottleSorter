#include <bits/stdc++.h>

template<typename _valueType> class Leaf {
public:
    _valueType value;
    Leaf* parent = nullptr;
    std::vector<Leaf<_valueType>> children;

    // Root of tree
    Leaf<_valueType>() = default;

    // Root of tree (with value)
    Leaf<_valueType>(_valueType value) {
        this->value = value;
    }

    // Branch of tree
    Leaf<_valueType>(_valueType value, Leaf* parent) {
        this->value = value;
        this->parent = parent;
    }

    Leaf<_valueType>* addChild(_valueType childValue) {
        Leaf<_valueType> child = Leaf(childValue, this);
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
