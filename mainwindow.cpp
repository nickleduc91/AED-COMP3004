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

    //Singals and Slots for UI buttons to handle functionality when pressing them
    connect(ui->powerButton, SIGNAL(released()), this, SLOT(powerOn()));
    connect(ui->checkButton, SIGNAL(released()), this, SLOT(checkResponsiveness()));
    connect(ui->callButton, SIGNAL(released()), this, SLOT(callForHelp()));
    connect(ui->attachButton, SIGNAL(released()), this, SLOT(attach()));
    connect(ui->analyzeButton, SIGNAL(released()), this, SLOT(analyze()));
    connect(ui->shockButton, SIGNAL(released()), this, SLOT(shock()));
    connect(ui->changeBatteries, SIGNAL(released()), this, SLOT(changeBatteries()));
    connect(ui->compressButton, SIGNAL(released()), this, SLOT(compress()));
    connect(ui->ageBox, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxAgeChanged(int)));
    connect(ui->breatheButton, SIGNAL(released()), this, SLOT(breaths()));
    connect(ui->plugInOutButton, SIGNAL(released()), this, SLOT(plugInOut()));
    connect(ui->VTachButton, SIGNAL(released()), this, SLOT(VTach()));
    connect(ui->VFibButton, SIGNAL(released()), this, SLOT(VFib()));
    connect(ui->flatlineButton, SIGNAL(released()), this, SLOT(flatline()));
    connect(ui->sinusButton, SIGNAL(released()), this, SLOT(sinus()));

    //ECG Graph setup
    ui->ecgGraph->addGraph();
    ui->ecgGraph->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->ecgGraph->xAxis->setRange(0,1000);
    ui->ecgGraph->yAxis->setRange(0,140);
    ui->ecgGraph->xAxis->setTickLabels(false);
    ui->ecgGraph->yAxis->setTickLabels(false);

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

    //Creating the AED
    AED* aedDevice = new AED();
    aed = aedDevice;

    //Singals and Slots to handle functionality from other classses
    connect(aed->display->getGraphics(), &Graphics::callHandleIlluminateGraphic, this, &MainWindow::handleIlluminateGraphic);
    connect(aed->display->getGraphics(), &Graphics::callHandleDisableStep, this, &MainWindow::handleDisableStep);
    connect(aed->display->getLCD(), &LCD::callHandlelogToDisplay, this, &MainWindow::handleLogToDisplay);
    connect(aed, &AED::callHandleStatusUpdate, this, &MainWindow::handleStatusUpdate);
    connect(aed, SIGNAL(updateBatteryLevel(int)), this, SLOT(updateBatteryLevel(int)));
    connect(aed, SIGNAL(deadBattery()), this, SLOT(deadAED()));
    connect(aed, SIGNAL(pushHarder()), this, SLOT(needHarderCompressions()));
    connect(aed->display->getLCD(), &LCD::callHandleResetECG, this, &MainWindow::handleResetECG);
    connect(aed, &AED::enableRhythms, this, &MainWindow::handleEnableRhythms);

    //Signals and Slots to handle functionality for the graphs
    connect(aed, &AED::vfib_graph_signal, this, &MainWindow::vfib_graph_slot);
    connect(aed, &AED::vtac_graph_signal, this, &MainWindow::vtac_graph_slot);
    connect(aed, &AED::normal_graph_signal, this, &MainWindow::normal_graph_slot);
    connect(aed, &AED::flatline_graph_signal, this, &MainWindow::flatline_graph_slot);
}

MainWindow::~MainWindow()
{
    //Destructor for MainWindow
    delete ui;
}

void MainWindow::handleEnableRhythms(bool enable) {
    if(enable) {
        ui->VFibButton->setEnabled(true);
        ui->VTachButton->setEnabled(true);
        ui->flatlineButton->setEnabled(true);
        ui->sinusButton->setEnabled(true);
    } else {
        ui->VFibButton->setDisabled(true);
        ui->VTachButton->setDisabled(true);
        ui->flatlineButton->setDisabled(true);
        ui->sinusButton->setDisabled(true);
    }

}

void MainWindow::sinus() {
    aed->handleRhythmChange(0);
}

void MainWindow::VTach() {
    aed->handleRhythmChange(2);
}

void MainWindow::VFib() {
    aed->handleRhythmChange(1);
}

void MainWindow::flatline() {
    aed->handleRhythmChange(3);
}

