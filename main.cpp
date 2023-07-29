#include <bits/stdc++.h>
#include <vector>

enum Color {
    red, blue, purple, white, blank, orange
};

const int COLORS_PER_BOTTLE = 4;


struct Bottle {
    int id;
    Color colors[COLORS_PER_BOTTLE];
};

void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

void printPerms(std::vector<std::array<int, 2>> indicesPermutations) {
    for (int i = 0; i < indicesPermutations.size(); i++) {
        for (int j = 0; j < 2; j++) {
            std::cout << indicesPermutations[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void printBottles(std::vector<Bottle> bottles) {
    std::cout << "Bottles: " << std::endl;
    for (int i = 0; i < bottles.size(); i++) {
        Bottle bottle = bottles[i];
        printf("%u: %u, %u, %u, %u\n", bottle.id, bottle.colors[0], bottle.colors[1], bottle.colors[2], bottle.colors[3]);
    }
}

bool vectorContains(std::vector<std::array<int, 2>> vector, std::array<int, 2> element) {
    for (int i = 0; i < vector.size(); i++) {
        if (vector[i][0] == element[0] && vector[i][1] == element[1])
            return true;
    }
    return false;
}

// https://www.geeksforgeeks.org/heaps-algorithm-for-generating-permutations/#
// Generating permutation using Heap Algorithm
void heapPermutation(std::vector<std::array<int, 2>> *indicesPermutations, int array[], int size) {
    // if size becomes 1 then prints the obtained
    // permutation
    if (size == 1) {
        std::array<int, 2> outputPermutations{};

        for (int i = 0; i < 2; i++) {
            outputPermutations[i] = array[i];
        }

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

int getFirstColorOfBottleIndex(Bottle bottle) {
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

std::optional<Color> getFirstColorOfBottle(Bottle bottle) {
    std::optional<Color> firstColor;

    int firstColorOfBottleIndex = getFirstColorOfBottleIndex(bottle);
    if (firstColorOfBottleIndex != -1)
        firstColor = bottle.colors[firstColorOfBottleIndex];

    return firstColor;
}

std::vector<std::array<int, 2>> getPossibleMoves(std::vector<Bottle> bottles) {
    std::vector<std::array<int, 2>> indicesPermutations;

    int bottleCount = bottles.size();

    int listOfIndices[bottleCount];
    for (int i = 0; i < bottleCount; i++) {
        listOfIndices[i] = i;
    }

    heapPermutation(&indicesPermutations, listOfIndices, bottleCount);

    for (int i = indicesPermutations.size() - 1; i >= 0; i--) {
        Bottle from = bottles[indicesPermutations[i][0]];
        Bottle to = bottles[indicesPermutations[i][1]];

        std::optional<Color> firstColorOfFromBottle = getFirstColorOfBottle(from);
        std::optional<Color> firstColorOfToBottle = getFirstColorOfBottle(to);

        // from bottle is empty;
        if (!firstColorOfFromBottle.has_value()) {
            indicesPermutations.erase(
                    std::remove(indicesPermutations.begin(), indicesPermutations.end(), indicesPermutations[i]),
                    indicesPermutations.end()
            );
            continue;
        }

        // from bottle top is not the same as to bottle top (to bottle top has to have liquid for this to trigger
        if (firstColorOfToBottle.has_value() && firstColorOfFromBottle.value() != firstColorOfToBottle.value()) {
            indicesPermutations.erase(
                    std::remove(indicesPermutations.begin(), indicesPermutations.end(), indicesPermutations[i]),
                    indicesPermutations.end()
            );
            continue;
        }
    }

    return indicesPermutations;
}

void transferLiquid(Bottle *from, Bottle *to) {
    std::optional<Color> toTransfer = getFirstColorOfBottle(*from);
    int amountToTransfer = 0;
    for (int i = COLORS_PER_BOTTLE - 1; i >= 0; i--) {
        if (from->colors[i] == toTransfer.value()) {
            amountToTransfer++;
            from->colors[i] = blank;
        } else {
            break;
        }
    }
    int firstColorOfToBottleIndex = getFirstColorOfBottleIndex(*to);
    for (int i = COLORS_PER_BOTTLE - 1; i >= firstColorOfToBottleIndex+1 && amountToTransfer > 0; i--, amountToTransfer--) {
        to->colors[i] = toTransfer.value();
    }
}

std::array<int, 2> bestMove(std::vector<std::array<int, 2>> possibleMoves, int depth = 0) {

}

int main() {
    std::vector<Bottle> bottles;
    bottles.push_back(Bottle{0, {purple, orange, purple, orange}});
    bottles.push_back(Bottle{1, {orange, purple, orange, purple}});
    bottles.push_back(Bottle{2, {blank, blank, blank, blank}});

    std::vector<std::array<int, 2>> possibleMoves = getPossibleMoves(bottles);

    std::cout << possibleMoves.size() << std::endl;
    printPerms(possibleMoves);

    printBottles(bottles);

    transferLiquid(&bottles[0], &bottles[2]);
    printBottles(bottles);

    return 0;
}