#include "boardstatus.h"
#include <QDebug>
#include <QString>
#include <QTime>

eChessType hashTable[64] = {
    NONE_STATU          ,   //  0   000000
    DEATH_ONE            ,   //  1   000001
    LIVE_ONE            ,   //  2   000010
    DEATH_TWO           ,   //  3   000011
    LIVE_ONE            ,   //  4   000100
    DEATH_TWO_1         ,   //  5   000101
    LIVE_TWO            ,   //  6   000110
    DEATH_THREE         ,   //  7   000111
    LIVE_ONE            ,   //  8   001000
    DEATH_TWO_2         ,   //  9   001001
    LIVE_TWO_1          ,   //  10  001010
    DEATH_THREE_1       ,   //  11  001011
    LIVE_TWO            ,   //  12  001100
    DEATH_THREE_1       ,   //  13  001101
    LIVE_THREE          ,   //  14  001110
    DEATH_FOUR          ,   //  15  001111
    LIVE_ONE            ,   //  16  010000
    LIVE_ONE            ,   //  17  010001
    LIVE_TWO_2          ,   //  18  010010
    DEATH_THREE_2       ,   //  19  010011
    LIVE_TWO_1          ,   //  20  010100
    DEATH_THREE_2       ,   //  21  010101
    LIVE_THREE_1        ,   //  22  010110
    DEATH_FOUR_1        ,   //  23  010111
    LIVE_TWO            ,   //  24  011000
    DEATH_THREE_2       ,   //  25  011001
    LIVE_THREE_1        ,   //  26  011010
    DEATH_FOUR_1        ,   //  27  011011
    LIVE_THREE          ,   //  28  011100
    DEATH_FOUR_1        ,   //  29  011101
    LIVE_FOUR           ,   //  30  011110
    FIVE                ,   //  31  011111
    DEATH_ONE            ,   //  32  100000
    DEATH_TWO_4         ,   //  33  100001
    DEATH_TWO_3         ,   //  34  100010
    DEATH_THREE_3       ,   //  35  100011
    DEATH_TWO_2         ,   //  36  100100
    DEATH_TWO           ,   //  37  100101
    LIVE_TWO            ,   //  38  100110
    DEATH_FOUR_2        ,   //  39  100111
    DEATH_TWO_1         ,   //  40  101000
    DEATH_THREE_3       ,   //  41  101001
    DEATH_THREE_2       ,   //  42  101010
    DEATH_FOUR_2        ,   //  43  101011
    DEATH_THREE_1       ,   //  44  101100
    DEATH_FOUR_2        ,   //  45  101101
    DEATH_FOUR_1        ,   //  46  101110
    FIVE_1              ,   //  47  101111
    DEATH_TWO           ,   //  48  110000
    DEATH_THREE_3       ,   //  49  110001
    DEATH_THREE_2       ,   //  50  110010
    DEATH_FOUR_2        ,   //  51  110011
    DEATH_THREE_1       ,   //  52  110100
    DEATH_FOUR_2        ,   //  53  110101
    DEATH_FOUR_1        ,   //  54  110110
    FIVE_1              ,   //  55  110111
    DEATH_THREE         ,   //  56  111000
    DEATH_FOUR_2        ,   //  57  111001
    DEATH_FOUR_1        ,   //  58  111010
    FIVE_1              ,   //  59  111011
    DEATH_FOUR          ,   //  60  111100
    FIVE_1              ,   //  61  111101
    FIVE                ,   //  62  111110
    FIVE                    //  63  111111
};

BoardStatus::BoardStatus(int boardSize)
{
    this->board = new BoardPoint * [boardSize];
    this->boardSize = boardSize;
    this->emptyFlag = true;
    this->newChessToTry = new QPoint(0, 0);

    for (int i = 0; i < 10; i++)
    {
        this->nextStep[i] = new QPoint(0, 0);
    }

    for (int i = 0; i < boardSize; i++)
    {
        this->board[i] = new BoardPoint[boardSize];

        for (int j = 0; j < boardSize; j++)
        {
            this->board[i][j].chess = noChess;
            this->board[i][j].boardFlags[LR] = false;
            this->board[i][j].boardFlags[TB] = false;
            this->board[i][j].boardFlags[LO] = false;
            this->board[i][j].boardFlags[RO] = false;
        }
    }

    for (int i = 0; i < chessTypeNum; i++)
    {
        this->numOfTypesBlack[i] = 0;
        this->numOfTypesWhite[i] = 0;
    }
}

