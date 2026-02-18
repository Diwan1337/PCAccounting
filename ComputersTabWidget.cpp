#include "ComputersTabWidget.h"
#include "projects/src/core/ApplicationController.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAbstractItemView>
#include <QGroupBox>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QDate>
#include <QSignalBlocker>

#include <algorithm>
#include <cstdlib>
#include <string>
#include <exception>
#include <vector>

#include "models/Employee.h"
#include "models/Computer.h"
#include "ComputerDialog.h"

namespace {
QDate parseUiDate(const std::string& value)
{
    QString text = QString::fromStdString(value).trimmed();
    if (text.isEmpty())
        return QDate();

    return QDate::fromString(text, "dd.MM.yyyy");
}

QString safeText(const std::string& value)
{
    return value.empty() ? QString("-") : QString::fromStdString(value);
}
}

ComputersTabWidget::ComputersTabWidget(ApplicationController* controller,
                                       QWidget* parent)
    : QWidget(parent),
    controller(controller)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* filtersLayout = new QHBoxLayout();
    maxRamFilter = new QSpinBox();
    maxStorageFilter = new QSpinBox();
    serviceFilter = new QComboBox();
    sortFilter = new QComboBox();

    maxRamFilter->setRange(0, 2048);
    maxRamFilter->setValue(0);
    maxRamFilter->setSpecialValueText("любой");

    maxStorageFilter->setRange(0, 16384);
    maxStorageFilter->setValue(0);
    maxStorageFilter->setSpecialValueText("любой");

    serviceFilter->addItems({
        "Все",
        "Просрочено ТО (до сегодня)",
        "ТО в ближайшие 30 дней",
        "Без даты ТО"
    });

    sortFilter->addItems({"RAM", "Диск", "Дата ТО", "Модель", "Инвентарный"});

    btnResetFilters = new QPushButton("Сброс фильтров");
    btnFullTable = new QPushButton("Полная таблица");

    filtersLayout->addWidget(new QLabel("RAM <="));
    filtersLayout->addWidget(maxRamFilter);
    filtersLayout->addWidget(new QLabel("Диск <="));
    filtersLayout->addWidget(maxStorageFilter);
    filtersLayout->addWidget(new QLabel("ТО:"));
    filtersLayout->addWidget(serviceFilter);
    filtersLayout->addWidget(new QLabel("Сортировка:"));
    filtersLayout->addWidget(sortFilter);
    filtersLayout->addWidget(btnResetFilters);
    filtersLayout->addWidget(btnFullTable);

    layout->addLayout(filtersLayout);

    table = new QTableWidget();
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels(
        {"ID", "Модель", "RAM", "Сотрудник"}
        );
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(table);

    QGroupBox* detailsGroup = new QGroupBox("Подробная информация");
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsGroup);
    details = new QTextEdit();
    details->setReadOnly(true);
    details->setMinimumHeight(120);
    detailsLayout->addWidget(details);
    layout->addWidget(detailsGroup);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();

    btnAdd = new QPushButton("Добавить ПК");
    btnEdit = new QPushButton("Редактировать ПК");
    btnDelete = new QPushButton("Удалить ПК");
    btnFree = new QPushButton("Показать свободные");
    btnUnassign = new QPushButton("Отвязать ПК");

    buttonsLayout->addWidget(btnAdd);
    buttonsLayout->insertWidget(1, btnEdit);
    buttonsLayout->insertWidget(2, btnFree);
    buttonsLayout->insertWidget(3, btnUnassign);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(btnDelete);

    layout->addLayout(buttonsLayout);

    connect(table, &QTableWidget::itemSelectionChanged,
            this, &ComputersTabWidget::onSelectionChanged);

    connect(btnAdd, &QPushButton::clicked, this, &ComputersTabWidget::onAddComputer);
    connect(btnEdit, &QPushButton::clicked, this, &ComputersTabWidget::onEditComputer);
    connect(btnFree, &QPushButton::clicked, this, &ComputersTabWidget::onShowFreeComputers);
    connect(btnUnassign, &QPushButton::clicked, this, &ComputersTabWidget::onUnassignPc);
    connect(btnDelete, &QPushButton::clicked, this, &ComputersTabWidget::onDeleteComputer);

    connect(maxRamFilter, qOverload<int>(&QSpinBox::valueChanged),
            this, &ComputersTabWidget::onFilterChanged);
    connect(maxStorageFilter, qOverload<int>(&QSpinBox::valueChanged),
            this, &ComputersTabWidget::onFilterChanged);
    connect(serviceFilter, &QComboBox::currentTextChanged,
            this, &ComputersTabWidget::onFilterChanged);
    connect(sortFilter, &QComboBox::currentTextChanged,
            this, &ComputersTabWidget::onFilterChanged);

    connect(btnResetFilters, &QPushButton::clicked,
            this, &ComputersTabWidget::onResetFilters);
    connect(btnFullTable, &QPushButton::clicked,
            this, &ComputersTabWidget::onShowFullTable);

    refresh();
}

