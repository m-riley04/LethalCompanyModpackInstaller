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

    //===== User Data
    void save();
    void load();

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
