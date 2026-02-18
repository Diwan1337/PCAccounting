#include "EmployeesTabWidget.h"
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
#include <QStringList>
#include <QDialog>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QLabel>
#include <QSignalBlocker>

#include <algorithm>
#include <exception>
#include <vector>

#include "models/Employee.h"
#include "models/Computer.h"
#include "EmployeeDialog.h"

EmployeesTabWidget::EmployeesTabWidget(ApplicationController* controller,
                                       QWidget* parent)
    : QWidget(parent),
    controller(controller)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* filtersLayout = new QHBoxLayout();
    instituteFilter = new QComboBox();
    departmentFilter = new QComboBox();
    statusFilter = new QComboBox();
    sortFilter = new QComboBox();

    instituteFilter->addItem("Все институты");
    departmentFilter->addItem("Все кафедры");
    statusFilter->addItems({"Все статусы", "Активен", "Уволен"});
    sortFilter->addItems({"Фамилия", "Институт", "Кафедра", "Должность", "Статус"});

    btnResetFilters = new QPushButton("Сброс фильтров");
    btnFullTable = new QPushButton("Полная таблица");

    filtersLayout->addWidget(new QLabel("Институт:"));
    filtersLayout->addWidget(instituteFilter);
    filtersLayout->addWidget(new QLabel("Кафедра:"));
    filtersLayout->addWidget(departmentFilter);
    filtersLayout->addWidget(new QLabel("Статус:"));
    filtersLayout->addWidget(statusFilter);
    filtersLayout->addWidget(new QLabel("Сортировка:"));
    filtersLayout->addWidget(sortFilter);
    filtersLayout->addWidget(btnResetFilters);
    filtersLayout->addWidget(btnFullTable);

    layout->addLayout(filtersLayout);

    table = new QTableWidget();
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels(
        {"ID", "Фамилия", "Должность", "ПК"}
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

    btnAdd = new QPushButton("Добавить сотрудника");
    btnEdit = new QPushButton("Редактировать сотрудника");
    btnDelete = new QPushButton("Удалить сотрудника");
    btnAssign = new QPushButton("Назначить ПК");

    buttonsLayout->addWidget(btnAdd);
    buttonsLayout->insertWidget(1, btnEdit);
    buttonsLayout->insertWidget(2, btnAssign);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(btnDelete);

    layout->addLayout(buttonsLayout);

    connect(table, &QTableWidget::itemSelectionChanged,
            this, &EmployeesTabWidget::onSelectionChanged);

    connect(btnAdd, &QPushButton::clicked, this, &EmployeesTabWidget::onAddEmployee);
    connect(btnEdit, &QPushButton::clicked, this, &EmployeesTabWidget::onEditEmployee);
    connect(btnAssign, &QPushButton::clicked, this, &EmployeesTabWidget::onAssignPc);
    connect(btnDelete, &QPushButton::clicked, this, &EmployeesTabWidget::onDeleteEmployee);

    connect(instituteFilter, &QComboBox::currentTextChanged,
            this, &EmployeesTabWidget::onFilterChanged);
    connect(departmentFilter, &QComboBox::currentTextChanged,
            this, &EmployeesTabWidget::onFilterChanged);
    connect(statusFilter, &QComboBox::currentTextChanged,
            this, &EmployeesTabWidget::onFilterChanged);
    connect(sortFilter, &QComboBox::currentTextChanged,
            this, &EmployeesTabWidget::onFilterChanged);

    connect(btnResetFilters, &QPushButton::clicked,
            this, &EmployeesTabWidget::onResetFilters);
    connect(btnFullTable, &QPushButton::clicked,
            this, &EmployeesTabWidget::onShowFullTable);

    refresh();
}

void EmployeesTabWidget::refresh()
{
    rebuildTable();
}

void EmployeesTabWidget::applyFilter(const QString& text)
{
    currentFilter = text;
    rebuildTable();
}

void EmployeesTabWidget::setButtonsEnabled(bool enabled)
{
    btnAdd->setEnabled(enabled);
    btnEdit->setEnabled(enabled);
    btnDelete->setEnabled(enabled);
    btnAssign->setEnabled(enabled);
    btnResetFilters->setEnabled(enabled);
    btnFullTable->setEnabled(enabled);
    instituteFilter->setEnabled(enabled);
    departmentFilter->setEnabled(enabled);
    statusFilter->setEnabled(enabled);
    sortFilter->setEnabled(enabled);
}