void MainWindow::handleResetECG() {
    //Handle resetting the ECG graph
    //Clear all the data points
    ui->ecgGraph->graph(0)->data().data()->clear();
    ui->ecgGraph->replot();
}
void MainWindow::onSpinBoxAgeChanged(int age) {
    //Handles if patient has a hairy chest that is required to shave to use AED
    if(age <= 8) {
        ui->hairyChestBox->setDisabled(true);
        ui->hairyChestBox->setChecked(false);
    } else {
        ui->hairyChestBox->setEnabled(true);
    }
}

void MainWindow::plugInOut() {
    //Handles plugging in Electrode
    //Button labels
    if(!aed->electrode->isElectrodePluggedIn()) {
        ui->plugInOutButton->setText("Unplug Electrode");
    } else {
        ui->plugInOutButton->setText("Plug In Electrode");
    }
    //AED functions when Electrodes are plugged in
    aed->handlePlugInOutElectrode();
}

void MainWindow::powerOn() {
    //Handles turning on the AED
    if(!aed->isOn()) {
        //setting self test variables from ui
        bool ecgCircuitry = ui->ecg_circuitry->isChecked();
        bool defibCharge = ui->defib_charge_discharge->isChecked();
        bool microprocessorHardSoftware = ui->microprossesor_hard_soft->isChecked();
        bool cprCircuitrySensor = ui->cpr_circuitry_sensor->isChecked();
        bool audioCircuitry = ui->audio_circuitry->isChecked();

        //set victim weight and height
        int victimAge = ui->ageBox->value();
        int victimWeight = ui->weightBox->value();
        bool isWet = ui->wetChestBox->isChecked();
        bool isHairy = ui->hairyChestBox->isChecked();

        aed->setVictim(victimAge, victimWeight, isHairy, isWet);

        //Disable clipper based on age (hair is associated with age)
        isHairy ? ui->useClipperBox->setDisabled(false) : ui->useClipperBox->setDisabled(true);
        //Disable towel based on if patient is wet
        isWet ? ui->useTowelBox->setDisabled(false) : ui->useTowelBox->setDisabled(true);

        //Disable height and weight spinners when aed turns on
        ui->ageBox->setDisabled(true);
        ui->weightBox->setDisabled(true);
        ui->wetChestBox->setDisabled(true);
        ui->hairyChestBox->setDisabled(true);

        //calling self test and exiting function if test fails
        if(!aed->performSelfTest(ecgCircuitry,defibCharge,microprocessorHardSoftware,cprCircuitrySensor,audioCircuitry)) {return;}
        ui->aedFrame->setStyleSheet(nullptr);
        aed->handlePowerOn();
    } else {
        //turn of display when AED is off
        disableButtons();
        //Enable height and weight spinners when aed turns off
        ui->ageBox->setEnabled(true);
        ui->weightBox->setEnabled(true);
        ui->wetChestBox->setEnabled(true);
        ui->status_text->setStyleSheet("background-color: white; border:");

        aed->handlePowerOff();
    }

}

void MainWindow::deadAED(){
    //When battery is dead, sets battery to 0, display to black
    //Highlights change battery button
    //Displays to LCD to change batteries
    ui->battery_lcd->display(0);
    ui->battery_lcd->setStyleSheet("background-color: black; color: white; border-radius: 7px;");
    ui->changeBatteries->setStyleSheet("background-color: yellow;");
    //This toggles the power to off
    aed->setIsPoweredOn(true);
    aed->setDelayedMessage("CHANGE BATTERIES", 1000);
    powerOn();
}

void MainWindow::updateBatteryLevel(int batteryLevel) {
    //Updates battery level display depending on battery level
    //Sets colour to correspond with battery level
    ui->battery_lcd->display(batteryLevel);
    if (batteryLevel <= 20 && batteryLevel > 10){
        ui->battery_lcd->setStyleSheet("background-color: yellow; border-radius: 7px;");
    } else if (batteryLevel <= 10) {
        ui->battery_lcd->setStyleSheet("background-color: red; color: white; border-radius: 7px;");
    }
}

void MainWindow::checkResponsiveness() {
    //Prompts the user to check responsiveness of patient
    aed->handleCheckResponsiveness();
}

void MainWindow::callForHelp() {
    //Prompts the user to call for help
    aed->handleCallForHelp();
}

void MainWindow::attach() {
    //AED checks for correct electrode placement when atttaching
    bool isLeftChecked = ui->leftNipBox->isChecked();
    bool isRightChecked = ui->rightNipBox->isChecked();
    bool isbackBoxChecked = ui->backBox->isChecked();
    bool istearPadChecked = ui->tearPadBox->isChecked();
    bool towel = ui->useTowelBox->isChecked();
    bool clip = ui->useClipperBox->isChecked();
    //Handles attaching electrodes to user
    aed->handleAttach(isLeftChecked, isRightChecked, isbackBoxChecked, istearPadChecked, towel, clip);
}

