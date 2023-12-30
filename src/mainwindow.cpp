#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "appexceptions.h"
#include <QFileDialog>
#include <QCoreApplication>
#include <QDir>
#include <QTimer>
#include <QMessageBox>
#include <QFontDatabase>
#include <QDesktopServices>
#include <Windows.h>



/* When given a stylesheet string and a .var file path, replaces
 * all the variables found in the stylesheet string.
 * Returns a new stylesheet string.
*/
QString replaceStylesheetVariables(const QString &stylesheet, const QString &path) {
    QString text = stylesheet;

    // Check path validity
    if (path == "" || !QFile::exists(path)) {
        throw std::invalid_argument("The path is invalid or does not exist.");
    }

    // Open variables file
    QFile file(path);

    // Check if the variables file was opened
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR: variables file was not able to be opened.";
        file.close();
        return text;
    }

    // Initialize map of variables
    QMap<QString, QString> vars;

    // Read all the lines of the file
    QTextStream in(&file);
    while (!in.atEnd()) {
        // Read the line as a string
        std::string line = in.readLine().toStdString();

        // Check if it is a comment
        if (line.at(0) == '/' && line.at(1) == '/') {
            // Skip that line
            continue;
        }

        // Clean the line of spaces
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

        // Split the string from a delimeter
        std::string delimeter = "=";
        size_t delimeterIndex = line.find(delimeter);
        std::string variable = line.substr(0, delimeterIndex);
        std::string value = line.substr(delimeterIndex+1, line.length()-1);

        // Assign the variable and value to the vars map
        vars[QString(variable.c_str())] = QString(value.c_str());
    }

    // Sort the variable names by length in descending order
    QList<QString> sortedVars = vars.keys();
    std::sort(sortedVars.begin(), sortedVars.end(), [](const QString &a, const QString &b) {
        return a.length() > b.length(); // Sort by length, longest first
    });

    // Replace the mentions of each variable with their value
    for (auto & variable : sortedVars) {
        text.replace(variable, vars.value(variable));
    }

    // Close the file
    file.close();

    // Return the new text string
    return text;
}

/* Reads a stylesheet from a given path and returns the raw text from the file.
 * If there is a "varsPath" string, uses the given varsPath to read custom qss variables
 * from the .vars file. Variables are line-by-line and are formatted like so:
 *
 * @name = value
*/
QString readStylesheet(const QString &path, const QString &varsPath = "") {
    // Check stylesheet path validity
    if (path == "" || !QFile::exists(path)) {
        throw std::invalid_argument("The stylesheet path is invalid or does not exist.");
    }

    // Initialize final return string
    QString text;

    // Open the qss file
    QFile file(path);

    // Check if the file was opened
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ERROR: qss stylesheet was not opened.";
        return "";
    }

    // Save to text
    text = QString(file.readAll().toStdString().c_str());

    // Check for custom variables
    if (varsPath != "") {
        text = replaceStylesheetVariables(text, varsPath);
    }

    // Close the file
    file.close();

    // Return the stylesheet file as a string
    return text;

}

