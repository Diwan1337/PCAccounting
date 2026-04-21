#pragma once

#include <QObject>
#include <QWidget>

#include "backend/models/Computer.h"

class QGroupBox;
class ApplicationController;
class QLabel;
class QPushButton;
class QSpinBox;
class QTableWidget;

class StatsTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatsTabWidget(ApplicationController* controller,
                            QWidget* parent = nullptr);

    void refresh();

private slots:
    void onReportRam();
    void onSaveReport();

private:
    void clearReportView();
    void populateReportTable(const std::vector<Computer>& report);
    QString buildReportText() const;

    ApplicationController* controller;
    QLabel* statsLabel;
    QPushButton* btnReportRam;
    QPushButton* btnSaveReport;
    QLabel* reportTitleLabel;
    QLabel* reportSummaryLabel;
    QSpinBox* ramThresholdSpin;
    QTableWidget* reportTable;
    std::vector<Computer> currentReport;
};
