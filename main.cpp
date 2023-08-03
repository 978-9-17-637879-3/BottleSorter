#include <bits/stdc++.h>
#include <vector>
#include "tree.hpp"

const int COLORS_PER_BOTTLE = 4;
const bool IS_BALL = true; // if they are balls instead of liquid, the pouring mechanism is different
const bool FIND_SHORTEST = true;
const bool SKIP_AFTER_ONE_SHORTEST_SOL = true;
const int MAXIMUM_DEPTH = 10000;
const bool LOG = true;

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

struct FindResult {
    std::optional<std::vector<Move>> sequence;
    std::optional<Move> lastMove;
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

bool vectorContains(std::vector<Move> vector, Move moveSearchTarget) {
    for (const Move &move: vector) {
        if (move.fromID == moveSearchTarget.fromID && move.toID == moveSearchTarget.toID)
            return true;
    }
    return false;
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

FindResult
depthFirstSearch(const std::vector<Move> &path,
                 const std::vector<Bottle> &bottles,
                 std::vector<std::vector<Move>> *solutionsPtr,
                 Move indicesPermutations[],
                 const int &indicesPermutationsCount,
                 int *shortestSolutionLengthPtr,
                 int maxDepth,
                 int depth = 0) {

    std::vector<Move> possibleMoves = getPossibleMoves(bottles, indicesPermutations, indicesPermutationsCount);

//    std::cout << "Setup:" <<std::endl;
//    printSequence(path);
//    printBottles(bottles);
//    printMoves(possibleMoves);
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
//        printSequence(newPath);
        if (newPath.size() >= *shortestSolutionLengthPtr) {
            continue;
        }
        newPath.push_back(newMove);

//        std::cout << "\x1B[2J\x1B[H";
//        std::cout << "Unique Sequences Calculated: " << sequencesSeenPtr->size() << std::endl;
        std::vector<Bottle> newBottles = bottles;
        transferLiquid(&newBottles[newPath.back().fromID], &newBottles[newPath.back().toID]);

        FindResult result;
        result.lastMove = newMove;

        if (gameOver(newBottles)) {
            result.sequence = newPath;
            if (result.sequence.value().size() < *shortestSolutionLengthPtr) {
                *shortestSolutionLengthPtr = newPath.size();
            }
        }

        while (!result.sequence.has_value() && result.lastMove && depth < maxDepth - 1) {
            result = depthFirstSearch(newPath, newBottles, solutionsPtr, indicesPermutations, indicesPermutationsCount,
                                      shortestSolutionLengthPtr, maxDepth, depth + 1);
        }

        if (result.sequence.has_value()) {
            if (depth > 0)
                solutionsPtr->push_back(result.sequence.value());
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
    depthFirstSearch(std::vector<Move>{}, bottles, &solutions, indicesPermutations, indicesPermutationsCount,
                     &shortestSolutionLength, MAXIMUM_DEPTH);

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
        depthFirstSearch(std::vector<Move>{}, bottles, &solutions, indicesPermutations,
                         indicesPermutationsCount,
                         &shortestSolutionLength, longestSolutionAllowed);

        if (!solutions.empty()) return solutions;
    }

    return std::vector<std::vector<Move>>{};
}

void walkMoveBranchBreadthFirst(Leaf<Move> *moveBranchPtr, const std::vector<Bottle> &bottles,
                                Move indicesPermutations[],
                                const int &indicesPermutationsCount, std::vector<std::vector<Move>> *solutionsPtr) {
    if (SKIP_AFTER_ONE_SHORTEST_SOL && !solutionsPtr->empty()) return;

    // if branch is dead, it is not viable to spawn new branches, kill
//    if (moveBranchPtr->dead) return;

    // continue down tree
    if (!moveBranchPtr->children.empty()) {
        for (long i = 0; i < moveBranchPtr->children.size(); i++) {
            walkMoveBranchBreadthFirst(&moveBranchPtr->children[i], bottles, indicesPermutations,
                                       indicesPermutationsCount,
                                       solutionsPtr);
        }
        return;
    }

    // reached bottom of tree, generate sequence of our travelled path
    std::vector<Move> reverseSequence = {};
    Leaf<Move> *currBranchPtr = moveBranchPtr;
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
//            moveBranchPtr->addDead(possibleMove);
            continue;
        }
        if (IS_BALL) {
            // verbose move (same ball is moved twice in a row e.g. 0 1 -> 1 2 == 0 2)
            if (reverseSequence.front().toID == possibleMove.fromID) {
//                moveBranchPtr->addDead(possibleMove);
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
            solutionsPtr->push_back(reverseSequence);
        }
        moveBranchPtr->addChild(possibleMove);
//        printSequence(finalSequence);
//        sequencesPtr->push_back(finalSequence);
    }


}

std::vector<std::vector<Move>> runBreadthFirstSearch(const std::vector<Bottle> &bottles, Move indicesPermutations[],
                                                     const int &indicesPermutationsCount) {
    Leaf<Move> moveTree;
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
            walkMoveBranchBreadthFirst(&moveTree, bottles, indicesPermutations, indicesPermutationsCount, &solutions);
        }
        if (!solutions.empty())
            return solutions;
    }
    return std::vector<std::vector<Move>>{};
}


