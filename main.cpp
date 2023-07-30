#include <bits/stdc++.h>
#include <vector>

enum Color {
    blank, red, blue, yellow
};

const int COLORS_PER_BOTTLE = 4;

struct Bottle {
    Color colors[COLORS_PER_BOTTLE];
};

struct Move {
    int fromID;
    int toID;
};

struct FindResult {
    std::optional<std::vector<Move>> sequence;
    std::optional<Move> lastMove;
};

void printMoves(const std::vector<Move>& moves) {
    for (const Move& move : moves) {
        std::cout << move.fromID+1 << " " << move.toID+1;
        std::cout << ", ";
    }
    std::cout << std::endl;
}

void printBottles(const std::vector<Bottle> &bottles) {
    std::cout << "Bottles: " << std::endl;
    for (int i = 0; i < bottles.size(); i++) {
        Bottle bottle = bottles[i];
        printf("%u: %u, %u, %u, %u\n", i, bottle.colors[0], bottle.colors[1], bottle.colors[2], bottle.colors[3]);
    }
}

bool vectorContains(std::vector<Move> vector, Move moveSearchTarget) {
    for (const Move& move : vector) {
        if (move.fromID == moveSearchTarget.fromID && move.toID == moveSearchTarget.toID)
            return true;
    }
    return false;
}

int getTopColorOfBottleBottomIndex(Bottle bottle) {
    int firstColorIndex = -1;
    std::optional<Color> firstColor;
    for (int i = COLORS_PER_BOTTLE - 1; i >= 0; i--) {
        if (bottle.colors[i] != blank) {
            if (firstColor.has_value() && firstColor.value() != bottle.colors[i]) {
                break;
            }
            firstColorIndex = i;
            firstColor = bottle.colors[i];
        }
    }
    return firstColorIndex;
}

int getTopColorOfBottleTopIndex(Bottle bottle) {
    int firstColorIndex = -1;
    std::optional<Color> firstColor;
    for (int i = COLORS_PER_BOTTLE - 1; i >= 0; i--) {
        if (bottle.colors[i] != blank) {
            firstColorIndex = i;
            break;
        }
    }
    return firstColorIndex;
}

std::optional<Color> getFirstColorOfBottle(Bottle bottle) {
    std::optional<Color> firstColor;

    int firstColorOfBottleIndex = getTopColorOfBottleBottomIndex(bottle);
    if (firstColorOfBottleIndex != -1)
        firstColor = bottle.colors[firstColorOfBottleIndex];

    return firstColor;
}

void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

// https://www.geeksforgeeks.org/heaps-algorithm-for-generating-permutations/#
// Generating permutation using Heap Algorithm
void heapPermutation(std::vector<Move> *indicesPermutations, int array[], int size) {
    // if size becomes 1 then prints the obtained
    // permutation
    if (size == 1) {
        Move outputPermutations{array[0], array[1]};

        if (!vectorContains(*indicesPermutations, outputPermutations)) {
            indicesPermutations->push_back(outputPermutations);
        }

        return;
    }

    for (int i = 0; i < size; i++) {
        heapPermutation(indicesPermutations, array, size - 1);

        // if size is odd, swap 0th i.e (first) and
        // (size-1)th i.e (last) element
        if (size % 2 == 1)
            swap(array[0], array[size - 1]);

            // If size is even, swap ith and
            // (size-1)th i.e (last) element
        else
            swap(array[i], array[size - 1]);
    }
}

std::vector<Move> getPossibleMoves(const std::vector<Bottle> &bottles) {
    std::vector<Move> movePermutations;

    int bottleCount = bottles.size();

    int listOfIndices[bottleCount];
    for (int i = 0; i < bottleCount; i++) {
        listOfIndices[i] = i;
    }

    heapPermutation(&movePermutations, listOfIndices, bottleCount);

    for (int i = movePermutations.size() - 1; i >= 0; i--) {
        // if from and to are the same
        if (movePermutations[i].fromID == movePermutations[i].toID) {
            movePermutations.erase(movePermutations.begin() + i);
            continue;
        }

        Bottle from = bottles[movePermutations[i].fromID];
        Bottle to = bottles[movePermutations[i].toID];

        std::optional<Color> firstColorOfFromBottle = getFirstColorOfBottle(from);
        std::optional<Color> firstColorOfToBottle = getFirstColorOfBottle(to);

        // from bottle is complete; this may need to be removed...
        if (firstColorOfFromBottle.has_value() && std::all_of(std::begin(from.colors), std::end(from.colors), [&](Color c) -> bool {
            return firstColorOfFromBottle.value() == c;
        })) {
            movePermutations.erase(movePermutations.begin() + i);
            continue;
        }

        // from bottle is empty;
        if (!firstColorOfFromBottle.has_value()) {
            movePermutations.erase(movePermutations.begin() + i);
            continue;
        }

        // to bottle is full
        if (to.colors[COLORS_PER_BOTTLE - 1] != blank) {
            movePermutations.erase(movePermutations.begin() + i);
            continue;
        }

        // from bottle top is not the same as to bottle top (to bottle top has to have liquid for this to trigger
        if (firstColorOfToBottle.has_value() && firstColorOfFromBottle.value() != firstColorOfToBottle.value()) {
            movePermutations.erase(movePermutations.begin() + i);
            continue;
        }
    }

    return movePermutations;
}

