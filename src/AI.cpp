#include<iostream>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include "../include/AI.h"
#include "../include/GameState.h"

AI::AI(bool topSideOfBoard, CheckersBoard *board, Button *buttons): Player(topSideOfBoard, board, buttons){
    currentIndex = 0;
    killMove = false;
}

AI::~AI(){
    delete Board;
    Board = NULL;
    delete boardButtons;
    boardButtons = NULL;
}

int AI::extentValue(int y){
    if (team[currentIndex].isKing()) {
        return 50;
    }
    if (y == 7*topSide) {
        return 100;
    }
    if (topSide) {
        return (5*y);
    }
    else{
        return (35-(y*5));
    }
}

double AI::returnBigger(int left, int right){
    double biggest = 0;
    if(left>right){
        biggest = left;
    }
    if(right>left){
        biggest = right;
    }
    if(left==right){
        /* initialize random seed: */
        srand(static_cast<unsigned int>(time(NULL)));
        /* generate secret number between 1 and 2: */
        int randNum = rand() % 4;
        if(randNum%2==0){
            biggest = left;
        }
        else{
            biggest = right;
        }
    }
    return biggest;
}

int AI::returnRandomIndex(vector<int> bestPiecesList){
    /* initialize random seed: */
    srand(static_cast<unsigned int>(time(NULL)));
    /* generate secret number from 0 to vectorSize*/
    int randPiece = rand() % bestPiecesList.size();
    return bestPiecesList[randPiece];

}

bool AI::changeWithDirection(int *x, int *y, Directions direction){
    switch (direction) {
        case LEFT:
            *x -= 1;
            *y += ONE;
            break;
        case RIGHT:
            *x += 1;
            *y += ONE;
            break;
        case BACK_LEFT:
            *x -= 1;
            *y -= ONE;
            break;
        case BACK_RIGHT:
            *x += 1;
            *y -= ONE;
            break;
        default:
            return false;
            break;
    }
    return true;
}

int AI::threatCheckArea(int x, int y, Directions checkDirection){
    if(!changeWithDirection(&x, &y, checkDirection)){
        return -1;
    }
    if(x<0 || y<0 || y>7 || x>7){
        return -1;
    }
    return Board->virtualBoard[x][y];
}

bool AI::killCheckArea(int x, int y, Directions checkDirection){
    if(!changeWithDirection(&x, &y, checkDirection)){
        return false;
    }
    if(x<0 || y<0 || y>7 || x>7){
        return false;
    }
    if(Board->virtualBoard[x][y] == EMPTY_PIECE){
        return true;
    }
    return false;
}

