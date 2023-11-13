// DO NOT REMOVE THE FOLLOWING LINE
#if !defined(TESTER_P1) && !defined(TESTER_P2)
// DO NOT REMOVE THE ABOVE LINE
#include "reversi.h"
// DO NOT REMOVE THE FOLLOWING LINE
#endif
// DO NOT REMOVE THE ABOVE LINE
#include <limits.h>
// #include <sys/time.h>
// #include <sys/resource.h>


void printBoard(char board[][26], int n) {
    char alpha_row;
    char alpha_col;
    for(int row=0; row<=n;row++){
        for(int col=0; col<=n;col++){
            if ((row==0)&&(col==0)){
                printf("  ");
            }else if (row==0){
                alpha_row='a'+col-1;
                printf("%c", alpha_row);    
            }else if(col==0){
                alpha_col='a'+row-1;
                printf("%c ",alpha_col);
            }else{
                printf("%c",board[row-1][col-1]);
            }
        }
        printf("\n");
    }
}

bool positionInBounds(int n, int row, int col) {
    return (row<n&&row>=0)&&(col<n&&col>=0);
}

bool checkLegalInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol) {
    char oppoColor = colour == 'B' ? 'W' : 'B';
    if (board[row][col] != 'U') {
        return false;
    }
    
    row += deltaRow;
    col += deltaCol;

    if (!(positionInBounds(n, row, col) && board[row][col] == oppoColor)) {
        return false;
    }

    do {
        row += deltaRow;
        col += deltaCol;
    }
    while (positionInBounds(n, row, col) && board[row][col] == oppoColor);

    return (positionInBounds(n, row, col) && board[row][col] == colour);
}

bool checkLegalSimple(char board[][26], int n, int row, int col, char colour) {
    for(int i=-1;i<=1;i++){
        for(int j=-1;j<=1;j++){
            if(i!=0||j!=0){
                if(checkLegalInDirection(board,n,row,col,colour,i,j)){
                    return true;
                }
            }
        }
    }
    return false;
}

bool MoveEnd(char board[][26], int n, char colour){
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      if(checkLegalSimple(board,n,i,j,colour)){
        return false;
      }
    }
  }
  return true;
}

bool isUnstable(const char board[][26], int n, int r, int c) {
    for (int h = -1; h <= 1; h++) {
        for (int v = -1; v <= 1; v++) {
            if (!(h == 0 && v == 0)) {
                if (positionInBounds(n, r + h, c + v) && board[r + h][c + v] == 'U') {
                    return true;
                }
            }
        }
    }
    return false;
}

int countLegalMoves(const char board[][26], int n, char turn) {
    int currIdx = 0;
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (checkLegalSimple(board, n, r, c, turn)) {
                currIdx++;
            }
        }
    }
    return currIdx;
}

