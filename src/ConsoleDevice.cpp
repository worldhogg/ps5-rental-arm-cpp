#include "ConsoleDevice.h"

ConsoleDevice::ConsoleDevice()
    : m_id(0), m_pricePerDay(0.0), m_status(ConsoleStatus::Free) {}

ConsoleDevice::ConsoleDevice(int id, const QString& name, double pricePerDay)
    : m_id(id), m_name(name), m_pricePerDay(pricePerDay), m_status(ConsoleStatus::Free) {}

int ConsoleDevice::getId() const { return m_id; }
QString ConsoleDevice::getName() const { return m_name; }
double ConsoleDevice::getPricePerDay() const { return m_pricePerDay; }
ConsoleStatus ConsoleDevice::getStatus() const { return m_status; }

void ConsoleDevice::setStatus(ConsoleStatus status) { m_status = status; }

QString ConsoleDevice::getStatusText() const {
    switch (m_status) {
        case ConsoleStatus::Free:        return "Свободна";
        case ConsoleStatus::Reserved:    return "Забронирована";
        case ConsoleStatus::Rented:      return "В аренде";
        case ConsoleStatus::CheckReturn: return "Проверка возврата";
        case ConsoleStatus::Unavailable: return "Недоступна";
        default:                         return "Неизвестно";
    }
}
