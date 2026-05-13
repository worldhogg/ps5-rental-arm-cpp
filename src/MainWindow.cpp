#include "MainWindow.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QDate>
#include <QSplitter>
#include <QStandardItemModel>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_boxCheckedOrderId(-1),
      m_boxCheckPassed(false),
      m_mediaRecordSaved(false)
{
    setWindowTitle("АРМ специалиста сервиса аренды PS5");
    setMinimumSize(1200, 780);
    setupUi();
}

void MainWindow::setupUi() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // ============================================================
    // ЛЕВАЯ ПАНЕЛЬ — формы ввода
    // ============================================================
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(8);

    QGroupBox* groupClient = new QGroupBox("Данные клиента");
    QFormLayout* formClient = new QFormLayout(groupClient);

    m_editFullName = new QLineEdit();
    m_editFullName->setPlaceholderText("Иванов Иван Иванович");
    formClient->addRow("ФИО:", m_editFullName);

    m_editPhone = new QLineEdit();
    m_editPhone->setPlaceholderText("+7 (999) 000-00-00");
    formClient->addRow("Телефон:", m_editPhone);

    m_editDocument = new QLineEdit();
    m_editDocument->setPlaceholderText("Серия и номер паспорта");
    formClient->addRow("Документ:", m_editDocument);

    leftLayout->addWidget(groupClient);

    QGroupBox* groupConsole = new QGroupBox("Выбор консоли");
    QFormLayout* formConsole = new QFormLayout(groupConsole);

    m_comboConsole = new QComboBox();
    refreshConsoleCombo();
    formConsole->addRow("Консоль:", m_comboConsole);

    leftLayout->addWidget(groupConsole);

    QGroupBox* groupRental = new QGroupBox("Параметры аренды");
    QFormLayout* formRental = new QFormLayout(groupRental);

    m_dateStart = new QDateEdit(QDate::currentDate());
    m_dateStart->setCalendarPopup(true);
    m_dateStart->setDisplayFormat("dd.MM.yyyy");
    formRental->addRow("Дата начала:", m_dateStart);

    m_dateEnd = new QDateEdit(QDate::currentDate().addDays(1));
    m_dateEnd->setCalendarPopup(true);
    m_dateEnd->setDisplayFormat("dd.MM.yyyy");
    formRental->addRow("Дата окончания:", m_dateEnd);

    m_spinGamepads = new QSpinBox();
    m_spinGamepads->setRange(0, 3);
    m_spinGamepads->setValue(0);
    m_spinGamepads->setSuffix(" шт.");
    formRental->addRow("Доп. геймпады:", m_spinGamepads);

    leftLayout->addWidget(groupRental);

    QGroupBox* groupGames = new QGroupBox("Дополнительные игры (150–200 ₽/день)");
    QVBoxLayout* gamesLayout = new QVBoxLayout(groupGames);

    const QVector<Game>& games = m_manager.getAvailableGames();
    for (const Game& game : games) {
        QString label = QString("%1 (%2 ₽/день)")
                        .arg(game.getTitle())
                        .arg((int)game.getPricePerDay());
        QCheckBox* cb = new QCheckBox(label);
        m_gameCheckboxes.append(cb);
        gamesLayout->addWidget(cb);
    }

    leftLayout->addWidget(groupGames);

    QPushButton* btnCalc = new QPushButton("Рассчитать стоимость");
    btnCalc->setMinimumHeight(36);
    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::onCalculateClicked);
    leftLayout->addWidget(btnCalc);

    QGroupBox* groupResult = new QGroupBox("Итоговая стоимость");
    QVBoxLayout* resultLayout = new QVBoxLayout(groupResult);
    m_labelResult = new QLabel("Нажмите «Рассчитать»");
    m_labelResult->setAlignment(Qt::AlignCenter);
    m_labelResult->setStyleSheet("font-size: 16px; font-weight: bold; color: #2e7d32;");
    resultLayout->addWidget(m_labelResult);
    leftLayout->addWidget(groupResult);

    QPushButton* btnCreate = new QPushButton("Создать аренду");
    btnCreate->setMinimumHeight(40);
    btnCreate->setStyleSheet("background-color: #1565c0; color: white; font-weight: bold; font-size: 14px;");
    connect(btnCreate, &QPushButton::clicked, this, &MainWindow::onCreateOrderClicked);
    leftLayout->addWidget(btnCreate);

    leftLayout->addStretch();

    // ============================================================
    // ПРАВАЯ ПАНЕЛЬ — OpenGL + таблица + бокс
    // ============================================================
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setSpacing(8);

    // --- OpenGL ---
    QGroupBox* groupGL = new QGroupBox("Статус консолей (OpenGL)");
    QVBoxLayout* glLayout = new QVBoxLayout(groupGL);

    m_glWidget = new ConsoleOpenGLWidget();
    m_glWidget->setMinimumHeight(170);
    m_glWidget->updateConsoles(m_manager.getConsoles());
    glLayout->addWidget(m_glWidget);

    QHBoxLayout* legendLayout = new QHBoxLayout();
    auto addLegend = [&](const QString& text, const QString& color) {
        QLabel* lbl = new QLabel(text);
        lbl->setStyleSheet(
            QString("background-color:%1;color:white;padding:2px 6px;"
                    "border-radius:3px;font-size:11px;").arg(color));
        legendLayout->addWidget(lbl);
    };
    addLegend("Свободна",          "#2e7d32");
    addLegend("Забронирована",     "#1565c0");
    addLegend("В аренде",          "#e65100");
    addLegend("Проверка возврата", "#b71c1c");
    addLegend("Недоступна",        "#424242");
    legendLayout->addStretch();
    glLayout->addLayout(legendLayout);

    rightLayout->addWidget(groupGL);

    // --- Таблица заказов ---
    QGroupBox* groupOrders = new QGroupBox("Журнал заказов");
    QVBoxLayout* ordersLayout = new QVBoxLayout(groupOrders);

    // 9 колонок: добавлена «Договор»
    m_tableOrders = new QTableWidget(0, 9);
    m_tableOrders->setHorizontalHeaderLabels({
        "№", "Клиент", "Консоль", "Начало", "Конец", "Дней", "Сумма", "Статус", "Договор"
    });
    m_tableOrders->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableOrders->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableOrders->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableOrders->setAlternatingRowColors(true);
    ordersLayout->addWidget(m_tableOrders);

    // --- Кнопки управления заказом ---
    QGroupBox* groupActions = new QGroupBox("Управление выбранным заказом");
    QHBoxLayout* actionsLayout = new QHBoxLayout(groupActions);

    m_btnStartOrder = new QPushButton("Начать аренду");
    m_btnStartOrder->setToolTip("Заказ переходит в статус «Активен» (консоль выдана клиенту)");
    m_btnStartOrder->setStyleSheet("background-color:#1b5e20;color:white;font-weight:bold;");
    connect(m_btnStartOrder, &QPushButton::clicked, this, &MainWindow::onStartOrderClicked);

    m_btnReturnCheck = new QPushButton("Проверка возврата");
    m_btnReturnCheck->setToolTip("Клиент вернул консоль — отправить на проверку");
    m_btnReturnCheck->setStyleSheet("background-color:#bf360c;color:white;font-weight:bold;");
    connect(m_btnReturnCheck, &QPushButton::clicked, this, &MainWindow::onReturnCheckClicked);

    m_btnCompleteOrder = new QPushButton("Завершить");
    m_btnCompleteOrder->setToolTip("Завершить после успешной проверки бокса и осмотра");
    m_btnCompleteOrder->setStyleSheet("background-color:#37474f;color:white;font-weight:bold;");
    connect(m_btnCompleteOrder, &QPushButton::clicked, this, &MainWindow::onCompleteOrderClicked);

    m_btnCancelOrder = new QPushButton("Отменить заказ");
    m_btnCancelOrder->setToolTip("Отменить созданный или активный заказ");
    m_btnCancelOrder->setStyleSheet("background-color:#880e4f;color:white;font-weight:bold;");
    connect(m_btnCancelOrder, &QPushButton::clicked, this, &MainWindow::onCancelOrderClicked);

    QPushButton* btnExportCsv = new QPushButton("Экспорт в CSV");
    btnExportCsv->setToolTip("Сохранить журнал заказов в файл CSV");
    connect(btnExportCsv, &QPushButton::clicked, this, &MainWindow::onExportCsvClicked);

    actionsLayout->addWidget(m_btnStartOrder);
    actionsLayout->addWidget(m_btnReturnCheck);
    actionsLayout->addWidget(m_btnCompleteOrder);
    actionsLayout->addWidget(m_btnCancelOrder);
    actionsLayout->addStretch();
    actionsLayout->addWidget(btnExportCsv);

    ordersLayout->addWidget(groupActions);
    rightLayout->addWidget(groupOrders, 1);

    // ============================================================
    // БЛОК: Проверка возврата через бокс
    // ============================================================
    QGroupBox* groupBox = new QGroupBox("Проверка возврата через бокс");
    QHBoxLayout* boxMainLayout = new QHBoxLayout(groupBox);

    // Часть 1: автоматическая техническая проверка
    QGroupBox* groupAutoCheck = new QGroupBox("Автоматическая проверка");
    QVBoxLayout* autoCheckLayout = new QVBoxLayout(groupAutoCheck);

    QPushButton* btnRunCheck = new QPushButton("Запустить проверку бокса");
    btnRunCheck->setStyleSheet("background-color:#4527a0;color:white;font-weight:bold;");
    connect(btnRunCheck, &QPushButton::clicked, this, &MainWindow::onRunBoxCheckClicked);
    autoCheckLayout->addWidget(btnRunCheck);

    m_labelBoxCheckResult = new QLabel("Проверка не запущена");
    m_labelBoxCheckResult->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_labelBoxCheckResult->setStyleSheet("font-family:monospace;font-size:12px;color:#37474f;");
    m_labelBoxCheckResult->setWordWrap(true);
    autoCheckLayout->addWidget(m_labelBoxCheckResult);
    autoCheckLayout->addStretch();

    boxMainLayout->addWidget(groupAutoCheck);

    // Часть 2: фото- и видеофиксация
    QGroupBox* groupMedia = new QGroupBox("Фото- и видеофиксация");
    QVBoxLayout* mediaLayout = new QVBoxLayout(groupMedia);

    // Фото
    QHBoxLayout* photoRow = new QHBoxLayout();
    QPushButton* btnPhoto = new QPushButton("Добавить фото");
    btnPhoto->setStyleSheet("background-color:#00695c;color:white;font-weight:bold;");
    connect(btnPhoto, &QPushButton::clicked, this, &MainWindow::onAddPhotoClicked);
    m_labelPhotoPath = new QLabel("Фото не добавлено");
    m_labelPhotoPath->setStyleSheet("color:#78909c;font-size:11px;");
    m_labelPhotoPath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    photoRow->addWidget(btnPhoto);
    photoRow->addWidget(m_labelPhotoPath);
    mediaLayout->addLayout(photoRow);

    // Видео
    QHBoxLayout* videoRow = new QHBoxLayout();
    QPushButton* btnVideo = new QPushButton("Добавить видео");
    btnVideo->setStyleSheet("background-color:#00695c;color:white;font-weight:bold;");
    connect(btnVideo, &QPushButton::clicked, this, &MainWindow::onAddVideoClicked);
    m_labelVideoPath = new QLabel("Видео не добавлено");
    m_labelVideoPath->setStyleSheet("color:#78909c;font-size:11px;");
    m_labelVideoPath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    videoRow->addWidget(btnVideo);
    videoRow->addWidget(m_labelVideoPath);
    mediaLayout->addLayout(videoRow);

    // Внешнее состояние
    QFormLayout* mediaForm = new QFormLayout();
    m_comboCondition = new QComboBox();
    m_comboCondition->addItems({
        "Без повреждений",
        "Есть следы использования",
        "Требуется ручная проверка"
    });
    mediaForm->addRow("Внешнее состояние:", m_comboCondition);

    m_editAdminComment = new QLineEdit();
    m_editAdminComment->setPlaceholderText("Комментарий администратора");
    mediaForm->addRow("Комментарий:", m_editAdminComment);
    mediaLayout->addLayout(mediaForm);

    QPushButton* btnSaveInspection = new QPushButton("Сохранить осмотр");
    btnSaveInspection->setStyleSheet("background-color:#e65100;color:white;font-weight:bold;");
    connect(btnSaveInspection, &QPushButton::clicked, this, &MainWindow::onSaveInspectionClicked);
    mediaLayout->addWidget(btnSaveInspection);

    m_labelMediaStatus = new QLabel("");
    m_labelMediaStatus->setStyleSheet("color:#2e7d32;font-size:11px;font-weight:bold;");
    mediaLayout->addWidget(m_labelMediaStatus);

    mediaLayout->addStretch();
    boxMainLayout->addWidget(groupMedia);

    rightLayout->addWidget(groupBox);

    // ============================================================
    // Сплиттер
    // ============================================================
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    mainLayout->addWidget(splitter);
}

