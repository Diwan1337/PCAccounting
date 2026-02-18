#include "mainwindow.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QStatusBar>
#include <QToolBar>
#include <QMenu>

#include "EmployeesTabWidget.h"
#include "ComputersTabWidget.h"
#include "StatsTabWidget.h"

void MainWindow::setupUi()
{
    resize(1000, 600);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // Поиск
    QHBoxLayout* searchLayout = new QHBoxLayout();
    QLabel* searchLabel = new QLabel("Поиск:");
    searchEdit = new QLineEdit();

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchEdit);

    mainLayout->addLayout(searchLayout);

    // Вкладки
    tabWidget = new QTabWidget();
    mainLayout->addWidget(tabWidget);

    setupEmployeesTab();
    setupComputersTab();
    setupStatsTab();

    connect(searchEdit, &QLineEdit::textChanged,
            this, &MainWindow::onSearchTextChanged);

    connect(tabWidget, &QTabWidget::currentChanged,
            this, [this](int) {
                onSearchTextChanged(searchEdit->text());
            });

    setupMenu();

    QToolBar* toolBar = addToolBar("Main");
    toolBar->addAction(actionNew);
    toolBar->addAction(actionOpen);
    toolBar->addAction(actionSave);

    statusBar()->showMessage("Готово");

    refreshStats();
}

void MainWindow::setupMenu()
{
    QMenu* fileMenu = menuBar()->addMenu("Файл");

    actionNew = new QAction("Создать", this);
    actionOpen = new QAction("Открыть", this);
    actionSave = new QAction("Сохранить", this);

    fileMenu->addAction(actionNew);
    fileMenu->addAction(actionOpen);
    fileMenu->addAction(actionSave);

    connect(actionNew, &QAction::triggered, this, &MainWindow::onNewDatabase);
    connect(actionOpen, &QAction::triggered, this, &MainWindow::onOpenDatabase);
    connect(actionSave, &QAction::triggered, this, &MainWindow::onSaveDatabase);
}

void MainWindow::setupEmployeesTab()
{
    employeesTab = new EmployeesTabWidget(controller, this);
    tabWidget->addTab(employeesTab, "Сотрудники");

    connect(employeesTab, &EmployeesTabWidget::dataChanged, this, [this]() {
        refreshEmployees();
        refreshComputers();
        refreshStats();
    });
}

void MainWindow::setupComputersTab()
{
    computersTab = new ComputersTabWidget(controller, this);
    tabWidget->addTab(computersTab, "Компьютеры");

    connect(computersTab, &ComputersTabWidget::dataChanged, this, [this]() {
        refreshEmployees();
        refreshComputers();
        refreshStats();
    });
}

void MainWindow::setupStatsTab()
{
    statsTab = new StatsTabWidget(controller, this);
    tabWidget->addTab(statsTab, "Статистика");
}
