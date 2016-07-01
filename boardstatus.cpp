#include "boardstatus.h"
#include <QDebug>
#include <QString>
#include <QTime>

BoardStatus::BoardStatus(int boardSize)
{
    this->board = new BoardPoint * [boardSize];
    this->boardSize = boardSize;
    this->emptyFlag = true;
    this->newChessToTry = new QPoint(0, 0);

    for (int i = 0; i < boardSize; i++)
    {
        this->board[i] = new BoardPoint[boardSize];

        for (int j = 0; j < boardSize; j++)
        {
            this->board[i][j].chess = noChess;
            this->board[i][j].blackStatusEffact = NULL;
            this->board[i][j].whiteStatusEffact = NULL;
            this->board[i][j].statusIncluded = NULL;
        }
    }

    for (int i = 0; i < chessTypeNum; i++)
    {
        this->blackChessStatus[i].num = 0;
        this->blackChessStatus[i].first = NULL;

        this->whiteChessStatus[i].num = 0;
        this->whiteChessStatus[i].first = NULL;
    }
}

QPoint BoardStatus::getNextStep()
{
    if (this->isEmpty())
    {
        return getBoardCenter();
    }

    return lookForward();
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

QPoint BoardStatus::lookForward()
{
    lookForwardMax(0, -ifinity, ifinity);

    return getNextStep_Debug();
}

int BoardStatus::lookForwardMax(int depth, int alpha, int beta)
{
    int temp;

    if (depth >= depthMax)
    {
        return getEvaluate();
    }

    for (int i = 0; i <= boardSize; i++)
    {
        for (int j = 0; j <= boardSize; j++)
        {
            if (isValid(i, j))
            {
                addChessAt(i, j, whiteChess);
                temp = lookForwardMin(depth + 1, alpha, beta);
                removeChessAt(i, j);
                if (temp >= beta)
                {
                    return alpha;
                }
                else if (temp > alpha)
                {
                    alpha = temp;
                }
            }
        }
    }

    return alpha;
}

int BoardStatus::lookForwardMin(int depth,int alpha, int beta)
{
    int temp;

    if (depth >= depthMax)
    {
        return getEvaluate();
    }

    for (int i = 0; i <= boardSize; i++)
    {
        for (int j = 0; j <= boardSize; j++)
        {
            if (isValid(i, j))
            {
                addChessAt(i, j, blackChess);
                temp = lookForwardMax(depth + 1, alpha, beta);
                removeChessAt(i, j);

                if (temp <= alpha)
                {
                    return beta;
                }
                else if (temp < beta)
                {
                    beta = temp;
                }
            }

        }
    }

    return beta;
}

void BoardStatus::addChessAt(int x, int y, eChessColor color)
{
    if (!haveChess(x, y))
    {
        this->board[x][y].chess = color;
        this->newChessToTry->setX(x);
        this->newChessToTry->setY(y);
        //addFlag 为 true 表示增加棋子方式更新
        updateStatus(x, y, color, true);
    }
}

void BoardStatus::removeChessAt(int x, int y)
{
    if (haveChess(x, y))
    {
        eChessColor color = this->board[x][y].chess;
        this->board[x][y].chess = noChess;
        this->newChessToTry->setX(x);
        this->newChessToTry->setY(y);
        //addFlag 为 false 表示删除棋子方式更新
        updateStatus(x, y, color, false);
    }
}

bool BoardStatus::isOutOfBoard(int x, int y)
{
    if (x > this->boardSize || y > this->boardSize)
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

eChessColor BoardStatus::haveChess(QPoint & location)
{
    return haveChess(location.x(), location.y());
}

bool BoardStatus::isValid(int x, int y)
{
    if (haveChess(x, y))
    {
        return false;
    }

    int xl = x - 2, xr = x + 3, yt = y - 2, yb = y + 3;
    for (int i = xl; i < xr; i++)
    {
        for (int j = yt; j < yb; j++)
        {
            if ((i == x && j == y) || isOutOfBoard(i, j))
            {
                continue;
            }
            if (haveChess(x, y))
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

    for (int i = 0; i < chessTypeNum; i++)
    {
        value += evaluateMap[i] * this->whiteChessStatus[i].num;
        value -= evaluateMap[i] * this->blackChessStatus[i].num;
    }

    return value;
}

void BoardStatus::updateStatus(int x, int y, eChessColor color, bool arFlag)
{
    updateStatusBlack(x, y, color, arFlag);
    updateStatusWhite(x, y, color, arFlag);
}

void BoardStatus::updateStatusBlack(int x, int y, eChessColor color, bool arFlag)
{
    switch (color + arFlag)
    {
    case blackChess:
        rmSameColorBS(board[x][y].statusIncluded, color);
        break;
    case blackChess + 1:
        addSameColorBS(board[x][y].blackStatusEffact, color);
        break;
    case whiteChess:
        rmDiffColorBS(board[x][y].blackStatusEffact, color);
        break;
    case whiteChess + 1:
        addDiffColorBS(board[x][y].blackStatusEffact, color);
    }
}

void BoardStatus::updateStatusWhite(int x, int y, eChessColor color, bool arFlag)
{
    switch (color + arFlag)
    {
    case blackChess:
        rmDiffColorBS(board[x][y].whiteStatusEffact, color);
        break;
    case blackChess + 1:
        addDiffColorBS(board[x][y].whiteStatusEffact, color);
        break;
    case whiteChess:
        rmSameColorBS(board[x][y].whiteStatusEffact, color);
        break;
    case whiteChess + 1:
        addSameColorBS(board[x][y].whiteStatusEffact, color);
        break;
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

void BoardStatus::addSameColorBS(pStatusLink *StatusEffact, eChessColor color)
{
    if (StatusEffact == NULL)
    {
        addStatusOne(color);
    }
}

void BoardStatus::addDiffColorBS(pStatusLink *StatusEffact, eChessColor color)
{

}

void BoardStatus::rmSameColorBS(pStatusLink *StatusIncluded, eChessColor color)
{

}

void BoardStatus::rmDiffColorBS(pStatusLink *StatusIncluded, eChessColor color)
{

}

void BoardStatus::addStatusOne(eChessColor color)
{

}