int getScore(const char board[][26], int n, char ownColor) { 
    // final score is a weighted average of different metrics
    // the weights used in this code referred to an implementation of Kartik Kukreja
    // at https://kartikkukreja.wordpress.com/2013/03/30/heuristic-function-for-reversiothello/

    int V[8][8] = {0.2, -0.03, 0.11, 0.08, 0.08, 0.11, -0.03, 0.2, 
                   -0.03, -0.07, -0.04, 0.01, 0.01, -0.04, -0.07, -0.03, 
                   0.11, -0.04, 0.02, 0.02, 0.02, 0.02, -0.04, 0.11, 
                   0.08, 0.01, 0.02, -0.03, -0.03, 0.02, 0.01, 0.08, 
                   0.08, 0.01, 0.02, -0.03, -0.03, 0.02, 0.01, 0.08, 
                   0.11, -0.04, 0.02, 0.02, 0.02, 0.02, -0.04, 0.11, 
                   -0.03, -0.07, -0.04, 0.01, 0.01, -0.04, -0.07, -0.03, 
                   0.2, -0.03, 0.11, 0.08, 0.08, 0.11, -0.03, 0.2};


    // piece differences and number of pieces with at least one unoccupied tile around it
    // for convenience we call this "unstable" pieces


    char oppoColor = (ownColor == 'B') ? 'W' : 'B';
    // gets the score difference by having black piece cnt - white piece cnt
    int cntDiff = 0;
    int totalCnt = 0;
    int unstableDiff = 0;
    int totalUnstable = 0;
    double stableDegree = 0;

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (board[r][c] == ownColor) {
                cntDiff++;
                totalCnt++;
                stableDegree += V[r][c];
                if (isUnstable(board, n, r, c)) {
                    unstableDiff++;
                    totalUnstable++;
                }
            } else if (board[r][c] != 'U') {
                cntDiff--;
                totalCnt++;
                stableDegree -= V[r][c];
                if (isUnstable(board, n, r, c)) {
                    unstableDiff--;
                    totalUnstable++;
                }
            }
        }
    }

    double cntMetric = totalCnt == 0 ? 0 : (double)cntDiff / (double)totalCnt;
    double unstableMetric = totalUnstable == 0 ? 0 : -(double)unstableDiff / (double)totalUnstable;


    // count diff of number of corner pieces
    int cornerDiff = 0;
    if (board[0][0] == ownColor) {
        cornerDiff++;
    } else if (board[0][0] == oppoColor) {
        cornerDiff--;
    }
    if (board[0][n - 1] == ownColor) {
        cornerDiff++;
    } else if (board[0][n - 1] == oppoColor) {
        cornerDiff--;
    }
    if (board[n - 1][0] == ownColor) {
        cornerDiff++;
    } else if (board[n - 1][0] == oppoColor) {
        cornerDiff--;
    }
    if (board[n - 1][n - 1] == ownColor) {
        cornerDiff++;
    } else if (board[n - 1][n - 1] == oppoColor) {
        cornerDiff--;
    }

    double cornerMetric = 0.25 * (double)cornerDiff;

    // count number of pieces immediately next to corner
    int cornerNeighborDiff = 0;
    if (board[0][0] == 'U') {
        if (board[0][1] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[0][1] == oppoColor) {
            cornerNeighborDiff--;
        }
        if (board[1][1] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[1][1] == oppoColor) {
            cornerNeighborDiff--;
        }
        if (board[1][0] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[1][0] == oppoColor) {
            cornerNeighborDiff--;
        }
    } 
    if (board[0][n - 1] == 'U') {
        if (board[0][n - 2] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[0][n - 2] == oppoColor) {
            cornerNeighborDiff--;
        }
        if (board[1][n - 1] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[1][n - 1] == oppoColor) {
            cornerNeighborDiff--;
        }
        if (board[1][n - 2] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[1][n - 2] == oppoColor) {
            cornerNeighborDiff--;
        }
    } 
    if (board[n - 1][0] == 'U') {
        if (board[n - 2][0] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[n - 2][0] == oppoColor) {
            cornerNeighborDiff--;
        }
        if (board[n - 1][1] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[n - 1][1] == oppoColor) {
            cornerNeighborDiff--;
        }
        if (board[n - 2][1] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[n - 2][1] == oppoColor) {
            cornerNeighborDiff--;
        }
    } 
    if (board[n - 1][n - 1] == 'U') {
        if (board[n - 2][n - 1] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[n - 2][n - 1] == oppoColor) {
            cornerNeighborDiff--;
        }
        if (board[n - 1][n - 2] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[n - 1][n - 2] == oppoColor) {
            cornerNeighborDiff--;
        }
        if (board[n - 2][n - 2] == ownColor) {
            cornerNeighborDiff++;
        } else if (board[n - 2][n - 2] == oppoColor) {
            cornerNeighborDiff--;
        }
    } 

    double cornerNeighborMetric = -0.125 * (double)cornerNeighborDiff;

    // calculate mobility 
    int myMoveCnt = countLegalMoves(board, n, ownColor);
    int oppoMoveCnt = countLegalMoves(board, n, oppoColor);
    int totalMoveCnt = myMoveCnt + oppoMoveCnt;
    double mobilityMetric = totalMoveCnt == 0 ? 0 : (double)(myMoveCnt - oppoMoveCnt) / (double)(myMoveCnt + oppoMoveCnt);
    double weightedScore;
    if (n == 8) {
        weightedScore = cntMetric * 10 + cornerMetric * 800 + cornerNeighborMetric * 380 + mobilityMetric * 79 + unstableMetric * 75 + stableDegree * 10;
    } else {
        weightedScore = cntMetric * 10 + cornerMetric * 800 + cornerNeighborMetric * 380 + mobilityMetric * 79 + unstableMetric * 75;
    }
    return weightedScore;
}

