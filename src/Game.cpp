#include "Game.h"

Game::Game() : m_id(0), m_pricePerDay(0.0) {}

Game::Game(int id, const QString& title, double pricePerDay)
    : m_id(id), m_title(title), m_pricePerDay(pricePerDay) {}

int Game::getId() const { return m_id; }
QString Game::getTitle() const { return m_title; }
double Game::getPricePerDay() const { return m_pricePerDay; }
