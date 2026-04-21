#pragma once

#include <QObject>
#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QAction>
#include <QLineEdit>
#include <QWidget>

class ApplicationController;  // forward declaration
class QCloseEvent;
class EmployeesTabWidget;
class ComputersTabWidget;
class StatsTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ApplicationController* controller,
                        QWidget *parent = nullptr);

private:
    void setupUi();
    void setupEmployeesTab();
    void setupComputersTab();
    void setupStatsTab();
    void setupMenu();
    void refreshEmployees();
    void refreshComputers();
    void refreshStats();
    bool confirmDiscardChanges();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onNewDatabase();
    void onOpenDatabase();
    void onSaveDatabase();

private:
    QAction* actionNew;
    QAction* actionOpen;
    QAction* actionSave;

    ApplicationController* controller;

    QLineEdit* searchEdit;
    QTabWidget* tabWidget;

    EmployeesTabWidget* employeesTab;
    ComputersTabWidget* computersTab;
    StatsTabWidget* statsTab;
};