int getLegalMoves(const char board[][26], int n, char turn, int* moveX, int* moveY) {
    int currIdx = 0;
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (checkLegalSimple(board, n, r, c, turn)) {
                moveX[currIdx] = r;
                moveY[currIdx] = c;
                currIdx++;
            }
        }
    }
    return currIdx;
}

void flip(char board[][26], int n, char ownColor, int row, int col) {
    char oppoColor = ownColor == 'B' ? 'W' : 'B';
    for (int h = -1; h <= 1; h++) {
        for (int v = -1; v <= 1; v++) {
            if (!(h == 0 && v == 0)) {
                if (checkLegalInDirection(board, n, row, col, ownColor, h, v)) {
                    int currR = row, currC = col;
                    do {
                        board[currR][currC] = ownColor;
                        currR += h;
                        currC += v;
                    } 
                    while (positionInBounds(n, currR, currC) && board[currR][currC] == oppoColor);
                }
            }
        }
    }
}

int minimax(char board[][26], int n, char myColor, char currColor, int depth, int alpha, int beta) {
    if (depth == 5 || (MoveEnd(board, n, 'B') && MoveEnd(board, n, 'W'))) {
        return getScore(board, n, myColor);
    }
    // get all possible moves
    // since board max size is 26^2 and 4 positions are taken, a max of 672 possible places are there
    int moveX[672];
    int moveY[672];

    int moveCnt = getLegalMoves(board, n, currColor, moveX, moveY);

    char nextColor = (currColor == 'B') ? 'W' : 'B';

    if (moveCnt == 0) {
        return minimax(board, n, myColor, nextColor, depth, alpha, beta);
    }

    int bestScore = currColor == myColor ? INT_MIN : INT_MAX;
    
    for (int i = 0; i < moveCnt; i++) {
        char temp[26][26];
        
        memcpy(temp, board, 26 * 26 * sizeof(char));
        flip(temp, n, currColor, moveX[i], moveY[i]);
        
        int score = minimax(temp, n, myColor, nextColor, depth + 1, alpha, beta);

        if (currColor == myColor) {
            bestScore = score > bestScore ? score : bestScore;
            alpha = score > alpha ? score : alpha;
            if (beta <= alpha) {
                break;
            }
        } else {
            bestScore = score < bestScore ? score : bestScore;
            beta = score < beta ? score : beta;
            if (beta <= alpha) {
                break;
            }
        }
    }
    return bestScore;
}