void ComputersTabWidget::refresh()
{
    rebuildTable();
}

void ComputersTabWidget::applyFilter(const QString& text)
{
    currentFilter = text;
    rebuildTable();
}

void ComputersTabWidget::setButtonsEnabled(bool enabled)
{
    btnAdd->setEnabled(enabled);
    btnEdit->setEnabled(enabled);
    btnDelete->setEnabled(enabled);
    btnFree->setEnabled(enabled);
    btnUnassign->setEnabled(enabled);
    btnResetFilters->setEnabled(enabled);
    btnFullTable->setEnabled(enabled);
    maxRamFilter->setEnabled(enabled);
    maxStorageFilter->setEnabled(enabled);
    serviceFilter->setEnabled(enabled);
    sortFilter->setEnabled(enabled);
}

void ComputersTabWidget::rebuildTable()
{
    bool loaded = controller->isLoaded();
    setButtonsEnabled(loaded);

    table->clearContents();
    table->setRowCount(0);

    if (!loaded) {
        updateDetails();
        return;
    }

    const auto& computers = controller->getComputers();
    const auto& employees = controller->getEmployees();

    int ramLimit = maxRamFilter->value();
    int storageLimit = maxStorageFilter->value();
    QString serviceCriterion = serviceFilter->currentText();
    QString sortCriterion = sortFilter->currentText();
    QDate today = QDate::currentDate();

    std::vector<const Computer*> filtered;
    filtered.reserve(computers.size());

    for (const auto& c : computers) {
        QString model = QString::fromStdString(c.model);
        QString inventory = QString::fromStdString(c.inventoryNumber);
        QString serial = QString::fromStdString(c.serialNumber);

        if (!currentFilter.isEmpty() &&
            !model.contains(currentFilter, Qt::CaseInsensitive) &&
            !inventory.contains(currentFilter, Qt::CaseInsensitive) &&
            !serial.contains(currentFilter, Qt::CaseInsensitive))
            continue;

        if (ramLimit > 0 && c.ramSize > ramLimit)
            continue;

        if (storageLimit > 0 && c.storageSize > storageLimit)
            continue;

        QDate maintenanceDate = parseUiDate(c.lastMaintenanceDate);

        if (serviceCriterion == "Просрочено ТО (до сегодня)") {
            if (!maintenanceDate.isValid() || maintenanceDate > today)
                continue;
        }
        else if (serviceCriterion == "ТО в ближайшие 30 дней") {
            if (!maintenanceDate.isValid() ||
                maintenanceDate < today ||
                maintenanceDate > today.addDays(30))
                continue;
        }
        else if (serviceCriterion == "Без даты ТО") {
            if (maintenanceDate.isValid())
                continue;
        }

        filtered.push_back(&c);
    }

    std::sort(filtered.begin(), filtered.end(),
              [&](const Computer* left, const Computer* right) {
                  if (sortCriterion == "RAM") {
                      if (left->ramSize == right->ramSize)
                          return left->id < right->id;
                      return left->ramSize < right->ramSize;
                  }

                  if (sortCriterion == "Диск") {
                      if (left->storageSize == right->storageSize)
                          return left->id < right->id;
                      return left->storageSize < right->storageSize;
                  }

                  if (sortCriterion == "Дата ТО") {
                      QDate l = parseUiDate(left->lastMaintenanceDate);
                      QDate r = parseUiDate(right->lastMaintenanceDate);
                      if (!l.isValid() && !r.isValid())
                          return left->id < right->id;
                      if (!l.isValid())
                          return false;
                      if (!r.isValid())
                          return true;
                      if (l == r)
                          return left->id < right->id;
                      return l < r;
                  }

                  if (sortCriterion == "Инвентарный") {
                      QString l = QString::fromStdString(left->inventoryNumber);
                      QString r = QString::fromStdString(right->inventoryNumber);
                      if (l == r)
                          return left->id < right->id;
                      return QString::localeAwareCompare(l, r) < 0;
                  }

                  QString l = QString::fromStdString(left->model);
                  QString r = QString::fromStdString(right->model);
                  if (l == r)
                      return left->id < right->id;
                  return QString::localeAwareCompare(l, r) < 0;
              });

    int row = 0;
    for (const auto* c : filtered) {
        table->insertRow(row);
        table->setItem(row, 0,
            new QTableWidgetItem(QString::number(c->id)));
        table->setItem(row, 1,
            new QTableWidgetItem(QString::fromStdString(c->model)));
        table->setItem(row, 2,
            new QTableWidgetItem(QString::number(c->ramSize)));

        QString empText = "-";
        for (const auto& e : employees) {
            if (e.computerId.has_value() &&
                e.computerId.value() == c->id) {
                QString initials = QString::fromStdString(e.initials);
                QString name = QString::fromStdString(e.lastName);
                if (!initials.isEmpty())
                    name += " " + initials;
                empText = "ID: " + QString::number(e.id) + " | " + name;
                break;
            }
        }

        table->setItem(row, 3,
            new QTableWidgetItem(empText));
        row++;
    }

    updateDetails();
}