void EmployeesTabWidget::refreshFilterValues()
{
    const auto& employees = controller->getEmployees();

    QString selectedInstitute = instituteFilter->currentText();
    QString selectedDepartment = departmentFilter->currentText();
    QString selectedStatus = statusFilter->currentText();

    std::vector<QString> institutes;
    std::vector<QString> departments;
    std::vector<QString> statuses;

    for (const auto& e : employees) {
        QString institute = QString::fromStdString(e.institute).trimmed();
        if (!institute.isEmpty())
            institutes.push_back(institute);

        QString department = QString::fromStdString(e.department).trimmed();
        if (!department.isEmpty())
            departments.push_back(department);

        QString status = QString::fromStdString(e.status).trimmed();
        if (!status.isEmpty())
            statuses.push_back(status);
    }

    auto prepareUnique = [](std::vector<QString>& values) {
        std::sort(values.begin(), values.end(),
                  [](const QString& left, const QString& right) {
                      return QString::localeAwareCompare(left, right) < 0;
                  });
        values.erase(std::unique(values.begin(), values.end()), values.end());
    };

    prepareUnique(institutes);
    prepareUnique(departments);
    prepareUnique(statuses);

    {
        QSignalBlocker b1(instituteFilter);
        instituteFilter->clear();
        instituteFilter->addItem("Все институты");
        for (const auto& value : institutes)
            instituteFilter->addItem(value);

        int idx = instituteFilter->findText(selectedInstitute);
        instituteFilter->setCurrentIndex(idx >= 0 ? idx : 0);
    }

    {
        QSignalBlocker b2(departmentFilter);
        departmentFilter->clear();
        departmentFilter->addItem("Все кафедры");
        for (const auto& value : departments)
            departmentFilter->addItem(value);

        int idx = departmentFilter->findText(selectedDepartment);
        departmentFilter->setCurrentIndex(idx >= 0 ? idx : 0);
    }

    {
        QSignalBlocker b3(statusFilter);
        statusFilter->clear();
        statusFilter->addItem("Все статусы");
        for (const auto& value : statuses)
            statusFilter->addItem(value);

        int idx = statusFilter->findText(selectedStatus);
        statusFilter->setCurrentIndex(idx >= 0 ? idx : 0);
    }
}

