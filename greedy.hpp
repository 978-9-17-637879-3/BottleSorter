#include "shared.hpp"
#include "tree.hpp"

long scoreGame(const std::vector<Bottle> &bottles) {
    long score = 0;
    bool gameFinished = true;
    for (const Bottle &bottle: bottles) {
        long bottomStreak = 0;
        bool bottleComplete = true;
        std::map<Color, int> colorCountMap;
        long streak = 1;
        for (const Color &color: bottle.colors) {
            colorCountMap[color] =
                    colorCountMap.count(color) == 0 ?
                    1
                                                    :
                    colorCountMap[color] + 1;
        }

        if (bottle.colors[0] != blank) {
            for (int i = 1; i < COLORS_PER_BOTTLE; i++) {
                if (bottle.colors[i] == bottle.colors[i - 1]) {
                    streak++;
                    if (streak > bottomStreak) {
                        bottomStreak = streak;
                    }
                } else {
                    break;
                }
            }
        }

        if (colorCountMap.begin()->second != 4) {
            bottleComplete = false;
            gameFinished = false;
        }

        for (const std::pair<const Color, int> &colorCount: colorCountMap) {
            score -= colorCount.second;
        }
        score += bottomStreak * 6;
        if (bottle.colors[0] != blank && bottleComplete) {
            score += 10;
        }
    }

    if (gameFinished)
        return 1000000;
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
            if (IS_BALL && possibleMove.fromID == nodePtr->value.move.toID) {
                continue;
            }
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