void ComputersTabWidget::onAddComputer()
{
    if (!controller->isLoaded()) {
        QMessageBox::warning(this, "Ошибка", "Сначала создайте БД");
        return;
    }

    Computer c;
    do {
        c.inventoryNumber = "INV-" + std::to_string(rand() % 10000);
    } while (!controller->isInventoryNumberUnique(c.inventoryNumber));

    do {
        c.serialNumber = "SN-" + std::to_string(rand() % 10000);
    } while (!controller->isSerialNumberUnique(c.serialNumber));

    c.manufacturer = "Unknown";
    c.model = "";
    c.cpuModel = "";
    c.chipset = "";
    c.ramSize = 8;
    c.storageType = "SSD";
    c.storageSize = 256;
    c.roomNumber = "";
    c.condition = "Рабочее";
    c.commissioningDate = "";
    c.lastMaintenanceDate = "";
    c.warrantyExpirationDate = "";

    ComputerDialog dialog(this);
    dialog.setWindowTitle("Добавить ПК");
    dialog.setComputer(c);

    if (dialog.exec() != QDialog::Accepted)
        return;

    Computer created = dialog.getComputer();

    try {
        controller->addComputer(created);
    }
    catch (const std::exception& ex) {
        QMessageBox::critical(this, "Ошибка", ex.what());
        return;
    }

    emit dataChanged();
}

void ComputersTabWidget::onEditComputer()
{
    if (!controller->isLoaded())
        return;

    int row = table->currentRow();
    if (row < 0 || !table->item(row, 0)) {
        QMessageBox::warning(this, "Ошибка", "Выберите ПК");
        return;
    }

    int id = table->item(row, 0)->text().toInt();

    const auto& computers = controller->getComputers();
    const Computer* current = nullptr;
    for (const auto& c : computers) {
        if (c.id == id) {
            current = &c;
            break;
        }
    }

    if (!current) {
        QMessageBox::warning(this, "Ошибка", "ПК не найден");
        return;
    }

    ComputerDialog dialog(this);
    dialog.setWindowTitle("Редактировать ПК");
    dialog.setComputer(*current);

    if (dialog.exec() != QDialog::Accepted)
        return;

    Computer updated = dialog.getComputer();

    try {
        if (!controller->updateComputer(updated)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось обновить ПК");
            return;
        }
    }
    catch (const std::exception& ex) {
        QMessageBox::critical(this, "Ошибка", ex.what());
        return;
    }

    emit dataChanged();
}

