#pragma once
#include <QString>
#include <QDate>
#include <QVector>
#include "Client.h"
#include "ConsoleDevice.h"
#include "Game.h"

// Статусы заказа аренды
enum class RentalStatus {
    Created,     // Создан (ожидает начала)
    Active,      // Активен (консоль выдана клиенту)
    CheckReturn, // Ожидает проверки возврата
    Completed,   // Завершён
    Canceled     // Отменён
};

// Заказ аренды — объединяет клиента, консоль, даты и опции
class RentalOrder {
public:
    RentalOrder();
    RentalOrder(int orderId,
                const Client& client,
                const ConsoleDevice& console,
                const QDate& startDate,
                const QDate& endDate,
                int extraGamepads,
                const QVector<Game>& selectedGames);

    int getOrderId() const;
    Client getClient() const;
    ConsoleDevice getConsole() const;
    QDate getStartDate() const;
    QDate getEndDate() const;
    int getExtraGamepads() const;
    QVector<Game> getSelectedGames() const;

    // Статус заказа
    RentalStatus getRentalStatus() const;
    void setRentalStatus(RentalStatus status);
    QString getRentalStatusText() const;

    // Количество дней аренды
    int getDaysCount() const;

    // Итоговая сумма
    double getTotalPrice() const;

    // Цена геймпада в день
    static double gamepadPricePerDay();

private:
    int m_orderId;
    Client m_client;
    ConsoleDevice m_console;
    QDate m_startDate;
    QDate m_endDate;
    int m_extraGamepads;
    QVector<Game> m_selectedGames;
    RentalStatus m_rentalStatus;
};
