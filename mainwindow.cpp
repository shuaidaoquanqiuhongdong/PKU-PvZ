#include "mainwindow.h"
#include <QAudioOutput>
#include <QGuiApplication>
#include <QScreen>
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
    , currentMode(ClassicMode)
{
    setWindowTitle("PvZLiteQt - 植物大战僵尸");

    constexpr int BaseW = 1408;
    constexpr int BaseH = 888;
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen)
    {
        QRect geom = screen->availableGeometry();
        qreal sw = static_cast<qreal>(geom.width()) / BaseW;
        qreal sh = static_cast<qreal>(geom.height()) / BaseH;
        scaleFactor = qMin(sw, sh);
        if (scaleFactor > 1.0) scaleFactor = 1.0;
        if (scaleFactor < 0.5) scaleFactor = 0.5;
    }

    setFixedSize(static_cast<int>(BaseW * scaleFactor),
                 static_cast<int>(BaseH * scaleFactor));

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    mainMenuWidget = new MainMenuWidget(this);
    stackedWidget->addWidget(mainMenuWidget);

    showMainMenu();

    connect(mainMenuWidget, &MainMenuWidget::startClicked,
            this, &MainWindow::startGame);
    connect(mainMenuWidget, &MainMenuWidget::zombieModeClicked,
            this, &MainWindow::startZombieMode);
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
    currentMode = ClassicMode;
    startGameInternal();
}

void MainWindow::startZombieMode()
{
    currentMode = ZombieMode;
    startGameInternal();
}

void MainWindow::startGameInternal()
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

    connect(gameEngine, &GameEngine::entityCreated,
            gameView, &GameView::addEntityItem);
    connect(gameEngine, &GameEngine::entityCreated,
            animationManager, &AnimationManager::bindEntity);
    connect(gameEngine, &GameEngine::entityDied,
            animationManager, &AnimationManager::playDeathEffect);

    connect(gameEngine, &GameEngine::sunChanged,
            topBar, &TopBarWidget::setSunValue);
    connect(gameEngine, &GameEngine::sunChanged,
            cardPanel, &CardPanel::updateCardState);
    connect(gameEngine, &GameEngine::waveChanged,
            topBar, &TopBarWidget::setWaveInfo);

    connect(gameEngine, &GameEngine::entityAnimationChanged,
            animationManager, &AnimationManager::playAnimation);

    connect(gameEngine, &GameEngine::bulletHit,
            animationManager, &AnimationManager::playBulletHitEffect);
    connect(gameEngine, &GameEngine::sunCollected,
            animationManager, qOverload<Sun*>(&AnimationManager::playSunCollectAnimation));

    connect(animationManager, &AnimationManager::deathAnimationFinished,
            gameEngine, &GameEngine::removeEntitySafely);
    connect(animationManager, &AnimationManager::sunCollectAnimationFinished,
            gameEngine, [](Sun* sun) {
                if (sun) {
                    sun->die();
                    sun->markDeathAnimDone();
                }
            });

    connect(gameEngine, &GameEngine::gameOver,
            this, &MainWindow::showGameOver);

    connect(gameOverWidget, &GameOverWidget::restartClicked,
            this, &MainWindow::restartGame);
    connect(gameOverWidget, &GameOverWidget::mainMenuClicked,
            this, &MainWindow::goToMainMenu);

    connect(topBar, &TopBarWidget::pauseClicked, this, [=]() {
        if (!gameEngine) return;
        if (paused)
            gameEngine->resume();
        else
            gameEngine->pause();
        paused = !paused;
        topBar->setPaused(paused);
        gameView->setPaused(paused);
    });

    QString resPath = ":/images/";
    if (currentMode == ClassicMode) {
        cardPanel->addPlantCard("Firefan",      "不知火蛙",   100, resPath + "firefan/idle_0.png");
        cardPanel->addPlantCard("Kimsunflower", "金日葵",     50,  resPath + "kimsunflower/idle_0.png");
        cardPanel->addPlantCard("Bengbear",     "熊绷果",     50,  resPath + "bengbear/idle_0.png");
        cardPanel->addPlantCard("Rainchili",    "带派辣椒",   125, resPath + "rainchili/idle_0.png");

        connect(gameView, &GameView::cellClicked, this, [=](int row, int col) {
            QString type = cardPanel->selectedCardType();
            if (!type.isEmpty()) {
                bool ok = gameEngine->placePlant(type, row, col);
                if (ok) {
                    cardPanel->clearSelection();
                    gameView->setPlantSelectionMode(false);
                }
            }
        });

        connect(cardPanel, &CardPanel::cardSelected, this, [=](const QString& cardType, bool isPlant) {
            if (isPlant && !cardType.isEmpty()) {
                gameView->setPlantSelectionMode(true);
            } else {
                gameView->setPlantSelectionMode(false);
            }
        });
    } else {
        cardPanel->addZombieCard("GenziZombie",   "普通僵尸",   50,  resPath + "genzizombie/idle_0.png");
        cardPanel->addZombieCard("DancingZombie", "舞王僵尸",   100, resPath + "dancingzombie/idle_0.png");

        connect(gameView, &GameView::zombieCellClicked, this, [=](int row, int col) {
            QString type = cardPanel->selectedCardType();
            if (!type.isEmpty()) {
                bool ok = gameEngine->placeZombie(type, row, col);
                if (ok) {
                    cardPanel->clearSelection();
                    gameView->setPlantSelectionMode(false);
                }
            }
        });

        connect(cardPanel, &CardPanel::cardSelected, this, [=](const QString& cardType, bool isPlant) {
            if (!isPlant && !cardType.isEmpty()) {
                gameView->setPlantSelectionMode(true);
            } else {
                gameView->setPlantSelectionMode(false);
            }
        });
    }

    if (!qFuzzyCompare(scaleFactor, 1.0))
    {
        gameView->applyScale(scaleFactor);
        topBar->setFixedHeight(static_cast<int>(50 * scaleFactor));
        cardPanel->setFixedHeight(static_cast<int>(70 * scaleFactor));
    }

    gameView->initScene();
    topBar->setSunValue(150);

    stackedWidget->setCurrentWidget(gamePageWidget);

    if (currentMode == ZombieMode) {
        gameView->setZombieMode(true);
        gameEngine->startZombieMode();
    } else {
        gameEngine->start();
    }

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
    if (currentMode == ClassicMode) {
        startGame();
    } else {
        startZombieMode();
    }
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
