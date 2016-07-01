#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    boardLocation.setX(40);
    boardLocation.setY(40);

    QSize windowSize = QSize(boardLocation.x() * 2
                             + (boardSize - 1) * gridSize,
                             boardLocation.y() * 2
                             + (boardSize - 1) * gridSize);
    this->resize(windowSize);
    this->setWindowTitle("五子棋");

    this->centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);

    timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(timerUpdate1()));
    //timer1->start(500);

    varInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    drawBoard();
    drawAllChess();
    showLastChess();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    QPoint mouse = e->pos();
    if (onGrid(mouse))
    {
        getMouseLocation(mouse);

        if (!haveChess(chessLocation))
        {
            boardStatus->playChess(chessLocation.x(), chessLocation.y());
            playChess(chessLocation);
            if (turnsFlag == chessColorM)
            {
                autoPlayChess();
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    QPoint mouse = e->pos();
    if (onGrid(mouse))
    {
        setCursor(Qt::PointingHandCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::varInit()
{
    boardStatus = new BoardStatus(boardSize);

    chessColorM = 0;
    turnsFlag = !chessColorM;
    chessNum = 0;

    memset(board, 0, boardSize * boardSize * sizeof(char));
    memset(tuple, 0, (tupleNum + 1) * sizeof(int));

    //空的五元组
    tupleScoreTable[0] = 1;
    //白
    tupleScoreTable[1] = 15;
    //白白
    tupleScoreTable[2] = 400;
    //白白白
    tupleScoreTable[3] = 18000;
    //白白白白
    tupleScoreTable[4] = 8000000;
    //黑
    tupleScoreTable[5] = 35;
    //黑黑
    tupleScoreTable[6] = 800;
    //黑黑黑
    tupleScoreTable[7] = 25000;
    //黑黑黑黑
    tupleScoreTable[8] = 800000;
    //其它
    tupleScoreTable[9] = 0;

    if (chessColorM == 1)
    {
        autoPlayChess();
    }
}

void MainWindow::drawBoard()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPoint b = boardLocation;

    for (int i = 0; i < boardSize; i++)
    {
        int grid_temp = i * gridSize;
        painter.drawLine(b.x(), b.y() + grid_temp, b.x() +
                         (boardSize - 1) * gridSize, b.y() + grid_temp);
        painter.drawLine(b.x() + grid_temp, b.y(), b.x() +
                         grid_temp, b.y() + (boardSize - 1) * gridSize);
    }
}

void MainWindow::drawAllChess()
{
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            drawChess(i, j, board[i][j]);
        }
    }
}

void MainWindow::drawChess(int x, int y, char chess)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);

    QPoint chessCenter;
    chessCenter.setX(boardLocation.x() + x * gridSize);
    chessCenter.setY(boardLocation.y() + y * gridSize);

    switch (chess)
    {
    case 0:
        return;
    case 1:
        brush.setColor(Qt::white);
        break;
    case 2:
        brush.setColor(Qt::black);
    }

    painter.setBrush(brush);
    painter.drawEllipse(chessCenter, chessSize, chessSize);
}

bool MainWindow::inBoard(QPoint mouse)
{
    QPoint boardLocation_temp = QPoint(boardLocation.x()
                                       - onGridSize,
                                       boardLocation.y()
                                       - onGridSize);

    QSize size = QSize((boardSize - 1) * gridSize + onGridSize * 2,
                       (boardSize - 1) * gridSize + onGridSize * 2);
    QRect rect = QRect(boardLocation_temp, size);

    return rect.contains(mouse);
}

bool MainWindow::onGrid(QPoint mouse)
{
    if (!inBoard(mouse))
    {
        return false;
    }

    QPoint point = QPoint((mouse.x() - boardLocation.x()
                           + gridSize / 2) % gridSize ,
                          (mouse.y() - boardLocation.y()
                           + gridSize / 2) % gridSize);
    QRect grid = QRect(onGridSize, onGridSize,
                       gridSize - onGridSize * 2,
                       gridSize - onGridSize * 2);

    return grid.contains(point);
}

void MainWindow::getMouseLocation(QPoint mouse)
{
    QPoint point = QPoint((mouse.x() - boardLocation.x()
                           + gridSize / 2) / gridSize,
                          (mouse.y() - boardLocation.y()
                           + gridSize / 2) / gridSize);

    chessLocation = point;
}

bool MainWindow::haveChess(QPoint point)
{
    if (point.x() > boardSize || point.x() < 0
            || point.y() > boardSize || point.y() < 0)
    {
        return true;
    }

    if (board[point.x()][point.y()] == 0)
    {
        return false;
    }

    return true;
}

void MainWindow::playChess(QPoint point)
{
    board[point.x()][point.y()] = (turnsFlag + chessColorM) % 2 + 1;

    updateTupleScore(point);

    if (isWin())
    {
        if (turnsFlag == 0)
        {
            QMessageBox::about(this, "Game Over", "You Lose !");
        }
        else
        {
            QMessageBox::about(this, "Game Over", "You Win !");
        }
        varInit();
        changeTurn();
    }

    changeTurn();
    chessNum++;
    this->update();
}