// ================================================================
// Рассчитать стоимость
// ================================================================
void MainWindow::onCalculateClicked() {
    QString errMsg;
    if (!validateInput(errMsg)) {
        m_labelResult->setText(errMsg);
        m_labelResult->setStyleSheet("font-size:13px;color:#c62828;");
        return;
    }

    int consoleIdx = m_comboConsole->currentIndex();
    QDate start    = m_dateStart->date();
    QDate end      = m_dateEnd->date();
    int gamepads   = m_spinGamepads->value();
    QVector<int> gameIndices = getSelectedGameIndices();

    double total = m_manager.calculatePrice(consoleIdx, start, end, gamepads, gameIndices);
    int days     = start.daysTo(end);
    if (days < 1) days = 1;

    m_labelResult->setText(
        QString("Итого: %1 ₽  (%2 дней)").arg(total, 0, 'f', 0).arg(days));
    m_labelResult->setStyleSheet("font-size:16px;font-weight:bold;color:#1b5e20;");
}

// ================================================================
// Создать заказ
// ================================================================
void MainWindow::onCreateOrderClicked() {
    QString errMsg;
    if (!validateInput(errMsg)) {
        QMessageBox::warning(this, "Ошибка ввода", errMsg);
        return;
    }

    Client client(m_editFullName->text().trimmed(),
                  m_editPhone->text().trimmed(),
                  m_editDocument->text().trimmed());

    int consoleIdx = m_comboConsole->currentIndex();
    QDate start    = m_dateStart->date();
    QDate end      = m_dateEnd->date();
    int gamepads   = m_spinGamepads->value();
    QVector<int> gameIndices = getSelectedGameIndices();

    bool ok = m_manager.createOrder(client, consoleIdx, start, end, gamepads, gameIndices);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка",
            "Не удалось создать заказ.\nКонсоль недоступна или неверные даты.");
        return;
    }

    refreshOrderTable();
    m_glWidget->updateConsoles(m_manager.getConsoles());
    refreshConsoleCombo();

    const RentalOrder& order = m_manager.getOrders().last();
    const RentalContract* contract = m_manager.getContractByOrderId(order.getOrderId());
    QString contractInfo = contract
        ? QString("\nДоговор: %1").arg(contract->getContractNumber())
        : "";

    QMessageBox::information(this, "Готово",
        QString("Аренда создана!\nЗаказ № %1\nСумма: %2 ₽\nСтатус: %3%4")
        .arg(order.getOrderId())
        .arg(order.getTotalPrice(), 0, 'f', 0)
        .arg(order.getRentalStatusText())
        .arg(contractInfo));
}

