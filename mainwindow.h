#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include <QMediaPlayer>

class MainMenuWidget;
class GamePageWidget;
class GameOverWidget;
class GuideWidget;
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
    void startZombieMode();
    void showGuide();
    void showGameOver(bool win);
    void restartGame();
    void goToMainMenu();

private:
    void clearGame();
    void startGameInternal();
    enum GameMode { ClassicMode, ZombieMode };
    GameMode currentMode;
    QStackedWidget* stackedWidget;
    MainMenuWidget* mainMenuWidget;
    GuideWidget* guideWidget;
    GamePageWidget* gamePageWidget;
    GameOverWidget* gameOverWidget;
    GameEngine* gameEngine;
    AnimationManager* animationManager;
    QMediaPlayer* bgmPlayer;
    bool paused = false;
    qreal scaleFactor = 1.0;
};

#endif
