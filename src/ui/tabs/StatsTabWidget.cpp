#include "StatsTabWidget.h"
#include "backend/core/ApplicationController.h"

#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QStringConverter>

StatsTabWidget::StatsTabWidget(ApplicationController* controller,
                               QWidget* parent)
    : QWidget(parent),
      controller(controller)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    QGroupBox* summaryBox = new QGroupBox("Сводка по базе", this);
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryBox);

    statsLabel = new QLabel("Статистика будет здесь", summaryBox);
    statsLabel->setWordWrap(true);
    summaryLayout->addWidget(statsLabel);
    layout->addWidget(summaryBox);

    QGroupBox* reportBox = new QGroupBox("Отчет по объему ОЗУ", this);
    QVBoxLayout* reportLayout = new QVBoxLayout(reportBox);
    reportLayout->setSpacing(10);

    reportTitleLabel = new QLabel(
        "Сформируйте отчет по компьютерам с объемом ОЗУ меньше заданного значения.",
        reportBox);
    reportTitleLabel->setWordWrap(true);
    reportLayout->addWidget(reportTitleLabel);

    QHBoxLayout* controlsLayout = new QHBoxLayout();
    QLabel* ramLabel = new QLabel("Порог ОЗУ (ГБ):", reportBox);
    ramThresholdSpin = new QSpinBox(reportBox);
    ramThresholdSpin->setRange(1, 4096);
    ramThresholdSpin->setValue(8);
    ramThresholdSpin->setSuffix(" ГБ");
    ramThresholdSpin->setMinimumWidth(120);

    btnReportRam = new QPushButton("Сформировать отчет", reportBox);
    btnSaveReport = new QPushButton("Сохранить отчет", reportBox);

    controlsLayout->addWidget(ramLabel);
    controlsLayout->addWidget(ramThresholdSpin);
    controlsLayout->addWidget(btnReportRam);
    controlsLayout->addWidget(btnSaveReport);
    controlsLayout->addStretch();
    reportLayout->addLayout(controlsLayout);

    reportSummaryLabel = new QLabel(reportBox);
    reportSummaryLabel->setWordWrap(true);
    reportLayout->addWidget(reportSummaryLabel);

    reportTable = new QTableWidget(reportBox);
    reportTable->setColumnCount(6);
    reportTable->setHorizontalHeaderLabels({
        "ID",
        "Инвентарный номер",
        "Модель",
        "Производитель",
        "RAM (ГБ)",
        "Состояние"
    });
    reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    reportTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    reportTable->setSelectionMode(QAbstractItemView::SingleSelection);
    reportTable->setAlternatingRowColors(true);
    reportTable->verticalHeader()->setVisible(false);
    reportTable->horizontalHeader()->setStretchLastSection(true);
    reportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    reportLayout->addWidget(reportTable);

    layout->addWidget(reportBox, 1);

    connect(btnReportRam, &QPushButton::clicked, this, &StatsTabWidget::onReportRam);
    connect(btnSaveReport, &QPushButton::clicked, this, &StatsTabWidget::onSaveReport);

    refresh();
}

void StatsTabWidget::refresh()
{
    bool loaded = controller->isLoaded();
    btnReportRam->setEnabled(loaded);
    ramThresholdSpin->setEnabled(loaded);

    if (!loaded) {
        statsLabel->setText("База данных не загружена.");
        reportSummaryLabel->setText("Загрузите или создайте базу данных, чтобы сформировать отчет.");
        clearReportView();
        return;
    }

    int empCount = controller->getEmployees().size();
    int pcCount = controller->getComputers().size();

    statsLabel->setText(
        "Сотрудников: " + QString::number(empCount) +
        "\nКомпьютеров: " + QString::number(pcCount)
    );

    if (currentReport.empty()) {
        reportSummaryLabel->setText("Отчет еще не сформирован.");
        btnSaveReport->setEnabled(false);
    } else {
        reportSummaryLabel->setText(
            "Последний отчет: найдено " + QString::number(currentReport.size()) +
            " ПК с ОЗУ меньше " + QString::number(ramThresholdSpin->value()) + " ГБ."
        );
        btnSaveReport->setEnabled(true);
    }
}

void StatsTabWidget::onReportRam()
{
    if (!controller->isLoaded())
        return;

    int value = ramThresholdSpin->value();
    currentReport = controller->getReportRamLessThan(value);
    populateReportTable(currentReport);

    reportSummaryLabel->setText(
        "Найдено " + QString::number(currentReport.size()) +
        " ПК с ОЗУ меньше " + QString::number(value) + " ГБ."
    );
    btnSaveReport->setEnabled(!currentReport.empty());
}

void StatsTabWidget::onSaveReport()
{
    if (currentReport.empty()) {
        QMessageBox::information(this, "Сохранение отчета", "Сначала сформируйте отчет.");
        return;
    }

    QString defaultName =
        "ram_report_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss") + ".txt";

    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Сохранить отчет",
        defaultName,
        "Text files (*.txt);;CSV files (*.csv);;All files (*.*)"
    );

    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи.");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << buildReportText();
    file.close();

    QMessageBox::information(this, "Сохранение отчета", "Отчет успешно сохранен.");
}

void StatsTabWidget::clearReportView()
{
    currentReport.clear();
    reportTable->setRowCount(0);
    btnSaveReport->setEnabled(false);
}

void StatsTabWidget::populateReportTable(const std::vector<Computer>& report)
{
    reportTable->setRowCount(0);

    for (int row = 0; row < static_cast<int>(report.size()); ++row) {
        const auto& computer = report[row];
        reportTable->insertRow(row);
        reportTable->setItem(row, 0, new QTableWidgetItem(QString::number(computer.id)));
        reportTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(computer.inventoryNumber)));
        reportTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(computer.model)));
        reportTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(computer.manufacturer)));
        reportTable->setItem(row, 4, new QTableWidgetItem(QString::number(computer.ramSize)));
        reportTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(computer.condition)));
    }

    if (report.empty()) {
        reportSummaryLabel->setText(
            "По выбранному порогу ОЗУ компьютеры не найдены."
        );
        btnSaveReport->setEnabled(false);
    }
}

QString StatsTabWidget::buildReportText() const
{
    QString text;
    text += "Отчет по компьютерам с ОЗУ меньше заданного значения\n";
    text += "Дата формирования: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + "\n";
    text += "Порог ОЗУ: " + QString::number(ramThresholdSpin->value()) + " ГБ\n";
    text += "Найдено: " + QString::number(currentReport.size()) + "\n\n";

    for (const auto& computer : currentReport) {
        text += "ID: " + QString::number(computer.id) +
                " | Инв. номер: " + QString::fromStdString(computer.inventoryNumber) +
                " | Модель: " + QString::fromStdString(computer.model) +
                " | Производитель: " + QString::fromStdString(computer.manufacturer) +
                " | RAM: " + QString::number(computer.ramSize) + " ГБ" +
                " | Состояние: " + QString::fromStdString(computer.condition) + "\n";
    }

    return text;
}
