#include "EmployeeDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

EmployeeDialog::EmployeeDialog(QWidget* parent)
    : QDialog(parent),
      instituteEdit(new QLineEdit(this)),
      departmentEdit(new QLineEdit(this)),
      lastNameEdit(new QLineEdit(this)),
      initialsEdit(new QLineEdit(this)),
      positionEdit(new QLineEdit(this)),
      phoneEdit(new QLineEdit(this)),
      emailEdit(new QLineEdit(this)),
      statusCombo(new QComboBox(this))
{
    QFormLayout* layout = new QFormLayout(this);

    QRegularExpression phoneRx("^\\+\\d\\(\\d{3}\\)\\d{2}-\\d{2}-\\d{2}$");
    phoneEdit->setValidator(new QRegularExpressionValidator(phoneRx, phoneEdit));
    phoneEdit->setPlaceholderText("+7(999)12-34-56");

    QRegularExpression emailRx("^[^@\\s]+@[^@\\s]+\\.[^@\\s]+$");
    emailEdit->setValidator(new QRegularExpressionValidator(emailRx, emailEdit));

    statusCombo->addItems({"Активен", "Уволен"});

    layout->addRow("Институт:", instituteEdit);
    layout->addRow("Кафедра:", departmentEdit);
    layout->addRow("Фамилия:", lastNameEdit);
    layout->addRow("Инициалы:", initialsEdit);
    layout->addRow("Должность:", positionEdit);
    layout->addRow("Телефон:", phoneEdit);
    layout->addRow("Email:", emailEdit);
    layout->addRow("Статус:", statusCombo);

    QDialogButtonBox* buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [this, phoneRx, emailRx]() {
        if (lastNameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Фамилия обязательна");
            return;
        }
        QString phone = phoneEdit->text().trimmed();
        if (!phone.isEmpty() && !phoneRx.match(phone).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Телефон в формате +X(XXX)XX-XX-XX");
            return;
        }
        QString email = emailEdit->text().trimmed();
        if (!email.isEmpty() && !emailRx.match(email).hasMatch()) {
            QMessageBox::warning(this, "Ошибка", "Некорректный email");
            return;
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    baseEmployee.id = 0;
    baseEmployee.computerId = std::nullopt;
    baseEmployee.status = "Активен";
}

void EmployeeDialog::setEmployee(const Employee& employee)
{
    baseEmployee = employee;

    instituteEdit->setText(QString::fromStdString(employee.institute));
    departmentEdit->setText(QString::fromStdString(employee.department));
    lastNameEdit->setText(QString::fromStdString(employee.lastName));
    initialsEdit->setText(QString::fromStdString(employee.initials));
    positionEdit->setText(QString::fromStdString(employee.position));
    phoneEdit->setText(QString::fromStdString(employee.phone));
    emailEdit->setText(QString::fromStdString(employee.email));
    statusCombo->setCurrentText(QString::fromStdString(employee.status));
}

Employee EmployeeDialog::getEmployee() const
{
    Employee result = baseEmployee;
    result.institute = instituteEdit->text().toStdString();
    result.department = departmentEdit->text().toStdString();
    result.lastName = lastNameEdit->text().toStdString();
    result.initials = initialsEdit->text().toStdString();
    result.position = positionEdit->text().toStdString();
    result.phone = phoneEdit->text().toStdString();
    result.email = emailEdit->text().toStdString();
    result.status = statusCombo->currentText().toStdString();

    if (result.status == "Уволен")
        result.computerId.reset();

    return result;
}