void EmployeesTabWidget::rebuildTable()
{
    bool loaded = controller->isLoaded();
    setButtonsEnabled(loaded);

    table->clearContents();
    table->setRowCount(0);

    if (!loaded) {
        updateDetails();
        return;
    }

    refreshFilterValues();

    const auto& employees = controller->getEmployees();
    const auto& computers = controller->getComputers();

    std::vector<const Employee*> filtered;
    filtered.reserve(employees.size());

    QString instituteCriterion = instituteFilter->currentText();
    QString departmentCriterion = departmentFilter->currentText();
    QString statusCriterion = statusFilter->currentText();
    QString sortCriterion = sortFilter->currentText();

    for (const auto& e : employees) {
        QString lastName = QString::fromStdString(e.lastName);
        QString position = QString::fromStdString(e.position);
        QString institute = QString::fromStdString(e.institute);
        QString department = QString::fromStdString(e.department);
        QString email = QString::fromStdString(e.email);

        if (!currentFilter.isEmpty() &&
            !lastName.contains(currentFilter, Qt::CaseInsensitive) &&
            !position.contains(currentFilter, Qt::CaseInsensitive) &&
            !institute.contains(currentFilter, Qt::CaseInsensitive) &&
            !department.contains(currentFilter, Qt::CaseInsensitive) &&
            !email.contains(currentFilter, Qt::CaseInsensitive))
            continue;

        if (instituteCriterion != "Все институты" && institute != instituteCriterion)
            continue;

        if (departmentCriterion != "Все кафедры" && department != departmentCriterion)
            continue;

        QString status = QString::fromStdString(e.status);
        if (statusCriterion != "Все статусы" && status != statusCriterion)
            continue;

        filtered.push_back(&e);
    }

    auto compareTextField = [](const QString& left, const QString& right) {
        return QString::localeAwareCompare(left, right) < 0;
    };

    std::sort(filtered.begin(), filtered.end(),
              [&](const Employee* left, const Employee* right) {
                  QString l;
                  QString r;

                  if (sortCriterion == "Институт") {
                      l = QString::fromStdString(left->institute);
                      r = QString::fromStdString(right->institute);
                  }
                  else if (sortCriterion == "Кафедра") {
                      l = QString::fromStdString(left->department);
                      r = QString::fromStdString(right->department);
                  }
                  else if (sortCriterion == "Должность") {
                      l = QString::fromStdString(left->position);
                      r = QString::fromStdString(right->position);
                  }
                  else if (sortCriterion == "Статус") {
                      l = QString::fromStdString(left->status);
                      r = QString::fromStdString(right->status);
                  }
                  else {
                      l = QString::fromStdString(left->lastName);
                      r = QString::fromStdString(right->lastName);
                  }

                  if (l == r)
                      return left->id < right->id;

                  return compareTextField(l, r);
              });

    int row = 0;
    for (const auto* e : filtered) {
        table->insertRow(row);
        table->setItem(row, 0,
            new QTableWidgetItem(QString::number(e->id)));
        table->setItem(row, 1,
            new QTableWidgetItem(QString::fromStdString(e->lastName)));
        table->setItem(row, 2,
            new QTableWidgetItem(QString::fromStdString(e->position)));

        QString pcText = "-";

        if (e->computerId.has_value()) {
            int compId = e->computerId.value();
            for (const auto& c : computers) {
                if (c.id == compId) {
                    pcText = "ID: " + QString::number(c.id) +
                             " | " + QString::fromStdString(c.inventoryNumber) +
                             " | " + QString::fromStdString(c.serialNumber) +
                             " | " + QString::fromStdString(c.model);
                    break;
                }
            }
        }

        table->setItem(row, 3,
            new QTableWidgetItem(pcText));
        row++;
    }

    updateDetails();
}

void EmployeesTabWidget::onAddEmployee()
{
    if (!controller->isLoaded()) {
        QMessageBox::warning(this, "Ошибка", "Сначала создайте БД");
        return;
    }

    EmployeeDialog dialog(this);
    dialog.setWindowTitle("Добавить сотрудника");

    if (dialog.exec() != QDialog::Accepted)
        return;

    Employee e = dialog.getEmployee();
    e.employmentDate = "";

    try {
        controller->addEmployee(e);
    }
    catch (const std::exception& ex) {
        QMessageBox::critical(this, "Ошибка", ex.what());
        return;
    }

    emit dataChanged();
}

void EmployeesTabWidget::onEditEmployee()
{
    if (!controller->isLoaded())
        return;

    int row = table->currentRow();
    if (row < 0 || !table->item(row, 0)) {
        QMessageBox::warning(this, "Ошибка", "Выберите сотрудника");
        return;
    }

    int id = table->item(row, 0)->text().toInt();

    const auto& employees = controller->getEmployees();
    const Employee* current = nullptr;
    for (const auto& e : employees) {
        if (e.id == id) {
            current = &e;
            break;
        }
    }

    if (!current) {
        QMessageBox::warning(this, "Ошибка", "Сотрудник не найден");
        return;
    }

    EmployeeDialog dialog(this);
    dialog.setWindowTitle("Редактировать сотрудника");
    dialog.setEmployee(*current);

    if (dialog.exec() != QDialog::Accepted)
        return;

    Employee updated = dialog.getEmployee();

    try {
        if (!controller->updateEmployee(updated)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось обновить сотрудника");
            return;
        }
    }
    catch (const std::exception& ex) {
        QMessageBox::critical(this, "Ошибка", ex.what());
        return;
    }

    emit dataChanged();
}

