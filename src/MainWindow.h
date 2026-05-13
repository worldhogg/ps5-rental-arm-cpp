#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include "RentalManager.h"
#include "ConsoleOpenGLWidget.h"
#include "ReturnBoxSimulator.h"
#include "ReturnMediaRecord.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onCalculateClicked();
    void onCreateOrderClicked();

    void onStartOrderClicked();
    void onReturnCheckClicked();
    void onCompleteOrderClicked();
    void onCancelOrderClicked();

    void onExportCsvClicked();

    // Бокс: автоматическая проверка
    void onRunBoxCheckClicked();

    // Бокс: медиафиксация
    void onAddPhotoClicked();
    void onAddVideoClicked();
    void onSaveInspectionClicked();

private:
    RentalManager m_manager;
    ReturnBoxSimulator m_boxSimulator;

    // Состояние текущей проверки (сбрасывается после завершения)
    int  m_boxCheckedOrderId;   // -1 если проверка не привязана к заказу
    bool m_boxCheckPassed;
    bool m_mediaRecordSaved;
    ReturnMediaRecord m_currentMedia;

    // --- Блок: данные клиента ---
    QLineEdit* m_editFullName;
    QLineEdit* m_editPhone;
    QLineEdit* m_editDocument;

    // --- Блок: выбор консоли ---
    QComboBox* m_comboConsole;

    // --- Блок: параметры аренды ---
    QDateEdit* m_dateStart;
    QDateEdit* m_dateEnd;
    QSpinBox*  m_spinGamepads;

    // --- Блок: игры ---
    QVector<QCheckBox*> m_gameCheckboxes;

    // --- Блок: результат ---
    QLabel* m_labelResult;

    // --- Таблица заказов ---
    QTableWidget* m_tableOrders;

    // --- OpenGL виджет ---
    ConsoleOpenGLWidget* m_glWidget;

    // --- Кнопки управления заказом ---
    QPushButton* m_btnStartOrder;
    QPushButton* m_btnReturnCheck;
    QPushButton* m_btnCompleteOrder;
    QPushButton* m_btnCancelOrder;

    // --- Блок: проверка бокса ---
    QLabel*      m_labelBoxCheckResult;

    // --- Блок: медиафиксация ---
    QLabel*      m_labelPhotoPath;
    QLabel*      m_labelVideoPath;
    QComboBox*   m_comboCondition;
    QLineEdit*   m_editAdminComment;
    QLabel*      m_labelMediaStatus;

    // --- Вспомогательные методы ---
    void setupUi();
    void refreshOrderTable();
    void refreshConsoleCombo();
    QVector<int> getSelectedGameIndices() const;
    bool validateInput(QString& errorMsg) const;

    void applyOrderAction(bool (RentalManager::*action)(int));
    void resetBoxCheckState();
};
