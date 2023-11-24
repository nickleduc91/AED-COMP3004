#ifndef LCD_H
#define LCD_H

#include <string>
#include <QObject>
#include <QTimer>

using namespace std;

class LCD : public QObject {
    Q_OBJECT
    public:
        LCD();
        void setMessage(string message);

    private:

    signals:
        void callHandlelogToDisplay(string message);

};
#endif // LCD_H
