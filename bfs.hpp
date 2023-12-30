#include "shared.hpp"
#include "tree.hpp"
#include <algorithm>

void walkMoveBranchBreadthFirst(Tree<Move> *moveBranchPtr, const std::vector<Bottle> &bottles,
                                Move indicesPermutations[],
                                const int &indicesPermutationsCount, std::vector<std::vector<Move>> &solutions) {
    if (SKIP_AFTER_ONE_SHORTEST_SOL && !solutions.empty()) return;

    // continue down tree
    if (!moveBranchPtr->children.empty()) {
        for (long i = 0; i < moveBranchPtr->children.size(); i++) {
            walkMoveBranchBreadthFirst(&moveBranchPtr->children[i], bottles, indicesPermutations,
                                       indicesPermutationsCount,
                                       solutions);
        }
        return;
    }

    // reached bottom of tree, generate sequence of our travelled path
    std::vector<Move> reverseSequence = {};
    Tree<Move> *currBranchPtr = moveBranchPtr;
    while (currBranchPtr->parent != nullptr) {
        reverseSequence.push_back(currBranchPtr->value);
        currBranchPtr = currBranchPtr->parent;
    }

    // Apply sequence to bottles
    std::vector<Bottle> bottlesAfterSequence = bottles;
    for (long i = reverseSequence.size() - 1; i >= 0; i--) {
        transferLiquid(&bottlesAfterSequence[reverseSequence[i].fromID],
                       &bottlesAfterSequence[reverseSequence[i].toID]);
    }

    // Breathe
    std::vector<Move> possibleMoves = getPossibleMoves(bottlesAfterSequence, indicesPermutations,
                                                       indicesPermutationsCount);

    for (const Move &possibleMove: possibleMoves) {
        // backtracked move
        if (reverseSequence.front().fromID == possibleMove.toID &&
            reverseSequence.front().toID == possibleMove.fromID) {
            continue;
        }
        if (IS_BALL) {
            // verbose move (same ball is moved twice in a row e.g. 0 1 -> 1 2 == 0 2)
            if (reverseSequence.front().toID == possibleMove.fromID) {
                continue;
            }
        }

        // Generate bottles that would occur after this move
        std::vector<Bottle> finalBottles = bottlesAfterSequence;
        transferLiquid(&finalBottles[possibleMove.fromID], &finalBottles[possibleMove.toID]);

        if (gameOver(finalBottles)) {
            std::reverse(reverseSequence.begin(), reverseSequence.end());
            reverseSequence.push_back(possibleMove);
            printSequence(reverseSequence);
            solutions.push_back(reverseSequence);
        }
        moveBranchPtr->addChild(possibleMove);
    }


}

std::vector<std::vector<Move>> runBreadthFirstSearch(const std::vector<Bottle> &bottles, Move indicesPermutations[],
                                                     const int &indicesPermutationsCount) {
    Tree<Move> moveTree;
    for (int longestSolutionAllowed = 1; longestSolutionAllowed <= MAXIMUM_DEPTH; longestSolutionAllowed++) {
        std::cout << "Tree size (method): ";
        std::cout << moveTree.size() << std::endl;
        std::cout << "Depth: " << longestSolutionAllowed << std::endl;
        std::vector<std::vector<Move>> solutions;
        if (longestSolutionAllowed == 1) {
            for (const Move &move: getPossibleMoves(bottles, indicesPermutations, indicesPermutationsCount)) {
                moveTree.addChild(move);
            }
        } else {
            walkMoveBranchBreadthFirst(&moveTree, bottles, indicesPermutations, indicesPermutationsCount, solutions);
        }
        if (!solutions.empty())
            return solutions;
    }
    return std::vector<std::vector<Move>>{};
}