void MainWindow::analyze() {
    //Handles analyzing patient's condition once electrodes are placed
    aed->handleAnalyze();
}

void MainWindow::shock() {
    //Handles administering shock to patient
    aed->handleShock();
}

void MainWindow::compress() {
    //Disables compress button once pressed

    ui->compressButton->setStyleSheet("");
    ui->compressButton->setEnabled(false);

    //Checks if compressions administered to patient are of correct depth specification (Adult vs Child)
    //Handles administering compressions to patient

    if ((aed->isAdult() && (ui->twoInches->isChecked() || ui->twoAndHalfInches->isChecked())) || (!aed->isAdult() && (ui->oneAndHalfInches->isChecked() || ui->twoInches->isChecked() || ui->twoAndHalfInches->isChecked()))){
       aed->handleCompress(true);
    } else if ((!aed->isAdult() && (ui->oneInch->isChecked() || ui->halfInches->isChecked())) || (aed->isAdult() && (ui->oneAndHalfInches->isChecked() || ui->oneInch->isChecked() || ui->halfInches->isChecked()))){
       aed->handleCompress(false);
    }
}

void MainWindow::breaths() {
    //Disables breathe button once pressed
    ui->breatheButton->setStyleSheet("");
    ui->breatheButton->setEnabled(false);
    //Handles administering breathes to patient
    aed->handleBreathe();
}

void MainWindow::needHarderCompressions() {
    //If compressions are not deep enough, prompt user for harder compressions and reenables compress button
    ui->compressButton->setEnabled(true);
    ui->compressButton->setStyleSheet("background-color: yellow;");
}

void MainWindow::logInfo(const string message) {
    //Used for forrmatting and displaying text on UI (LCD)
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

//Function to handle updating the the graphics
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
        ui->twoInches->setChecked(true);
        ui->analyzeButton->setStyleSheet("");
        ui->analyzeButton->setDisabled(true);
        ui->shockButton->setStyleSheet("");
        ui->shockButton->setDisabled(true);
    } else if (step == 6) {
        ui->shockButton->setStyleSheet("background-color: yellow;");
        ui->shockButton->setEnabled(true);
        ui->analyzeButton->setStyleSheet("");
        ui->analyzeButton->setDisabled(true);
    } else if (step == 7) {
        ui->breatheButton->setEnabled(true);
        ui->breatheButton->setStyleSheet("background-color: yellow;");
    }
}

void MainWindow::handleLogToDisplay(string message, string type) {
    //Handles updating messages on UI based on message type.
    if(type == "time") {
        ui->elapsedTimeLabel->setText(QString::fromStdString(message));
    } else if(type == "shock") {
        ui->shockCount->display(QString::fromStdString(message));
    } else {
        logInfo(message);
    }

}

void MainWindow::handleStatusUpdate(string message,bool status) {
    //Handles updating UI based on AED status (working/not working)
    ui->status_text->setText(QString::fromStdString(message));
    if(status){
        ui->status_text->setStyleSheet("color: green;border: 1px solid green; background-color: white;");
        ui->test_control_panel->setEnabled(false);
    }else{
        ui->status_text->setStyleSheet("color: red;border: 1px solid red; background-color: white;");
        aed->failedSelfTest();
    }
}

void MainWindow::changeBatteries() {
    //Handles chaning the batteries of the AED
    aed->setBatteryLevel(100);
    ui->battery_lcd->display(100);
    ui->battery_lcd->setStyleSheet("background-color: rgb(46, 194, 126); color: rgb(255, 255, 255); border-radius: 7px;");
    ui->changeBatteries->setStyleSheet(""); //clears the styleSheet and sets the background to normal
    aed->handleNewBatteries();
    //Toggles power to On
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

    ui->useTowelBox->setChecked(false);
    ui->useClipperBox->setChecked(false);
    ui->hairyChestBox->setDisabled(true);

    //self test panel reset
    ui->status_text->setText("SELF-TEST STATUS");
    ui->status_text->setStyleSheet("");
    ui->test_control_panel->setEnabled(true); //enable test panel when off
}

void MainWindow::handleDisableStep(int step) {
    //Handles disabling certain buttons based on current AED step
    if(step == 4) {
        ui->analyzeButton->setDisabled(true);
        ui->analyzeButton->setStyleSheet("");
    } else if(step == 6) {
        ui->shockButton->setDisabled(true);
        ui->shockButton->setStyleSheet("");
    } else if(step == 5) {
        ui->compressButton->setDisabled(true);
        ui->compressButton->setStyleSheet("");
    }
}

