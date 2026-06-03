#include "GuideWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QFrame>
#include <QScrollArea>

GuideWidget::GuideWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: #1a3a1a;");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 顶部返回按钮
    auto* topBarLayout = new QHBoxLayout();
    backButton = new QPushButton("← 返回主菜单", this);
    backButton->setFixedSize(200, 45);
    backButton->setStyleSheet(
        "QPushButton { font-size: 18px; background-color: #607D8B; color: white;"
        "border-radius: 8px; font-weight: bold; border: 2px solid #546E7A; }"
        "QPushButton:hover { background-color: #78909C; border-color: #90A4AE; }"
        "QPushButton:pressed { background-color: #455A64; }");
    topBarLayout->addWidget(backButton, 0, Qt::AlignLeft);
    topBarLayout->addStretch();

    auto* titleLabel = new QLabel("📖 图鉴", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 36px; font-weight: bold; color: #4CAF50; padding: 10px;");

    // 标签页按钮
    auto* tabLayout = new QHBoxLayout();
    plantTabBtn = new QPushButton("🌱 植物", this);
    plantTabBtn->setFixedSize(150, 45);
    plantTabBtn->setStyleSheet(
        "QPushButton { font-size: 18px; background-color: #4CAF50; color: white;"
        "border-radius: 8px; font-weight: bold; border: 2px solid #45a049; }"
        "QPushButton:hover { background-color: #66BB6A; }");

    zombieTabBtn = new QPushButton("🧟 僵尸", this);
    zombieTabBtn->setFixedSize(150, 45);
    zombieTabBtn->setStyleSheet(
        "QPushButton { font-size: 18px; background-color: #607D8B; color: white;"
        "border-radius: 8px; font-weight: bold; border: 2px solid #546E7A; }"
        "QPushButton:hover { background-color: #78909C; }");

    tabLayout->addStretch();
    tabLayout->addWidget(plantTabBtn);
    tabLayout->addSpacing(20);
    tabLayout->addWidget(zombieTabBtn);
    tabLayout->addStretch();

    // 内容区域
    contentStack = new QStackedWidget(this);
    createPlantPage();
    createZombiePage();

    mainLayout->addLayout(topBarLayout);
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    mainLayout->addLayout(tabLayout);
    mainLayout->addWidget(contentStack, 1);

    connect(backButton, &QPushButton::clicked, this, &GuideWidget::onBackClicked);
    connect(plantTabBtn, &QPushButton::clicked, this, &GuideWidget::onTabPlantClicked);
    connect(zombieTabBtn, &QPushButton::clicked, this, &GuideWidget::onTabZombieClicked);
}

void GuideWidget::onBackClicked()
{
    emit backToMenuClicked();
}

void GuideWidget::onTabPlantClicked()
{
    contentStack->setCurrentIndex(0);
    plantTabBtn->setStyleSheet(
        "QPushButton { font-size: 18px; background-color: #4CAF50; color: white;"
        "border-radius: 8px; font-weight: bold; border: 2px solid #45a049; }"
        "QPushButton:hover { background-color: #66BB6A; }");
    zombieTabBtn->setStyleSheet(
        "QPushButton { font-size: 18px; background-color: #607D8B; color: white;"
        "border-radius: 8px; font-weight: bold; border: 2px solid #546E7A; }"
        "QPushButton:hover { background-color: #78909C; }");
}

void GuideWidget::onTabZombieClicked()
{
    contentStack->setCurrentIndex(1);
    zombieTabBtn->setStyleSheet(
        "QPushButton { font-size: 18px; background-color: #FF7043; color: white;"
        "border-radius: 8px; font-weight: bold; border: 2px solid #E64A19; }"
        "QPushButton:hover { background-color: #FF8A65; }");
    plantTabBtn->setStyleSheet(
        "QPushButton { font-size: 18px; background-color: #607D8B; color: white;"
        "border-radius: 8px; font-weight: bold; border: 2px solid #546E7A; }"
        "QPushButton:hover { background-color: #78909C; }");
}

