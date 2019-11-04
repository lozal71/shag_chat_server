#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
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
