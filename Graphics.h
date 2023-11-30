#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <QObject>
using namespace std;
#include <string>
#include <iostream>

class Graphics : public QObject {
    Q_OBJECT
    public:
        Graphics();
        void illuminateGraphic(int step);
        void disableStep(int step) { emit callHandleDisableStep(step); }

    private:

    signals:
        void callHandleIlluminateGraphic(int step);
        void callHandleDisableStep(int step);

};
#endif // GRAPHICS_H
