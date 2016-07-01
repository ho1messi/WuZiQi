#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "boardstatus.h"

enum checkDir{
    Top,
    RightTop,
    Right,
    RightBottom,
    Bottom,
    LeftBottom,
    Left,
    LeftTop
};

const int boardSize = 15;
const int gridSize = 40;
const int chessSize = 17;
const int onGridSize = 4;

const int tupleNum = 4 * boardSize * boardSize
        - 24 * boardSize + 32;

const int tupleB = boardSize * boardSize - 4 * boardSize;
const int tupleB_H = boardSize;
const int tupleR = boardSize * boardSize - 4 * boardSize;
const int tupleR_H = boardSize - 4;
const int tupleRB = boardSize * boardSize - 8 * boardSize + 16;
const int tupleRB_H = boardSize - 4;
const int tupleLB = boardSize * boardSize - 8 * boardSize + 16;
const int tupleLB_H = boardSize - 4;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    void varInit();
    void drawBoard();
    void drawAllChess();
    void drawChess(int x, int y, char chess);
    bool inBoard(QPoint mouse);
    bool onGrid(QPoint mouse);
    void getMouseLocation(QPoint mouse);
    bool haveChess(QPoint point);
    void playChess(QPoint point);
    void changeTurn();
    bool isWin();
    int checkChess(QPoint chess, checkDir dir, char chessFlag);
    QPoint getNextChess(QPoint chess, checkDir dir);
    int getTupleNum(QPoint chess, checkDir dir);
    QPoint getTupleLocation(int tupleNum, checkDir &dir);
    bool existTuple(QPoint chess, checkDir dir);
    int *getTuple(QPoint chess);
    void updateTupleScore(QPoint chess);
    int getTupleScore(QPoint chess);
    int getChessScore(QPoint chess, checkDir dir);
    int getScore(int whiteNum, int blackNum);
    void checkAllTuple();
    void autoPlayChess();
    bool boardEmpty();
    void showLastChess();

private slots:
    void timerUpdate1();

private:
    Ui::MainWindow *ui;

    char board[boardSize][boardSize];
    int tuple[tupleNum + 1];
    int boardScore[boardSize][boardSize];
    int tupleScoreTable[10];
    int chessNum;
    QPoint boardLocation;
    QPoint chessLocation;
    char turnsFlag;
    char chessColorM;
    QTimer *timer1;
    BoardStatus *boardStatus;
};

#endif // MAINWINDOW_H