//=== CONSTRUCTOR/DESTRUCTOR
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , logger(new Logger(QDir::currentPath().toStdString(), "log.txt"))
{
    // Initialize Debug Logging
    manager.setLogPath(QDir::currentPath().toStdString() + "\\" + "log.txt");

    // Set Core Preferences
    initialize_core();

    // Set up fonts
    initialize_fonts();

    // Set up stylesheet
    initialize_stylesheet();

    // Set up UI
    logger->log("Setting up UI...");
    ui->setupUi(this);

    // Set up connections
    initialize_connections();

    // Read user data
    load();

    // Initialize Widgets
    initialize_welcome();
    ui->stack_installation->setCurrentIndex(0);
    ui->stack_pages->setCurrentWidget(ui->page_installation);

    if (!firstOpen) {
        logger->log("Returning user detected: skipping first-time installation.");

        initialize_home();
        ui->stack_installation->setCurrentWidget(ui->page_welcome);
        ui->stack_pages->setCurrentWidget(ui->page_home);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete logger;
}

//=== App Initializations
void MainWindow::initialize_core() {
    logger->log("Assigning application variables...");
    QCoreApplication::setApplicationName("TheWolfPackManager");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("Restless Medicine Studios");
    QCoreApplication::setOrganizationDomain("restlessmedicine.com");

    logger->log("Loading app icon...");
    QIcon icon(QDir::currentPath() + "\\icon.ico");
    this->setWindowIcon(icon);
}

void MainWindow::initialize_fonts() {
    logger->log("Loading fonts...");
    const QString fontName = "dos";
    const QString fontPath = QDir::currentPath() + "\\styling\\fonts\\" + fontName + ".ttf";
    const int fontID = QFontDatabase::addApplicationFont(fontPath);
    QString family = QFontDatabase::applicationFontFamilies(fontID).at(0);
    QFont font(family, 8);
    QApplication::setFont(font);
}

void MainWindow::initialize_stylesheet() {
    logger->log("Loading stylesheet...");
    const QString stylesheetVariablesName = "stylesheet";
    const QString stylesheetVariablesPath = QDir::currentPath() + "\\styling\\" + stylesheetVariablesName + ".vars";
    const QString stylesheetName = "stylesheet";
    const QString stylesheetPath = QDir::currentPath() + "\\styling\\" + stylesheetName + ".qss";
    const QString stylesheet = readStylesheet(stylesheetPath, stylesheetVariablesPath);
    setStyleSheet(stylesheet);
}

void MainWindow::initialize_connections() {
    //=== Widgets
    logger->log("Connecting UI widgets...");
    connect(ui->btn_next, &QPushButton::clicked, this, &MainWindow::clicked_next);
    connect(ui->btn_back, &QPushButton::clicked, this, &MainWindow::clicked_back);
    connect(ui->btn_browseLethalCompanyLocation, &QPushButton::clicked, this, &MainWindow::clicked_browse);
    connect(ui->btn_update, &QPushButton::clicked, this, &MainWindow::clicked_update);
    connect(ui->btn_restart, &QPushButton::clicked, this, &MainWindow::clicked_restart);
    connect(ui->btn_reset, &QPushButton::clicked, this, &MainWindow::clicked_reset);
    connect(ui->btn_settings, &QPushButton::clicked, this, &MainWindow::clicked_settings);
    connect(ui->btn_github, &QPushButton::clicked, this, &MainWindow::clicked_github);
    connect(ui->checkbox_eula, &QCheckBox::stateChanged, this, &MainWindow::checked_eula);
    connect(ui->line_lethalCompanyLocation, &QLineEdit::textChanged, this, &MainWindow::typed_gameLocation);

    //=== Installation and download signals/slots
    logger->log("Connecting installation/download signals and slots...");
    connect(&manager, &Manager::bepInExDownloaded, this, &MainWindow::onBepInExDownloaded);
    connect(&manager, &Manager::bepInExUnzipped, this, &MainWindow::onBepInExUnzipped);
    connect(&manager, &Manager::bepInExInstalled, this, &MainWindow::onBepInExInstalled);
    connect(&manager, &Manager::modpackDownloaded, this, &MainWindow::onModpackDownloaded);
    connect(&manager, &Manager::modpackUnzipped, this, &MainWindow::onModpackUnzipped);
    connect(&manager, &Manager::modpackInstalled, this, &MainWindow::onModpackInstalled);
}

// Saves the user data
void MainWindow::save() {
    logger->log("Saving user data to save file...");

    try {
        // Set all data values
        dataHandler.setValue("pageCompleted", QVariant(pageCompleted));
        dataHandler.setValue("modpackInstalled", QVariant(modpackInstalled));
        dataHandler.setValue("firstOpen", QVariant(firstOpen));
        dataHandler.setValue("releaseUrl", QVariant(releaseUrl.c_str()));
        dataHandler.setValue("githubUrl", QVariant(githubUrl.c_str()));
        dataHandler.setValue("changelog", QVariant(changelog.c_str()));
        dataHandler.setValue("modpackVersion", QVariant(modpackVersion.c_str()));
        dataHandler.setValue("gameDirectory", QVariant(gameDirectory.c_str()));

        // Save with data handler
        dataHandler.save();
    } catch (...) {
        logger->log("ERROR: Failed to save user data.");
    }
}

// Loads the user data from the data file. If it doesn't exist, loads default values.
void MainWindow::load() {
    logger->log("Loading user data...");

    // Load all variables
    try {
        pageCompleted       = dataHandler.getValue("pageCompleted", true).toBool();
        modpackInstalled    = dataHandler.getValue("modpackInstalled", false).toBool();
        firstOpen           = dataHandler.getValue("firstOpen", true).toBool();
        releaseUrl      = dataHandler.getValue("releaseUrl", "").toString().toStdString();
        githubUrl       = dataHandler.getValue("githubUrl", "").toString().toStdString();
        changelog       = dataHandler.getValue("changelog", "").toString().toStdString();
        modpackVersion      = dataHandler.getValue("modpackVersion", "1.0.0").toString().toStdString();
        gameDirectory       = dataHandler.getValue("gameDirectory", "").toString().toStdString();
    } catch (...) {
        logger->log("ERROR: Failed to reset user data");
    }
}

// Resets the user data and sets them back to their default values
void MainWindow::reset() {
    logger->log("Resetting user data...");
    try {
        pageCompleted = true;
        modpackInstalled = false;
        firstOpen = true;
        releaseUrl = "";
        githubUrl = "github.com";
        changelog = "";
        modpackVersion = "1.0.0";
        gameDirectory = "";

        dataHandler.setValue("pageCompleted", pageCompleted);
        dataHandler.setValue("modpackInstalled", modpackInstalled);
        dataHandler.setValue("firstOpen", firstOpen);
        dataHandler.setValue("releaseUrl", QVariant(releaseUrl.c_str()));
        dataHandler.setValue("githubUrl", QVariant(githubUrl.c_str()));
        dataHandler.setValue("changelog", QVariant(changelog.c_str()));
        dataHandler.setValue("modpackVersion", QVariant(modpackVersion.c_str()));
        dataHandler.setValue("gameDirectory", QVariant(gameDirectory.c_str()));

        save();
    } catch (...) {
        logger->log("ERROR: Failed to reset user data");
    }
}

// Clears the cache directory from the executable path
void MainWindow::clearCache() {
    logger->log("Preparing to clear app cache...");
    std::string cacheDirectory = QDir::currentPath().toStdString() + "\\cache";

    if (std::filesystem::exists(QDir::currentPath().toStdString()) && std::filesystem::exists(cacheDirectory)) {
        std::filesystem::remove_all(cacheDirectory);
        std::filesystem::create_directory(cacheDirectory);
        logger->log("App cache has been cleared.");
        return;
    }
    logger->log("App cache was not cleared successfully (path error)");
}

// Uninstalls the modpack and BepInEx
void MainWindow::uninstall() {
    logger->log("Preparing to uninstall...");
    try {
        Installer::uninstall(gameDirectory);
        clearCache();
    } catch (GameNotFoundException) {
        logger->log("ERROR: Uninstallation failed because game installation was not found.");
        return;
    } catch (...) {
        logger->log("ERROR: An unexpected error has caused installation has failed.");
        return;
    }

    logger->log("Modpack has been uninstalled successfully.");
}

//===== PAGE INITIALIZATIONS
void MainWindow::initialize_welcome() {
    pageCompleted = true;
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(false);
    ui->btn_next->setText("Next");
    logger->log("Installer 'Welcome' page initialized.");
}

void MainWindow::initialize_eula() {
    pageCompleted = ui->checkbox_eula->isChecked();
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(true);
    ui->btn_next->setText("Next");
    logger->log("Installer 'EULA' page initialized.");
    logger->log("Awaiting EULA confirnmation...");
}

void MainWindow::initialize_configuration() {
    logger->log("Initializing installer page 'Configuration'...");
    pageCompleted = true;
    ui->btn_next->setText("Install");
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(true);

    logger->log("Attempting to find the game installation...");
    try {
        // Find the game installation
        gameDirectory = manager.locateGameLocation();
        ui->line_lethalCompanyLocation->setText(QString(gameDirectory.c_str()));
        ui->label_found->setText(QString("Yes"));

        // Find the space available
        std::string spaceAvailable = std::to_string(double(manager.getSpaceAvailable()/10000000000.0));
        ui->label_spaceAvailable->setText(QString(spaceAvailable.c_str()));

        // Log it
        logger->log("Game installation found.");
    } catch (GameNotFoundException e) {
        ui->label_found->setText(QString("No"));
        logger->log("Game installation NOT found.");
    }

    logger->log("Attempting to find BepInEx installation...");
    try {
        // Find if BepInEx is installed
        if (manager.isBepInExInstalled()) {
            ui->label_bepinexFound->setText(QString("Yes"));
            logger->log("BepInEx installation found.");
        } else {
            ui->label_bepinexFound->setText(QString("No"));
            logger->log("BepInEx installation NOT found.");
        }
    } catch (GameNotFoundException e) {
        ui->label_bepinexFound->setText(QString("No"));
        logger->log("BepInEx installation NOT found.");
    }

    logger->log("Installer 'Configration' page initialized.");
    logger->log("Awaiting configuration...");
}

void MainWindow::initialize_working() {
    logger->log("--- BEGINNING INSTALLATION PHASE ---");
    pageCompleted = false;
    ui->btn_next->setText("Next");
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(false);

    // Configure console
    logger->log("Initializing console...");
    update_console();
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update_console);
    connect(&manager, &Manager::modpackInstalled, timer, &QTimer::stop);
    connect(&manager, &Manager::modpackInstalled, timer, &QTimer::deleteLater);
    timer->start(1000);

    // Check if BepInEx is insalled
    if (!manager.isBepInExInstalled()) {
        // Download BepInEx
        ui->label_progress->setText("Downloading BepInEx...");
        logger->log("Downloading BepInEx...");
        manager.doDownloadBepInEx();
    } else {
        // Download modpack
        ui->label_progress->setText("Downloading modpack...");
        manager.doDownload();
    }
}

