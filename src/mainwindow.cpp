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
#include <QScrollBar>
#include <QProcess>

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

// Naviagte to a given widget from a given widget stack
void navigate(QStackedWidget * stack, QWidget * page) {
    stack->setCurrentWidget(page);
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
        ui->stack_home->setCurrentWidget(ui->page_index);
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
    //connect(ui->btn_browseSettings, &QPushButton::clicked, this, &MainWindow::clicked_browse);
    connect(ui->btn_update, &QPushButton::clicked, this, &MainWindow::clicked_update);
    connect(ui->btn_restart, &QPushButton::clicked, this, &MainWindow::clicked_restart);
    connect(ui->btn_retry, &QPushButton::clicked, this, &MainWindow::clicked_restart);
    connect(ui->btn_reset, &QPushButton::clicked, this, &MainWindow::clicked_reset);
    connect(ui->btn_home, &QPushButton::clicked, this, &MainWindow::clicked_home);
    connect(ui->btn_settings, &QPushButton::clicked, this, &MainWindow::clicked_settings);
    connect(ui->btn_github, &QPushButton::clicked, this, &MainWindow::clicked_github);
    connect(ui->btn_managerGithub, &QPushButton::clicked, this, &MainWindow::clicked_managerGithub);
    connect(ui->btn_clearCache, &QPushButton::clicked, this, &MainWindow::clicked_clearCache);
    connect(ui->btn_uninstall, &QPushButton::clicked, this, &MainWindow::clicked_uninstall);
    connect(ui->btn_open, &QPushButton::clicked, this, &MainWindow::clicked_openGameLocation);
    connect(ui->btn_openAppLocation, &QPushButton::clicked, this, &MainWindow::clicked_openAppLocation);
    connect(ui->btn_log, &QPushButton::clicked, this, &MainWindow::clicked_openLog);
    connect(ui->btn_logError, &QPushButton::clicked, this, &MainWindow::clicked_openLog);
    connect(ui->btn_logSettings, &QPushButton::clicked, this, &MainWindow::clicked_openLog);
    connect(ui->checkbox_eula, &QCheckBox::stateChanged, this, &MainWindow::checked_eula);
    connect(ui->line_lethalCompanyLocation, &QLineEdit::textChanged, this, &MainWindow::typed_gameLocation);
    connect(ui->line_lethalCompanyLocationSettings, &QLineEdit::textChanged, this, &MainWindow::typed_gameLocation);

    //=== BepInEx signals/slots
    logger->log("Connecting BepInEx installation/download signals and slots...");
    connect(&manager, &Manager::bepInExDownloaded, this, &MainWindow::onBepInExDownloaded);
    connect(&manager, &Manager::bepInExUnzipped, this, &MainWindow::onBepInExUnzipped);
    connect(&manager, &Manager::bepInExInstalled, this, &MainWindow::onBepInExInstalled);

    //=== Modpack signals/slots
    logger->log("Connecting modpack installation/download signals and slots...");
    connect(&manager, &Manager::modpackFetched, this, &MainWindow::onModpackFetched);
    connect(&manager, &Manager::modpackDownloaded, this, &MainWindow::onModpackDownloaded);
    connect(&manager, &Manager::modpackUnzipped, this, &MainWindow::onModpackUnzipped);
    connect(&manager, &Manager::modpackInstalled, this, &MainWindow::onModpackInstalled);

    //=== Update signals/slots
    logger->log("Connecting update installation/download signals and slots...");
    connect(&manager, &Manager::updateFetched, this, &MainWindow::onUpdateFetched);
    connect(&manager, &Manager::updateDownloaded, this, &MainWindow::onUpdateDownloaded);
    connect(&manager, &Manager::updateUnzipped, this, &MainWindow::onUpdateUnzipped);
    connect(&manager, &Manager::updateInstalled, this, &MainWindow::onUpdateInstalled);
    connect(&manager, &Manager::updateFailed, this, &MainWindow::onUpdateFailed);
    connect (&manager, &Manager::fetched, this, &MainWindow::update_home);
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
        releaseUrl = "https://api.github.com/repos/m-riley04/TheWolfPack/releases/latest";
        githubUrl = "https://github.com/m-riley04/TheWolfPack";
        gameDirectory = "";

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
        QMessageBox::information(this, "Cache cleared.", "The application's cache has been removed from the system.");
        return;
    }
    QMessageBox::warning(this, "Cache not cleared.", "The application ran into an unexpected error when clearing the cache. Cache was not cleared.");
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

        // Log it
        logger->log("Game installation found.");

        // Check game storage
        logger->log("Checking game disk storage...");
        std::string currentRoot = QDir(gameDirectory.c_str()).rootPath().toStdString();
        const double spaceCurrent = manager.getAvailableStorage(currentRoot)/1000000000.00000;
        ui->label_spaceAvailableGame->setText(QString(std::to_string(spaceCurrent).c_str()));
        if (!manager.hasEnoughStorage(currentRoot, 1000000000)) {
            ui->label_spaceAvailableGame->setStyleSheet("color: red");
            QString warningMessage = "WARNING: Not enough storage on game location disk (" + QString(currentRoot.c_str()) + "). You MUST have at least 1 gigabyte of free space.";
            QMessageBox::warning(this, "Not enough storage.", warningMessage, QMessageBox::Ok);
        } else {
            ui->label_spaceAvailableGame->setStyleSheet("color: white");
        }
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

    logger->log("Checking manager disk storage...");
    std::string currentRoot = QDir::rootPath().toStdString();
    const double spaceCurrent = manager.getAvailableStorage(currentRoot)/1000000000.00000;
    ui->label_spaceAvailableCurrent->setText(QString(std::to_string(spaceCurrent).c_str()));
    if (!manager.hasEnoughStorage(currentRoot, 1000000000)) {
        ui->label_spaceAvailableCurrent->setStyleSheet("color: red");
        // Tell the user they do not have enough storage until they have enough storage
        QString warningMessage = "WARNING: Not enough storage on current working disk (" + QString(currentRoot.c_str()) + "). You MUST have at least 1 gigabyte of free space.";
        while (!manager.hasEnoughStorage(currentRoot, 1000000000)) {
            QMessageBox::StandardButton reply = QMessageBox::warning(this, "Not enough storage.", warningMessage, QMessageBox::Retry | QMessageBox::Cancel);
            if (reply == QMessageBox::StandardButton::Cancel) {
                initialize_cancel();
                navigate(ui->stack_installation, ui->page_canceled);
                break;
            }
        }
    }
    ui->label_spaceAvailableCurrent->setStyleSheet("color: white");

    logger->log("Installer 'Configration' page initialized.");
    logger->log("Awaiting configuration...");
}

