#include "mainwindow.h"
#include "../projects/src/core/ApplicationController.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>

#include <exception>

bool MainWindow::confirmDiscardChanges()
{
    if (!controller->isDirty())
        return true;

    auto result = QMessageBox::warning(
        this,
        "Несохраненные изменения",
        "Есть несохраненные изменения. Сохранить перед продолжением?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (result == QMessageBox::Save) {
        onSaveDatabase();
        return !controller->isDirty();
    }

    if (result == QMessageBox::Discard)
        return true;

    return false;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (confirmDiscardChanges())
        event->accept();
    else
        event->ignore();
}

void MainWindow::onNewDatabase()
{
    if (!confirmDiscardChanges())
        return;

    bool ok;
    QString password = QInputDialog::getText(
        this,
        "Создание БД",
        "Введите пароль:",
        QLineEdit::Password,
        "",
        &ok
    );

    if (!ok || password.isEmpty())
        return;

    controller->createNewDatabase(password.toStdString());
    QMessageBox::information(this, "Готово", "Новая база создана");
    refreshEmployees();
    refreshComputers();
    refreshStats();
}

void MainWindow::onOpenDatabase()
{
    if (!confirmDiscardChanges())
        return;

    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Открыть БД",
        "",
        "Database (*.db *.bin)"
    );

    if (filePath.isEmpty())
        return;

    bool ok;
    QString password = QInputDialog::getText(
        this,
        "Пароль",
        "Введите пароль:",
        QLineEdit::Password,
        "",
        &ok
    );

    if (!ok)
        return;

    try {
        controller->loadDatabase(
            filePath.toStdString(),
            password.toStdString()
        );
        QMessageBox::information(this, "Готово", "База загружена");
        refreshEmployees();
        refreshComputers();
        refreshStats();
    }
    catch (const std::exception& ex) {
        QMessageBox::critical(this, "Ошибка", ex.what());
    }
}

void MainWindow::onSaveDatabase()
{
    if (!controller->isLoaded()) {
        QMessageBox::warning(this, "Ошибка", "База не создана");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Сохранить БД",
        "",
        "Database (*.db *.bin)"
    );

    if (filePath.isEmpty())
        return;

    try {
        controller->saveDatabase(filePath.toStdString());
        QMessageBox::information(this, "Готово", "База сохранена");
    }
    catch (const std::exception& ex) {
        QMessageBox::critical(this, "Ошибка", ex.what());
    }
}
