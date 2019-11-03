#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server = new chatServer();
    sessionClient = new session();
    dbChat = new reciprocityDB();
    ui->pbStartServer->setEnabled(true);
    connectServerUI();
}

MainWindow::~MainWindow()
{
    delete server;
    delete sessionClient;
    delete  dbChat;
    delete ui;
}

void MainWindow::connectServerUI()
{
    // нажимаем кнопку - запускаем сервер
    connect(ui->pbStartServer, &QPushButton::clicked,
            server, &chatServer::serverStart);
    // запуск сервера
    connect (server, &chatServer::serverStarted,
             this, &MainWindow::netSuccess);
    // закрытие сессии
    connect(server, &chatServer::oneSessionClosed,
            this, &MainWindow::logSessionClosed);
    // связь с БД
    connect(dbChat, &reciprocityDB::dbConnected,
            this, &MainWindow::logConnectDb);
    // ошибка на сервере
    connect(server, &chatServer::serverError,
            this, &MainWindow::netError);

    connect (sessionClient, &session::logQueryReaded,
             this, &MainWindow::logServer);
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

void MainWindow::logSessionClosed(int id)
{
    ui->teStatus->insertPlainText("session "+QString::number(id) +" closed\n");

}

void MainWindow::logConnectDb()
{
    ui->teStatus->insertPlainText("connect to DB\n");
}

void MainWindow::logServer(QString sParam)
{
    ui->teStatus->insertPlainText(sParam);
}