void EmployeesTabWidget::onDeleteEmployee()
{
    if (!controller->isLoaded())
        return;

    int row = table->currentRow();
    if (row < 0 || !table->item(row, 0)) {
        QMessageBox::warning(this, "Ошибка", "Выберите сотрудника");
        return;
    }

    int id = table->item(row, 0)->text().toInt();

    if (QMessageBox::question(this,
                              "Подтверждение",
                              "Удалить сотрудника?") == QMessageBox::Yes)
    {
        controller->removeEmployee(id);
        emit dataChanged();
    }
}

void EmployeesTabWidget::onAssignPc()
{
    if (!controller->isLoaded())
        return;

    int row = table->currentRow();
    if (row < 0 || !table->item(row, 0)) {
        QMessageBox::warning(this, "Ошибка", "Выберите сотрудника");
        return;
    }

    int empId = table->item(row, 0)->text().toInt();

    const auto& employees = controller->getEmployees();
    const auto& computers = controller->getComputers();

    for (const auto& e : employees) {
        if (e.id == empId && e.status == "Уволен") {
            QMessageBox::warning(this, "Ошибка", "Нельзя назначить ПК уволенному сотруднику");
            return;
        }
    }

    if (computers.empty()) {
        QMessageBox::information(this, "Инфо", "Нет доступных ПК");
        return;
    }

    QStringList items;
    for (const auto& c : computers) {
        QString suffix;
        for (const auto& e : employees) {
            if (e.computerId.has_value() &&
                e.computerId.value() == c.id) {
                QString initials = QString::fromStdString(e.initials);
                QString name = QString::fromStdString(e.lastName);
                if (!initials.isEmpty())
                    name += " " + initials;
                suffix = " (занят: " + name + ")";
                break;
            }
        }
        QString info = "ID: " + QString::number(c.id) +
                       " | " + QString::fromStdString(c.inventoryNumber) +
                       " | " + QString::fromStdString(c.serialNumber) +
                       " | " + QString::fromStdString(c.model) +
                       " | RAM " + QString::number(c.ramSize) + "GB";
        items << info + suffix;
    }

    bool ok;
    QString selected = QInputDialog::getItem(
        this,
        "Назначить ПК",
        "Выберите компьютер:",
        items,
        0,
        false,
        &ok
    );

    if (!ok)
        return;

    bool parsed = false;
    int compId = selected.section('|', 0, 0).remove("ID:").trimmed().toInt(&parsed);
    if (!parsed) {
        QMessageBox::warning(this, "Ошибка", "Не удалось определить выбранный ПК");
        return;
    }

    int assignedEmpId = -1;
    QString assignedEmpName;
    for (const auto& e : employees) {
        if (e.computerId.has_value() &&
            e.computerId.value() == compId) {
            assignedEmpId = e.id;
            assignedEmpName = QString::fromStdString(e.lastName);
            break;
        }
    }

    if (assignedEmpId == empId) {
        QMessageBox::information(this, "Инфо", "Этот ПК уже назначен этому сотруднику");
        return;
    }

    if (assignedEmpId != -1) {
        QString question = "ПК уже назначен сотруднику " + assignedEmpName +
                           ". Переназначить?";
        if (QMessageBox::question(this,
                                  "Подтверждение",
                                  question) != QMessageBox::Yes)
            return;

        controller->unassignComputerByComputerId(compId);
    }

    if (!controller->assignComputer(empId, compId)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось назначить ПК");
        return;
    }

    QMessageBox::information(this, "Готово", "ПК назначен");
    emit dataChanged();
}

void EmployeesTabWidget::onSelectionChanged()
{
    updateDetails();
}

void EmployeesTabWidget::onFilterChanged()
{
    rebuildTable();
}

void EmployeesTabWidget::onResetFilters()
{
    {
        QSignalBlocker b1(instituteFilter);
        QSignalBlocker b2(departmentFilter);
        QSignalBlocker b3(statusFilter);
        QSignalBlocker b4(sortFilter);

        instituteFilter->setCurrentIndex(0);
        departmentFilter->setCurrentIndex(0);
        statusFilter->setCurrentIndex(0);
        sortFilter->setCurrentIndex(0);
    }

    rebuildTable();
}

