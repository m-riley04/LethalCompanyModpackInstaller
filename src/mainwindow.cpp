#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "appexceptions.h"
#include <QFileDialog>
#include <QCoreApplication>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Set Core Preferences
    QCoreApplication::setApplicationName("TheWolfPackInstaller");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("Restless Medicine Studios");
    QCoreApplication::setOrganizationDomain("restlessmedicine.com");

    // Set up UI
    ui->setupUi(this);

    // Connect Widgets
    connect(ui->btn_next, &QPushButton::clicked, this, &MainWindow::clicked_next);
    connect(ui->btn_back, &QPushButton::clicked, this, &MainWindow::clicked_back);
    connect(ui->checkbox_eula, &QCheckBox::stateChanged, this, &MainWindow::checked_eula);
    connect(ui->line_lethalCompanyLocation, &QLineEdit::textChanged, this, &MainWindow::typed_gameLocation);
    connect(ui->btn_browseLethalCompanyLocation, &QPushButton::clicked, this, &MainWindow::clicked_browse);

    // Read user data
    load();

    // Initialize Widgets
    initialize_welcome();
    ui->stack_installation->setCurrentIndex(0);

    if (!firstOpen) {
        ui->stack_pages->setCurrentWidget(ui->page_home);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::save() {
    // Set all data values
    dataHandler.setValue("pageCompleted", QVariant(pageCompleted));
    dataHandler.setValue("modpackInstalled", QVariant(modpackInstalled));
    dataHandler.setValue("firstOpen", QVariant(firstOpen));

    // Save with data handler
    dataHandler.save();
}

void MainWindow::load() {
    // Load all variables
    try {
        pageCompleted       = dataHandler.getValue("pageCompleted", true).toBool();
        modpackInstalled    = dataHandler.getValue("modpackInstalled", false).toBool();
        firstOpen           = dataHandler.getValue("firstOpen", true).toBool();

        dataHandler.load();
    } catch (...) {
        std::cerr << "Failed to load user data.\n";
    }
}

void MainWindow::initialize_welcome() {
    pageCompleted = true;
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(false);
}

void MainWindow::initialize_eula() {
    pageCompleted = ui->checkbox_eula->isChecked();
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(true);
}

void MainWindow::initialize_configuration() {
    pageCompleted = true;
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(true);

    try {
        // Find the game installation
        ui->line_lethalCompanyLocation->setText(QString(manager.locateGameLocation().c_str()));
        ui->label_found->setText(QString("Yes"));

        // Find the space available
        std::string spaceAvailable = std::to_string(double(manager.getSpaceAvailable()/10000000000.0));
        ui->label_spaceAvailable->setText(QString(spaceAvailable.c_str()));
    } catch (GameNotFoundException e) {
        ui->label_found->setText(QString("No"));
    }

    try {
        // Find BepInEx installation
        manager.isBepInExInstalled();
        ui->label_bepinexFound->setText(QString("Yes"));

    } catch (BepInExNotInstalledException e) {
        ui->label_bepinexFound->setText(QString("No"));
    }
}

void MainWindow::initialize_working() {
    pageCompleted = false;
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(false);

    // Check if BepInEx is insalled
    if (!manager.isBepInExInstalled()) {
        // Download BepInEx
        manager.downloadBepInEx();

        // Install BepInEx
        manager.installBepInEx();
    }

    // Start download process
    manager.download();

    // Start installation process
    manager.install();

    // Successful
    initialize_done();
    ui->stack_installation->setCurrentWidget(ui->page_done);
}

void MainWindow::initialize_done() {
    pageCompleted = true;
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(false);

    ui->btn_next->setText("Finish");
    connect(ui->btn_next, &QPushButton::clicked, this, &MainWindow::clicked_finish);
}

void MainWindow::initialize_error() {
    pageCompleted = true;
    ui->btn_next->setEnabled(false);
    ui->btn_back->setEnabled(false);
}

void MainWindow::clicked_next() {
    int currentIndex = ui->stack_installation->currentIndex();
    int nextIndex = currentIndex + 1;

    if (nextIndex > ui->stack_installation->count()) {
        return;
    }

    ui->stack_installation->setCurrentIndex(nextIndex);

    switch (nextIndex) {
    case (0):
        initialize_welcome();
        break;
    case (1):
        initialize_eula();
        break;
    case (2):
        initialize_configuration();
        break;
    case (3):
        initialize_working();
        break;
    case (4):
        initialize_done();
        break;
    case (5):
        initialize_error();
        break;
    default:
        initialize_welcome();
        break;
    }
}

void MainWindow::clicked_back() {
    int currentIndex = ui->stack_installation->currentIndex();
    int previousIndex = currentIndex - 1;

    if (previousIndex < 0) {
        return;
    }

    ui->stack_installation->setCurrentIndex(previousIndex);

    switch (previousIndex) {
    case (0):
        initialize_welcome();
        break;
    case (1):
        initialize_eula();
        break;
    case (2):
        initialize_configuration();
        break;
    case (3):
        initialize_working();
        break;
    case (4):
        initialize_done();
        break;
    case (5):
        initialize_error();
        break;
    default:
        initialize_welcome();
        break;
    }
}

void MainWindow::clicked_finish() {
    ui->stack_pages->setCurrentWidget(ui->page_home);
    firstOpen = false;
    modpackInstalled = true;
    save();
}

void MainWindow::clicked_browse() {
    std::string path = QFileDialog::getExistingDirectory().toStdString();

    if (std::filesystem::exists(path) && path != "") {
        manager.setGameDirectory(path);
        ui->line_lethalCompanyLocation->setText(QString(path.c_str()));
        ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid green");

        // Find the space available
        std::string spaceAvailable = std::to_string(double(manager.getSpaceAvailable()/10000000000.0));
        ui->label_spaceAvailable->setText(QString(spaceAvailable.c_str()));
        return;
    }
    ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid red");
}

void MainWindow::checked_eula() {
    pageCompleted = !pageCompleted;
    ui->btn_next->setEnabled(pageCompleted);
}

void MainWindow::typed_gameLocation() {
    // Check if the typed path exists
    if (std::filesystem::exists(ui->line_lethalCompanyLocation->text().toStdString())) {
        manager.setGameDirectory(ui->line_lethalCompanyLocation->text().toStdString());
        ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid green");
        // Find the space available
        std::string spaceAvailable = std::to_string(double(manager.getSpaceAvailable()/10000000000.0));
        ui->label_spaceAvailable->setText(QString(spaceAvailable.c_str()));
        return;
    }
    ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid red");
}

//=== OVERRIDES
void MainWindow::closeEvent(QCloseEvent *event) {
    this->save();
}
