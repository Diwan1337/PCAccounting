#include "mainwindow.h"

#include "EmployeesTabWidget.h"
#include "ComputersTabWidget.h"

void MainWindow::onSearchTextChanged(const QString& text)
{
    int tab = tabWidget->currentIndex();
    if (tab == 0 && employeesTab) {
        employeesTab->applyFilter(text);
    }
    else if (tab == 1 && computersTab) {
        computersTab->applyFilter(text);
    }
}
