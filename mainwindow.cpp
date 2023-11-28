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
    connect(ui->attachButton, SIGNAL(released()), this, SLOT(attach()));
    connect(ui->analyzeButton, SIGNAL(released()), this, SLOT(analyze()));
    connect(ui->shockButton, SIGNAL(released()), this, SLOT(shock()));
    connect(ui->changeBatteries, SIGNAL(released()), this, SLOT(changeBatteries()));


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
    ui->status_text->setStyleSheet("background-color: white; border:");

    AED* aedDevice = new AED();
    aed = aedDevice;

    connect(aed->display->getGraphics(), &Graphics::callHandleIlluminateGraphic, this, &MainWindow::handleIlluminateGraphic);
    connect(aed->display->getLCD(), &LCD::callHandlelogToDisplay, this, &MainWindow::handleLogToDisplay);
    connect(aed, &AED::callHandleStatusUpdate, this, &MainWindow::handleStatusUpdate);
    connect(aed, SIGNAL(updateBatteryLevel(int)), this, SLOT(updateBatteryLevel(int)));
    connect(aed, SIGNAL(deadBattery()), this, SLOT(deadAED()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::powerOn() {
    if(!aed->isOn()) {
        //setting self test variables from ui
        bool defibConnection = ui->defib_electro->isChecked();
        bool ecgCircuitry = ui->ecg_circuitry->isChecked();
        bool defibCharge = ui->defib_charge_discharge->isChecked();
        bool microprocessorHardSoftware = ui->microprossesor_hard_soft->isChecked();
        bool cprCircuitrySensor = ui->cpr_circuitry_sensor->isChecked();
        bool audioCircuitry = ui->audio_circuitry->isChecked();

        //set victim weight and height
        int victimAge = ui->ageBox->value();
        int victimWeight = ui->weightBox->value();
        aed->setVictim(victimAge, victimWeight);

        //Disable height and weight spinners when aed turns on
        ui->ageBox->setDisabled(true);
        ui->weightBox->setDisabled(true);

        //calling self test and exiting function if test fails
        if(!aed->performSelfTest(defibConnection,ecgCircuitry,defibCharge,microprocessorHardSoftware,cprCircuitrySensor,audioCircuitry)) {return;}
        ui->aedFrame->setStyleSheet(nullptr);
        aed->handlePowerOn();
    } else {
        disableButtons();
        //Enable height and weight spinners when aed turns off
        ui->ageBox->setEnabled(true);
        ui->weightBox->setEnabled(true);
        ui->status_text->setStyleSheet("background-color: white; border:");

        aed->handlePowerOff();
    }

}

void MainWindow::deadAED(){
    ui->battery_lcd->display(0);
    ui->battery_lcd->setStyleSheet("background-color: black; color: white; border-radius: 7px;");
    ui->changeBatteries->setStyleSheet("background-color: yellow;");
    aed->setIsPoweredOn(true);
    powerOn();
}

void MainWindow::updateBatteryLevel(int batteryLevel) {
    ui->battery_lcd->display(batteryLevel);
    if (batteryLevel <= 20 && batteryLevel > 10){
        ui->battery_lcd->setStyleSheet("background-color: yellow; border-radius: 7px;");
    } else if (batteryLevel <= 10) {
        ui->battery_lcd->setStyleSheet("background-color: red; color: white; border-radius: 7px;");
    }
}

void MainWindow::checkResponsiveness() {
    aed->handleCheckResponsiveness();
}

void MainWindow::callForHelp() {
    aed->handleCallForHelp();
}

void MainWindow::attach() {

    bool isLeftChecked = ui->leftNipBox->isChecked();
    bool isRightChecked = ui->rightNipBox->isChecked();
    bool isbackBoxChecked = ui->backBox->isChecked();
    bool istearPadChecked = ui->tearPadBox->isChecked();

    aed->handleAttach(isLeftChecked, isRightChecked, isbackBoxChecked, istearPadChecked);
}

void MainWindow::analyze() {
    aed->handleAnalyze();
}

void MainWindow::shock() {
    aed->handleShock();
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
        ui->analyzeButton->setStyleSheet("background-color: yellow;");
        ui->analyzeButton->setEnabled(true);
        ui->attachButton->setStyleSheet("");
        ui->attachButton->setDisabled(true);
    } else if(step == 5) {
        ui->compressButton->setStyleSheet("background-color: yellow;");
        ui->compressButton->setEnabled(true);
        ui->analyzeButton->setStyleSheet("");
        ui->analyzeButton->setDisabled(true);
        ui->shockButton->setStyleSheet("");
        ui->shockButton->setDisabled(true);
    } else if (step == 6) {
        ui->shockButton->setStyleSheet("background-color: yellow;");
        ui->shockButton->setEnabled(true);
        ui->analyzeButton->setStyleSheet("");
        ui->analyzeButton->setDisabled(true);
    }
}

void MainWindow::handleLogToDisplay(string message, string type) {
    if(type == "time") {
        ui->elapsedTimeLabel->setText(QString::fromStdString(message));
    } else if(type == "shock") {
        ui->shockCount->display(QString::fromStdString(message));
    } else {
        logInfo(message);
    }

}

void MainWindow::handleStatusUpdate(string message,bool status) {
    ui->status_text->setText(QString::fromStdString(message));
    if(status){
        ui->status_text->setStyleSheet("color: green;border: 1px solid green; background-color: white;");
        ui->test_control_panel->setEnabled(false);
    }else{
        ui->status_text->setStyleSheet("color: red;border: 1px solid red; background-color: white;");
    }
}

void MainWindow::changeBatteries() {
    aed->setBatteryLevel(100);
    ui->battery_lcd->display(100);
    ui->battery_lcd->setStyleSheet("background-color: rgb(46, 194, 126); color: rgb(255, 255, 255); border-radius: 7px;");
    ui->changeBatteries->setStyleSheet(""); //clears the styleSheet and sets the background to normal
    aed->handleNewBatteries();
    aed->setIsPoweredOn(true);
    powerOn();
}

void MainWindow::disableButtons() {
    //Disable all buttons until power is on
    ui->aedFrame->setStyleSheet("background-color: rgba(51, 51, 51, 150);");
    ui->checkButton->setDisabled(true);
    ui->callButton->setDisabled(true);
    ui->attachButton->setDisabled(true);
    ui->analyzeButton->setDisabled(true);
    ui->compressButton->setDisabled(true);
    ui->breatheButton->setDisabled(true);
    ui->shockButton->setDisabled(true);

    ui->checkButton->setStyleSheet("");
    ui->callButton->setStyleSheet("");
    ui->attachButton->setStyleSheet("");
    ui->analyzeButton->setStyleSheet("");
    ui->compressButton->setStyleSheet("");
    ui->breatheButton->setStyleSheet("");
    ui->shockButton->setStyleSheet("");

    //self test panel reset
    ui->status_text->setText("SELF-TEST STATUS");
    ui->status_text->setStyleSheet("");
    ui->test_control_panel->setEnabled(true); //enable test panel when off
}
