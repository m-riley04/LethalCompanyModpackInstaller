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
    void clearCache();
    void uninstall();

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
    void initialize_cancel();
    void initialize_home();

    //===== Updates
    void update_console();
    void update_background();
    void update_home();

    //===== Button Commands
    void clicked_next();
    void clicked_back();
    void clicked_browse();
    void clicked_finish();
    void clicked_restart();
    void clicked_update();
    void clicked_home();
    void clicked_settings();
    void clicked_reset();
    void clicked_clearCache();
    void clicked_github();
    void clicked_managerGithub();
    void clicked_uninstall();
    void clicked_openAppLocation();
    void clicked_openLog();
    void clicked_openGameLocation();

    //===== Checkbox Commands
    void checked_eula();

    //===== Textbox Commands
    void typed_gameLocation();

public slots:
    void onBepInExDownloaded();
    void onBepInExUnzipped();
    void onBepInExInstalled();

    void onModpackFetched();
    void onModpackDownloaded();
    void onModpackUnzipped();
    void onModpackInstalled();

    void onInstallationError();
    void onUpToDate();
    void onOutOfDate();

    void onUpdateChecked();
    void onUpdateFetched();
    void onUpdateDownloaded();
    void onUpdateUnzipped();
    void onUpdateInstalled();
    void onUpdateFailed();

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
    std::string releaseUrl;
    std::string githubUrl;
    std::string gameDirectory;
};
#endif // MAINWINDOW_H