// ================================================================
// Управление заказами — вспомогательный метод
// ================================================================
void MainWindow::applyOrderAction(bool (RentalManager::*action)(int)) {
    int row = m_tableOrders->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Выберите заказ",
            "Пожалуйста, выберите строку в таблице заказов.");
        return;
    }

    bool ok;
    int orderId = m_tableOrders->item(row, 0)->text().toInt(&ok);
    if (!ok) return;

    bool result = (m_manager.*action)(orderId);
    if (!result) {
        QMessageBox::warning(this, "Невозможное действие",
            "Данное действие недоступно для выбранного заказа.\n"
            "Проверьте текущий статус заказа.");
        return;
    }

    refreshOrderTable();
    m_glWidget->updateConsoles(m_manager.getConsoles());
    refreshConsoleCombo();
}

void MainWindow::onStartOrderClicked() {
    applyOrderAction(&RentalManager::startOrder);
}

void MainWindow::onReturnCheckClicked() {
    applyOrderAction(&RentalManager::sendToReturnCheck);
}

// ================================================================
// Завершить аренду — теперь с проверкой условий
// ================================================================
void MainWindow::onCompleteOrderClicked() {
    int row = m_tableOrders->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Выберите заказ",
            "Пожалуйста, выберите строку в таблице заказов.");
        return;
    }

    bool ok;
    int orderId = m_tableOrders->item(row, 0)->text().toInt(&ok);
    if (!ok) return;

    // Ищем заказ в коллекции
    const auto& orders = m_manager.getOrders();
    int oi = -1;
    for (int i = 0; i < orders.size(); ++i) {
        if (orders[i].getOrderId() == orderId) { oi = i; break; }
    }
    if (oi < 0) return;

    if (orders[oi].getRentalStatus() != RentalStatus::CheckReturn) {
        QMessageBox::warning(this, "Невозможное действие",
            "Завершить аренду можно только для заказа\n"
            "в статусе «Проверка возврата».");
        return;
    }

    // Проверка: бокс запускался для этого заказа
    if (m_boxCheckedOrderId != orderId || !m_boxCheckPassed) {
        QMessageBox::warning(this, "Проверка бокса не пройдена",
            "Сначала запустите автоматическую проверку бокса\n"
            "для выбранного заказа и убедитесь, что она пройдена.");
        return;
    }

    // Проверка: осмотр сохранён с медиафайлами
    if (!m_mediaRecordSaved || !m_currentMedia.isComplete()) {
        QMessageBox::warning(this, "Осмотр не сохранён",
            "Добавьте фото или видео и нажмите «Сохранить осмотр».");
        return;
    }

    // Если внешнее состояние требует ручной проверки
    if (m_currentMedia.needsManualReview()) {
        m_manager.sendContractToManualReview(orderId);
        refreshOrderTable();
        QMessageBox::information(this, "Ручная проверка",
            "Возврат отправлен на ручную проверку.\n"
            "Договор остаётся действующим.\n\n"
            "Специалист должен просмотреть фото/видео и\n"
            "принять решение вручную.");
        return;
    }

    // Всё в порядке — завершаем
    bool result = m_manager.completeOrder(orderId);
    if (!result) {
        QMessageBox::warning(this, "Ошибка", "Не удалось завершить заказ.");
        return;
    }
    m_manager.closeContractForOrder(orderId);

    resetBoxCheckState();
    refreshOrderTable();
    m_glWidget->updateConsoles(m_manager.getConsoles());
    refreshConsoleCombo();

    QMessageBox::information(this, "Готово",
        "Возврат принят. Договор закрыт.");
}

