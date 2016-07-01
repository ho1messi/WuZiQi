#ifndef BOARDSTATUS_H
#define BOARDSTATUS_H

#include <QPoint>

const int chessTypeNum = 23;
const int depthMax = 3;
const int ifinity = 2147483647;         //2^31 - 1

enum eDirection {
    LR = 0,         //左右方向
    TB = 1,         //上下方向
    LO = 2,         //左斜方向
    RO = 3          //右斜方向
};

enum eChessColor {
    noChess     = 0,
    whiteChess  = -1,
    blackChess  = 10,
    noSpace     = -127
};

enum eChessType {
    FIVE                    = 0 ,            // 五
    FIVE_1                  = 1 ,
    LIVE_FOUR               = 2 ,            //活四
    DEATH_FOUR              = 3 ,            //死四
    DEATH_FOUR_1            = 4 ,
    DEATH_FOUR_2            = 5 ,
    LIVE_THREE              = 6 ,            //活三
    LIVE_THREE_1            = 7 ,
    DEATH_THREE             = 8 ,            //死三
    DEATH_THREE_1           = 9 ,
    DEATH_THREE_2           = 10,
    DEATH_THREE_3           = 11,
    LIVE_TWO                = 12,            //活二
    LIVE_TWO_1              = 13,
    LIVE_TWO_2              = 14,
    DEATH_TWO               = 15,            //死二
    DEATH_TWO_1             = 16,
    DEATH_TWO_2             = 17,
    DEATH_TWO_3             = 18,
    DEATH_TWO_4             = 19,
    LIVE_ONE                = 20,            //活一
    DEATH_ONE                = 21,            //死一
    NONE_STATU              = 22             //都无
};

//白棋评分表
const int evaluateMapWhite[chessTypeNum] = {
    10000000,   //  0   FIVE                     五
    1000000,    //  1   FIVE_1
    3000000,    //  2   LIVE_FOUR               活四
    800000,     //  3   DEATH_FOUR              死四
    200000,     //  4   DEATH_FOUR_1
    40000,      //  5   DEATH_FOUR_2
    50000,      //  6   LIVE_THREE              活三
    30000,      //  7   LIVE_THREE_1
    500,        //  8   DEATH_THREE             死三
    400,        //  9   DEATH_THREE_1
    300,        //  10  DEATH_THREE_2
    100,        //  11  DEATH_THREE_3
    80,         //  12  LIVE_TWO                活二
    70,         //  13  LIVE_TWO_1
    30,         //  14  LIVE_TWO_2
    20,         //  15  DEATH_TWO               死二
    18,         //  16  DEATH_TWO_1
    16,         //  17  DEATH_TWO_2
    14,         //  18  DEATH_TWO_3
    10,         //  19  DEATH_TWO_4
    8,          //  20  LIVE_ONE                活一
    2,          //  21  DEATH_ONE               死一
    0,          //  22  NONE_STATU              都无
};

//黑棋评分表
const int evaluateMapBlack[chessTypeNum] = {
    100000000,  //  0   FIVE                     五
    30000000,   //  1   FIVE_1
    60000000,   //  2   LIVE_FOUR               活四
    40000000,   //  3   DEATH_FOUR              死四
    20000000,   //  4   DEATH_FOUR_1
    40000,      //  5   DEATH_FOUR_2
    80000,      //  6   LIVE_THREE              活三
    50000,      //  7   LIVE_THREE_1
    500,        //  8   DEATH_THREE             死三
    400,        //  9   DEATH_THREE_1
    300,        //  10  DEATH_THREE_2
    100,        //  11  DEATH_THREE_3
    80,         //  12  LIVE_TWO                活二
    70,         //  13  LIVE_TWO_1
    30,         //  14  LIVE_TWO_2
    20,         //  15  DEATH_TWO               死二
    18,         //  16  DEATH_TWO_1
    16,         //  17  DEATH_TWO_2
    14,         //  18  DEATH_TWO_3
    10,         //  19  DEATH_TWO_4
    8,          //  20  LIVE_ONE                活一
    2,          //  21  DEATH_ONE               死一
    0,          //  22  NONE_STATU              都无
};

struct BoardPoint {
    eChessColor chess;
    bool boardFlags[4];
};

class BoardStatus
{
public:
    BoardStatus(int boardSize = 15);
    QPoint getNextStep();
    void playChess(int x, int y);

private:
    void lookForward();
    void lookForwardOnce();
    int ABNegaMax (int depth, int alpha, int beta);
    inline void addChessAt(int x, int y, eChessColor color);
    inline void removeChessAt(int x, int y);
    bool isOutOfBoard(int x, int y);
    eChessColor haveChess(int x, int y);
    eChessColor haveChess(const QPoint & location);
    inline bool isValid(int x, int y);
    int getEvaluate();
    int getEvaluateDouble();
    void clearBoardFlag();
    void updateStatus();
    inline void checkChessStatus(const QPoint & chess, eDirection direct);
    inline void getNextPoint(QPoint & chess, eDirection direct, bool directFlag);
    inline void getHashValue(unsigned long bitMap, int bitWidth, eChessColor color, bool doubleFlag);
    bool isEmpty();
    QPoint getBoardCenter();
    void setBoardFlag(const QPoint & chess, eDirection direct);
    void setBoardFlag(int x, int y, eDirection direct);
    bool canWin();
    void getKeyChess();
    bool isWin(int x, int y);

//调试用函数
private:
    QPoint getNextStep_Debug();
    void printBoard_Debug();
    void printTypeNum();

public:
    void getBoard(char board[][15]);
    void getBoardStatus(QPoint chess);

private:
    int boardSize;
    int numOfTypesBlack[chessTypeNum];
    int numOfTypesWhite[chessTypeNum];
    BoardPoint **board;
    bool emptyFlag;
    QPoint *nextStep[10];
};

#endif // BOARDSTATUS_H