void MainWindow::initialize_working() {
    logger->log("--- BEGINNING INSTALLATION PHASE ---");
    pageCompleted = false;
    ui->btn_next->setText("Next");
    ui->btn_next->setEnabled(pageCompleted);
    ui->btn_back->setEnabled(false);

    // Check storage again
    logger->log("Re-checking manager disk storage...");
    if (!manager.hasEnoughStorage(QDir::currentPath().toStdString(), 1000000000.00000)) {
        initialize_error();
        navigate(ui->stack_installation, ui->page_error);
        return;
    }
    logger->log("Re-checking game drive storage...");
    if (!manager.hasEnoughStorage(gameDirectory, 1000000000)) {
        initialize_error();
        navigate(ui->stack_installation, ui->page_error);
        return;
    }

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
        logger->log("Preparing to download BepInEx...");
        manager.doDownloadBepInEx();
    } else {
        // Download modpack
        ui->label_progress->setText("Fetching modpack...");
        logger->log("Preparing to fetch the modpack...");
        manager.doFetchModpack();
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

void MainWindow::initialize_cancel() {
    logger->log("=== INSTALLATION CANCELED ===");
    pageCompleted = true;
    ui->btn_next->setEnabled(false);
    ui->btn_back->setEnabled(false);
}

void MainWindow::initialize_home() {
    // Initialize the release url
    if (releaseUrl == "") {
        // Get the latest release URL
        releaseUrl = manager.fetchLatestReleaseURL();
    }

    // Set the game folder ui box
    ui->line_lethalCompanyLocationSettings->setText(QString(gameDirectory.c_str()));

    // Initialize installed release local variables
    QJsonObject installation = manager.getInstallationRelease();
    QString installedVersion = installation.value("tag_name").toString();
    QString installedChangelog = installation.value("body").toString();
    if (installedVersion != QString() && installedChangelog != QString()) {
        ui->text_changelog->setMarkdown(installedChangelog);
        ui->label_version->setText(installedVersion);
    }

    // Fetch the data
    manager.doFetch();

    // Show user that latest release data is loading
    ui->label_versionLatest->setText("Checking...");
    ui->text_changelogLatest->setMarkdown("Checking...");
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

    // Set the scroll to the bottom
    ui->text_console->verticalScrollBar()->setValue(ui->text_console->verticalScrollBar()->maximum());
}

void MainWindow::update_background() {

}

void MainWindow::update_home() {
    // Initialize latest release local variables
    QJsonObject latest = manager.getLatestRelease();
    QString latestVersion = latest.value("tag_name").toString();
    QString latestChangelog = latest.value("body").toString();
    if (latestVersion != QString() && latestChangelog != QString()) {
        ui->text_changelogLatest->setMarkdown(latestChangelog);
        ui->label_versionLatest->setText(latestVersion);
    }

    // Initialize installed release local variables
    QJsonObject installation = manager.getInstallationRelease();
    QString installedVersion = installation.value("tag_name").toString();
    QString installedChangelog = installation.value("body").toString();
    if (installedVersion != QString() && installedChangelog != QString()) {
        ui->text_changelog->setMarkdown(installedChangelog);
        ui->label_version->setText(installedVersion);
    }
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
        gameDirectory = path;
        manager.setGameDirectory(path);
        ui->line_lethalCompanyLocation->setText(QString(path.c_str()));
        ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid green");

        // Check game storage
        logger->log("Checking game disk storage...");
        std::string currentRoot = QDir(gameDirectory.c_str()).rootPath().toStdString();
        const double spaceCurrent = manager.getAvailableStorage(currentRoot)/1000000000.00000;
        ui->label_spaceAvailableGame->setText(QString(std::to_string(spaceCurrent).c_str()));
        if (!manager.hasEnoughStorage(currentRoot, 1000000000)) {
            ui->label_spaceAvailableGame->setStyleSheet("color: red");
            QString warningMessage = "WARNING: Not enough storage on game location disk (" + QString(currentRoot.c_str()) + "). You MUST have at least 1 gigabyte of free space.";
            QMessageBox::warning(this, "Not enough storage.", warningMessage, QMessageBox::Ok);
        }
        ui->label_spaceAvailableGame->setStyleSheet("color: white");
        return;
    }
    logger->log("Path does not exist. User cannot continue.");
    ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid red");
}

