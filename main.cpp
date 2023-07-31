#include <bits/stdc++.h>
#include <vector>

const int COLORS_PER_BOTTLE = 4;
const bool IS_BALL = true; // if they are balls instead of liquid, the pouring mechanism is different
const bool FIND_SHORTEST = false;
const int MAXIMUM_DEPTH = 1000;

enum Color {
    blank, red, blue, yellow, green, orange, purple, aqua, grey, lilac, lime
};

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

void printMoves(const std::vector<Move> &moves) {
    for (const Move &move: moves) {
        std::cout << move.fromID << " " << move.toID;
        std::cout << std::endl;
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
    for (const Move &move: vector) {
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
void heapPermutation(std::vector<Move> *indicesPermutations, int array[], int sizeOfArray, int size) {
    // size of permutations size 2 is nPr(bottleCount. 2) == bottleCount!/(bottleCount-2)! == bottleCount*(bottleCount-1);
    if (indicesPermutations->size() == sizeOfArray*(sizeOfArray-1)) {
        return;
    }
    // if size becomes 1 then prints the obtained
    // permutation
    if (size == 1) {
        Move outputPermutations{array[0], array[1]};

        if (!vectorContains(*indicesPermutations, outputPermutations)) {
            indicesPermutations->push_back(outputPermutations);
            std::cout << "\x1B[2J\x1B[H";
            std::cout << "Index permutations: " << indicesPermutations->size() << "/" << sizeOfArray*(sizeOfArray-1) << std::endl;
        }

        return;
    }

    for (int i = 0; i < size; i++) {
        heapPermutation(indicesPermutations, array, sizeOfArray, size - 1);

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

std::vector<Move> getPossibleMoves(const std::vector<Bottle> &bottles, const std::vector<Move> &indicesPermutations) {
    std::vector<Move> movePermutations = indicesPermutations;

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
        if (firstColorOfFromBottle.has_value() &&
            std::all_of(std::begin(from.colors), std::end(from.colors), [&](Color c) -> bool {
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

    int topColorOfToBottleTopIndex = getTopColorOfBottleTopIndex(*to);

    int amountToTransfer = 0;
    for (int i = getTopColorOfBottleTopIndex(*from); i >= 0; i--) {
        if (from->colors[i] == toTransfer.value() &&
            (IS_BALL ?
             amountToTransfer < 1 :
             (amountToTransfer + topColorOfToBottleTopIndex < COLORS_PER_BOTTLE - 1)
            )) {
            amountToTransfer++;
            (*from).colors[i] = blank;
        } else {
            break;
        }
    }
    for (int i = topColorOfToBottleTopIndex + 1;
         amountToTransfer > 0 && i < COLORS_PER_BOTTLE; i++, amountToTransfer--) {
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


bool
sequenceVectorContainsSequence(const std::vector<std::vector<Move>> &sequenceList, const std::vector<Move> &sequence) {
    for (const std::vector<Move> &s : sequenceList) {
        if (s.size() != sequence.size())
            continue;
        for (long i = 0; i < s.size(); i++) {
            if (s[i].fromID == sequence[i].fromID && s[i].toID == sequence[i].toID) {
                return true;
            }
        }
    }
    return false;
}

FindResult
findSolution(const std::vector<Move> &path, const std::vector<Bottle> &bottles,
             std::vector<std::vector<Move>> *sequenceSeenPtr, const std::vector<Move> &indicesPermutations, int depth = 0) {
    if (depth == MAXIMUM_DEPTH) {
        throw std::invalid_argument("fuck");
    }

    std::vector<Move> possibleMoves = getPossibleMoves(bottles, indicesPermutations);

    for (const Move &newMove: possibleMoves) {
        if (!path.empty()) {
            // duplicate move
            if (!IS_BALL) {
                if (path.back().fromID == newMove.fromID && path.back().toID == newMove.toID) {
                    continue;  //kill this branch, we're backtracking
                }
            }
            // backtracked newMove
            if (path.back().fromID == newMove.toID && path.back().toID == newMove.fromID) {
                continue; //kill this branch, we're backtracking
            }
        }
        std::vector<Move> newPath = path;
        // if a ball is being transferred twice in a row, optimize the transfer by eliminating the middleman
        if (IS_BALL) {
            if (newPath.size() > 1) {
                Move lastMove = newPath.back();
                if (lastMove.toID == newMove.fromID) {
                    newPath.pop_back();
                    newPath.push_back(Move{lastMove.fromID, newMove.toID});
                } else {
                    newPath.push_back(newMove);
                }
            } else {
                newPath.push_back(newMove);
            }
        } else {
            newPath.push_back(newMove);
        }
        if (sequenceVectorContainsSequence(*sequenceSeenPtr, newPath)) {
            continue;
        }
        sequenceSeenPtr->push_back(newPath);
        std::cout << "\x1B[2J\x1B[H";
        std::cout << "Unique Sequences Calculated: " << sequenceSeenPtr->size() << std::endl;
        std::vector<Bottle> newBottles = bottles;
        transferLiquid(&newBottles[newPath.back().fromID], &newBottles[newPath.back().toID]);

        FindResult result;
        result.lastMove = newMove;

        if (scoreGame(newBottles) == 10000000) {
            result.sequence = newPath;
        }

        while (!result.sequence.has_value() && result.lastMove && depth < MAXIMUM_DEPTH-1) {
            result = findSolution(newPath, newBottles, sequenceSeenPtr, indicesPermutations, depth + 1);
        }

        if (result.sequence.has_value())
            return result;
    }

    return FindResult{};
}


int main() {
    std::vector<Bottle> bottles;
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{orange, yellow, lilac, orange}});
    bottles.push_back(Bottle{{blue, red, grey, yellow}});
    bottles.push_back(Bottle{{grey, red, green, orange}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{yellow, lilac, blue, lilac}});
    bottles.push_back(Bottle{{red, red, green, grey}});
    bottles.push_back(Bottle{{orange, yellow, green, blue}});
    bottles.push_back(Bottle{{grey, green, blue, lilac}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});

    // Calculate all permutations bottles size 2 (used for finding possible moves)
    int bottleCount = bottles.size();

    int listOfIndices[bottleCount];
    for (int i = 0; i < bottleCount; i++) {
        listOfIndices[i] = i;
    }

    std::vector<Move> indicesPermutations;
    heapPermutation(&indicesPermutations, listOfIndices, bottleCount, bottleCount);

    // Solve
    std::vector<std::vector<Move>> sequencesSeen;
    std::vector<std::vector<Move>> solutions;

    std::optional<std::vector<Move>> lastSolution = findSolution(std::vector<Move>{}, bottles, &sequencesSeen, indicesPermutations).sequence;
    if (lastSolution.has_value()) {
        std::cout << "FIRST SOLUTION FOUND " << std::endl;
        printMoves(lastSolution.value());
    } else {
        std::cout << "NO SOLUTIONS" << std::endl;
        return 0;
    }

    if (FIND_SHORTEST) {
        while (lastSolution.has_value()) {
            solutions.push_back(lastSolution.value());
            lastSolution = findSolution(std::vector<Move>{}, bottles, &sequencesSeen, indicesPermutations).sequence;
        }

        std::sort(solutions.begin(), solutions.end(), [](const std::vector<Move> &a, const std::vector<Move> &b) {
            return a.size() < b.size();
        });

        std::cout << "SHORTEST SOLUTION FOUND " << std::endl;
        printMoves(solutions.front());
    }

    std::cout << "Explored " << sequencesSeen.size() << " move sequences!" << std::endl;

    return 0;
}
