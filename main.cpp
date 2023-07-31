#include <bits/stdc++.h>
#include <vector>

const int COLORS_PER_BOTTLE = 4;
const bool IS_BALL = true; // if they are balls instead of liquid, the pouring mechanism is different
const bool FIND_SHORTEST = true;
const int MAXIMUM_DEPTH = 40;

enum Color {
    blank, yellow, green, blue, red, grey, orange, violet, lime, maroon, navy, cyan, black, olive
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

void printSequence(const std::vector<Move> &moves) {
    if (moves.empty()) return;
    for (long i = 0; i < moves.size()-1; i++) {
        std::cout << moves[i].fromID << " " << moves[i].toID;
        std::cout << "->";
    }
    std::cout << moves.back().fromID << " " << moves.back().toID;
    std::cout <<  "|end" << std::endl;
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
    Color firstColor;

    int firstColorOfBottleIndex = getTopColorOfBottleBottomIndex(bottle);
    if (firstColorOfBottleIndex != -1)
        firstColor = bottle.colors[firstColorOfBottleIndex];
    else firstColor = blank;

    return firstColor;
}

std::vector<Move> getPossibleMoves(const std::vector<Bottle> &bottles, const std::vector<Move> &indicesPermutations) {
    std::vector<Move> possibleMoves;

    for (const Move &movePerm : indicesPermutations) {
        Bottle from = bottles[movePerm.fromID];
        Bottle to = bottles[movePerm.toID];

        Color firstColorOfFromBottle = getFirstColorOfBottle(from);
        Color firstColorOfToBottle = getFirstColorOfBottle(to);

        // from bottle is complete; this may need to be removed...
        if (firstColorOfFromBottle != blank &&
            std::all_of(std::begin(from.colors), std::end(from.colors), [&](Color c) -> bool {
                return firstColorOfFromBottle == c;
            })) {
            continue;
        }

        // from bottle is empty;
        if (from.colors[0] == blank) {
            continue;
        }

        // to bottle is full
        if (to.colors[COLORS_PER_BOTTLE - 1] != blank) {
            continue;
        }

        // from bottle top is not the same as to bottle top (to bottle top has to have liquid for this to trigger
        if (firstColorOfToBottle != blank && firstColorOfFromBottle != firstColorOfToBottle) {
            continue;
        }

        possibleMoves.push_back(movePerm);
    }

    return possibleMoves;
}

void transferLiquid(Bottle *from, Bottle *to) {
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
    long score = 0;
    for (const Bottle &bottle: bottles) {
        for (int i = 1; i < COLORS_PER_BOTTLE; i++) {
            if (bottle.colors[0] != bottle.colors[i]) return false;
        }
    }
    return true;
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
depthFirstSearch(const std::vector<Move> &path,
                 const std::vector<Bottle> &bottles,
                 std::vector<std::vector<Move>> *sequencesSeenPtr,
                 std::vector<std::vector<Move>> *solutionsPtr,
                 const std::vector<Move> &indicesPermutations,
                 int* shortestSolutionLengthPtr,
                 int depth = 0) {

    std::vector<Move> possibleMoves = getPossibleMoves(bottles, indicesPermutations);

//    std::cout << "Setup:" <<std::endl;
//    printSequence(path);
//    printBottles(bottles);
//    printMoves(possibleMoves);
    for (const Move &newMove: possibleMoves) {
        if (depth == 0) {
            std::cout << "";
        }
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
        if (newPath.size() >= *shortestSolutionLengthPtr) {
            continue;
        }
            newPath.push_back(newMove);

        sequencesSeenPtr->push_back(newPath);
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

        while (!result.sequence.has_value() && result.lastMove && depth < MAXIMUM_DEPTH-1) {
            result = depthFirstSearch(newPath, newBottles, sequencesSeenPtr, solutionsPtr, indicesPermutations,
                                      shortestSolutionLengthPtr, depth + 1);
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

std::vector<std::vector<Move>> runDepthFirstSearch(std::vector<std::vector<Move>> sequencesSeen, std::vector<Bottle> bottles, std::vector<Move> indicesPermutations) {
    std::vector<std::vector<Move>> solutions;

    int shortestSolutionLength = MAXIMUM_DEPTH;
    depthFirstSearch(std::vector<Move>{}, bottles, &sequencesSeen, &solutions, indicesPermutations,
                     &shortestSolutionLength);

    return solutions;
}

int main() {
    /*
    tubeCount = document.querySelectorAll('rect').length
    tubeHeight = Number(document.querySelector('#tubeHeight > option:nth-child(2)').text);
    circles = Array.from(document.querySelectorAll('circle'))
    colors = {ball1: "yellow", ball2: "green", ball3: "blue", ball4: "red", ball5: "grey", ball6: "orange", ball7: "violet", ball8: "lime", ball9: "maroon", ball10: "navy", ball11: "cyan", ball12: "black", ball13: "olive"}
    lines = []
		for (let i = 0; i < tubeCount; i++) {
        const tubeCircles = circles.filter(circle => circle.id.startsWith(`ball_${i}_`)).reverse()
        const tubeCircleColors = tubeCircles.map(circle => colors[circle.classList[0]])
        while (tubeCircleColors.length < tubeHeight) {
            tubeCircleColors.push("blank");
        }
        console.log(tubeCircleColors)
        lines.push(`bottles.push_back(Bottle{{${tubeCircleColors[0]}, ${tubeCircleColors[1]}, ${tubeCircleColors[2]}, ${tubeCircleColors[3]}}});`)
    }
	console.log(lines.join('\n'))
     */


    std::vector<Bottle> bottles;
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{blank, blank, blank, blank}});
    bottles.push_back(Bottle{{green, yellow, blank, blank}});
    bottles.push_back(Bottle{{yellow, green, blank, blank}});
    bottles.push_back(Bottle{{green, yellow, green, blank}});

    // Calculate all permutations bottles size 2 (used for finding possible moves)
    int bottleCount = bottles.size();

    std::vector<Move> indicesPermutations;
    for (int i = 0; i < bottleCount; i++) {
        for (int j = 0; j < bottleCount; j++) {
            if (i==j) continue;
            indicesPermutations.push_back(Move{i,j});
        }
    }

    // Solve
    std::vector<std::vector<Move>> sequencesSeen;
    std::vector<std::vector<Move>> solutions = runDepthFirstSearch(sequencesSeen, bottles, indicesPermutations);

    std::sort(solutions.begin(), solutions.end(), [](const std::vector<Move> &a, const std::vector<Move> &b) {
        return a.size() < b.size();
    });

    if (solutions.empty()) {
        std::cout << "NO SOLUTIONS" << std::endl;
        std::cout << "Explored " << sequencesSeen.size() << " sequences" << std::endl;
        return 0;
    } else {
        std::cout << "BEST SOLUTION FOUND " << std::endl;
        printMoves(solutions.front());
    }

//    if (FIND_SHORTEST) {
//        while (bestSolution.has_value()) {
//            solutions.push_back(bestSolution.value());
//            bestSolution = depthFirstSearch(std::vector<Move>{}, bottles, &sequencesSeen, indicesPermutations, &shortestSolutionLength).sequence;
//        }
//
//        std::sort(solutions.begin(), solutions.end(), [](const std::vector<Move> &a, const std::vector<Move> &b) {
//            return a.size() < b.size();
//        });
//
//        std::cout << "SHORTEST SOLUTION FOUND " << std::endl;
//        printMoves(solutions.front());
//    }


    return 0;
}