void MainWindow::clicked_update() {
    // Check the update version
    connect(&manager, &Manager::fetched, this, &MainWindow::onUpdateChecked);
    ui->btn_update->setText("Checking...");
    ui->btn_update->setDisabled(true);
    manager.doFetch();
}

void MainWindow::clicked_home() {
    navigate(ui->stack_home, ui->page_index);
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
    initialize_welcome();
    navigate(ui->stack_installation, ui->page_welcome);
    navigate(ui->stack_pages, ui->page_installation);
    ui->stack_installation->setCurrentWidget(0);
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

void MainWindow::clicked_openGameLocation() {
    logger->log("User opened the game's currently installed directory...");
    QString path(gameDirectory.c_str());
    QString command = "explorer " + QString("\"") + path + QString(" \"");
    QProcess::startDetached(command);
    QDesktopServices::openUrl(path);
}

void MainWindow::clicked_openAppLocation() {
    logger->log("User opened the application's current directory...");
    QString path = QDir::currentPath();
    QString command = "explorer " + QString("\"") + path + QString(" \"");
    QProcess::startDetached(command);
    //QDesktopServices::openUrl(QDir::currentPath());
}

void MainWindow::clicked_openLog() {
    logger->log("User opened the log... hello!");
    QString path = QDir::currentPath() + "\\" + "log.txt";
    QString command = "notepad " + QString("\"") + path + QString(" \"");
    QProcess::startDetached(command);
    //QDesktopServices::openUrl(QDir::currentPath() + "\\" + "log.txt");
}

void MainWindow::clicked_uninstall() {
    logger->log("Confirming uninstallation...");
    // Confirm if they want to reset
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm", "Are you sure you would like to uninstall the modpack? This will clear the app cache/user data, delete BepInEx and modpack files, and restart the installation process.",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        logger->log("User has chosen to uninstasll the application.");

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
        logger->log("User has chosen NOT to uninstall the application.");
    }
}