void ComputersTabWidget::onDeleteComputer()
{
    if (!controller->isLoaded())
        return;

    int row = table->currentRow();
    if (row < 0 || !table->item(row, 0)) {
        QMessageBox::warning(this, "Ошибка", "Выберите ПК");
        return;
    }

    int id = table->item(row, 0)->text().toInt();

    if (QMessageBox::question(this,
                              "Подтверждение",
                              "Удалить компьютер?") == QMessageBox::Yes)
    {
        controller->removeComputer(id);
        emit dataChanged();
    }
}

void ComputersTabWidget::onShowFreeComputers()
{
    if (!controller->isLoaded())
        return;

    auto free = controller->getFreeComputers();

    if (free.empty()) {
        QMessageBox::information(this, "Свободные ПК", "Свободных ПК нет");
        return;
    }

    QString text;
    for (const auto& c : free) {
        text += "ID: " + QString::number(c.id) +
                " | " + QString::fromStdString(c.inventoryNumber) +
                " | " + QString::fromStdString(c.serialNumber) +
                " | " + QString::fromStdString(c.model) +
                " | RAM " + QString::number(c.ramSize) + "GB\n";
    }

    QMessageBox::information(this, "Свободные ПК", text);
}

void ComputersTabWidget::onUnassignPc()
{
    if (!controller->isLoaded())
        return;

    int row = table->currentRow();
    if (row < 0 || !table->item(row, 0)) {
        QMessageBox::warning(this, "Ошибка", "Выберите ПК");
        return;
    }

    int id = table->item(row, 0)->text().toInt();

    if (QMessageBox::question(this,
                              "Подтверждение",
                              "Отвязать ПК?") == QMessageBox::Yes)
    {
        if (!controller->unassignComputerByComputerId(id)) {
            QMessageBox::information(this, "Инфо", "ПК не назначен");
            return;
        }

        emit dataChanged();
    }
}

void ComputersTabWidget::onSelectionChanged()
{
    updateDetails();
}

void ComputersTabWidget::onFilterChanged()
{
    rebuildTable();
}

void ComputersTabWidget::onResetFilters()
{
    {
        QSignalBlocker b1(maxRamFilter);
        QSignalBlocker b2(maxStorageFilter);
        QSignalBlocker b3(serviceFilter);
        QSignalBlocker b4(sortFilter);

        maxRamFilter->setValue(0);
        maxStorageFilter->setValue(0);
        serviceFilter->setCurrentIndex(0);
        sortFilter->setCurrentIndex(0);
    }

    rebuildTable();
}