int makeMove(char board[][26], int n, char turn, int *row, int *col) {
    // get all possible moves
    // since board max size is 26^2 and 4 positions are taken, a max of 672 possible places are there
    // struct rusage usage; // a structure to hold "resource usage" (including time)
    // struct timeval start, end; // will hold the start and end times
    // getrusage(RUSAGE_SELF, &usage);
    // start = usage.ru_utime;
    // double timeStart = start.tv_sec + start.tv_usec / 1000000.0; // in seconds

    int moveX[672];
    int moveY[672];

    int moveCnt = getLegalMoves(board, n, turn, moveX, moveY);
    
    int bestScore = INT_MIN;

    *row = moveX[0], *col = moveY[0];
    char oppoColour = turn == 'B' ? 'W' : 'B';
    for (int i = 0; i < moveCnt; i++) {
        // end = usage.ru_utime;
        // double timeEnd = start.tv_sec + start.tv_usec / 1000000.0; // in seconds
        // if (timeEnd - timeStart > 0.9) break;
        char temp[26][26];
        memcpy(temp, board, 26 * 26 * sizeof(char));
        flip(temp, n, turn, moveX[i], moveY[i]);
        
        int score = minimax(temp, n, turn, oppoColour, 1, INT_MIN, INT_MAX);
        if (score > bestScore) {
            bestScore = score;
            *row = moveX[i];
            *col = moveY[i];
        }
    }
    return 0;
}

//*******************************************************
// Note: Please only put your main function below
// DO NOT REMOVE THE FOLLOWING LINE
#ifndef TESTER_P2
// DO NOT REMOVE THE ABOVE LINE


int main(void) {
    char board2[26][26];
    int size_board;
    //initialize the board, dimension

    printf("Enter the board dimension: ");
    scanf("%d",&size_board);

    for(int row=0; row<size_board;row++){
        for(int col=0; col<size_board;col++){
            board2[row][col]='U';
        }
    }

    board2[size_board/2-1][size_board/2-1]='W';
    board2[size_board/2-1][size_board/2]='B';
    board2[size_board/2][size_board/2-1]='B';
    board2[size_board/2][size_board/2]='W';

    char computer_color;
    printf("Computer plays (B/W): ");
    scanf(" %c", &computer_color);
    char user_color = (computer_color == 'B') ? 'W' : 'B';
    //user_colour is the opposite of computer_colour
    printBoard(board2, size_board);

    char turn='B';
    bool next_loop;
    bool user_lose;

    while(!(MoveEnd(board2,size_board,'B')&&MoveEnd(board2,size_board,'W'))){
        next_loop=false;
        if (turn == 'B') {
            if(MoveEnd(board2,size_board,'B')){
                printf("%c player has no valid move.\n",'B');
                turn='W';
                next_loop=true;
            } 
        } else {
            if(MoveEnd(board2,size_board,'W')){
                printf("%c player has no valid move.\n",'W');
                turn='B';
                next_loop=true;
            }
        }
        
        if(!next_loop){
            if(turn==computer_color){
                int row, col;
                makeMove(board2,size_board,computer_color,&row,&col);
                printf("Computer places %c at %c%c.\n",computer_color,'a'+row,'a'+col);
                flip(board2, size_board, computer_color, row, col);
                turn=user_color;
                printBoard(board2,size_board);
            }else{
                int row, col;
                char user_move[3];
                printf("Enter move for colour %c (RowCol): ", user_color);
                scanf("%s",user_move);
                row=user_move[0]-'a';
                col=user_move[1]-'a';
                if(!checkLegalSimple(board2,size_board,row, col, user_color)){
                    printf("Invalid move.\n");
                    user_lose = true;
                    break;
                }else{
                    flip(board2, size_board, user_color, row, col);
                }
                printBoard(board2,size_board);
                turn=computer_color;
            }
        }
    }
    int B_score=0,W_score=0;
    for(int i=0;i<size_board;i++){
        for(int j=0;j<size_board;j++){
            if(board2[i][j]=='B'){
                B_score++;
            }else if(board2[i][j]=='W'){
                W_score++;
            }
        }
    }

    if(user_lose){
        printf("%c player wins.\n",computer_color);
    }else if(B_score>W_score){
        printf("%c player wins.\n",'B');
    }else if (B_score<W_score){
        printf("%c player wins.\n",'W');
    }else if (B_score==W_score){
        printf("Draw!\n");
    }
}

// DO NOT REMOVE THE FOLLOWING LINE
#endif
// DO NOT REMOVE THE ABOVE LINE
//*******************************************************