#include "mainwindow.h"
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
    , zombieSpawnTimer(nullptr)
{
    setWindowTitle("PvZLiteQt - 植物大战僵尸");
    setFixedSize(1408, 908);  // 768 + TopBar 50 + CardPanel 90

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

    // 引擎 → 主窗口：游戏结束
    connect(gameEngine, &GameEngine::gameOver,
            this, &MainWindow::showGameOver);

    // 结算界面按钮
    connect(gameOverWidget, &GameOverWidget::restartClicked,
            this, &MainWindow::restartGame);
    connect(gameOverWidget, &GameOverWidget::mainMenuClicked,
            this, &MainWindow::goToMainMenu);

    // 暂停按钮
    connect(topBar, &TopBarWidget::pauseClicked, this, [=]() {
        // 暂停功能由 Core 组后续实现
    });

    // 每 6 秒生成一个僵尸
    zombieSpawnTimer = new QTimer(this);
    connect(zombieSpawnTimer, &QTimer::timeout, this, [=]() {
        static int totalSpawned = 0;
        if (totalSpawned >= 10) return;
        int row = totalSpawned % 5;
        auto* zombie = new GenziZombie(row, 8);
        zombie->setPos(gameView->cellToScenePos(row, 8));
        gameEngine->addZombie(zombie);
        gameView->addEntityItem(zombie);
        animationManager->bindEntity(zombie);
        animationManager->playAnimation(zombie, AnimationState::Walk);
        totalSpawned++;
    });
    zombieSpawnTimer->start(6000);

    // 初始化卡片
    cardPanel->addPlantCard("Shooter",     "射手",       100, "");
    cardPanel->addPlantCard("SunProducer", "阳光生产者",  50,  "");
    cardPanel->addPlantCard("Wall",        "坚果墙",      50,  "");

    // 初始化场景
    gameView->initScene();
    topBar->setSunValue(150);

    stackedWidget->setCurrentWidget(gamePageWidget);
    gameEngine->start();
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
    if (zombieSpawnTimer) {
        zombieSpawnTimer->stop();
        zombieSpawnTimer->deleteLater();
        zombieSpawnTimer = nullptr;
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
