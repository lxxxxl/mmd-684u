#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // assign bitmasks to PushButtons
    ui->pushButton_1->setProperty("mask",0b0000000000000001);
    ui->pushButton_2->setProperty("mask",0b0000000000000010);
    ui->pushButton_4->setProperty("mask",0b0000000000001000);
    ui->pushButton_5->setProperty("mask",0b0000000000010000);
    ui->pushButton_6->setProperty("mask",0b0000000000100000);
    ui->pushButton_7->setProperty("mask",0b0000000001000000);
    ui->pushButton_8->setProperty("mask",0b0000000010000000);
    ui->pushButton_9->setProperty("mask",0b0000000100000000);
    ui->pushButton_10->setProperty("mask",0b0000001000000000);
    ui->pushButton_11->setProperty("mask",0b0000010000000000);
    ui->pushButton_12->setProperty("mask",0b0000100000000000);
    ui->pushButton_13->setProperty("mask",0b0001000000000000);
    ui->pushButton_14->setProperty("mask",0b0010000000000000);
    ui->pushButton_15->setProperty("mask",0b0100000000000000);

    // setup onClick signals-slots
    connect(ui->pushButton_1, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_2, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_4, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_5, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_6, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_7, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_8, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_9, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_10, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_11, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_12, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_13, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_14, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);
    connect(ui->pushButton_15, &QPushButton::released, this, &MainWindow::on_pushButton_clicked);

    // initial velue of hexcode
    this->hexcode = 0;

    //prevent window resizing
    this->setFixedSize(QSize(280, 360));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QPushButton *button = (QPushButton*)sender();


    unsigned short mask = button->property("mask").toUInt();
    if (button->isChecked())
        this->hexcode |= mask;
    else
        this->hexcode &= ~mask;

    ui->lineEdit->setText(QString("0b%1").arg(this->hexcode, 16, 2, QLatin1Char( '0' )));
}