void ComputersTabWidget::onShowFullTable()
{
    if (!controller->isLoaded())
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Полная таблица ПК");
    dialog.resize(1500, 750);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTableWidget* fullTable = new QTableWidget(&dialog);
    fullTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fullTable->setColumnCount(16);
    fullTable->setHorizontalHeaderLabels({
        "ID", "Инвентарный", "Серийный", "Производитель", "Модель",
        "CPU", "Чипсет", "RAM", "Тип диска", "Объем диска",
        "Кабинет", "Состояние", "Дата ввода", "Дата ТО", "Гарантия", "Сотрудник"
    });
    fullTable->horizontalHeader()->setStretchLastSection(true);

    const auto& computers = controller->getComputers();
    const auto& employees = controller->getEmployees();

    fullTable->setRowCount(static_cast<int>(computers.size()));

    for (int row = 0; row < static_cast<int>(computers.size()); ++row) {
        const auto& c = computers[row];

        QString empText = "-";
        for (const auto& e : employees) {
            if (e.computerId.has_value() && e.computerId.value() == c.id) {
                QString initials = QString::fromStdString(e.initials);
                QString name = QString::fromStdString(e.lastName);
                if (!initials.isEmpty())
                    name += " " + initials;
                empText = "ID: " + QString::number(e.id) + " | " + name;
                break;
            }
        }

        fullTable->setItem(row, 0, new QTableWidgetItem(QString::number(c.id)));
        fullTable->setItem(row, 1, new QTableWidgetItem(safeText(c.inventoryNumber)));
        fullTable->setItem(row, 2, new QTableWidgetItem(safeText(c.serialNumber)));
        fullTable->setItem(row, 3, new QTableWidgetItem(safeText(c.manufacturer)));
        fullTable->setItem(row, 4, new QTableWidgetItem(safeText(c.model)));
        fullTable->setItem(row, 5, new QTableWidgetItem(safeText(c.cpuModel)));
        fullTable->setItem(row, 6, new QTableWidgetItem(safeText(c.chipset)));
        fullTable->setItem(row, 7, new QTableWidgetItem(QString::number(c.ramSize)));
        fullTable->setItem(row, 8, new QTableWidgetItem(safeText(c.storageType)));
        fullTable->setItem(row, 9, new QTableWidgetItem(QString::number(c.storageSize)));
        fullTable->setItem(row, 10, new QTableWidgetItem(safeText(c.roomNumber)));
        fullTable->setItem(row, 11, new QTableWidgetItem(safeText(c.condition)));
        fullTable->setItem(row, 12, new QTableWidgetItem(safeText(c.commissioningDate)));
        fullTable->setItem(row, 13, new QTableWidgetItem(safeText(c.lastMaintenanceDate)));
        fullTable->setItem(row, 14, new QTableWidgetItem(safeText(c.warrantyExpirationDate)));
        fullTable->setItem(row, 15, new QTableWidgetItem(empText));
    }

    layout->addWidget(fullTable);

    QPushButton* closeButton = new QPushButton("Закрыть", &dialog);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeButton);

    dialog.exec();
}

void ComputersTabWidget::updateDetails()
{
    if (!controller->isLoaded()) {
        details->setText("База не загружена");
        return;
    }

    int row = table->currentRow();
    if (row < 0 || !table->item(row, 0)) {
        details->setText("Выберите ПК");
        return;
    }

    int id = table->item(row, 0)->text().toInt();
    const auto& computers = controller->getComputers();
    const auto& employees = controller->getEmployees();

    const Computer* comp = nullptr;
    for (const auto& c : computers) {
        if (c.id == id) {
            comp = &c;
            break;
        }
    }

    if (!comp) {
        details->setText("ПК не найден");
        return;
    }

    QString text;
    text += "ID: " + QString::number(comp->id) + "\n";
    text += "Инвентарный номер: " + safeText(comp->inventoryNumber) + "\n";
    text += "Серийный номер: " + safeText(comp->serialNumber) + "\n";
    text += "Производитель: " + safeText(comp->manufacturer) + "\n";
    text += "Модель: " + safeText(comp->model) + "\n";
    text += "CPU: " + safeText(comp->cpuModel) + "\n";
    text += "Чипсет: " + safeText(comp->chipset) + "\n";
    text += "RAM: " + QString::number(comp->ramSize) + " GB\n";
    text += "Тип накопителя: " + safeText(comp->storageType) + "\n";
    text += "Объем накопителя: " + QString::number(comp->storageSize) + " GB\n";
    text += "Кабинет: " + safeText(comp->roomNumber) + "\n";
    text += "Состояние: " + safeText(comp->condition) + "\n";
    text += "Дата ввода: " + safeText(comp->commissioningDate) + "\n";
    text += "Дата обслуживания: " + safeText(comp->lastMaintenanceDate) + "\n";
    text += "Гарантия до: " + safeText(comp->warrantyExpirationDate) + "\n";

    QString empText = "-";
    for (const auto& e : employees) {
        if (e.computerId.has_value() &&
            e.computerId.value() == comp->id) {
            QString initials = QString::fromStdString(e.initials);
            QString name = QString::fromStdString(e.lastName);
            if (!initials.isEmpty())
                name += " " + initials;
            empText = "ID: " + QString::number(e.id) + " | " + name;
            break;
        }
    }
    text += "Сотрудник: " + empText;

    details->setText(text);
}