void MainWindow::changeTurn()
{
    switch (turnsFlag)
    {
    case 0:
        turnsFlag = 1;
        break;
    case 1:
        turnsFlag = 0;
        break;
    }
}

bool MainWindow::isWin()
{
    if (checkChess(chessLocation, Top, turnsFlag + 1) +
            checkChess(chessLocation, Bottom, turnsFlag + 1) > 5
        || checkChess(chessLocation, RightTop, turnsFlag + 1) +
            checkChess(chessLocation, LeftBottom, turnsFlag + 1) > 5
        || checkChess(chessLocation, Right, turnsFlag + 1) +
            checkChess(chessLocation, Left, turnsFlag + 1) > 5
        || checkChess(chessLocation, RightBottom, turnsFlag + 1) +
            checkChess(chessLocation, LeftTop, turnsFlag + 1) > 5)
    {
        return true;
    }

    return false;
}

int MainWindow::checkChess(QPoint chess, checkDir dir, char chessFlag)
{
    if (chess.x() < 0 || chess.x() >= boardSize
            || chess.y() < 0 || chess.y() >= boardSize)
    {
        return 0;
    }

    if (board[chess.x()][chess.y()] == chessFlag)
    {
        QPoint nextChess = getNextChess(chess, dir);
        return 1 + checkChess(nextChess, dir, chessFlag);
    }

    return 0;
}

QPoint MainWindow::getNextChess(QPoint chess, checkDir dir)
{
    QPoint nextChess;
    switch (dir)
    {
    case Top:
        nextChess = QPoint(chess.x(), chess.y() - 1);
        break;
    case RightTop:
        nextChess = QPoint(chess.x() + 1, chess.y() - 1);
        break;
    case Right:
        nextChess = QPoint(chess.x() + 1, chess.y());
        break;
    case RightBottom:
        nextChess = QPoint(chess.x() + 1, chess.y() + 1);
        break;
    case Bottom:
        nextChess = QPoint(chess.x(), chess.y() + 1);
        break;
    case LeftBottom:
        nextChess = QPoint(chess.x() - 1, chess.y() + 1);
        break;
    case Left:
        nextChess = QPoint(chess.x() - 1, chess.y());
        break;
    case LeftTop:
        nextChess = QPoint(chess.x() - 1, chess.y() - 1);
        break;
    }

    return nextChess;
}

int MainWindow::getTupleNum(QPoint chess, checkDir dir)
{
    if (!existTuple(chess, dir))
    {
        return 0;
    }

    switch (dir)
    {
    case Bottom:
        return chess.y() * tupleB_H + chess.x() + 1;
    case Right:
        return tupleB + chess.y() * tupleR_H + chess.x() + 1;
    case RightBottom:
        return tupleB + tupleR + chess.y() * tupleRB_H
                + chess.x() + 1;
    case LeftBottom:
        return tupleB + tupleR + tupleRB + chess.y() * tupleLB_H
                + chess.x() - 3;
    default:
        return 0;
    }

    return 0;
}

QPoint MainWindow::getTupleLocation(int tupleNum, checkDir &dir)
{
    QPoint chess;

    if (tupleNum <= tupleB)
    {
        chess.setX(--tupleNum % tupleB_H);
        chess.setY(tupleNum / tupleB_H);
        dir = Bottom;
    }
    else if (tupleNum <= tupleB + tupleR)
    {
        tupleNum -= tupleB + 1;
        chess.setX(tupleNum % tupleR_H);
        chess.setY(tupleNum / tupleR_H);
        dir = Right;
    }
    else if (tupleNum <= tupleB + tupleR + tupleRB)
    {
        tupleNum -= tupleB + tupleR + 1;
        chess.setX(tupleNum % tupleRB_H);
        chess.setY(tupleNum / tupleRB_H);
        dir = RightBottom;
    }
    else
    {
        tupleNum -= tupleB + tupleR + tupleRB + 1;
        chess.setX(tupleNum % tupleLB_H + 4);
        chess.setY(tupleNum / tupleLB_H);
        dir = LeftBottom;
    }

    return chess;
}

bool MainWindow::existTuple(QPoint chess, checkDir dir)
{
    switch (dir)
    {
    case Bottom:
        if (chess.x() < boardSize && chess.x() > -1
                && chess.y() < boardSize - 4 && chess.y() > -1)
        {
            return true;
        }
        break;
    case Right:
        if (chess.x() < boardSize - 4 && chess.x() > -1
                && chess.y() < boardSize && chess.y() > -1)
        {
            return true;
        }
        break;
    case RightBottom:
        if (chess.x() < boardSize - 4 && chess.x() > -1
                && chess.y() < boardSize - 4 && chess.y() > -1)
        {
            return true;
        }
        break;
    case LeftBottom:
        if (chess.x() < boardSize && chess.x() > 3
                && chess.y() < boardSize - 4 && chess.y() > -1)
        {
            return true;
        }
    default:
        return false;
    }

    return false;
}

