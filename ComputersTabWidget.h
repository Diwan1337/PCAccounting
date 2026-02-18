#pragma once

#include <QObject>
#include <QWidget>
#include <QString>

class ApplicationController;
class QTableWidget;
class QTextEdit;
class QPushButton;
class QComboBox;
class QSpinBox;

class ComputersTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ComputersTabWidget(ApplicationController* controller,
                                QWidget* parent = nullptr);

    void refresh();
    void applyFilter(const QString& text);

signals:
    void dataChanged();

private slots:
    void onAddComputer();
    void onEditComputer();
    void onDeleteComputer();
    void onShowFreeComputers();
    void onUnassignPc();
    void onSelectionChanged();
    void onFilterChanged();
    void onResetFilters();
    void onShowFullTable();

private:
    void rebuildTable();
    void updateDetails();
    void setButtonsEnabled(bool enabled);

    ApplicationController* controller;
    QString currentFilter;

    QTableWidget* table;
    QTextEdit* details;
    QPushButton* btnAdd;
    QPushButton* btnEdit;
    QPushButton* btnDelete;
    QPushButton* btnFree;
    QPushButton* btnUnassign;
    QPushButton* btnResetFilters;
    QPushButton* btnFullTable;

    QSpinBox* maxRamFilter;
    QSpinBox* maxStorageFilter;
    QComboBox* serviceFilter;
    QComboBox* sortFilter;
};