void GuideWidget::createPlantPage()
{
    auto* plantPage = new QWidget();
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    auto* plantGrid = new QGridLayout();
    plantGrid->setSpacing(20);
    plantGrid->setContentsMargins(20, 20, 20, 20);

    // 不知火蛙
    auto* firefanCard = new QFrame();
    firefanCard->setStyleSheet("background-color: #2a5a2a; border-radius: 10px; padding: 15px;");
    auto* firefanLayout = new QVBoxLayout(firefanCard);
    auto* firefanImgLabel = new QLabel();
    QPixmap firefanPix(":/images/firefan/idle_0.png");
    if (!firefanPix.isNull()) {
        firefanImgLabel->setPixmap(firefanPix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    firefanImgLabel->setAlignment(Qt::AlignCenter);
    auto* firefanName = new QLabel("不知火蛙", firefanCard);
    firefanName->setStyleSheet("font-size: 18px; font-weight: bold; color: #4CAF50;");
    firefanName->setAlignment(Qt::AlignCenter);
    auto* firefanInfo = new QLabel("💴 100阳光\n💢 发射火焰\n❤️ 中等血量", firefanCard);
    firefanInfo->setStyleSheet("font-size: 14px; color: #8BC34A;");
    firefanInfo->setAlignment(Qt::AlignCenter);
    firefanLayout->addWidget(firefanImgLabel);
    firefanLayout->addWidget(firefanName);
    firefanLayout->addWidget(firefanInfo);

    // 金日葵
    auto* sunflowerCard = new QFrame();
    sunflowerCard->setStyleSheet("background-color: #2a5a2a; border-radius: 10px; padding: 15px;");
    auto* sunflowerLayout = new QVBoxLayout(sunflowerCard);
    auto* sunflowerImgLabel = new QLabel();
    QPixmap sunflowerPix(":/images/kimsunflower/idle_0.png");
    if (!sunflowerPix.isNull()) {
        sunflowerImgLabel->setPixmap(sunflowerPix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    sunflowerImgLabel->setAlignment(Qt::AlignCenter);
    auto* sunflowerName = new QLabel("金日葵", sunflowerCard);
    sunflowerName->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFD700;");
    sunflowerName->setAlignment(Qt::AlignCenter);
    auto* sunflowerInfo = new QLabel("💴 50阳光\n☀️ 产生阳光\n❤️ 低血量", sunflowerCard);
    sunflowerInfo->setStyleSheet("font-size: 14px; color: #8BC34A;");
    sunflowerInfo->setAlignment(Qt::AlignCenter);
    sunflowerLayout->addWidget(sunflowerImgLabel);
    sunflowerLayout->addWidget(sunflowerName);
    sunflowerLayout->addWidget(sunflowerInfo);

    // 熊绷果
    auto* bearCard = new QFrame();
    bearCard->setStyleSheet("background-color: #2a5a2a; border-radius: 10px; padding: 15px;");
    auto* bearLayout = new QVBoxLayout(bearCard);
    auto* bearImgLabel = new QLabel();
    QPixmap bearPix(":/images/bengbear/idle_0.png");
    if (!bearPix.isNull()) {
        bearImgLabel->setPixmap(bearPix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    bearImgLabel->setAlignment(Qt::AlignCenter);
    auto* bearName = new QLabel("熊绷果", bearCard);
    bearName->setStyleSheet("font-size: 18px; font-weight: bold; color: #795548;");
    bearName->setAlignment(Qt::AlignCenter);
    auto* bearInfo = new QLabel("💴 50阳光\n🛡️ 高血量\n❤️ 肉盾", bearCard);
    bearInfo->setStyleSheet("font-size: 14px; color: #8BC34A;");
    bearInfo->setAlignment(Qt::AlignCenter);
    bearLayout->addWidget(bearImgLabel);
    bearLayout->addWidget(bearName);
    bearLayout->addWidget(bearInfo);

    // 带派辣椒
    auto* chiliCard = new QFrame();
    chiliCard->setStyleSheet("background-color: #2a5a2a; border-radius: 10px; padding: 15px;");
    auto* chiliLayout = new QVBoxLayout(chiliCard);
    auto* chiliImgLabel = new QLabel();
    QPixmap chiliPix(":/images/rainchili/idle_0.png");
    if (!chiliPix.isNull()) {
        chiliImgLabel->setPixmap(chiliPix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    chiliImgLabel->setAlignment(Qt::AlignCenter);
    auto* chiliName = new QLabel("带派辣椒", chiliCard);
    chiliName->setStyleSheet("font-size: 18px; font-weight: bold; color: #FF5722;");
    chiliName->setAlignment(Qt::AlignCenter);
    auto* chiliInfo = new QLabel("💴 125阳光\n💥 爆炸伤害\n❤️ 一次性", chiliCard);
    chiliInfo->setStyleSheet("font-size: 14px; color: #8BC34A;");
    chiliInfo->setAlignment(Qt::AlignCenter);
    chiliLayout->addWidget(chiliImgLabel);
    chiliLayout->addWidget(chiliName);
    chiliLayout->addWidget(chiliInfo);

    plantGrid->addWidget(firefanCard, 0, 0);
    plantGrid->addWidget(sunflowerCard, 0, 1);
    plantGrid->addWidget(bearCard, 1, 0);
    plantGrid->addWidget(chiliCard, 1, 1);
    plantGrid->setRowStretch(2, 1);

    auto* plantContainer = new QWidget();
    plantContainer->setLayout(plantGrid);
    scrollArea->setWidget(plantContainer);

    auto* plantPageLayout = new QVBoxLayout(plantPage);
    plantPageLayout->addWidget(scrollArea);
    plantPageLayout->setContentsMargins(0, 0, 0, 0);

    contentStack->addWidget(plantPage);
}

void GuideWidget::createZombiePage()
{
    auto* zombiePage = new QWidget();
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: transparent; border: none;");

    auto* zombieGrid = new QGridLayout();
    zombieGrid->setSpacing(20);
    zombieGrid->setContentsMargins(20, 20, 20, 20);

    // 艮子僵尸
    auto* genziCard = new QFrame();
    genziCard->setStyleSheet("background-color: #3a2a2a; border-radius: 10px; padding: 15px;");
    auto* genziLayout = new QVBoxLayout(genziCard);
    auto* genziImgLabel = new QLabel();
    QPixmap genziPix(":/images/genzizombie/idle_0.png");
    if (!genziPix.isNull()) {
        genziImgLabel->setPixmap(genziPix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    genziImgLabel->setAlignment(Qt::AlignCenter);
    auto* genziName = new QLabel("艮子僵尸", genziCard);
    genziName->setStyleSheet("font-size: 18px; font-weight: bold; color: #FF7043;");
    genziName->setAlignment(Qt::AlignCenter);
    auto* genziInfo = new QLabel("💴 50阳光\n🚶 普通速度\n❤️ 中等血量", genziCard);
    genziInfo->setStyleSheet("font-size: 14px; color: #FFAB91;");
    genziInfo->setAlignment(Qt::AlignCenter);
    genziLayout->addWidget(genziImgLabel);
    genziLayout->addWidget(genziName);
    genziLayout->addWidget(genziInfo);

    // 舞王僵尸
    auto* danceCard = new QFrame();
    danceCard->setStyleSheet("background-color: #3a2a2a; border-radius: 10px; padding: 15px;");
    auto* danceLayout = new QVBoxLayout(danceCard);
    auto* danceImgLabel = new QLabel();
    QPixmap dancePix(":/images/dancingzombie/idle_0.png");
    if (!dancePix.isNull()) {
        danceImgLabel->setPixmap(dancePix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    danceImgLabel->setAlignment(Qt::AlignCenter);
    auto* danceName = new QLabel("舞王僵尸", danceCard);
    danceName->setStyleSheet("font-size: 18px; font-weight: bold; color: #FF5722;");
    danceName->setAlignment(Qt::AlignCenter);
    auto* danceInfo = new QLabel("💴 100阳光\n💃 召唤同伴\n❤️ 中等血量", danceCard);
    danceInfo->setStyleSheet("font-size: 14px; color: #FFAB91;");
    danceInfo->setAlignment(Qt::AlignCenter);
    danceLayout->addWidget(danceImgLabel);
    danceLayout->addWidget(danceName);
    danceLayout->addWidget(danceInfo);

    // 伴舞僵尸
    auto* dancerCard = new QFrame();
    dancerCard->setStyleSheet("background-color: #3a2a2a; border-radius: 10px; padding: 15px;");
    auto* dancerLayout = new QVBoxLayout(dancerCard);
    auto* dancerImgLabel = new QLabel();
    QPixmap dancerPix(":/images/dancingzombie/idle_0.png");
    if (!dancerPix.isNull()) {
        dancerImgLabel->setPixmap(dancerPix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    dancerImgLabel->setAlignment(Qt::AlignCenter);
    auto* dancerName = new QLabel("伴舞僵尸", dancerCard);
    dancerName->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFA000;");
    dancerName->setAlignment(Qt::AlignCenter);
    auto* dancerInfo = new QLabel("👯 跟随舞王\n🚶 普通速度\n❤️ 低血量", dancerCard);
    dancerInfo->setStyleSheet("font-size: 14px; color: #FFAB91;");
    dancerInfo->setAlignment(Qt::AlignCenter);
    dancerLayout->addWidget(dancerImgLabel);
    dancerLayout->addWidget(dancerName);
    dancerLayout->addWidget(dancerInfo);

    zombieGrid->addWidget(genziCard, 0, 0);
    zombieGrid->addWidget(danceCard, 0, 1);
    zombieGrid->addWidget(dancerCard, 1, 0);
    zombieGrid->setRowStretch(2, 1);

    auto* zombieContainer = new QWidget();
    zombieContainer->setLayout(zombieGrid);
    scrollArea->setWidget(zombieContainer);

    auto* zombiePageLayout = new QVBoxLayout(zombiePage);
    zombiePageLayout->addWidget(scrollArea);
    zombiePageLayout->setContentsMargins(0, 0, 0, 0);

    contentStack->addWidget(zombiePage);
}