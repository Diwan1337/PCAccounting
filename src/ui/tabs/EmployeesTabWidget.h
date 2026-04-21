#pragma once

#include <QObject>
#include <QWidget>
#include <QString>

class ApplicationController;
class QTableWidget;
class QTextEdit;
class QPushButton;
class QComboBox;

class EmployeesTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EmployeesTabWidget(ApplicationController* controller,
                                QWidget* parent = nullptr);

    void refresh();
    void applyFilter(const QString& text);

signals:
    void dataChanged();

private slots:
    void onAddEmployee();
    void onEditEmployee();
    void onDeleteEmployee();
    void onAssignPc();
    void onSelectionChanged();
    void onFilterChanged();
    void onResetFilters();
    void onShowFullTable();

private:
    void rebuildTable();
    void updateDetails();
    void setButtonsEnabled(bool enabled);
    void refreshFilterValues();

    ApplicationController* controller;
    QString currentFilter;

    QTableWidget* table;
    QTextEdit* details;
    QPushButton* btnAdd;
    QPushButton* btnEdit;
    QPushButton* btnDelete;
    QPushButton* btnAssign;
    QPushButton* btnResetFilters;
    QPushButton* btnFullTable;

    QComboBox* instituteFilter;
    QComboBox* departmentFilter;
    QComboBox* statusFilter;
    QComboBox* sortFilter;
};
