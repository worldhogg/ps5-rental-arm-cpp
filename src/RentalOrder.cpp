#include "RentalOrder.h"

RentalOrder::RentalOrder()
    : m_orderId(0), m_extraGamepads(0), m_rentalStatus(RentalStatus::Created) {}

RentalOrder::RentalOrder(int orderId,
                         const Client& client,
                         const ConsoleDevice& console,
                         const QDate& startDate,
                         const QDate& endDate,
                         int extraGamepads,
                         const QVector<Game>& selectedGames)
    : m_orderId(orderId),
      m_client(client),
      m_console(console),
      m_startDate(startDate),
      m_endDate(endDate),
      m_extraGamepads(extraGamepads),
      m_selectedGames(selectedGames),
      m_rentalStatus(RentalStatus::Created)
{}

int RentalOrder::getOrderId() const { return m_orderId; }
Client RentalOrder::getClient() const { return m_client; }
ConsoleDevice RentalOrder::getConsole() const { return m_console; }
QDate RentalOrder::getStartDate() const { return m_startDate; }
QDate RentalOrder::getEndDate() const { return m_endDate; }
int RentalOrder::getExtraGamepads() const { return m_extraGamepads; }
QVector<Game> RentalOrder::getSelectedGames() const { return m_selectedGames; }

RentalStatus RentalOrder::getRentalStatus() const { return m_rentalStatus; }

void RentalOrder::setRentalStatus(RentalStatus status) {
    m_rentalStatus = status;
}

QString RentalOrder::getRentalStatusText() const {
    switch (m_rentalStatus) {
        case RentalStatus::Created:     return "Создан";
        case RentalStatus::Active:      return "Активен";
        case RentalStatus::CheckReturn: return "Проверка возврата";
        case RentalStatus::Completed:   return "Завершён";
        case RentalStatus::Canceled:    return "Отменён";
        default:                        return "Неизвестно";
    }
}

int RentalOrder::getDaysCount() const {
    int days = m_startDate.daysTo(m_endDate);
    return days > 0 ? days : 1;
}

double RentalOrder::gamepadPricePerDay() {
    return 150.0;
}

double RentalOrder::getTotalPrice() const {
    int days = getDaysCount();

    double consoleTotal = m_console.getPricePerDay() * days;

    double gamesTotal = 0.0;
    for (const Game& game : m_selectedGames) {
        gamesTotal += game.getPricePerDay() * days;
    }

    double gamepadsTotal = m_extraGamepads * gamepadPricePerDay() * days;

    return consoleTotal + gamesTotal + gamepadsTotal;
}
