#pragma once
#include <vector>
#include <iostream>

const int COLORS_PER_BOTTLE = 4;
const bool IS_BALL = true; // if they are balls instead of liquid, the pouring mechanism is different
const bool FIND_SHORTEST = true;
const bool SKIP_AFTER_ONE_SHORTEST_SOL = true;
const int MAXIMUM_DEPTH = 10000;
const bool LOG = false;

enum Color {
    blank, yellow, green, blue, red, grey, orange, violet, lime, maroon, navy, cyan, black, olive
};

struct Bottle {
    Color colors[COLORS_PER_BOTTLE];

    bool operator==(const Bottle &other) {
        for (int i = 0; i < COLORS_PER_BOTTLE; i++) {
            if (this->colors[i] != other.colors[i])
                return false;
        }
        return true;
    }

    bool operator!=(const Bottle &other) {
        return !operator==(other);
    }
};


struct Move {
    char fromID;
    char toID;
};

void printMoves(const std::vector<Move> &moves) {
    for (const Move &move: moves) {
        printf("%u %u", move.fromID, move.toID);
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printSequence(const std::vector<Move> &moves) {
    if (moves.empty()) return;
    for (long i = 0; i < moves.size() - 1; i++) {
        printf("(%u %u)", moves[i].fromID, moves[i].toID);
        std::cout << "->";
    }
    printf("(%u %u)\n", moves.back().fromID, moves.back().toID);
}

void printBottles(const std::vector<Bottle> &bottles) {
    std::cout << "Bottles: " << std::endl;
    for (int i = 0; i < bottles.size(); i++) {
        Bottle bottle = bottles[i];
        printf("%u: %u, %u, %u, %u\n", i, bottle.colors[0], bottle.colors[1], bottle.colors[2], bottle.colors[3]);
    }
}

int getTopColorOfBottleBottomIndex(Bottle bottle) {
    int firstColorIndex = -1;
    Color firstColor = blank;
    for (int i = COLORS_PER_BOTTLE - 1; i >= 0; i--) {
        if (bottle.colors[i] != blank) {
            if (firstColor != blank && firstColor != bottle.colors[i]) {
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

    for (int i = COLORS_PER_BOTTLE - 1; i >= 0; i--) {
        if (bottle.colors[i] != blank) {
            firstColorIndex = i;
            break;
        }
    }

    return firstColorIndex;
}

Color getFirstColorOfBottle(Bottle bottle) {
    int firstColorOfBottleIndex = getTopColorOfBottleBottomIndex(bottle);

    if (firstColorOfBottleIndex != -1)
        return bottle.colors[firstColorOfBottleIndex];
    else
        return blank;
}

std::vector<Move>
getPossibleMoves(const std::vector<Bottle> &bottles, Move indicesPermutations[], const int &indicesPermutationSize) {
    std::vector<Move> possibleMoves;

    for (int i = 0; i < indicesPermutationSize; i++) {
        Move possibleMove = indicesPermutations[i];

        Bottle from = bottles[possibleMove.fromID];
        Bottle to = bottles[possibleMove.toID];

        Color firstColorOfFromBottle = getFirstColorOfBottle(from);
        Color firstColorOfToBottle = getFirstColorOfBottle(to);

        // from bottle is complete or empty; this may need to be removed...
        bool fromBottleCompleteOrEmpty = true;
        for (int j = 0; j < COLORS_PER_BOTTLE; j++) {
            if (from.colors[j] != firstColorOfFromBottle) {
                fromBottleCompleteOrEmpty = false;
                break;
            }
        }

        if (fromBottleCompleteOrEmpty)
            continue;

        // to bottle is full
        if (to.colors[COLORS_PER_BOTTLE - 1] != blank) {
            continue;
        }

        // from bottle top is not the same as to bottle top (to bottle top has to have liquid for this to trigger
        if (firstColorOfToBottle != blank && firstColorOfFromBottle != firstColorOfToBottle) {
            continue;
        }

        possibleMoves.push_back(possibleMove);
    }

    return possibleMoves;
}

void transferLiquid(Bottle *from, Bottle *to) {
    if (from == to) {
        throw std::invalid_argument("Failed to transfer liquid (from and to bottle are the same)!");
    }
    Color toTransfer = getFirstColorOfBottle(*from);

    int topColorOfToBottleTopIndex = getTopColorOfBottleTopIndex(*to);

    int amountToTransfer = 0;
    for (int i = getTopColorOfBottleTopIndex(*from); i >= 0; i--) {
        if (from->colors[i] == toTransfer &&
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
        (*to).colors[i] = toTransfer;
    }
}

bool gameOver(const std::vector<Bottle> &bottles) {
    for (const Bottle &bottle: bottles) {
        for (int i = 1; i < COLORS_PER_BOTTLE; i++) {
            if (bottle.colors[0] != bottle.colors[i]) return false;
        }
    }
    return true;
}


bool
sequenceVectorContainsSequence(const std::vector<std::vector<Move>> &sequenceList, const std::vector<Move> &sequence) {
    for (const std::vector<Move> &s: sequenceList) {
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