void MainWindow::clicked_github() {
    logger->log("User opened the modpack github.");
    QUrl url(githubUrl.c_str());
    QDesktopServices::openUrl(url);
}

void MainWindow::clicked_managerGithub() {
    QUrl url("https://github.com/m-riley04/LethalCompanyModpackInstaller");
    QDesktopServices::openUrl(url);
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

        // Check game storage
        logger->log("Checking game disk storage...");
        std::string currentRoot = QDir(gameDirectory.c_str()).rootPath().toStdString();
        const double spaceCurrent = manager.getAvailableStorage(currentRoot)/1000000000.00000;
        ui->label_spaceAvailableGame->setText(QString(std::to_string(spaceCurrent).c_str()));
        if (!manager.hasEnoughStorage(currentRoot, 1000000000)) {
            ui->label_spaceAvailableGame->setStyleSheet("color: red");
            QString warningMessage = "WARNING: Not enough storage on game location disk (" + QString(currentRoot.c_str()) + "). You MUST have at least 1 gigabyte of free space.";
            QMessageBox::warning(this, "Not enough storage.", warningMessage, QMessageBox::Ok);
        }
        ui->label_spaceAvailableGame->setStyleSheet("color: white");
        return;
    }
    ui->line_lethalCompanyLocation->setStyleSheet("border: 1px solid red");
}

