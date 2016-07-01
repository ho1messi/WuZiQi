#ifndef BOARDSTATUS_H
#define BOARDSTATUS_H

#include <QPoint>

const int chessTypeNum = 9;
const int depthMax = 3;
const int ifinity = 9999999;

enum eChessColor {
    noChess     = 0,
    whiteChess  = -1,
    blackChess  = 10,
    noSpace     = -127
};

enum eChessType {
    FIVE                    = 0,            // 五
    LIVE_FOUR               = 1,            //活四
    DEATH_FOUR              = 2,            //死四
    LIVE_THREE              = 3,            //活三
    DEATH_THREE             = 4,            //死三
    LIVE_TWO                = 5,            //活二
    DEATH_TWO               = 6,            //死二
    ONE                     = 7,            // 一
    NONE_STATU              = 8             //都无
};

const int evaluateMap[chessTypeNum] = {
    ifinity,    //  0   FIVE                     五
    ifinity,    //  1   LIVE_FOUR               活四
    20000,      //  2   DEATH_FOUR              死四
    10000,      //  3   LIVE_THREE              活三
    100,        //  4   DEATH_THREE             死三
    10,         //  5   LIVE_TWO                活二
    1,          //  6   DEATH_TWO               死二
    0,          //  7   ONE                      一
    0,          //  8   NONE_STATU              都无
};

struct StepNode {
    QPoint location;
    char chessColor;
    StepNode* next;
};

struct ChessNode {
    QPoint location;
    ChessNode *next;
};

struct ChessStatus {
    ChessNode *chess;
    QPoint effectiveLoct[6];
    ChessStatus *next;
};

struct ChessStatusHead {
    int num;
    ChessStatus *first;
};

struct pStatusLink {
    ChessStatus *status;
    pStatusLink *next;
};

struct BoardPoint {
    eChessColor chess;
    pStatusLink *whiteStatusEffact;
    pStatusLink *blackStatusEffact;
    pStatusLink *statusIncluded;
};

class BoardStatus
{
public:
    BoardStatus(int boardSize = 15);
    QPoint getNextStep();
    void playChess(int x, int y);

private:
    QPoint lookForward();
    int lookForwardMax(int depth, int alpha, int beta);
    int lookForwardMin(int depth, int alpha, int beta);
    void addChessAt(int x, int y, eChessColor color);
    void removeChessAt(int x, int y);
    bool isOutOfBoard(int x, int y);
    eChessColor haveChess(int x, int y);
    eChessColor haveChess(QPoint & location);
    bool isValid(int x, int y);
    int getEvaluate();
    void updateStatus(int x, int y, eChessColor color, bool arFlag);
    void updateStatusBlack(int x, int y, eChessColor color, bool arFlag);
    void updateStatusWhite(int x, int y, eChessColor color, bool arFlag);
    bool isEmpty();
    QPoint getBoardCenter();
    void addSameColorBS(pStatusLink *StatusEffact, eChessColor color);
    void addDiffColorBS(pStatusLink *StatusEffact, eChessColor color);
    void rmSameColorBS(pStatusLink *StatusIncluded, eChessColor color);
    void rmDiffColorBS(pStatusLink *StatusIncluded, eChessColor color);
    void addStatusOne(eChessColor color);

//调试用函数
private:
    QPoint getNextStep_Debug();
    void printBoard_Debug();

private:
    int boardSize;
    ChessStatusHead blackChessStatus[chessTypeNum];
    ChessStatusHead whiteChessStatus[chessTypeNum];
    int numOfTypesBlack[chessTypeNum];
    int numOfTypesWhite[chessTypeNum];
    BoardPoint **board;
    StepNode *stepLink;
    bool emptyFlag;
    QPoint *newChessToTry;

};

#endif // BOARDSTATUS_H
