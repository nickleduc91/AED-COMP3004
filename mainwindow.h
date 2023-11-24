#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "AED.h"
#include <QVBoxLayout>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void logInfo(const string message);

private:
    Ui::MainWindow *ui;
    AED* aed;
    QLabel *label;
    QVBoxLayout *logWindow;

private slots:
    void powerOn();
    void checkResponsiveness();
    void callForHelp();
    void analyze();

public slots:
    void handleIlluminateGraphic(int step);
    void handleLogToDisplay(string message);
};
#endif // MAINWINDOW_H