//=== SLOTS
void MainWindow::onBepInExDownloaded() {
    logger->log("BepInEx downloaded successfully.");

    // Move on to unzipping
    logger->log("Preparing to unzip BepInEx...");
    ui->label_progress->setText("Unzipping BepInEx...");
    manager.doUnzipBepInEx();
}
void MainWindow::onBepInExUnzipped() {
    logger->log("BepInEx unzipped successfully.");

    // Move on to install
    logger->log("Preparing to install BepInEx...");
    ui->label_progress->setText("Installing BepInEx...");
    manager.doInstallBepInEx();
}
void MainWindow::onBepInExInstalled() {
    logger->log("BepInEx installed successfully.");

    // Move on to the modpack fetch
    logger->log("Preparing to fetch the modpack...");
    ui->label_progress->setText("Fetching modpack...");
    manager.doFetchModpack();
}
void MainWindow::onModpackFetched() {
    logger->log("Update fetched successfully.");

    // Move on to the modpack download
    ui->label_progress->setText("Downloading modpack...");
    logger->log("Preparing to download the modpack...");
    manager.doDownload();
}
void MainWindow::onModpackDownloaded() {
    logger->log("Modpack downloaded successfully.");

    // Move on to installation
    logger->log("Preparing to unzip the modpack...");
    ui->label_progress->setText("Unzipping modpack...");
    manager.doUnzip();
}
void MainWindow::onModpackUnzipped() {
    logger->log("Modpack unzipped successfully.");

    // Move on to installation
    logger->log("Preparing to install the modpack...");
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
void MainWindow::onUpdateChecked() {
    disconnect(&manager, &Manager::fetched, this, &MainWindow::onUpdateChecked);
    std::string installedVersion = manager.getInstallationRelease().value("tag_name").toString().toStdString();
    std::string latestVersion = manager.getLatestRelease().value("tag_name").toString().toStdString();
    if (installedVersion == latestVersion) {
        // Tell the user the modpack is up to date
        onUpToDate();
        return;
    }

    // If it's out of date...
    onOutOfDate();
}
void MainWindow::onUpdateFetched() {
    logger->log("Update fetched successfully.");

    logger->log("Preparing to download the update...");
    manager.doUpdateDownload();
}
void MainWindow::onUpdateDownloaded() {
    logger->log("Update downloaded successfully.");

    logger->log("Preparing to unzip the update...");
    manager.doUpdateUnzip();
}
void MainWindow::onUpdateUnzipped() {
    logger->log("Update unzipped successfully.");

    logger->log("Preparing to install the update...");
    manager.doUpdateInstall();
}
void MainWindow::onUpdateInstalled() {
    // Tell the user
    QMessageBox::information(this, "Updated", "Update has completed successfully!", QMessageBox::Ok);
    ui->btn_update->setText("Check for Update");
    ui->btn_update->setEnabled(true);

    initialize_home();
    logger->log("Update installed successfully.");
    logger->log("Modpack updated successfully.");
    logger->log("=== UPDATING COMPLETE ===" );
}
void MainWindow::onUpdateFailed() {
    logger->log("=== UPDATE FAILED ===");
    QMessageBox::information(this, "Update failed.", "Update has failed! Please try again.", QMessageBox::Ok);
    ui->btn_update->setText("Check for Update");
    ui->btn_update->setEnabled(true);
}
void MainWindow::onUpToDate() {
    logger->log("Modpack is up to date!");
    ui->btn_update->setEnabled(true);
    ui->btn_update->setText("Check for Update");
    QMessageBox::information(this, "Up to date", "The modpack is up to date!", QMessageBox::Ok);
}
void MainWindow::onOutOfDate() {
    logger->log("Modpack is out of date.");
    ui->btn_update->setText("Update");

    // Check storage
    logger->log("Checking available space on current disk...");
    if (!manager.hasEnoughStorage(QDir::currentPath().toStdString(), 1000000000)) {
        // Tell the user they do not have enough storage until they have enough storage
        QString message = "WARNING: Not enough storage on current working disk (" + QDir::rootPath() + "). You MUST have at least 1 gigabyte of free space.";
        QMessageBox::information(this, "Not enough storage.", message, QMessageBox::Ok);
        ui->btn_update->setEnabled(true);
        return;
    }
    logger->log("Checking available space on game disk...");
    if (gameDirectory != "") {
        if (!manager.hasEnoughStorage(gameDirectory, 1000000000)) {
            // Tell the user they do not have enough storage until they have enough storage
            QString message = "WARNING: Not enough storage on current game disk (" + QString(std::filesystem::path(gameDirectory).root_path().string().c_str()) + "). You MUST have at least 1 gigabyte of free space.";
            QMessageBox::information(this, "Not enough storage.", message, QMessageBox::Ok);
            ui->btn_update->setEnabled(true);
            return;
        }
    }

    // Confirm if they want to update
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Update", "A newer modpack version has been found. Would you like to download and install the latest version of the modpack?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        logger->log("=== UPDATING ===");
        ui->btn_update->setText("Updating...");

        // Clear the old folders
        manager.clearPatchers();
        manager.clearPlugins();
        manager.clearConfig();

        // Fetch latest version
        manager.doUpdateFetch();
    }
}

//=== OVERRIDES
void MainWindow::closeEvent(QCloseEvent *event) {
    logger->log("=== USER CLOSED THE PROGRAM ===");
    this->save();
}