void MainWindow::onCancelOrderClicked() {
    applyOrderAction(&RentalManager::cancelOrder);
}

// ================================================================
// Экспорт в CSV
// ================================================================
void MainWindow::onExportCsvClicked() {
    if (m_manager.getOrders().isEmpty()) {
        QMessageBox::information(this, "Экспорт CSV", "Нет заказов для экспорта.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this, "Сохранить журнал заказов", "orders.csv",
        "CSV файлы (*.csv);;Все файлы (*)");

    if (filePath.isEmpty()) return;

    bool ok = m_manager.exportOrdersToCsv(filePath);
    if (ok) {
        QMessageBox::information(this, "Экспорт CSV",
            QString("Файл сохранён:\n%1").arg(filePath));
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл.");
    }
}

// ================================================================
// Проверка бокса
// ================================================================
void MainWindow::onRunBoxCheckClicked() {
    int row = m_tableOrders->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Выберите заказ",
            "Выберите заказ в таблице перед запуском проверки бокса.");
        return;
    }

    bool ok;
    int orderId = m_tableOrders->item(row, 0)->text().toInt(&ok);
    if (!ok) return;

    // Ищем заказ
    const auto& orders = m_manager.getOrders();
    int oi = -1;
    for (int i = 0; i < orders.size(); ++i) {
        if (orders[i].getOrderId() == orderId) { oi = i; break; }
    }
    if (oi < 0) return;

    if (orders[oi].getRentalStatus() != RentalStatus::CheckReturn) {
        QMessageBox::warning(this, "Недоступно",
            "Проверку бокса можно запустить только для заказа\n"
            "в статусе «Проверка возврата».");
        return;
    }

    const ConsoleDevice& console = orders[oi].getConsole();
    int extraGamepads = orders[oi].getExtraGamepads();

    ReturnCheckResult result = m_boxSimulator.runFullCheck(console, extraGamepads);

    m_boxCheckedOrderId = orderId;
    m_boxCheckPassed    = result.isPassed();

    QString resultText = result.getSummary();
    m_labelBoxCheckResult->setText(resultText);

    if (result.isPassed()) {
        m_labelBoxCheckResult->setStyleSheet(
            "font-family:monospace;font-size:12px;color:#1b5e20;");
    } else {
        m_labelBoxCheckResult->setStyleSheet(
            "font-family:monospace;font-size:12px;color:#b71c1c;");
    }
}

