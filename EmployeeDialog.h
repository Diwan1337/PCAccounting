#pragma once

#include <QObject>
#include <QDialog>
#include "models/Employee.h"

class QLineEdit;
class QComboBox;

class EmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeeDialog(QWidget* parent = nullptr);

    void setEmployee(const Employee& employee);
    Employee getEmployee() const;

private:
    Employee baseEmployee{};

    QLineEdit* instituteEdit;
    QLineEdit* departmentEdit;
    QLineEdit* lastNameEdit;
    QLineEdit* initialsEdit;
    QLineEdit* positionEdit;
    QLineEdit* phoneEdit;
    QLineEdit* emailEdit;
    QComboBox* statusCombo;
};
