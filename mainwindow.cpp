#include "mainwindow.h"
#include <QAudioOutput>
#include "ui/MainMenuWidget.h"
#include "ui/GamePageWidget.h"
#include "ui/GameOverWidget.h"
#include "core/GameEngine.h"
#include "animation/AnimationManager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , gameEngine(nullptr)
    , animationManager(nullptr)
    , gamePageWidget(nullptr)
    , gameOverWidget(nullptr)
    , bgmPlayer(nullptr)
{
    setWindowTitle("PvZLiteQt - 植物大战僵尸");
    setFixedSize(1408, 888);  // 768 + TopBar 50 + CardPanel 70

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    mainMenuWidget = new MainMenuWidget(this);
    stackedWidget->addWidget(mainMenuWidget);

    showMainMenu();

    connect(mainMenuWidget, &MainMenuWidget::startClicked,
            this, &MainWindow::startGame);
    connect(mainMenuWidget, &MainMenuWidget::exitClicked,
            this, &QMainWindow::close);
}

MainWindow::~MainWindow() {}

void MainWindow::showMainMenu()
{
    stackedWidget->setCurrentWidget(mainMenuWidget);
}

void MainWindow::startGame()
{
    clearGame();

    gameEngine = new GameEngine();
    animationManager = new AnimationManager(this);
    gamePageWidget = new GamePageWidget(this);
    gameOverWidget = new GameOverWidget(this);

    stackedWidget->addWidget(gamePageWidget);
    stackedWidget->addWidget(gameOverWidget);

    auto* gameView  = gamePageWidget->getGameView();
    auto* cardPanel = gamePageWidget->getCardPanel();
    auto* topBar    = gamePageWidget->getTopBarWidget();

    // 点击格子 → 种植植物
    connect(gameView, &GameView::cellClicked, this, [=](int row, int col) {
        QString type = cardPanel->selectedPlantType();
        if (!type.isEmpty()) {
            bool ok = gameEngine->placePlant(type, row, col);
            if (ok) {
                cardPanel->clearSelection();
            }
        }
    });

    // 引擎 → 视图：实体加入场景
    connect(gameEngine, &GameEngine::entityCreated,
            gameView, &GameView::addEntityItem);
    // 引擎 → 动画：绑定实体动画
    connect(gameEngine, &GameEngine::entityCreated,
            animationManager, &AnimationManager::bindEntity);
    // 引擎 → 动画：实体死亡播放死亡效果
    connect(gameEngine, &GameEngine::entityDied,
            animationManager, &AnimationManager::playDeathEffect);

    // 引擎 → UI：状态更新
    connect(gameEngine, &GameEngine::sunChanged,
            topBar, &TopBarWidget::setSunValue);
    connect(gameEngine, &GameEngine::sunChanged,
            cardPanel, &CardPanel::updateCardState);

    // 引擎 → 动画：攻击/状态变化
    connect(gameEngine, &GameEngine::entityAnimationChanged,
            animationManager, &AnimationManager::playAnimation);

    // 引擎 → 动画：子弹命中
    connect(gameEngine, &GameEngine::bulletHit,
            animationManager, &AnimationManager::playBulletHitEffect);
    // 阳光收集动画暂时绕过（动画内部有bug待修）
    connect(gameEngine, &GameEngine::sunCollected,
            animationManager, qOverload<Sun*>(&AnimationManager::playSunCollectAnimation));

    // 动画 → 引擎：死亡动画播完安全删除
    connect(animationManager, &AnimationManager::deathAnimationFinished,
            gameEngine, &GameEngine::removeEntitySafely);
    // 动画 → 引擎：阳光收集动画播完标记死亡
    connect(animationManager, &AnimationManager::sunCollectAnimationFinished,
            gameEngine, [](Sun* sun) {
                if (sun) sun->die();
            });

    // 引擎 → 主窗口：游戏结束
    connect(gameEngine, &GameEngine::gameOver,
            this, &MainWindow::showGameOver);

    // 结算界面按钮
    connect(gameOverWidget, &GameOverWidget::restartClicked,
            this, &MainWindow::restartGame);
    connect(gameOverWidget, &GameOverWidget::mainMenuClicked,
            this, &MainWindow::goToMainMenu);

    // 暂停/恢复按钮
    connect(topBar, &TopBarWidget::pauseClicked, this, [=]() {
        if (!gameEngine) return;
        if (paused)
            gameEngine->resume();
        else
            gameEngine->pause();
        paused = !paused;
    });

    // 初始化卡片
    cardPanel->addPlantCard("Firefan",      "不知火蛙",   100, "");
    cardPanel->addPlantCard("Kimsunflower", "金日葵",     50,  "");
    cardPanel->addPlantCard("Bengbear",     "熊绷果",     50,  "");
    cardPanel->addPlantCard("Rainchili",    "带派辣椒",   125, "");

    // 初始化场景
    gameView->initScene();
    topBar->setSunValue(150);

    stackedWidget->setCurrentWidget(gamePageWidget);
    gameEngine->start();

    // 背景音乐
    bgmPlayer = new QMediaPlayer(this);
    auto* audioOutput = new QAudioOutput(this);
    bgmPlayer->setAudioOutput(audioOutput);
    bgmPlayer->setSource(QUrl::fromLocalFile(
        QString(PROJECT_SOURCE_DIR) + "/resources/bgm.mp3"));
    bgmPlayer->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(0.5);
    bgmPlayer->play();
}

void MainWindow::showGameOver(bool win)
{
    if (gameOverWidget) {
        gameOverWidget->setResult(win);
        stackedWidget->setCurrentWidget(gameOverWidget);
    }
}

void MainWindow::restartGame()
{
    startGame();
}

void MainWindow::goToMainMenu()
{
    clearGame();
    showMainMenu();
}

void MainWindow::clearGame()
{
    if (bgmPlayer) {
        bgmPlayer->stop();
        bgmPlayer->deleteLater();
        bgmPlayer = nullptr;
    }
    if (gameEngine) {
        gameEngine->deleteLater();
        gameEngine = nullptr;
    }
    if (animationManager) {
        animationManager->deleteLater();
        animationManager = nullptr;
    }
    if (gamePageWidget) {
        stackedWidget->removeWidget(gamePageWidget);
        gamePageWidget->deleteLater();
        gamePageWidget = nullptr;
    }
    if (gameOverWidget) {
        stackedWidget->removeWidget(gameOverWidget);
        gameOverWidget->deleteLater();
        gameOverWidget = nullptr;
    }
}
