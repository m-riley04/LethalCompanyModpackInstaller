#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "appexceptions.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize Widgets
    initialize_welcome();
    ui->stack_installation->setCurrentIndex(0);

    // Connect Widgets
    connect(ui->btn_next, &QPushButton::clicked, this, &MainWindow::clicked_next);
    connect(ui->btn_back, &QPushButton::clicked, this, &MainWindow::clicked_back);
    connect(ui->checkbox_eula, &QCheckBox::stateChanged, this, &MainWindow::checked_eula);
    connect(ui->line_lethalCompanyLocation, &QLineEdit::textChanged, this, &MainWindow::typed_gameLocation);
    connect(ui->btn_browseLethalCompanyLocation, &QPushButton::clicked, this, &MainWindow::clicked_browse);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    ui->stack_installation->setCurrentWidget(ui->page_done);
}

void MainWindow::initialize_done() {
    pageCompleted = true;
    ui->btn_next->setEnabled(false);
    ui->btn_back->setEnabled(true);
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

void MainWindow::clicked_browse() {
    QFileDialog::getExistingDirectory();
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