void MainWindow::initialize_done() {
    pageCompleted = true;
    firstOpen = false;
    modpackInstalled = true;
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(false);

    // Save the user data
    save();

    // Set the button text
    connect(ui->btn_next, &QPushButton::clicked, this, &MainWindow::clicked_finish);
    ui->btn_next->setText("Finish");
    logger->log("Installer 'Done' page initialized.");
}

void MainWindow::initialize_error() {
    logger->log("=== ERROR THROWN ===");
    pageCompleted = false;
    ui->btn_next->setEnabled(false);
    ui->btn_back->setEnabled(false);
}

void MainWindow::initialize_home() {
    QString qVersion;
    QString qChangelog;

    if (releaseUrl == "" || changelog == "") {
        // Get the latest release URL
        releaseUrl = manager.fetchLatestReleaseURL();
        // Set the changelog, version, and other home page things
        QString qVersion = manager.fetchLatestRelease(releaseUrl).object().find("tag_name")->toString();
        QString qChangelog = manager.fetchLatestRelease(releaseUrl).object().find("body")->toString();
        modpackVersion = qVersion.toStdString();
        changelog = qChangelog.toStdString();
    }

    // Set initial UI properties
    ui->text_changelog->setMarkdown(qChangelog);
    ui->label_version->setText(qVersion);
    ui->stack_installation->setCurrentIndex(0);
    ui->stack_installation->setCurrentWidget(ui->page_welcome);
    ui->stack_pages->setCurrentWidget(ui->page_home);
}

