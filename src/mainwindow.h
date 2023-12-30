#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "manager.h"
#include "userdatahandler.h"
#include "logger.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //===== User Data/Functionalities
    void save();
    void load();
    void reset();

    //=== App Initializations
    void initialize_core();
    void initialize_fonts();
    void initialize_stylesheet();
    void initialize_connections();

    //===== Page Initializations
    void initialize_welcome();
    void initialize_eula();
    void initialize_configuration();
    void initialize_working();
    void initialize_done();
    void initialize_error();

    //===== Updates
    void update_console();

    //===== Button Commands
    void clicked_next();
    void clicked_back();
    void clicked_browse();
    void clicked_finish();
    void clicked_restart();
    void clicked_update();
    void clicked_reset();

    //===== Checkbox Commands
    void checked_eula();

    //===== Textbox Commands
    void typed_gameLocation();

public slots:
    void onBepInExDownloaded();
    void onBepInExUnzipped();
    void onBepInExInstalled();
    void onModpackDownloaded();
    void onModpackUnzipped();
    void onModpackInstalled();
    void onInstallationError();
    void onUpdateAvailable();
    void onUpdateUnavailable();
    void onModpackUpdated();

protected:
    //===== Overrides
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    Manager manager;
    UserDataHandler dataHandler;
    Logger * logger;

    bool pageCompleted;
    bool modpackInstalled;
    bool firstOpen;
};
#endif // MAINWINDOW_H
