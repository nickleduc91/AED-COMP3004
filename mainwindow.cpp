#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Disable all buttons until power is on
    disableButtons();

    //Slots
    connect(ui->powerButton, SIGNAL(released()), this, SLOT(powerOn()));
    connect(ui->checkButton, SIGNAL(released()), this, SLOT(checkResponsiveness()));
    connect(ui->callButton, SIGNAL(released()), this, SLOT(callForHelp()));
    connect(ui->attachButton, SIGNAL(released()), this, SLOT(analyze()));
    connect(ui->dontTouchButton, SIGNAL(released()), this, SLOT(compress()));

    //Set up the log info section on the GUI
    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter); // Set horizontal alignment to center
    QFont font = label->font();
    font.setPointSize(16);  // Set the font size to 16 (you can adjust as needed)
    label->setFont(font);
    logWindow = new QVBoxLayout;
    logWindow->addWidget(label);
    QWidget *scrollContent = new QWidget;
    scrollContent->setLayout(logWindow);
    ui->consoleScrollArea->setWidget(scrollContent);
    AED* aedDevice = new AED();
    aed = aedDevice;

    connect(aed->display->getGraphics(), &Graphics::callHandleIlluminateGraphic, this, &MainWindow::handleIlluminateGraphic);
    connect(aed->display->getLCD(), &LCD::callHandlelogToDisplay, this, &MainWindow::handleLogToDisplay);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::powerOn() {
    if(!aed->isOn()) {
        ui->aedFrame->setStyleSheet(nullptr);
        aed->handlePowerOn();
    } else {
        disableButtons();
        aed->handlePowerOff();
    }

}

void MainWindow::checkResponsiveness() {
    aed->handleCheckResponsiveness();
}

void MainWindow::callForHelp() {
    aed->handleCallForHelp();
}

void MainWindow::analyze() {
    aed->handleAnalyze();
}

void MainWindow::compress() {

}

void MainWindow::logInfo(const string message) {
    QString textToAdd = QString::fromStdString(message);

    // Split the message into words
    QStringList words = textToAdd.split(' ');

    // Check if the number of words is more than 3
    if (words.size() > 3) {
        // If yes, insert a line break after the third word
        QString formattedText = words.mid(0, 3).join(' ') + "\n" + words.mid(3).join(' ');
        label->setText(formattedText);
    } else {
        // If not, display the message as it is
        label->clear();
        label->setText(textToAdd);
    }
}

//Function to handle updating the status of the elevators
void MainWindow::handleIlluminateGraphic(int step) {
    if(step == 1) {
        ui->checkButton->setStyleSheet("background-color: yellow;");
        ui->checkButton->setEnabled(true);
    } else if(step == 2) {
        ui->callButton->setStyleSheet("background-color: yellow;");
        ui->callButton->setEnabled(true);
        ui->checkButton->setStyleSheet("");
        ui->checkButton->setDisabled(true);

    } else if(step == 3) {
        ui->attachButton->setStyleSheet("background-color: yellow;");
        ui->attachButton->setEnabled(true);
        ui->callButton->setStyleSheet("");
        ui->callButton->setDisabled(true);
    } else if(step == 4) {
        ui->dontTouchButton->setStyleSheet("background-color: yellow;");
        ui->dontTouchButton->setEnabled(true);
        ui->attachButton->setStyleSheet("");
        ui->attachButton->setDisabled(true);
    } else if(step == 5) {
        ui->compressButton->setStyleSheet("background-color: yellow;");
        ui->compressButton->setEnabled(true);
        ui->dontTouchButton->setStyleSheet("");
        ui->dontTouchButton->setDisabled(true);
    }
}

void MainWindow::handleLogToDisplay(string message, string type) {
    if(type == "time") {
        ui->elapsedTimeLabel->setText(QString::fromStdString(message));
    } else {
        logInfo(message);
    }

}

void MainWindow::disableButtons() {
    //Disable all buttons until power is on
    ui->aedFrame->setStyleSheet("background-color: rgba(51, 51, 51, 150);");
    ui->checkButton->setDisabled(true);
    ui->callButton->setDisabled(true);
    ui->attachButton->setDisabled(true);
    ui->dontTouchButton->setDisabled(true);
    ui->compressButton->setDisabled(true);
    ui->breatheButton->setDisabled(true);

    ui->checkButton->setStyleSheet("");
    ui->callButton->setStyleSheet("");
    ui->attachButton->setStyleSheet("");
    ui->dontTouchButton->setStyleSheet("");
    ui->compressButton->setStyleSheet("");
    ui->breatheButton->setStyleSheet("");
}























