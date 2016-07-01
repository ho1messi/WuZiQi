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

private slots:
    void onCheckAutoPlayTimerUpdate();

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
    void autoPlayChess();
    bool boardEmpty();
    void showLastChess();

private:
    Ui::MainWindow *ui;

    char board[boardSize][boardSize];
    int chessNum;
    QPoint boardLocation;
    QPoint chessLocation;
    char turnsFlag;
    char chessColorM;
    BoardStatus *boardStatus;

    QTimer *checkAutoPlayTimer;
};

#endif // MAINWINDOW_H