QPoint BoardStatus::getNextStep()
{
    if (this->isEmpty())
    {
        return getBoardCenter();
    }

    lookForward();

    if (canWin())
    {
        updateStatus();
        lookForwardOnce();
    }

    addChessAt(this->nextStep[0]->x(), this->nextStep[0]->y(), whiteChess);

    //printTypeNum();

    return *this->nextStep[0];
}

//调试用
QPoint BoardStatus::getNextStep_Debug()
{
    if (this->isEmpty())
    {
        return getBoardCenter();
    }

    QPoint nextStep;

    do {
        nextStep.setX(rand() % boardSize);
        nextStep.setY(rand() % boardSize);
    } while (haveChess(nextStep));

    addChessAt(nextStep.x(), nextStep.y(), whiteChess);
    printBoard_Debug();

    return nextStep;
}

//调试用
void BoardStatus::printBoard_Debug()
{
    QString s("");
    char c;

    for (int i = 0; i < boardSize; i++)
    {
        s = "";
        for (int j = 0; j < boardSize; j++)
        {
            if (board[j][i].chess == blackChess)
                c = '@';
            else if (board[j][i].chess == whiteChess)
                c = 'O';
            else
                c = '+';
            s = s + "   " + c;
        }
        qDebug() << s;
    }

    qDebug() << endl;
}

void BoardStatus::printTypeNum()
{
    for (int i = 0; i < chessTypeNum; i++)
    {
        qDebug() << "BLACK--" << numOfTypesBlack[chessTypeNum - i] << chessTypeNum - i;
        qDebug() << "WHITE--" << numOfTypesWhite[chessTypeNum - i] << chessTypeNum - i;
    }
    qDebug() << "-------------------------------" << endl;
}

//调试用
void BoardStatus::getBoard(char board[][15])
{
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            switch (board[i][j])
            {
            case 0:
                this->board[i][j].chess = noChess;
                break;
            case 1:
                this->board[i][j].chess = whiteChess;
                break;
            case 2:
                this->board[i][j].chess = blackChess;
                break;
            }
        }
    }

    printBoard_Debug();
}

void BoardStatus::getBoardStatus(QPoint chess)
{
    qDebug() << "LR-----" << this->board[chess.x()][chess.y()].boardFlags[LR];
    qDebug() << "TB-----" << this->board[chess.x()][chess.y()].boardFlags[TB];
    qDebug() << "LO-----" << this->board[chess.x()][chess.y()].boardFlags[LO];
    qDebug() << "RO-----" << this->board[chess.x()][chess.y()].boardFlags[RO];
}

void BoardStatus::playChess(int x, int y)
{
    addChessAt(x, y, blackChess);

    if (this->emptyFlag == false)
    {
        return;
    }

    for (int i = 0; i < this->boardSize; i++)
    {
        for (int j = 0; j < this->boardSize; j++)
        {
            if (haveChess(i, j))
            {
                this->emptyFlag = false;
                return;
            }
        }
    }
}

void BoardStatus::lookForward()
{
    this->nextStep[0]->setX(14);
    this->nextStep[0]->setY(14);
    //lookForwardMax(0, -ifinity, ifinity, 0, 0);
    ABNegaMax(0, -ifinity, ifinity);
}

int BoardStatus::ABNegaMax (int depth, int alpha, int beta)
{
    int score = 0;
    int scoreMax = -ifinity;
    if (depth >= depthMax)
    {
        score = getEvaluate();
        if (depth % 2)
        {
            score = -score;
        }
        //printBoard_Debug();
        //qDebug() << "         " << this->evaluateTemp << score;
        //qDebug() << x << y;

        return score;
    }

    for (int i = 0; i < boardSize; i++)
    //for (int i = 5; i < 10; i++)
    {
        for (int j = 0; j < boardSize; j++)
        //for (int j = 5; j < 10; j++)
        {
            //qDebug() << i << j << haveChess(i, j);
            if (isValid(i, j))
            {
                addChessAt(i, j, (depth % 2) ? blackChess : whiteChess);
                if (depth)
                {
                    score = -ABNegaMax(depth + 1, -beta, -alpha);
                }
                else
                {
                    score = -ABNegaMax(depth + 1, -beta, -alpha);
                }
                removeChessAt(i, j);

                if (score > scoreMax)
                {
                    //qDebug() << "----------------------------------------------";
                    //printTypeNum();
                    //printBoard_Debug();
                    //qDebug() << "         " << this->evaluateTemp << score;
                    this->nextStep[depth]->setX(i);
                    this->nextStep[depth]->setY(j);
                    scoreMax = score;
                }

                if (score > alpha)
                {
                    alpha = score;
                }
                if (alpha > beta)
                {
                    return beta;
                }
            }
        }
    }
    return alpha;
}