// ================================================================
// Медиафиксация
// ================================================================
void MainWindow::onAddPhotoClicked() {
    QString path = QFileDialog::getOpenFileName(
        this, "Выбрать фото", "",
        "Изображения (*.png *.jpg *.jpeg);;Все файлы (*)");
    if (path.isEmpty()) return;

    m_currentMedia.setPhotoPath(path);
    m_labelPhotoPath->setText(path.section('/', -1));
    m_labelPhotoPath->setStyleSheet("color:#37474f;font-size:11px;");
    m_mediaRecordSaved = false;
    m_labelMediaStatus->setText("");
}

void MainWindow::onAddVideoClicked() {
    QString path = QFileDialog::getOpenFileName(
        this, "Выбрать видео", "",
        "Видеофайлы (*.mp4 *.mov *.avi);;Все файлы (*)");
    if (path.isEmpty()) return;

    m_currentMedia.setVideoPath(path);
    m_labelVideoPath->setText(path.section('/', -1));
    m_labelVideoPath->setStyleSheet("color:#37474f;font-size:11px;");
    m_mediaRecordSaved = false;
    m_labelMediaStatus->setText("");
}

void MainWindow::onSaveInspectionClicked() {
    if (!m_currentMedia.isComplete()) {
        QMessageBox::warning(this, "Нет медиафайлов",
            "Добавьте хотя бы одно фото или видео.");
        return;
    }

    m_currentMedia.setExternalCondition(m_comboCondition->currentText());
    m_currentMedia.setAdminComment(m_editAdminComment->text().trimmed());
    m_mediaRecordSaved = true;

    m_labelMediaStatus->setText(
        QString("Осмотр сохранён. Состояние: %1")
        .arg(m_currentMedia.getExternalCondition()));
}

