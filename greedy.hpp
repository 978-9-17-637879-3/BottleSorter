#include "shared.hpp"
#include "tree.hpp"

long scoreGame(const std::vector<Bottle> &bottles) {
    long score = 0;
    bool gameFinished = true;
    for (const Bottle &bottle: bottles) {
        bool bottleComplete = true;

        std::map<Color, bool> colorSeenMap;
        for (const Color &color: bottle.colors) {
            colorSeenMap[color] = true;
            if (color != bottle.colors[0]) {
                bottleComplete = false;
            }
        }
        // dock points for a lot of different colors in one bottle
        score -= colorSeenMap.size();

        // if bottle is all the same
        if (bottleComplete) {
            // but not clear, give bonus
            if (bottle.colors[0] != blank)
                score += 10000;
        } else {
            // bottle is not all the same, so the game is not finished
            gameFinished = false;
        }

        int highestColorStreak = 0;
        int bottomIdxOfHighestColorStreak = -1;
        int streak = 1;
        for (int i = 1; i < COLORS_PER_BOTTLE; i++) {
            if (bottle.colors[i] == bottleComplete) {
                streak++;
                if (streak > highestColorStreak) {
                    highestColorStreak = streak;
                    bottomIdxOfHighestColorStreak = i - streak;
                }
            } else {
                streak = 1;
            }
        }

        if (streak > 1)
            score += highestColorStreak * 100 * (COLORS_PER_BOTTLE - bottomIdxOfHighestColorStreak);
    }
    if (gameFinished)
        score += 1000000;

    return score;
}


struct Game {
    std::vector<Bottle> bottles;

    bool operator==(const Game &other) {
        if (bottles.size() != other.bottles.size()) return false;
        for (long i = 0; i < bottles.size(); i++) {
            if (this->bottles[i] != other.bottles[i])
                return false;
        }
        return true;
    }

};

struct MoveBottlesScore {
    Move move;
    Game game;
    long score;
};

std::vector<Move> runGreedyBacktracker(const std::vector<Bottle> &startingBottles, Move indicesPermutations[],
                                       const int &indicesPermutationsCount) {
    // initialize tree
    Leaf<MoveBottlesScore> root = Leaf<MoveBottlesScore>(MoveBottlesScore{Move{-1, -1}, Game{startingBottles}, 0});
    Leaf<MoveBottlesScore> *nodePtr = &root;
    // initialize list of pointers to bad nodes, this will be used for backtracking;
    std::vector<Game> deadGames;
    long highScore = 0;
    long stepCount = 1;
    while (nodePtr->value.score < 1000000) {
        if (LOG) {
            stepCount++;
        }

        if (nodePtr->parent == nullptr) {
            if (LOG) std::cout << "At root" << std::endl;
        }

        std::vector<MoveBottlesScore> goodMoveBottleScores;
        std::vector<Move> possibleMoves = getPossibleMoves(nodePtr->value.game.bottles, indicesPermutations,
                                                           indicesPermutationsCount);
        for (const Move &possibleMove: possibleMoves) {
            std::vector<Bottle> newBottles = nodePtr->value.game.bottles;
            transferLiquid(&newBottles[possibleMove.fromID], &newBottles[possibleMove.toID]);
            const MoveBottlesScore &possibleMoveBottlesScore = MoveBottlesScore{possibleMove, newBottles,
                                                                                scoreGame(newBottles)};
            goodMoveBottleScores.push_back(possibleMoveBottlesScore);
        }

        if (goodMoveBottleScores.empty()) {
            deadGames.push_back(nodePtr->value.game);
            std::cout << "No possible moves at this stage, Backtracking" << std::endl;
            nodePtr = nodePtr->parent;
            continue;
        }

        std::sort(goodMoveBottleScores.begin(), goodMoveBottleScores.end(),
                  [](const MoveBottlesScore &a, const MoveBottlesScore &b) {
                      return a.score < b.score;
                  });

        Leaf<MoveBottlesScore> *newNodePtr = nullptr;

        bool shouldBacktrack = false;
        for (int i = goodMoveBottleScores.size() - 1; i >= 0; i--) {
            bool stateExists = false;
            for (const Game &game: deadGames) {
                if (goodMoveBottleScores.back().game == game) {
                    stateExists = true;
                    break;
                }
            }

            if (stateExists) {
                if (goodMoveBottleScores.size() == 1) {
                    if (LOG) {
                        std::cout << nodePtr->value.score << std::endl;
                    }
                    newNodePtr = nodePtr->parent;
                    shouldBacktrack = true;
                    break;
                } else {
                    goodMoveBottleScores.pop_back();
                    newNodePtr = nodePtr->addChild(goodMoveBottleScores.back());
                }
            } else {
                newNodePtr = nodePtr->addChild(goodMoveBottleScores.back());
            }
        }
        nodePtr = newNodePtr;

        if (shouldBacktrack) {
            continue;
        }

        deadGames.push_back(nodePtr->value.game);
        if (LOG) {
            if (nodePtr->value.score > highScore) {
                std::cout << "High score " << highScore << " -> " << nodePtr->value.score << std::endl;
                highScore = nodePtr->value.score;
            }
        }

    }

    std::vector<Move> reverseSequence = {};
    Leaf<MoveBottlesScore> *currBranchPtr = nodePtr;
    while (currBranchPtr->parent != nullptr) {
        reverseSequence.push_back(currBranchPtr->value.move);
        currBranchPtr = currBranchPtr->parent;
    }

    std::reverse(reverseSequence.begin(), reverseSequence.end());

    if (LOG) std::cout << "done" << std::endl;

    if (LOG) std::cout << currBranchPtr->size() - 1 << " states" << std::endl;

    if (LOG) std::cout << stepCount << " steps" << std::endl;
    return reverseSequence;
}