void BoardStatus::lookForwardOnce()
{
    int score = 0;
    int scoreMax = -ifinity;

    qDebug() << "---------------------------------";

    for (int i = 0; i < boardSize; i++)
    //for (int i = 5; i < 10; i++)
    {
        for (int j = 0; j < boardSize; j++)
        //for (int j = 5; j < 10; j++)
        {
            //qDebug() << i << j << haveChess(i, j);
            if (isValid(i, j))
            {
                addChessAt(i, j, whiteChess);
                score = getEvaluateDouble();
                removeChessAt(i, j);

                if (score > scoreMax)
                {
                    //qDebug() << "----------------------------------------------";
                    //printTypeNum();
                    //printBoard_Debug();
                    //qDebug() << "         " << this->evaluateTemp << score;
                    this->nextStep[0]->setX(i);
                    this->nextStep[0]->setY(j);
                    scoreMax = score;
                }
            }
        }
    }
}

int BoardStatus::lookForwardMax(int depth, int alpha, int beta, int x, int y)
{
    int temp = 0;

    if (depth >= depthMax)
    {
        temp = getEvaluate();
        //printBoard_Debug();
        //qDebug() << "         " << this->evaluateTemp << temp;
        //qDebug() << x << y;

        if (temp > this->evaluateTemp)
        {
            this->nextStep[depth]->setX(x);
            this->nextStep[depth]->setY(y);
            this->evaluateTemp = temp;
        }
        return temp;
    }

    //qDebug() << "Max--------" << depth;
    //for (int i = 0; i < boardSize; i++)
    for (int i = 0; i < boardSize; i++)
    {
        //for (int j = 0; j < boardSize; j++)
        for (int j = 0; j < boardSize; j++)
        {
            //qDebug() << i << j << haveChess(i, j);
            if (isValid(i, j))
            {
                addChessAt(i, j, whiteChess);
                if (depth)
                {
                    temp = lookForwardMin(depth + 1, alpha, beta, x, y);
                }
                else
                {
                    temp = lookForwardMin(depth + 1, alpha, beta, i, j);
                }
                removeChessAt(i, j);
                /*
                if (depth == 0 && temp > this->evaluateTemp)
                {
                    qDebug() << "         " << this->evaluateTemp << temp;
                    this->nextStep->setX(i);
                    this->nextStep->setY(j);
                    this->evaluateTemp = temp;
                    qDebug() << i << j;
                }
                */
                if (temp >= beta)
                {
                    //return alpha;
                }
                else if (temp > alpha)
                {
                    alpha = temp;
                }
            }
        }
    }

    //return alpha;
    return temp;
}

int BoardStatus::lookForwardMin(int depth,int alpha, int beta, int x, int y)
{
    int temp = 0;

    if (depth >= depthMax)
    {
        temp = getEvaluate();
        /*
        printBoard_Debug();
        qDebug() << "  MIN   " << this->evaluateTemp << temp;
        qDebug() << x << y;
        */
        if (temp >= this->evaluateTemp)
        {
            this->nextStep[depth]->setX(x);
            this->nextStep[depth]->setY(y);
            this->evaluateTemp = temp;
            //qDebug() << "  MIN   " << this->evaluateTemp << temp;
        }
        return temp;
    }

    //qDebug() << "Min--------" << depth;
    //for (int i = 0; i < boardSize; i++)
    for (int i = 5; i < 9; i++)
    {
        //for (int j = 0; j < boardSize; j++)
        for (int j = 5; j < 9; j++)
        {
            if (isValid(i, j))
            {
                addChessAt(i, j, blackChess);
                temp = lookForwardMax(depth + 1, alpha, beta, x, y);
                removeChessAt(i, j);

                if (temp <= alpha)
                {
                    //return beta;
                }
                else if (temp < beta)
                {
                    beta = temp;
                }
            }

        }
    }

    //return beta;
    return temp;
}