void transferLiquid(Bottle *from, Bottle *to) {
    std::optional<Color> toTransfer = getFirstColorOfBottle(*from);

    int topColorOfBottleTopIndex = getTopColorOfBottleTopIndex(*to);

    int amountToTransfer = 0;
    for (int i = getTopColorOfBottleTopIndex(*from); i >= 0; i--) {
        if (from->colors[i] == toTransfer.value() &&
            amountToTransfer + topColorOfBottleTopIndex < COLORS_PER_BOTTLE - 1) {
            amountToTransfer++;
            (*from).colors[i] = blank;
        } else {
            break;
        }
    }
    for (int i = topColorOfBottleTopIndex + 1; amountToTransfer > 0 && i < COLORS_PER_BOTTLE; i++, amountToTransfer--) {
        (*to).colors[i] = toTransfer.value();
    }
}

// https://stackoverflow.com/a/26958878
template<typename MAP>
const typename MAP::mapped_type &get_with_default(const MAP &m,
                                                  const typename MAP::key_type &key,
                                                  const typename MAP::mapped_type &defval) {
    typename MAP::const_iterator it = m.find(key);
    if (it == m.end())
        return defval;

    return it->second;
}

long scoreGame(const std::vector<Bottle> &bottles) {
    long score = 0;
    bool gameFinished = true;
    for (const Bottle &bottle: bottles) {
        std::map<Color, int> colorCountMap;
        bool allColorSame = true;
        for (const Color &color: bottle.colors) {
            colorCountMap[color] = get_with_default(colorCountMap, color, 0) + 1;
            if (color != bottle.colors[0]) {
                allColorSame = false;
            }
        }

        if (!allColorSame) gameFinished = false;

        // Bonus points if vial is solved (empty does not count as solved)
        if (allColorSame && bottle.colors[0] != blank) score += 10;
    }
    if (gameFinished) score = 10000000;
    return score;
}

FindResult
findBestSequence(const std::vector<Move> &path, const std::vector<Bottle> &bottles, int depth = 0) {
    if (depth == 100) {
        throw std::invalid_argument("fuck");
    }

    std::vector<Move> possibleMoves = getPossibleMoves(bottles);

    for (const Move &move: possibleMoves) {
        if (!path.empty()) {
            if (path.back().fromID == move.fromID && path.back().toID == move.toID) {
                continue;
            }
            if (path.back().fromID == move.toID && path.back().toID == move.fromID) {
                continue; //kill this branch, we're backtracking
            }
        }
        std::vector<Move> newPath = path;
        newPath.push_back(move);
        std::vector<Bottle> newBottles = bottles;
        transferLiquid(&newBottles[newPath.back().fromID], &newBottles[newPath.back().toID]);

        FindResult result;
        result.lastMove = move;

//        printBottles(bottles);
//        std::cout << move.fromID << " " << move.toID << std::endl;
//        printBottles(newBottles);
//        std::cout << std::endl;

        if (scoreGame(newBottles) == 10000000) {
            std::cout << "SOLUTION: ";
//            printBottles(newBottles);
            printMoves(newPath);
            result.sequence = newPath;
        }

        while (!result.sequence.has_value() && result.lastMove && depth < 99) {
            result = findBestSequence(newPath, newBottles, depth + 1);
        }

        if (result.sequence.has_value())
            return result;
    }

    return FindResult{};
}

int main() {
    std::vector<Bottle> bottles;
    bottles.push_back(Bottle{{blue, red, yellow, yellow}});
    bottles.push_back(Bottle{{blue, red, blue, red}});
    bottles.push_back(Bottle{{yellow, blue, red, yellow}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});

    findBestSequence(std::vector<Move>{}, bottles);

    return 0;
}