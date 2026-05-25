#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include <QMediaPlayer>

class MainMenuWidget;
class GamePageWidget;
class GameOverWidget;
class GameEngine;
class AnimationManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void showMainMenu();
    void startGame();
    void showGameOver(bool win);
    void restartGame();
    void goToMainMenu();

private:
    void clearGame();
    QStackedWidget* stackedWidget;
    MainMenuWidget* mainMenuWidget;
    GamePageWidget* gamePageWidget;
    GameOverWidget* gameOverWidget;
    GameEngine* gameEngine;
    AnimationManager* animationManager;
    QMediaPlayer* bgmPlayer;
    bool paused = false;
};

#endif
