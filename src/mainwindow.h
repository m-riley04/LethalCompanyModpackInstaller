#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "manager.h"

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

    //===== Page Initializations
    void initialize_welcome();
    void initialize_eula();
    void initialize_configuration();
    void initialize_working();
    void initialize_done();
    void initialize_error();

    //===== Button Commands
    void clicked_next();
    void clicked_back();
    void clicked_browse();

    //===== Checkbox Commands
    void checked_eula();

    //===== Textbox Commands
    void typed_gameLocation();

private:
    Ui::MainWindow *ui;
    bool pageCompleted;
    Manager manager;
};
#endif // MAINWINDOW_H