int *MainWindow::getTuple(QPoint chess)
{
    int *tuple_t = new int[21];
    tuple_t[0] = 1;
    QPoint chess_t[4] = {chess, chess, chess, chess};

    for (int i = 0; i < 5; i++)
    {
        if (existTuple(chess_t[0], Bottom))
        {
            tuple_t[tuple_t[0]++] = getTupleNum(chess_t[0], Bottom);
        }
        if (existTuple(chess_t[1], Right))
        {
            tuple_t[tuple_t[0]++] = getTupleNum(chess_t[1], Right);
        }
        if (existTuple(chess_t[2], RightBottom))
        {
            tuple_t[tuple_t[0]++] = getTupleNum(chess_t[2], RightBottom);
        }
        if (existTuple(chess_t[3], LeftBottom))
        {
            tuple_t[tuple_t[0]++] = getTupleNum(chess_t[3], LeftBottom);
        }

        chess_t[0].setY(chess_t[0].y() - 1);
        chess_t[1].setX(chess_t[1].x() - 1);
        chess_t[2].setX(chess_t[2].x() - 1);
        chess_t[2].setY(chess_t[2].y() - 1);
        chess_t[3].setX(chess_t[3].x() + 1);
        chess_t[3].setY(chess_t[3].y() - 1);
    }

    return tuple_t;
}

void MainWindow::updateTupleScore(QPoint chess)
{
    int *tuple_t = getTuple(chess);

    for (int i = 1; i < tuple_t[0]; i++)
    {
        checkDir dir;
        QPoint chess_t = getTupleLocation(tuple_t[i], dir);
        tuple[tuple_t[i]] = getChessScore(chess_t, dir);
    }
}

int MainWindow::getTupleScore(QPoint chess)
{
    int *tuple_t = getTuple(chess);
    int sum = 0;

    for (int i = 1; i < tuple_t[0]; i++)
    {
        sum += tuple[tuple_t[i]];
    }

    return sum;
}

int MainWindow::getChessScore(QPoint chess, checkDir dir)
{
    int whiteNum = 0;
    int blackNum = 0;

    for (int i = 0; i < 5; i++)
    {
        if (board[chess.x()][chess.y()] == 1)
        {
            whiteNum++;
        }
        if (board[chess.x()][chess.y()] == 2)
        {
            blackNum++;
        }

        chess = getNextChess(chess, dir);
    }

    return getScore(whiteNum, blackNum);
}

int MainWindow::getScore(int whiteNum, int blackNum)
{
    if (whiteNum == 0 && blackNum == 0)
    {
        return tupleScoreTable[0];
    }
    else if (whiteNum == 1 && blackNum == 0)
    {
        return tupleScoreTable[1];
    }
    else if (whiteNum == 2 && blackNum == 0)
    {
        return tupleScoreTable[2];
    }
    else if (whiteNum == 3 && blackNum == 0)
    {
        return tupleScoreTable[3];
    }
    else if (whiteNum == 4 && blackNum == 0)
    {
        return tupleScoreTable[4];
    }
    else if (whiteNum == 0 && blackNum == 1)
    {
        return tupleScoreTable[5];
    }
    else if (whiteNum == 0 && blackNum == 2)
    {
        return tupleScoreTable[6];
    }
    else if (whiteNum == 0 && blackNum == 3)
    {
        return tupleScoreTable[7];
    }
    else if (whiteNum == 0 && blackNum == 4)
    {
        return tupleScoreTable[8];
    }
    else
    {
        return tupleScoreTable[9];
    }
}

void MainWindow::autoPlayChess()
{
    int max = 0;
    QPoint chess;

    for (int i = 0; i <= boardSize; i++)
    {
        for (int j = 0; j <= boardSize; j++)
        {
            int score = 0;
            if (!haveChess(QPoint(i, j)))
            {
                score = getTupleScore(QPoint(i, j));
            }

            if (score > max)
            {
                max = score;
                chess = QPoint(i, j);
            }
        }
    }

    if (boardEmpty())
    {
        chess.setX(boardSize / 2);
        chess.setY(boardSize / 2);
    }

    chess = boardStatus->getNextStep();

    chessLocation = chess;
    playChess(chess);
}

bool MainWindow::boardEmpty()
{
    return (chessNum < 1);
}

void MainWindow::showLastChess()
{
    if (boardEmpty())
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QColor(0, 255, 0));
    painter.setPen(pen);

    QPoint chess(chessLocation.x() * gridSize + boardLocation.x(),
                 chessLocation.y() * gridSize + boardLocation.y());
    painter.drawLine(chess.x() - chessSize / 2, chess.y(),
                     chess.x() + chessSize / 2, chess.y());
    painter.drawLine(chess.x(), chess.y() - chessSize / 2,
                     chess.x(), chess.y() + chessSize / 2);
}

void MainWindow::timerUpdate1()
{
    autoPlayChess();
}