//===== UPDATES
void MainWindow::update_console() {
    QString logPath = QDir::currentPath() + "/log.txt";
    QFile logFile(logPath);
    if (!logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logger->log("Cannot open log file for console.");
        return;
    }

    QTextStream in(&logFile);
    QString finalString;

    // Break the file apart line by line
    while (!in.atEnd()) {
        QString line = in.readLine();
        // Add HTML line break
        line += "<br/>";

        // Connect to a final string
        finalString += line;
    }

    // Close the file
    logFile.close();

    // Set the HTML
    ui->text_console->setHtml(finalString);
}

//===== WIDGET COMMANDS
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

    // Save the user data
    save();
}

void MainWindow::clicked_browse() {
    logger->log("User is browsing filesystem for game directory...");
    std::string path = QFileDialog::getExistingDirectory().toStdString();

    logger->log("Checking if path is blank...");
    if (path == "") {
        logger->log("Path is blank. Directory selection canceled.");
        return;
    }

    logger->log("Checking if chosen path exists...");
    if (std::filesystem::exists(path)) {
        manager.setGameDirectory(path);
        ui->line_lethalCompanyLocation->setText(QString(path.c_str()));
        ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid green");

        // Find the space available
        logger->log("Checking avaliable space...");
        std::string spaceAvailable = std::to_string(double(manager.getSpaceAvailable()/10000000000.0));
        ui->label_spaceAvailable->setText(QString(spaceAvailable.c_str()));
        return;
    }
    logger->log("Path does not exist. User cannot continue.");
    ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid red");
}

void MainWindow::clicked_update() {
    logger->log("BepInEx downloaded successfully.");

    // Check the version
    std::string url = manager.fetchLatestReleaseURL();
    std::string latestModpackVersion = manager.fetchLatestVersion(url);
    if (modpackVersion == latestModpackVersion) {
        // Tell the user the modpack is up to date
        onUpToDate();
        return;
    }

    // If it's out of date...
    onOutOfDate();
}

void navigate(QStackedWidget * stack, QWidget * page) {
    stack->setCurrentWidget(page);
}

void MainWindow::clicked_settings() {
    navigate(ui->stack_home, ui->page_settings);
}

