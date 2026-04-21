# PCAccounting

Десктопное приложение на **Qt 6 (C++)** для учета сотрудников и закрепленных за ними ПК.

## Что умеет

- управление сотрудниками: добавление, редактирование, удаление;
- управление компьютерами: добавление, редактирование, удаление;
- связь сотрудник <-> ПК (назначение, отвязка, переназначение);
- фильтры и сортировка:
  - сотрудники: по институту, кафедре, статусу;
  - ПК: по RAM, объему диска, состоянию обслуживания (ТО);
- полноформатные таблицы по сотрудникам и ПК;
- поиск по ключевым полям;
- отчеты по RAM;
- сохранение/загрузка базы с шифрованием;
- валидация данных и проверка целостности перед сохранением.

## Технологии

- C++17
- Qt 6 Widgets
- CMake
- bcrypt (для криптографической части)

## Структура проекта

```text
PCAccountingQt/
  CMakeLists.txt
  README.md
  mainwindow.ui
  PCAccountingQt_ru_RU.ts
  docs/
  src/
    app/
      main.cpp
    ui/
      main_window/
      tabs/
      dialogs/
    backend/
      core/
      storage/
      crypto/
      models/
      utils/
  tests/
    backend/
  build/          # локальная сборка, не прикладывается
  projects/       # старый/архивный код, не прикладывается
  .qtcreator/     # настройки IDE, не прикладываются
  .trunk/         # служебные файлы линтера, не прикладываются
  .git/           # git-метаданные, не прикладываются
```

## Что сдавать

Для сдачи проекта достаточно оставить:

- `src/`
- `CMakeLists.txt`
- `mainwindow.ui`
- `PCAccountingQt_ru_RU.ts`
- `README.md`

По ситуации можно также приложить:

- `docs/`, если преподаватель просит UML, ТЗ, спецификации или скриншоты
- `tests/`, если преподаватель просит тесты или нужно показать проверку логики

Обычно не нужно прикладывать:

- `.git/`
- `.qtcreator/`
- `.trunk/`
- `build/`
- `projects/`
- бинарные и временные файлы: `*.exe`, `*.dll`, `*.obj`, `*.o`, `*.pdb`, `*.ilk`, `*.log`, `*.db`, `*.bin`

Если нужна "чистая" версия архива для сдачи, то оптимальный состав такой:

```text
PCAccountingQt/
  CMakeLists.txt
  README.md
  mainwindow.ui
  PCAccountingQt_ru_RU.ts
  src/
```

## Сборка (Windows, Qt + MinGW)

```bash
cmake -S . -B build
cmake --build build
```

Если у тебя несколько toolchain, проверь что используется корректный MinGW/Qt комплект.

## Запуск

После сборки исполняемый файл находится в папке `build` (конкретный путь зависит от генератора/IDE).

## UML (PlantUML)

- Актуальная диаграмма классов: `docs/uml/pcaccounting-class-diagram.puml`
- Можно открыть в VS Code (PlantUML extension) или на PlantUML server.

## Скриншоты

<img width="3253" height="1724" alt="diagram (18)" src="https://github.com/user-attachments/assets/ad3bd31c-3125-4e84-b807-dba40907e1b7" />

### Вкладка сотрудников
<img width="999" height="629" alt="image" src="https://github.com/user-attachments/assets/c8e07c8c-3765-4c90-b4d7-8811ee4e1349" />

### Вкладка компьютеров

<img width="996" height="629" alt="image" src="https://github.com/user-attachments/assets/c6c10d70-5b6b-444e-86ca-63af8303d3fe" />

### Полные таблицы

<img width="1378" height="716" alt="image" src="https://github.com/user-attachments/assets/25d71daf-b3e4-4022-8b1f-3a1faec7f5b4" />

## Автор

**Diwan1337**
