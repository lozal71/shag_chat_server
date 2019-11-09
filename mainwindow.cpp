#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    server = new chatServer(this, SLOT(logServer(QString)));
    server = new chatServer();
 //   sessionClient = new session();
//    dbChat = new reciprocityDB();
    ui->pbStartServer->setEnabled(true);
    setConnectServerUI();
}

MainWindow::~MainWindow()
{
    delete server;
    //delete dbChat;
    delete ui;
}

void MainWindow::setConnectServerUI()
{
    // нажимаем кнопку - запускаем сервер
    connect(ui->pbStartServer, &QPushButton::clicked,
            server, &chatServer::serverStart);
    // сервер запущен - делаем кнопку запуска сервера недоступной
    connect(server, &chatServer::serverStarted,
            this,&MainWindow::buttonStartNotEnable);
//    // логирование чтения запроса
//    connect (sessionClient, &session::logQueryReaded,
//             this, &windowServer::logServer);
//    // логирование связи с БД
//    connect(dbChat, &reciprocityDB::dbConnected,
//            this, &windowServer::logServer);
}

//void MainWindow::netError(const QString &text)
//{
//    ui->pbStartServer->setEnabled(true);
//    ui->teStatus->insertPlainText(text + "\n");
//}

void MainWindow::buttonStartNotEnable()
{
    ui->pbStartServer->setEnabled(false);
}

//void windowServer::logServer(const QString& text)
//{
//    ui->teStatus->insertPlainText(text);
//}