int AI::checkArea(int x, int y, Directions checkDirection, int points, int depth, int maxDepth, bool isKing){
    if(x<0 || y<0 || y>7 || x>7){
        if (depth == 1) {
                return OUT_OF_BOUND;
            }
            else{
                return points;
            }
    }

    if(sameTeam(Board->virtualBoard[x][y],TEAM_NUMBER)){
        if (depth == 1) {
            return OUT_OF_BOUND;
        }
        else{
            return points;
        }
    }

    if(sameTeam(Board->virtualBoard[x][y],ENEMY_TEAM_NUMBER)){
        //Check if I can kill
        if(killCheckArea(x, y, checkDirection)){
            points += extentValue(y) + KILL_PIECE;
            changeWithDirection(&x, &y, checkDirection);
        }
        else{
            if (depth == 1) {
                return OUT_OF_BOUND;
            }
            else{
                return points;
            }
        }
    }

    switch (checkDirection) {

        case LEFT:
            if(Board->virtualBoard[x][y] == EMPTY_PIECE){
                points += extentValue(y);
                //Check if board exits to left
                if(threatCheckArea(x, y, LEFT) != -1){
                    // Check if move will kill me left
                    if(sameTeam(threatCheckArea(x, y, LEFT), ENEMY_TEAM_NUMBER)){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will kill me from back left
                    if(sameTeam(threatCheckArea(x, y, BACK_LEFT), ENEMY_TEAM_NUMBER) && threatCheckArea(x, y, RIGHT)==EMPTY_PIECE){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will kill me from right
                    if(sameTeam(threatCheckArea(x, y, RIGHT),ENEMY_TEAM_NUMBER) && threatCheckArea(x, y, BACK_LEFT)==EMPTY_PIECE){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will block my team member on left
                    if(sameTeam(threatCheckArea(x, y, LEFT),TEAM_NUMBER) && sameTeam(threatCheckArea(x-1, y, BACK_LEFT),ENEMY_TEAM_NUMBER)){
                        points += extentValue(y) + BLOCK;
                    }
                    //Check if move will block my team member on right
                    if(sameTeam(threatCheckArea(x, y, RIGHT),TEAM_NUMBER) && sameTeam(threatCheckArea(x+1, y+ONE, RIGHT),ENEMY_TEAM_NUMBER)){
                        points += extentValue(y) + BLOCK;
                    }
                }
            }
            break;

        case RIGHT:
            if(Board->virtualBoard[x][y] == EMPTY_PIECE){
                points += extentValue(y);
                //Check if board exists to right
                if(threatCheckArea(x, y, RIGHT) != -1){
                    //Check if move will kill me from right
                    if(sameTeam(threatCheckArea(x, y, RIGHT),ENEMY_TEAM_NUMBER)){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will kill me from back right
                    if(sameTeam(threatCheckArea(x, y, BACK_RIGHT), ENEMY_TEAM_NUMBER) && threatCheckArea(x, y, LEFT)==EMPTY_PIECE){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    // Check if move will kill me from left
                    if(sameTeam(threatCheckArea(x, y, LEFT), ENEMY_TEAM_NUMBER) && threatCheckArea(x, y, BACK_RIGHT) == EMPTY_PIECE){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will block my team from killing me
                    if(sameTeam(threatCheckArea(x, y, LEFT),TEAM_NUMBER) && sameTeam(threatCheckArea(x-1, y+ONE, LEFT),ENEMY_TEAM_NUMBER)){
                        points += extentValue(y) + BLOCK;
                    }
                    //Check if move will block my team from killing me
                    if(sameTeam(threatCheckArea(x, y, RIGHT),TEAM_NUMBER) && sameTeam(threatCheckArea(x+1, y+ONE, RIGHT),ENEMY_TEAM_NUMBER)){
                        points += extentValue(y) + BLOCK;
                    }
                }
            }
            break;

        case BACK_LEFT:
            if(Board->virtualBoard[x][y] == EMPTY_PIECE){
                points += extentValue(y);
                //Check if board exits to left
                if(threatCheckArea(x, y, BACK_LEFT) != -1){
                    //Check if move will kill me from back left
                    if(threatCheckArea(x, y, BACK_LEFT)==ENEMY_TEAM_NUMBER+2){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will kill me from left
                    if(sameTeam(threatCheckArea(x, y, LEFT), ENEMY_TEAM_NUMBER) && threatCheckArea(x, y, BACK_RIGHT)==EMPTY_PIECE){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will kill me from back right
                    if(threatCheckArea(x, y, BACK_RIGHT)==ENEMY_TEAM_NUMBER+2 && threatCheckArea(x, y, LEFT) == EMPTY_PIECE){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will block my team member on left
                    if(sameTeam(threatCheckArea(x, y, LEFT),TEAM_NUMBER) && sameTeam(threatCheckArea(x-1, y+ONE, BACK_LEFT),ENEMY_TEAM_NUMBER)){
                        points += extentValue(y) + BLOCK;
                    }
                    //Check if move will block my team member on back left
                    if(sameTeam(threatCheckArea(x, y, BACK_LEFT),TEAM_NUMBER) && threatCheckArea(x-1, y-ONE, BACK_LEFT)==ENEMY_TEAM_NUMBER+2){
                        points += extentValue(y) + BLOCK;
                    }
                    //Check if move will block my team member on back right
                    if(sameTeam(threatCheckArea(x, y, BACK_RIGHT),TEAM_NUMBER) && threatCheckArea(x+1, y-ONE, BACK_RIGHT)==ENEMY_TEAM_NUMBER+2){
                        points += extentValue(y) + BLOCK;
                    }
                }
            }
        break;

        case BACK_RIGHT:
            if(Board->virtualBoard[x][y] == EMPTY_PIECE){
                points += extentValue(y);
                //Check if board exists to right
                if(threatCheckArea(x, y, BACK_RIGHT) != -1){
                    //Check if move will kill me from right
                    if(sameTeam(threatCheckArea(x, y, RIGHT), ENEMY_TEAM_NUMBER) && threatCheckArea(x, y, BACK_LEFT)==EMPTY_PIECE){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will kill me from back right
                    if(threatCheckArea(x, y, BACK_RIGHT)==ENEMY_TEAM_NUMBER+2){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will kill me from back left
                    if(threatCheckArea(x, y, BACK_LEFT)==ENEMY_TEAM_NUMBER+2 && threatCheckArea(x, y, RIGHT) == EMPTY_PIECE){
                        points += extentValue(y) - KILL_PIECE;
                    }
                    //Check if move will block my team member from right
                    if(sameTeam(threatCheckArea(x, y, RIGHT),TEAM_NUMBER) && sameTeam(threatCheckArea(x+1, y+ONE, RIGHT),ENEMY_TEAM_NUMBER)){
                        points += extentValue(y) + BLOCK;
                    }
                    //Check if move will block my team member from back right
                    if(sameTeam(threatCheckArea(x, y, BACK_RIGHT),TEAM_NUMBER) && threatCheckArea(x+1, y-ONE, BACK_RIGHT)==ENEMY_TEAM_NUMBER+2){
                        points += extentValue(y) + BLOCK;
                    }
                    //Check if move will block my team member from back left
                    if(sameTeam(threatCheckArea(x, y, BACK_LEFT),TEAM_NUMBER) && threatCheckArea(x-1, y-ONE, BACK_LEFT)==ENEMY_TEAM_NUMBER+2){
                        points += extentValue(y) + BLOCK;
                    }
                }
            }
            break;

        default:
            break;
    }
    if (depth >= maxDepth) {
        //cout<<"depth:    "<<depth<<"    points: "<<endl;
        return points;
    }
    else{
        if(isKing == true){
            //cout<<"depth:    "<<depth<<"    points: "<<endl;
            return (points += returnBigger(returnBigger(checkArea(x-1, y+ONE, LEFT, points, depth++, maxDepth, true),checkArea(x+1,y+ONE, RIGHT, points, depth++, maxDepth, true))/depth, returnBigger(checkArea(x-1, y-ONE, BACK_LEFT, points, depth++, maxDepth, true),checkArea(x+1,y-ONE, BACK_RIGHT, points, depth++, maxDepth, true))/depth));
        }
        //cout<<checkArea(x-1, y+ONE, LEFT, points, depth++, maxDepth, false)/depth<<" ||  "<<"right:  "<<checkArea(x+1,y+ONE, RIGHT, points, depth++, maxDepth, false)/depth<<endl;
        return (points += returnBigger(checkArea(x-1, y+ONE, LEFT, points, depth++, maxDepth, false),checkArea(x+1,y+ONE, RIGHT, points, depth++, maxDepth, false))/depth);
    }
    return 0;
}

void AI::moveCheck(int index, int maxDepth){
    if(maxDepth == 0 || team[index].x>8 || team[index].y>8 || team[index].x<0 || team[index].y<0){
        exit(-1);
    }

    double left = 0;
    double right = 0;
    double backLeft = 0;
    double backRight = 0;

    // Case 1: King piece, need to check every direction //
    if (team[index].isKing()) {
        left = checkArea(team[index].x-1, team[index].y+ONE, LEFT, left, 1, maxDepth, true);
        right = checkArea(team[index].x+1, team[index].y+ONE, RIGHT, right, 1, maxDepth, true);
        backLeft = checkArea(team[index].x-1, team[index].y-ONE, BACK_LEFT, backLeft, 1, maxDepth, true);
        backRight = checkArea(team[index].x+1, team[index].y-ONE, BACK_RIGHT, backLeft, 1, maxDepth, true);

        int largest = left;
        int bestDirection = LEFT;

        if (right > largest) {
            largest = right;
            bestDirection = RIGHT;
        }
        if (backLeft > largest) {
            largest = backLeft;
            bestDirection = BACK_LEFT;
        }
        if (backRight > largest) {
            largest = backRight;
            bestDirection = BACK_RIGHT;
        }
        if(left==right && right == backLeft && backLeft == backRight){
            /* initialize random seed: */
            srand(static_cast<unsigned int>(time(NULL)));
            /* generate secret number between 0 and 3: */
            int randNum = rand() % 4;
            switch (randNum) {
                case LEFT:
                    largest = left;
                    bestDirection = LEFT;
                    break;
                case RIGHT:
                    largest = right;
                    bestDirection = RIGHT;
                    break;
                case BACK_LEFT:
                    largest = backLeft;
                    bestDirection = BACK_LEFT;
                    break;
                case BACK_RIGHT:
                    largest = backLeft;
                    bestDirection = BACK_RIGHT;
                    break;
                default:
                    break;
            }
        }
        team[index].probability = largest;
        team[index].bestDirection = bestDirection;

    }
    // Case 2: Not king piece, only check left and right //
    else{
        //cout<<"Check Left"<<endl;
        left = checkArea(team[index].x-1, team[index].y+ONE, LEFT, left, 1, maxDepth, false);
        //cout<<"Check Right"<<endl;
        right = checkArea(team[index].x+1, team[index].y+ONE, RIGHT, right, 1, maxDepth, false);

        if(left>right){
            team[index].probability = left;
            team[index].bestDirection = LEFT;
        }
        if(right>left){
            team[index].probability = right;
            team[index].bestDirection = RIGHT;
        }
        if(left==right){
            /* initialize random seed: */
            srand(static_cast<unsigned int>(time(NULL)));
            /* generate secret number between 1 and 2: */
            int randNum = rand() % 4;
            if(randNum%2==0){
                team[index].probability = left;
                team[index].bestDirection = LEFT;
            }
            else{
                team[index].probability = right;
                team[index].bestDirection = RIGHT;
            }
        }
    }
    cout<< "index: " << index<< " L: " << left << " " << " R: " << right  << " BL: " <<backLeft<< " BR: " << backRight <<"  position: " << team[index].x <<"," << team[index].y << endl;
}

bool AI::makeMove(SDL_Event *event){
    cout<<"AI's Turn"<<endl;

    for(int index=0;index<team.size();index++){
        currentIndex = index;
        moveCheck(index, 7);
    }

    vector<int> bestPiecesList;
    double biggestProabability = team[0].probability;

    for(int teamIndex=0;teamIndex<team.size();teamIndex++){
        // If probability is the same, will stick with the first index
        if(team[teamIndex].probability > biggestProabability) {
            biggestProabability = team[teamIndex].probability;
        }
    }

    for(int teamIndex=0;teamIndex<team.size();teamIndex++){
        if(team[teamIndex].probability >= biggestProabability){
            bestPiecesList.push_back(teamIndex);
        }
    }
    int bestPieceIndex = returnRandomIndex(bestPiecesList);
    cout<< "the chosen one: " << bestPieceIndex << " -> "<< team[bestPieceIndex].x << "," << team[bestPieceIndex].y;

    int x = team[bestPieceIndex].x;
    int y = team[bestPieceIndex].y;

    // Makes sure the move isnt out of bounds //
    if (team[bestPieceIndex].probability != OUT_OF_BOUND) {

        switch (team[bestPieceIndex].bestDirection) {
            case LEFT:
                changeWithDirection(&x, &y, LEFT);
                if(sameTeam(Board->virtualBoard[x][y],ENEMY_TEAM_NUMBER)){
                    // Changes it again for moving 2 units diagonally //
                    changeWithDirection(&x, &y, LEFT);
                }
                cout<< " best move: " << x << "," << y << endl;
                movePiece(bestPieceIndex, x, y);
                return true;

            case RIGHT:
                changeWithDirection(&x, &y, RIGHT);
                if (sameTeam(Board->virtualBoard[x][y], ENEMY_TEAM_NUMBER)) {
                    // Changes it again for moving 2 units diagonally //
                    changeWithDirection(&x, &y, RIGHT);
                }
                cout<< " best move: " << x << "," << y << endl;
                movePiece(bestPieceIndex, x, y);
                return true;

            case BACK_LEFT:
                changeWithDirection(&x, &y, BACK_LEFT);
                if (sameTeam(Board->virtualBoard[x][y], ENEMY_TEAM_NUMBER)) {
                    // Changes it again for moving 2 units diagonally //
                    changeWithDirection(&x, &y, BACK_LEFT);
                }
                cout<< " best move: " << x << "," << y << endl;
                movePiece(bestPieceIndex, x, y);
                return true;

            case BACK_RIGHT:
                changeWithDirection(&x, &y, BACK_RIGHT);
                if (sameTeam(Board->virtualBoard[x][y], ENEMY_TEAM_NUMBER)) {
                    // Changes it again for moving 2 units diagonally //
                    changeWithDirection(&x, &y, BACK_RIGHT);
                }
                cout<< " best move: " << x << "," << y << endl;
                movePiece(bestPieceIndex, x, y);
                return true;

            default:
                break;
        }
    }
    return false;
}
