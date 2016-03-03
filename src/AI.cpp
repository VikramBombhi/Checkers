#include<iostream>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include "../include/AI.h"
#include "../include/GameState.h"

AI::AI(bool topSide, CheckersBoard *board, Button *buttons): Player(topSide, board, buttons){
}

AI::~AI(){
    delete Board;
    Board = NULL;
    delete boardButtons;
    boardButtons = NULL;
}

int AI::threatCheckArea(int x, int y, directions checkDirection){
    if (x>0 && y<7) {
        switch (checkDirection) {
            case LEFT:
                x -= 1;
                y += 1;
                break;
            case RIGHT:
                x += 1;
                y += 1;
                break;
            case BACK_LEFT:
                x -= 1;
                y -= 1;
                break;
            case BACK_RIGHT:
                x += 1;
                y -= 1;
            default:
                break;
        }
        if(Board->virtualBoard[x][y] == RED_PIECE){
            return RED_PIECE;
        }
        if(Board->virtualBoard[x][y] == BLACK_PIECE){
            return BLACK_PIECE;
        }
        if(Board->virtualBoard[x][y] == EMPTY_PIECE){
            return EMPTY_PIECE;
        }
    }
    return -1;
}

bool AI::killCheckLeft(int x, int y){
    if(x>0 && y<6){
        if(Board->virtualBoard[x-1][y+1] == EMPTY_PIECE){
            return true;
        }
        else{
            return false;
        }
    }
    return false;
}

bool AI::killCheckRight(int x, int y){
    if(x<7 && y<6){
        if(Board->virtualBoard[x+1][y+1] == EMPTY_PIECE){
            return true;
        }
        else{
            return false;
        }
    }
    return false;
}

int AI:: checkLeft(int x, int y, int left){
    if(x<1 || y>8){
        return -999999999;
    }

    if(Board->virtualBoard[x][y] == EMPTY_PIECE){
        left += y;

        //Check if board exits to left
        if(threatCheckArea(x, y, LEFT) == -1){
            return left;
        }
        // Check if move will kill me
        else if(threatCheckArea(x, y, LEFT) == RED_PIECE && threatCheckArea(x, y, BACK_RIGHT) == EMPTY_PIECE){
            left = left + y - KILL_PIECE;
        }

        //Check if board exists to right
        if(threatCheckArea(x, y, RIGHT) == -1){
            return left;
        }
        else if(threatCheckArea(x, y, RIGHT) == RED_PIECE && threatCheckArea(x, y, BACK_LEFT) == EMPTY_PIECE){
            left = left + y - KILL_PIECE;
        }
    }

    if(Board->virtualBoard[x][y] == RED_PIECE){
        //Check if I can kill to left
        if(killCheckLeft(x, y) == true){
            left = left + y + KILL_PIECE;
        }
    }

    if(Board->virtualBoard[x][y] == BLACK_PIECE){
        left -= 999999999;
    }
    return left;
}

int AI:: checkRight(int x, int y,int right){
    if(x>6 || y>8){
        return -999999999;
    }

    if(Board->virtualBoard[x][y] == EMPTY_PIECE){
        right += y;

        //Check if board exits to left
        if(threatCheckArea(x, y, LEFT) == -1){
            return right;
        }
        // Check if move will kill me
        else if(threatCheckArea(x, y, LEFT) == RED_PIECE && threatCheckArea(x, y, BACK_RIGHT) == EMPTY_PIECE){
            right = right + y - KILL_PIECE;
        }

        //Check if board exists to right
        if(threatCheckArea(x, y, RIGHT) == -1){
            return right;
        }
        else if(threatCheckArea(x, y, RIGHT) == RED_PIECE && threatCheckArea(x, y, BACK_LEFT) == EMPTY_PIECE){
            right = right + y - KILL_PIECE;
        }
    }

    if(Board->virtualBoard[x][y] == RED_PIECE){
        //Check if I can kill to right
        if(killCheckRight(x, y) == true){
            right = right + y + KILL_PIECE;
        }
    }

    if(Board->virtualBoard[x][y] == BLACK_PIECE){
        right -= 999999999;
    }
    return right;
}

void AI::moveCheck(int b, int depth){
    if(depth == 0 || team[b].x>8 || team[b].y>8 || team[b].x<0 || team[b].y<0){
        exit(-1);
    }
    
    int left = checkLeft(team[b].x-1, team[b].y+1, 0);
    int right = checkRight(team[b].x+1, team[b].y+1, 0);

    //cout<< "left: " << left << " " << "Right: " << right << "    b: " << b << "  position: " << team[b].x <<"," << team[b].y << endl;
    if(left>right){
        team[b].probability = left;
        team[b].leftVright = 0;
    }
    if(right>left){
        team[b].probability = right;
        team[b].leftVright = 1;
    }
    if(left==right){
        /* initialize random seed: */
        srand (time(NULL));
        /* generate secret number between 1 and 2: */
        int randNum = rand() % 3;
        if(randNum%2==0){
            team[b].probability = left;
            team[b].leftVright = 0;
        }
        else
        {
          team[b].probability = right;
            team[b].leftVright = 1;
        }
        }
}

bool AI::makeMove(SDL_Event *event){
    cout<<"AI's Turn"<<endl;
    for(int b=0;b<team.size();b++){
        moveCheck(b, 10);
    }
    int temp = 0;
    int bestPice = 0;
    for(int b=0;b<team.size();b++){
        if(team[b].probability>temp) {
            //cout<< "new neo: " << team[b].x << team[b].y << "    b: " << b << endl;
            temp = team[b].probability;
            bestPice = b;
        }
    }
    if(team[bestPice].leftVright == 0){
        cout<< "the chosen one: " << team[bestPice].x << "," << team[bestPice].y << "best move: " << team[bestPice].x-1 << "," << team[bestPice].y+1 << endl;
        if(Board->virtualBoard[team[bestPice].x-1][team[bestPice].y+1] == RED_PIECE){
            movePiece(bestPice, team[bestPice].x-2, team[bestPice].y+2);
            return true;
        }
        if(Board->virtualBoard[team[bestPice].x-1][team[bestPice].y+1] == EMPTY_PIECE){
            movePiece(bestPice, team[bestPice].x-1, team[bestPice].y+1);
            return true;
        }
    }
    else{
        cout<< "the chosen one: " << team[bestPice].x << "," << team[bestPice].y << "best move: " << team[bestPice].x+1 << "," << team[bestPice].y+1 << endl;
        if(Board->virtualBoard[team[bestPice].x+1][team[bestPice].y+1] == RED_PIECE){
            movePiece(bestPice, team[bestPice].x+2, team[bestPice].y+2);
            return true;
        }
        if(Board->virtualBoard[team[bestPice].x+1][team[bestPice].y+1] == EMPTY_PIECE){
            movePiece(bestPice, team[bestPice].x+1, team[bestPice].y+1);
            return true;
        }
    }
    return false;
}
