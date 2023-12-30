#include "shared.hpp"
#include <optional>

struct FindResult {
    std::optional<std::vector<Move>> sequence;
    std::optional<Move> lastMove;
};

FindResult
depthFirstSearch(const std::vector<Move> &path,
                 const std::vector<Bottle> &bottles,
                 std::vector<std::vector<Move>> &solutions,
                 Move indicesPermutations[],
                 const int &indicesPermutationsCount,
                 int &shortestSolutionLength,
                 int maxDepth,
                 int depth = 0) {

    std::vector<Move> possibleMoves = getPossibleMoves(bottles, indicesPermutations, indicesPermutationsCount);

    for (const Move &newMove: possibleMoves) {
        if (!path.empty()) {
            // duplicate move
            if (!IS_BALL) {
                if (path.back().fromID == newMove.fromID && path.back().toID == newMove.toID) {
                    continue;  //kill this branch, we're backtracking
                }
            }
            // backtracked move
            if (path.back().fromID == newMove.toID && path.back().toID == newMove.fromID) {
                continue; //kill this branch, we're backtracking
            }
        }
        std::vector<Move> newPath = path;
        if (newPath.size() >= shortestSolutionLength) {
            continue;
        }
        newPath.push_back(newMove);

        std::vector<Bottle> newBottles = bottles;
        transferLiquid(&newBottles[newPath.back().fromID], &newBottles[newPath.back().toID]);

        FindResult result;
        result.lastMove = newMove;

        if (gameOver(newBottles)) {
            result.sequence = newPath;
            if (result.sequence.value().size() < shortestSolutionLength) {
                shortestSolutionLength = newPath.size();
            }
        }

        while (!result.sequence.has_value() && result.lastMove && depth < maxDepth - 1) {
            result = depthFirstSearch(newPath, newBottles, solutions, indicesPermutations, indicesPermutationsCount,
                                      shortestSolutionLength, maxDepth, depth + 1);
        }

        if (result.sequence.has_value()) {
            if (depth > 0)
                solutions.push_back(result.sequence.value());
            else {
                continue;
            }
        }
    }

    return FindResult{};
}

std::vector<std::vector<Move>> runDepthFirstSearch(std::vector<Bottle> bottles, Move indicesPermutations[],
                                                   const int &indicesPermutationsCount) {
    std::vector<std::vector<Move>> solutions;

    int shortestSolutionLength = MAXIMUM_DEPTH;
    depthFirstSearch(std::vector<Move>{}, bottles, solutions, indicesPermutations, indicesPermutationsCount,
                     shortestSolutionLength, MAXIMUM_DEPTH);

    return solutions;
}

std::vector<std::vector<Move>>
runIterativeDeepeningDepthFirstSearch(std::vector<Bottle> bottles,
                                      Move indicesPermutations[],
                                      const int &indicesPermutationsCount) {
    for (int longestSolutionAllowed = 1; longestSolutionAllowed <= MAXIMUM_DEPTH; longestSolutionAllowed++) {
        std::cout << "Depth: " << longestSolutionAllowed << std::endl;

        std::vector<std::vector<Move>> solutions;

        int shortestSolutionLength = MAXIMUM_DEPTH;
        depthFirstSearch(std::vector<Move>{}, bottles, solutions, indicesPermutations,
                         indicesPermutationsCount,
                         shortestSolutionLength, longestSolutionAllowed);

        if (!solutions.empty()) return solutions;
    }

    return std::vector<std::vector<Move>>{};
}
