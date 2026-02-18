#include "StatsTabWidget.h"
#include "../projects/src/core/ApplicationController.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>

StatsTabWidget::StatsTabWidget(ApplicationController* controller,
                               QWidget* parent)
    : QWidget(parent),
    controller(controller)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    statsLabel = new QLabel("Статистика будет здесь");
    layout->addWidget(statsLabel);

    btnReportRam = new QPushButton("Отчет по RAM");
    layout->addWidget(btnReportRam);

    connect(btnReportRam, &QPushButton::clicked, this, &StatsTabWidget::onReportRam);

    refresh();
}

void StatsTabWidget::refresh()
{
    bool loaded = controller->isLoaded();
    btnReportRam->setEnabled(loaded);

    if (!loaded) {
        statsLabel->setText("База не загружена");
        return;
    }

    int empCount = controller->getEmployees().size();
    int pcCount = controller->getComputers().size();

    statsLabel->setText(
        "Сотрудников: " + QString::number(empCount) +
        "\nКомпьютеров: " + QString::number(pcCount)
    );
}

void StatsTabWidget::onReportRam()
{
    if (!controller->isLoaded())
        return;

    bool ok;
    int value = QInputDialog::getInt(
        this,
        "Отчет",
        "RAM меньше (ГБ):",
        8, 1, 512, 1, &ok
    );

    if (!ok)
        return;

    auto result = controller->getReportRamLessThan(value);

    QString text = "Найдено: " + QString::number(result.size()) + "\n\n";

    for (const auto& c : result) {
        text += QString::number(c.id) + " - " +
                QString::fromStdString(c.model) +
                " RAM: " + QString::number(c.ramSize) + "\n";
    }

    QMessageBox::information(this, "Отчет", text);
}