void EmployeesTabWidget::onShowFullTable()
{
    if (!controller->isLoaded())
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Полная таблица сотрудников");
    dialog.resize(1400, 700);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTableWidget* fullTable = new QTableWidget(&dialog);
    fullTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fullTable->setColumnCount(11);
    fullTable->setHorizontalHeaderLabels({
        "ID", "Институт", "Кафедра", "Фамилия", "Инициалы",
        "Должность", "Телефон", "Email", "Статус", "Дата приема", "ПК"
    });
    fullTable->horizontalHeader()->setStretchLastSection(true);

    const auto& employees = controller->getEmployees();
    const auto& computers = controller->getComputers();

    auto safe = [](const std::string& value) {
        return value.empty() ? QString("-") : QString::fromStdString(value);
    };

    fullTable->setRowCount(static_cast<int>(employees.size()));
    for (int row = 0; row < static_cast<int>(employees.size()); ++row) {
        const auto& e = employees[row];
        fullTable->setItem(row, 0, new QTableWidgetItem(QString::number(e.id)));
        fullTable->setItem(row, 1, new QTableWidgetItem(safe(e.institute)));
        fullTable->setItem(row, 2, new QTableWidgetItem(safe(e.department)));
        fullTable->setItem(row, 3, new QTableWidgetItem(safe(e.lastName)));
        fullTable->setItem(row, 4, new QTableWidgetItem(safe(e.initials)));
        fullTable->setItem(row, 5, new QTableWidgetItem(safe(e.position)));
        fullTable->setItem(row, 6, new QTableWidgetItem(safe(e.phone)));
        fullTable->setItem(row, 7, new QTableWidgetItem(safe(e.email)));
        fullTable->setItem(row, 8, new QTableWidgetItem(safe(e.status)));
        fullTable->setItem(row, 9, new QTableWidgetItem(safe(e.employmentDate)));

        QString pcText = "-";
        if (e.computerId.has_value()) {
            for (const auto& c : computers) {
                if (c.id == e.computerId.value()) {
                    pcText = "ID: " + QString::number(c.id) +
                             " | " + safe(c.inventoryNumber) +
                             " | " + safe(c.model);
                    break;
                }
            }
        }
        fullTable->setItem(row, 10, new QTableWidgetItem(pcText));
    }

    layout->addWidget(fullTable);

    QPushButton* closeButton = new QPushButton("Закрыть", &dialog);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeButton);

    dialog.exec();
}

void EmployeesTabWidget::updateDetails()
{
    if (!controller->isLoaded()) {
        details->setText("База не загружена");
        return;
    }

    int row = table->currentRow();
    if (row < 0 || !table->item(row, 0)) {
        details->setText("Выберите сотрудника");
        return;
    }

    int id = table->item(row, 0)->text().toInt();
    const auto& employees = controller->getEmployees();
    const auto& computers = controller->getComputers();

    const Employee* emp = nullptr;
    for (const auto& e : employees) {
        if (e.id == id) {
            emp = &e;
            break;
        }
    }

    if (!emp) {
        details->setText("Сотрудник не найден");
        return;
    }

    auto safe = [](const std::string& value) {
        return value.empty() ? QString("-") : QString::fromStdString(value);
    };

    QString text;
    text += "ID: " + QString::number(emp->id) + "\n";
    text += "Фамилия: " + safe(emp->lastName) + "\n";
    text += "Инициалы: " + safe(emp->initials) + "\n";
    text += "Институт: " + safe(emp->institute) + "\n";
    text += "Кафедра: " + safe(emp->department) + "\n";
    text += "Должность: " + safe(emp->position) + "\n";
    text += "Телефон: " + safe(emp->phone) + "\n";
    text += "Email: " + safe(emp->email) + "\n";
    text += "Статус: " + safe(emp->status) + "\n";
    text += "Дата приема: " + safe(emp->employmentDate) + "\n";

    QString pcText = "-";
    if (emp->computerId.has_value()) {
        int compId = emp->computerId.value();
        for (const auto& c : computers) {
            if (c.id == compId) {
                pcText = "ID: " + QString::number(c.id) +
                         " | " + QString::fromStdString(c.inventoryNumber) +
                         " | " + QString::fromStdString(c.serialNumber) +
                         " | " + QString::fromStdString(c.model);
                break;
            }
        }
    }
    text += "ПК: " + pcText;

    details->setText(text);
}
