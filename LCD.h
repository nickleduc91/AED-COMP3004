#ifndef LCD_H
#define LCD_H

#include <string>
#include <QObject>
#include <QTimer>
#include <QTime>

using namespace std;

class LCD : public QObject {
    Q_OBJECT

    public:
        LCD();
        void setMessage(string message);
        QTimer* getTimer() { return timer; }
        void resetElapsedTime();

    private slots:
        void updateElapsedTime();

    private:
        QTimer *timer;
        QTime elapsedTime;

    signals:
        void callHandlelogToDisplay(string message, string type);

};
#endif // LCD_H