void MainWindow::clicked_restart() {
    logger->log("=== INSTALLATION RESTARTING ===");

    // Clear/Reset all made changes
    clearCache();
    reset();
    uninstall();

    // Reset the pages
    ui->stack_pages->setCurrentWidget(ui->page_installation);
    ui->stack_installation->setCurrentWidget(ui->page_welcome);
    ui->stack_home->setCurrentWidget(ui->page_index);
}

void MainWindow::clicked_reset() {
    logger->log("Reset confirmation box opened.");

    // Confirm if they want to reset
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm", "Are you sure you would like to reset? This will uninstall the modpack, clear the app cache/user data, and restart the installation process.",
                                                       QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        logger->log("User has chosen to reset the application.");

        // Clear out the cache
        clearCache();

        // Uninstall the current installation
        uninstall();

        // Reset the user data
        reset();

        // Set initial pages
        ui->stack_pages->setCurrentWidget(ui->page_installation);
        ui->stack_installation->setCurrentWidget(ui->page_welcome);
        ui->stack_home->setCurrentWidget(ui->page_index);
        ui->stack_installation->setCurrentIndex(0);
    } else {
        logger->log("User has chosen NOT to reset the application.");
    }
}

void MainWindow::clicked_clearCache() {
    clearCache();
}

void MainWindow::clicked_github() {
    QDesktopServices::openUrl(QUrl(githubUrl.c_str()));
}

void MainWindow::checked_eula() {
    pageCompleted = !pageCompleted;
    ui->btn_next->setEnabled(pageCompleted);
}

void MainWindow::typed_gameLocation() {
    // Check if the typed path exists
    if (std::filesystem::exists(ui->line_lethalCompanyLocation->text().toStdString())) {
        gameDirectory = ui->line_lethalCompanyLocation->text().toStdString();
        manager.setGameDirectory(gameDirectory);
        ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid green");
        // Find the space available
        std::string spaceAvailable = std::to_string(double(manager.getSpaceAvailable()/10000000000.0));
        ui->label_spaceAvailable->setText(QString(spaceAvailable.c_str()));
        return;
    }
    ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid red");
}

//=== SLOTS
void MainWindow::onBepInExDownloaded() {
    logger->log("BepInEx downloaded successfully.");

    // Move on to unzipping
    ui->label_progress->setText("Unzipping BepInEx...");
    manager.doUnzipBepInEx();
}
void MainWindow::onBepInExUnzipped() {
    logger->log("BepInEx unzipped successfully.");

    // Move on to unzipping
    ui->label_progress->setText("Installing BepInEx...");
    manager.doInstallBepInEx();
}
void MainWindow::onBepInExInstalled() {
    logger->log("BepInEx installed successfully.");

    // Move on to the modpack download
    ui->label_progress->setText("Downloading modpack...");
    manager.doDownload();
}
void MainWindow::onModpackDownloaded() {
    logger->log("Modpack downloaded successfully.");

    // Move on to installation
    ui->label_progress->setText("Unzipping modpack...");
    manager.doUnzip();
}
void MainWindow::onModpackUnzipped() {
    logger->log("Modpack unzipped successfully.");

    // Move on to installation
    ui->label_progress->setText("Installing modpack...");
    manager.doInstall();
}
void MainWindow::onModpackInstalled() {
    logger->log("--- INSTALLATION PHASE ENDED ---");
    logger->log("Modpack installed successfully.");

    // Initialize the done and home pages
    initialize_done();
    initialize_home();
    ui->stack_installation->setCurrentWidget(ui->page_done);
}
void MainWindow::onInstallationError() {
    logger->log("=== INSTALLATION ERROR ===");
    ui->stack_installation->setCurrentWidget(ui->page_error);
}
void MainWindow::onModpackUpdated() {
    logger->log("Modpack updated successfully.");
}

void MainWindow::onUpToDate() {
    logger->log("Modpack is up to date!");
    QMessageBox::information(this, "Up to date", "The modpack is up to date!", QMessageBox::Ok);
}

void MainWindow::onOutOfDate() {
    logger->log("Modpack is out of date.");

    // Confirm if they want to reset
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Update", "A newer modpack version has been found. Would you like to download and install the latest version of the modpack?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        logger->log("=== UPDATING ===");

        // Clear the old folders
        manager.clearPatchers();
        manager.clearPlugins();
        manager.clearConfig();

        // Download latest version
        manager.doDownload();
    }
}

//=== OVERRIDES
void MainWindow::closeEvent(QCloseEvent *event) {
    logger->log("=== USER CLOSED THE PROGRAM ===");
    this->save();
}