inline void BoardStatus::addChessAt(int x, int y, eChessColor color)
{
    if (haveChess(x, y) == noChess)
    {
        this->board[x][y].chess = color;
        this->newChessToTry->setX(x);
        this->newChessToTry->setY(y);
        //addFlag 为 true 表示增加棋子方式更新
    }
}

inline void BoardStatus::removeChessAt(int x, int y)
{
    eChessColor color = haveChess(x, y);
    if (color != noChess && color != noSpace)
    {
        this->board[x][y].chess = noChess;
        this->newChessToTry->setX(x);
        this->newChessToTry->setY(y);
        //addFlag 为 false 表示删除棋子方式更新
    }
}

bool BoardStatus::isOutOfBoard(int x, int y)
{
    if (x >= this->boardSize || y >= this->boardSize
            || x < 0 || y < 0)
    {
        return true;
    }

    return false;
}

eChessColor BoardStatus::haveChess(int x, int y)
{
    if (isOutOfBoard(x, y))
    {
        return noSpace;
    }

    return this->board[x][y].chess;
}

eChessColor BoardStatus::haveChess(const QPoint & location)
{
    return haveChess(location.x(), location.y());
}

bool BoardStatus::isValid(int x, int y)
{
    if (haveChess(x, y))
    {
        return false;
    }

    eChessColor color;

    int xl = x - 2, xr = x + 3, yt = y - 2, yb = y + 3;
    for (int i = xl; i < xr; i++)
    {
        for (int j = yt; j < yb; j++)
        {
            if ((i == x && j == y) || isOutOfBoard(i, j))
            {
                continue;
            }

            color = haveChess(i, j);

            if (color == blackChess || color == whiteChess)
            {
                return true;
            }
        }
    }

    return false;
}

int BoardStatus::getEvaluate()
{
    int value = 0;

    updateStatus();
    for (int i = 0; i < chessTypeNum; i++)
    {
        value -= evaluateMapBlack[i] * this->numOfTypesBlack[i];
        value += evaluateMapWhite[i] * this->numOfTypesWhite[i];
    }

    return value;
}

int BoardStatus::getEvaluateDouble()
{
    int value = 0;

    updateStatus();
    for (int i = 0; i < chessTypeNum; i++)
    {
        value -= evaluateMapWhite[i] * this->numOfTypesBlack[i];
        value += evaluateMapWhite[i] * this->numOfTypesWhite[i];
    }

    return value;
}

void BoardStatus::updateStatus()
{
    clearBoardFlag();
    QPoint chess(0, 0);

    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            if (!haveChess(i, j))
            {
                continue;
            }
            chess.setX(i);
            chess.setY(j);
            checkChessStatus(chess, LR);
            checkChessStatus(chess, TB);
            checkChessStatus(chess, LO);
            checkChessStatus(chess, RO);
        }
    }

/*
    for (int i = 0; i < chessTypeNum; i++)
    {
        qDebug() << "BLACK--" << numOfTypesBlack[i] << i;
        qDebug() << "WHITE--" << numOfTypesWhite[i] << i;
    }
*/
}

void BoardStatus::clearBoardFlag()
{
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            this->board[i][j].boardFlags[LR] = false;
            this->board[i][j].boardFlags[TB] = false;
            this->board[i][j].boardFlags[LO] = false;
            this->board[i][j].boardFlags[RO] = false;
        }
    }

    for (int i = 0; i < chessTypeNum; i++)
    {
        this->numOfTypesBlack[i] = 0;
        this->numOfTypesWhite[i] = 0;
    }
}

