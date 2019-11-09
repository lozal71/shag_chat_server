#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class windowServer; }
QT_END_NAMESPACE

class windowServer : public QMainWindow
{
    Q_OBJECT

public:
    windowServer(QWidget *parent = nullptr);
    ~windowServer();

private:
    Ui::windowServer *ui;
    chatServer* server;
    session *sessionClient;
    reciprocityDB *dbChat;
    void setConnectServerUI();
//    void netError(const QString& text);
    void netSuccess();


public slots:
    void logServer(const QString& text);
};
#endif // MAINWINDOW_H
