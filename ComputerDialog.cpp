#include "ComputerDialog.h"

#include <QComboBox>
#include <QDate>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSpinBox>

ComputerDialog::ComputerDialog(QWidget* parent)
    : QDialog(parent),
      inventoryNumberEdit(new QLineEdit(this)),
      serialNumberEdit(new QLineEdit(this)),
      manufacturerEdit(new QLineEdit(this)),
      modelEdit(new QLineEdit(this)),
      cpuModelEdit(new QLineEdit(this)),
      chipsetEdit(new QLineEdit(this)),
      ramSizeSpin(new QSpinBox(this)),
      storageTypeCombo(new QComboBox(this)),
      storageSizeSpin(new QSpinBox(this)),
      roomNumberEdit(new QLineEdit(this)),
      conditionCombo(new QComboBox(this)),
      commissioningDateEdit(new QLineEdit(this)),
      lastMaintenanceDateEdit(new QLineEdit(this)),
      warrantyExpirationDateEdit(new QLineEdit(this))
{
    QFormLayout* layout = new QFormLayout(this);

    ramSizeSpin->setRange(1, 4096);
    storageSizeSpin->setRange(1, 102400);

    QRegularExpression dateRx("^\\d{2}\\.\\d{2}\\.\\d{4}$");

    storageTypeCombo->addItems({"SSD", "HDD", "NVMe"});
    conditionCombo->addItems({"Рабочее", "Неисправно", "На ремонте", "Списано"});

    commissioningDateEdit->setPlaceholderText("dd.MM.yyyy");
    lastMaintenanceDateEdit->setPlaceholderText("dd.MM.yyyy");
    warrantyExpirationDateEdit->setPlaceholderText("dd.MM.yyyy");

    layout->addRow("Инвентарный номер:", inventoryNumberEdit);
    layout->addRow("Серийный номер:", serialNumberEdit);
    layout->addRow("Производитель:", manufacturerEdit);
    layout->addRow("Модель:", modelEdit);
    layout->addRow("CPU:", cpuModelEdit);
    layout->addRow("Чипсет:", chipsetEdit);
    layout->addRow("RAM (ГБ):", ramSizeSpin);
    layout->addRow("Тип накопителя:", storageTypeCombo);
    layout->addRow("Объем накопителя:", storageSizeSpin);
    layout->addRow("Кабинет:", roomNumberEdit);
    layout->addRow("Состояние:", conditionCombo);
    layout->addRow("Дата ввода:", commissioningDateEdit);
    layout->addRow("Дата обслуживания:", lastMaintenanceDateEdit);
    layout->addRow("Гарантия до:", warrantyExpirationDateEdit);

    QDialogButtonBox* buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [this, dateRx]() {
        if (inventoryNumberEdit->text().trimmed().isEmpty() ||
            serialNumberEdit->text().trimmed().isEmpty() ||
            modelEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Инвентарный номер, серийный номер и модель обязательны");
            return;
        }
        if (storageTypeCombo->currentText().trimmed().isEmpty() ||
            conditionCombo->currentText().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Тип накопителя и состояние обязательны");
            return;
        }

        auto validateDate = [&](QLineEdit* edit, const QString& label, bool allowFuture) -> bool {
            QString text = edit->text().trimmed();
            if (text.isEmpty())
                return true;
            if (!dateRx.match(text).hasMatch()) {
                QMessageBox::warning(this, "Ошибка", "Некорректная дата (" + label + ")");
                return false;
            }
            QDate date = QDate::fromString(text, "dd.MM.yyyy");
            if (!date.isValid()) {
                QMessageBox::warning(this, "Ошибка", "Некорректная дата (" + label + ")");
                return false;
            }
            if (!allowFuture && date > QDate::currentDate()) {
                QMessageBox::warning(this, "Ошибка", "Дата в будущем (" + label + ")");
                return false;
            }
            return true;
        };

        if (!validateDate(commissioningDateEdit, "Дата ввода", false))
            return;
        if (!validateDate(lastMaintenanceDateEdit, "Дата обслуживания", false))
            return;
        if (!validateDate(warrantyExpirationDateEdit, "Гарантия до", true))
            return;

        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    baseComputer.id = 0;
}

void ComputerDialog::setComputer(const Computer& computer)
{
    baseComputer = computer;

    inventoryNumberEdit->setText(QString::fromStdString(computer.inventoryNumber));
    serialNumberEdit->setText(QString::fromStdString(computer.serialNumber));
    manufacturerEdit->setText(QString::fromStdString(computer.manufacturer));
    modelEdit->setText(QString::fromStdString(computer.model));
    cpuModelEdit->setText(QString::fromStdString(computer.cpuModel));
    chipsetEdit->setText(QString::fromStdString(computer.chipset));
    ramSizeSpin->setValue(computer.ramSize);
    auto setComboText = [](QComboBox* combo, const QString& text) {
        int idx = combo->findText(text);
        if (idx < 0) {
            combo->addItem(text);
            idx = combo->findText(text);
        }
        combo->setCurrentIndex(idx);
    };

    setComboText(storageTypeCombo, QString::fromStdString(computer.storageType));
    storageSizeSpin->setValue(computer.storageSize);
    roomNumberEdit->setText(QString::fromStdString(computer.roomNumber));
    setComboText(conditionCombo, QString::fromStdString(computer.condition));
    commissioningDateEdit->setText(QString::fromStdString(computer.commissioningDate));
    lastMaintenanceDateEdit->setText(QString::fromStdString(computer.lastMaintenanceDate));
    warrantyExpirationDateEdit->setText(QString::fromStdString(computer.warrantyExpirationDate));
}

Computer ComputerDialog::getComputer() const
{
    Computer result = baseComputer;
    result.inventoryNumber = inventoryNumberEdit->text().toStdString();
    result.serialNumber = serialNumberEdit->text().toStdString();
    result.manufacturer = manufacturerEdit->text().toStdString();
    result.model = modelEdit->text().toStdString();
    result.cpuModel = cpuModelEdit->text().toStdString();
    result.chipset = chipsetEdit->text().toStdString();
    result.ramSize = ramSizeSpin->value();
    result.storageType = storageTypeCombo->currentText().toStdString();
    result.storageSize = storageSizeSpin->value();
    result.roomNumber = roomNumberEdit->text().toStdString();
    result.condition = conditionCombo->currentText().toStdString();
    result.commissioningDate = commissioningDateEdit->text().toStdString();
    result.lastMaintenanceDate = lastMaintenanceDateEdit->text().toStdString();
    result.warrantyExpirationDate = warrantyExpirationDateEdit->text().toStdString();
    return result;
}