inline void BoardStatus::checkChessStatus(const QPoint & chess, eDirection direct)
{
    unsigned long bitMap = 0;
    int leftWidth = 0;
    int rightWidth = 0;
    int numOfSpace = 2;
    eChessColor color = haveChess(chess);
    QPoint nextChess = chess;
    eChessColor nextColor = color;
    bool doubleFlag = true;

    //qDebug() << chess.x() << chess.y();

    if (color == noChess || this->board[chess.x()][chess.y()].boardFlags[direct] == true)
    {
        return;
    }

    while (numOfSpace || (nextColor != noChess))
    {
        if (nextColor == color)
        {
            bitMap = bitMap << 1;
            bitMap = bitMap | 1;
            setBoardFlag(nextChess, direct);
            numOfSpace = 2;
        }
        else if (nextColor == noChess)
        {
            bitMap = bitMap << 1;
            numOfSpace--;
        }
        else
        {
            break;
        }

        rightWidth++;
        getNextPoint(nextChess, direct, true);
        //qDebug() << nextChess.x() << nextChess.y() << haveChess(nextChess);
        nextColor = haveChess(nextChess);
    }

    if (numOfSpace == 0)
    {
        rightWidth--;
        bitMap = bitMap >> 1;
        doubleFlag = false;
    }

    numOfSpace = 2;
    unsigned long bitMapTemp = (1 << rightWidth);
    nextChess = chess;
    getNextPoint(nextChess, direct, false);
    nextColor = haveChess(nextChess);

    while (numOfSpace || (nextColor != noChess))
    {
        if (nextColor == color)
        {
            bitMap = bitMap | bitMapTemp;
            setBoardFlag(nextChess, direct);
            numOfSpace = 2;
        }
        else if (nextColor == noChess)
        {
            numOfSpace--;
        }
        else
        {
            break;
        }

        leftWidth++;
        bitMapTemp = bitMapTemp << 1;
        getNextPoint(nextChess, direct, false);
        nextColor = haveChess(nextChess);
    }

    if (numOfSpace == 0)
    {
        leftWidth--;
        doubleFlag = false;
    }

    if (numOfSpace == 2 && leftWidth + rightWidth < 6)
    {
        bitMap = bitMap << (6 - leftWidth - rightWidth);
    }

    //printBoard_Debug();

    getHashValue(bitMap, leftWidth + rightWidth, color, doubleFlag);
}

inline void BoardStatus::getNextPoint(QPoint & chess, eDirection direct, bool directFlag)
{
    switch (direct)
    {
    case LR:
        chess.setX(chess.x() + (directFlag ? -1 : 1));
        chess.setY(chess.y());
        break;
    case TB:
        chess.setX(chess.x());
        chess.setY(chess.y() + (directFlag ? -1 : 1));
        break;
    case LO:
        chess.setX(chess.x() + (directFlag ? -1 : 1));
        chess.setY(chess.y() + (directFlag ? -1 : 1));
        break;
    case RO:
        chess.setX(chess.x() + (directFlag ? 1 : -1));
        chess.setY(chess.y() + (directFlag ? -1 : 1));
        break;
    }
}

inline void BoardStatus::getHashValue(unsigned long bitMap, int bitWidth, eChessColor color, bool doubleFlag)
{
    if (doubleFlag && bitWidth < 5)
    {
        return;
    }
    int *numOfTypes = (color == blackChess) ? this->numOfTypesBlack : this->numOfTypesWhite;

    numOfTypes[hashTable[bitMap % 64]]++;

    if (bitWidth > 6)
    {
        bitMap = bitMap >> (bitWidth - 6);
        numOfTypes[hashTable[bitMap % 64]]++;
    }
}

bool BoardStatus::isEmpty()
{
    return this->emptyFlag;
}

QPoint BoardStatus::getBoardCenter()
{
    int temp = (this->boardSize + 1) / 2;
    return QPoint(temp, temp);
}

void BoardStatus::setBoardFlag(const QPoint &chess, eDirection direct)
{
    this->setBoardFlag(chess.x(), chess.y(), direct);
}

void BoardStatus::setBoardFlag(int x, int y, eDirection direct)
{
    this->board[x][y].boardFlags[direct] = true;
}

bool BoardStatus::canWin()
{
    return (numOfTypesWhite[LIVE_FOUR] + numOfTypesWhite[FIVE_1] > 0) ||
           (numOfTypesWhite[DEATH_FOUR] + numOfTypesWhite[DEATH_FOUR_1] > 0) ||
           (numOfTypesBlack[LIVE_FOUR] + numOfTypesBlack[FIVE_1] > 0) ||
           (numOfTypesBlack[DEATH_FOUR] + numOfTypesBlack[DEATH_FOUR_1] > 1) ;
}

void BoardStatus::getKeyChess()
{
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            addChessAt(i, j, whiteChess);

            if (isWin(i, j))
            {
                nextStep[0]->setX(i);
                nextStep[0]->setY(j);
                removeChessAt(i, j);
                return;
            }
            removeChessAt(i, j);
        }
    }
}

bool BoardStatus::isWin(int x, int y)
{
    QPoint chess(x, y);
    clearBoardFlag();

    checkChessStatus(chess, LR);
    checkChessStatus(chess, TB);
    checkChessStatus(chess, LO);
    checkChessStatus(chess, RO);

    return (numOfTypesWhite[FIVE] > 0);
}
