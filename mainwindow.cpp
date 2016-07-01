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

    checkAutoPlayTimer = new QTimer();
    connect(checkAutoPlayTimer, &QTimer::timeout, this, &onCheckAutoPlayTimerUpdate);
    checkAutoPlayTimer->start(100);

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

void MainWindow::autoPlayChess()
{
    QPoint chess;

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

void MainWindow::onCheckAutoPlayTimerUpdate()
{
    if (turnsFlag == chessColorM)
    {
        autoPlayChess();
    }
}
