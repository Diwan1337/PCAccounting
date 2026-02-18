#pragma once

#include <QObject>
#include <QDialog>
#include "models/Computer.h"

class QLineEdit;
class QSpinBox;
class QComboBox;

class ComputerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComputerDialog(QWidget* parent = nullptr);

    void setComputer(const Computer& computer);
    Computer getComputer() const;

private:
    Computer baseComputer{};

    QLineEdit* inventoryNumberEdit;
    QLineEdit* serialNumberEdit;
    QLineEdit* manufacturerEdit;
    QLineEdit* modelEdit;
    QLineEdit* cpuModelEdit;
    QLineEdit* chipsetEdit;
    QSpinBox* ramSizeSpin;
    QComboBox* storageTypeCombo;
    QSpinBox* storageSizeSpin;
    QLineEdit* roomNumberEdit;
    QComboBox* conditionCombo;
    QLineEdit* commissioningDateEdit;
    QLineEdit* lastMaintenanceDateEdit;
    QLineEdit* warrantyExpirationDateEdit;
};