void MainWindow::vfib_graph_slot(){
    //Generating VFIB pattern for ECG Graph
    QVector<double> x, y;
    //Handles generating random numbers for chaotic representation of VFIB pattern
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 100.0); // Range for random values
    //Populate Y values. Constant for loop through, because X values correspond to time
    //Step is used for scale
    const double step = 2;
       for (int i = 0; i <= 100; i += step) {
           double x_val = i / 10.0;

           // Generate random values for a and b
           double a = (3 * dis(gen)) - (3 * dis(gen));
           double b = (2 * dis(gen)) - (2 * dis(gen));

           // Calculate a Y value derived from a sin wave value based on x value and random a & b values
           double result = 0.0;
           for (int n = 1; n <= 100; ++n) {
               result += sin((a * x_val * M_PI) + b) / (0.01 * n + 8);
           }

           x.append(x_val);
           y.append(result);
    }
    //Plot VFIB pattern on ECG graph
    ui->ecgGraph->xAxis->setTickLabels(false);
    ui->ecgGraph->yAxis->setTickLabels(false);
    ui->ecgGraph->graph(0)->setData(x, y);
    ui->ecgGraph->xAxis->setRange(-5, 5);
    ui->ecgGraph->yAxis->setRange(-10, 10);
    ui->ecgGraph->graph(0)->rescaleAxes();
    ui->ecgGraph->replot();
}

void MainWindow::vtac_graph_slot(){
    //Generating VTAC pattern for ECG Graph
    //Pattern is of repeating humps
    QVector<double> x, y;
    //Populate Y values. Constant for loop through, because X values correspond to time
    //Step is used for scale
    const double step = 2;
    for (int i = 0; i <= 100; i += step) {
        // Calculate a Y value derived from a sin wave value based on X value
        double x_val = i / 10.0;
        double result = 6 * (fabs(sin(x_val))) * fabs(sin(x_val));
        x.append(x_val);
        y.append(result);
    }
    //Plot VTAC pattern on ECG graph
    ui->ecgGraph->xAxis->setTickLabels(false);
    ui->ecgGraph->yAxis->setTickLabels(false);
    ui->ecgGraph->graph(0)->setData(x, y);
    ui->ecgGraph->xAxis->setRange(-5, 5);
    ui->ecgGraph->yAxis->setRange(-10, 10);
    ui->ecgGraph->graph(0)->rescaleAxes();
    ui->ecgGraph->replot();
}

void MainWindow::normal_graph_slot(){
    //Generating Normal Pattern for ECG Graph
    //Pattern is of steady rhythm and rate
    QVector<double> x, y;
    //Populate Y values. Constant for loop through, because X values correspond to time
    //Step is used for scale
    const double step = 2;
    for (int i = 0; i <= 300; i += step) {
            // Calculate a Y value derived from a sin wave value based on X value
            double x_val = i / 10.0;
            double result = (-0.5 * pow(sin(0.3 * x_val - 3), 10) +
                            0.8 * pow(sin(0.3 * x_val + 5.5), 22) +
                            7 * pow(sin(0.3 * x_val), 300) +
                            0.5 * pow(sin(0.3 * x_val + 0.9), 16) -
                            0.2 * pow(sin(0.5 * x_val), 10));
            x.append(x_val);
            y.append(result);
    }
    //Plot Normal pattern on ECG graph
    ui->ecgGraph->xAxis->setTickLabels(false);
    ui->ecgGraph->yAxis->setTickLabels(false);
    ui->ecgGraph->graph(0)->setData(x, y);
    ui->ecgGraph->xAxis->setRange(-5, 5);
    ui->ecgGraph->yAxis->setRange(-10, 10);
    ui->ecgGraph->graph(0)->rescaleAxes();
    ui->ecgGraph->replot();
}

void MainWindow::flatline_graph_slot(){
    //Generating a Flatline pattern for ECG Graph
    QVector<double> x ={0,1}, y = {5,5};
    //Simple data points to create a flat line
    //Plot flatline pattern on ECG graph
    ui->ecgGraph->xAxis->setTickLabels(false);
    ui->ecgGraph->yAxis->setTickLabels(false);
    ui->ecgGraph->graph(0)->setData(x, y);
    ui->ecgGraph->xAxis->setRange(-5, 5);
    ui->ecgGraph->yAxis->setRange(-10, 10);
    ui->ecgGraph->graph(0)->rescaleAxes();
    ui->ecgGraph->replot();
}
