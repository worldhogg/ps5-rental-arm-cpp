#pragma once
#include <QString>

// Статусы консоли
enum class ConsoleStatus {
    Free,           // Свободна (зелёный)
    Reserved,       // Забронирована (синий)
    Rented,         // В аренде (оранжевый)
    CheckReturn,    // Требуется проверка возврата (красный)
    Unavailable     // Недоступна (серый)
};

// Консоль PS5 — устройство для аренды
class ConsoleDevice {
public:
    ConsoleDevice();
    ConsoleDevice(int id, const QString& name, double pricePerDay);

    int getId() const;
    QString getName() const;
    double getPricePerDay() const;
    ConsoleStatus getStatus() const;

    void setStatus(ConsoleStatus status);

    // Текстовое описание статуса для отображения в таблице
    QString getStatusText() const;

private:
    int m_id;
    QString m_name;
    double m_pricePerDay;
    ConsoleStatus m_status;
};
