#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server = new chatServer();
}

MainWindow::~MainWindow()
{
    delete server;
    delete ui;
}

chatServer* MainWindow::getServer()
{
    return server;
}


void MainWindow::on_pbQuit_clicked()
{
     QApplication::exit();
}
