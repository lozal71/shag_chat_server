
#include "mainwindow.h"
#include "ui_mainwindow.h"

windowServer::windowServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::windowServer)
{
    ui->setupUi(this);
    server = new chatServer(this, SLOT(logServer(QString)));
    sessionClient = new session();
    dbChat = new reciprocityDB();
    ui->pbStartServer->setEnabled(true);
    setConnectServerUI();
}

windowServer::~windowServer()
{
    delete server;
    delete dbChat;
    delete ui;
}

void windowServer::setConnectServerUI()
{
    // нажимаем кнопку - запускаем сервер
    connect(ui->pbStartServer, &QPushButton::clicked,
            server, &chatServer::serverStart);
    // логирование действий на сервере
//    connect (server, &chatServer::logToMainWindow,
//             this, &MainWindow::logServer);
    connect(server, &chatServer::serverStarted,
            this,&windowServer::netSuccess);
    // логирование чтения запроса
    connect (sessionClient, &session::logQueryReaded,
             this, &windowServer::logServer);
    // логирование связи с БД
    connect(dbChat, &reciprocityDB::dbConnected,
            this, &windowServer::logServer);
}

//void MainWindow::netError(const QString &text)
//{
//    ui->pbStartServer->setEnabled(true);
//    ui->teStatus->insertPlainText(text + "\n");
//}

void windowServer::netSuccess()
{
    ui->pbStartServer->setEnabled(false);
    //ui->teStatus->insertPlainText(text);
 }

void windowServer::logServer(const QString& text)
{
    ui->teStatus->insertPlainText(text);
}