// ================================================================
// Вспомогательные методы
// ================================================================

void MainWindow::resetBoxCheckState() {
    m_boxCheckedOrderId = -1;
    m_boxCheckPassed    = false;
    m_mediaRecordSaved  = false;
    m_currentMedia      = ReturnMediaRecord();
    m_labelBoxCheckResult->setText("Проверка не запущена");
    m_labelBoxCheckResult->setStyleSheet(
        "font-family:monospace;font-size:12px;color:#37474f;");
    m_labelPhotoPath->setText("Фото не добавлено");
    m_labelPhotoPath->setStyleSheet("color:#78909c;font-size:11px;");
    m_labelVideoPath->setText("Видео не добавлено");
    m_labelVideoPath->setStyleSheet("color:#78909c;font-size:11px;");
    m_labelMediaStatus->setText("");
    m_comboCondition->setCurrentIndex(0);
    m_editAdminComment->clear();
}

void MainWindow::refreshOrderTable() {
    m_tableOrders->setRowCount(0);
    const QVector<RentalOrder>& orders = m_manager.getOrders();
    for (const RentalOrder& order : orders) {
        int row = m_tableOrders->rowCount();
        m_tableOrders->insertRow(row);

        m_tableOrders->setItem(row, 0, new QTableWidgetItem(
            QString::number(order.getOrderId())));
        m_tableOrders->setItem(row, 1, new QTableWidgetItem(
            order.getClient().getFullName()));
        m_tableOrders->setItem(row, 2, new QTableWidgetItem(
            order.getConsole().getName()));
        m_tableOrders->setItem(row, 3, new QTableWidgetItem(
            order.getStartDate().toString("dd.MM.yyyy")));
        m_tableOrders->setItem(row, 4, new QTableWidgetItem(
            order.getEndDate().toString("dd.MM.yyyy")));
        m_tableOrders->setItem(row, 5, new QTableWidgetItem(
            QString::number(order.getDaysCount())));
        m_tableOrders->setItem(row, 6, new QTableWidgetItem(
            QString("%1 ₽").arg(order.getTotalPrice(), 0, 'f', 0)));
        m_tableOrders->setItem(row, 7, new QTableWidgetItem(
            order.getRentalStatusText()));

        // Договор: номер + статус
        QString contractCell;
        const RentalContract* contract =
            m_manager.getContractByOrderId(order.getOrderId());
        if (contract) {
            contractCell = QString("%1 / %2")
                .arg(contract->getContractNumber())
                .arg(contract->getStatusText());
        }
        m_tableOrders->setItem(row, 8, new QTableWidgetItem(contractCell));
    }
}