long scoreGame(const std::vector<Bottle> &bottles) {
//    printBottles(bottles);

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


int main() {
    /*
    tubeCount = document.querySelectorAll('rect').length
    tubeHeight = Number(document.querySelector('#tubeHeight').value);
    circles = Array.from(document.querySelectorAll('circle'))
    colors = {
        ball1: "yellow",
        ball2: "green",
        ball3: "blue",
        ball4: "red",
        ball5: "grey",
        ball6: "orange",
        ball7: "violet",
        ball8: "lime",
        ball9: "maroon",
        ball10: "navy",
        ball11: "cyan",
        ball12: "black",
        ball13: "olive"
    }
    lines = []
    for (let i = 0; i < tubeCount; i++) {
        const tubeCircles = circles.filter(circle => circle.id.startsWith(`ball_${i}_`)).reverse()
        const tubeCircleColors = tubeCircles.map(circle => colors[circle.classList[0]])
        while (tubeCircleColors.length < tubeHeight) {
            tubeCircleColors.push("blank");
        }
        console.log(tubeCircleColors)
        let line = 'bottles.push_back(Bottle{{'
        for (let j = 0; j < tubeHeight - 1; j++) {
            line += tubeCircleColors[j];
            line += ', ';
        }
        line += `${tubeCircleColors[tubeHeight - 1]}}});`;
        lines.push(line);
    }
    console.log(lines.join('\n'));
     */


    std::vector<Bottle> bottles;
    bottles.push_back(Bottle{{blue, violet, lime, maroon}});
    bottles.push_back(Bottle{{blue, blue, yellow, maroon}});
    bottles.push_back(Bottle{{orange, yellow, blank, blank}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{red, orange, lime, blank}});
    bottles.push_back(Bottle{{red, violet, grey, blue}});
    bottles.push_back(Bottle{{grey, green, orange, lime}});
    bottles.push_back(Bottle{{maroon, maroon, green, orange}});
    bottles.push_back(Bottle{{yellow, yellow, lime, blank}});
    bottles.push_back(Bottle{{violet, violet, red, green}});
    bottles.push_back(Bottle{{green, grey, red, grey}});

    if (bottles.size() > 255) {
        std::cout << "Bottle count must be <=255" << std::endl;
        return 0;
    }

    // Calculate all permutations bottles size 2 (used for finding possible moves)
    char bottleCount = bottles.size();
    // the size of indicesPermutation is nPr(bottlesCount, 2) which is equivalent to bottleCount * (bottleCount-1)
    int indicesPermutationsCount = bottleCount * (bottleCount - 1);

    Move indicesPermutations[indicesPermutationsCount];
    int insertionPoint = 0;
    for (char i = 0; i < bottleCount; i++) {
        for (char j = 0; j < bottleCount; j++) {
            if (i == j) continue;
            indicesPermutations[insertionPoint] = (Move{i, j});
            insertionPoint++;
        }
    }

    // Solve
    auto start = std::chrono::high_resolution_clock::now();
    const std::vector<Move> &solution = runGreedyBacktracker(bottles, indicesPermutations, indicesPermutationsCount);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    if (LOG) std::cout << duration.count() << std::endl;
    std::cout << "SOLUTION FOUND " << std::endl;
    printSequence(solution);
    if (LOG) std::cout << solution.size() << " moves" << std::endl;


    return 0;
}
