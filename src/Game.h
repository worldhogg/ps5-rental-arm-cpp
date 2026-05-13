#pragma once
#include <QString>

// Игра, которую можно добавить к аренде
class Game {
public:
    Game();
    Game(int id, const QString& title, double pricePerDay);

    int getId() const;
    QString getTitle() const;
    double getPricePerDay() const;

private:
    int m_id;
    QString m_title;
    double m_pricePerDay;
};