void MainWindow::refreshConsoleCombo() {
    m_comboConsole->clear();
    const QVector<ConsoleDevice>& consoles = m_manager.getConsoles();
    for (const ConsoleDevice& c : consoles) {
        QString entry = QString("%1  [%2]  —  %3 ₽/день")
                        .arg(c.getName())
                        .arg(c.getStatusText())
                        .arg(c.getPricePerDay(), 0, 'f', 0);
        m_comboConsole->addItem(entry);

        if (c.getStatus() != ConsoleStatus::Free) {
            int idx = m_comboConsole->count() - 1;
            QStandardItemModel* model =
                qobject_cast<QStandardItemModel*>(m_comboConsole->model());
            if (model) {
                QStandardItem* item = model->item(idx);
                if (item) {
                    item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
                    item->setForeground(Qt::gray);
                }
            }
        }
    }
}

QVector<int> MainWindow::getSelectedGameIndices() const {
    QVector<int> indices;
    for (int i = 0; i < m_gameCheckboxes.size(); ++i) {
        if (m_gameCheckboxes[i]->isChecked())
            indices.append(i);
    }
    return indices;
}

bool MainWindow::validateInput(QString& errorMsg) const {
    if (m_editFullName->text().trimmed().isEmpty()) {
        errorMsg = "Введите ФИО клиента";
        return false;
    }
    if (m_editPhone->text().trimmed().isEmpty()) {
        errorMsg = "Введите телефон";
        return false;
    }
    if (m_editDocument->text().trimmed().isEmpty()) {
        errorMsg = "Введите номер документа";
        return false;
    }
    if (m_dateEnd->date() <= m_dateStart->date()) {
        errorMsg = "Дата окончания должна быть позже даты начала";
        return false;
    }
    return true;
}
