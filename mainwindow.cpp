
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server = new chatServer(this, SLOT(logServer(QString)));
    sessionClient = new session();
    dbChat = new reciprocityDB();
    ui->pbStartServer->setEnabled(true);
    setConnectServerUI();
}

MainWindow::~MainWindow()
{
    delete server;
    delete dbChat;
    delete ui;
}

void MainWindow::setConnectServerUI()
{
    // нажимаем кнопку - запускаем сервер
    connect(ui->pbStartServer, &QPushButton::clicked,
            server, &chatServer::serverStart);
    // логирование действий на сервере
//    connect (server, &chatServer::logToMainWindow,
//             this, &MainWindow::logServer);
    connect(server, &chatServer::serverStarted,
            this,&MainWindow::netSuccess);
    // логирование чтения запроса
    connect (sessionClient, &session::logQueryReaded,
             this, &MainWindow::logServer);
    // логирование связи с БД
    connect(dbChat, &reciprocityDB::dbConnected,
            this, &MainWindow::logServer);
}

//void MainWindow::netError(const QString &text)
//{
//    ui->pbStartServer->setEnabled(true);
//    ui->teStatus->insertPlainText(text + "\n");
//}

void MainWindow::netSuccess()
{
    ui->pbStartServer->setEnabled(false);
    //ui->teStatus->insertPlainText(text);
 }

void MainWindow::logServer(const QString& text)
{
    ui->teStatus->insertPlainText(text);
}

