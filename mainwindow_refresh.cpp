#include "mainwindow.h"

#include "EmployeesTabWidget.h"
#include "ComputersTabWidget.h"

void MainWindow::refreshEmployees()
{
    if (employeesTab)
        employeesTab->refresh();
}

void MainWindow::refreshComputers()
{
    if (computersTab)
        computersTab->refresh();
}
