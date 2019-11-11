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
    chatServer* getServer();
private slots:
    void on_pbQuit_clicked();

private:
    Ui::MainWindow *ui;
    chatServer* server;
};
#endif // MAINWINDOW_H
