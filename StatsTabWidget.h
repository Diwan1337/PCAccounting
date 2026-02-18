#pragma once

#include <QObject>
#include <QWidget>

class ApplicationController;
class QLabel;
class QPushButton;

class StatsTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatsTabWidget(ApplicationController* controller,
                            QWidget* parent = nullptr);

    void refresh();

private slots:
    void onReportRam();

private:
    ApplicationController* controller;
    QLabel* statsLabel;
    QPushButton* btnReportRam;
};
