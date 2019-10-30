#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server = new chatServer();
    connect(server, &chatServer::serverError,
            this, &MainWindow::netError);
    connect (server, &chatServer::serverStarted,
             this, &MainWindow::netSuccess);
    connect(ui->pbStartServer, &QPushButton::clicked,
            server, &chatServer::serverStart);
    connect(server, &chatServer::oneSessionClosed,
            this, &MainWindow::logSessionClosed);
    connect(server, &chatServer::dbConnected,
            this, &MainWindow::logConnectDb);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::netError(const QString &text)
{
    ui->pbStartServer->setEnabled(true);
    ui->teStatus->insertPlainText(text + "\n");
}

void MainWindow::netSuccess()
{
    ui->pbStartServer->setEnabled(false);
    ui->teStatus->insertPlainText("server start success\n");
 }

void MainWindow::logSessionClosed()
{
    ui->teStatus->insertPlainText("one session closed\n");

}

void MainWindow::logConnectDb()
{
    ui->teStatus->insertPlainText("connect to DB\n");
}


