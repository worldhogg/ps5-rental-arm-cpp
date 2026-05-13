#include "RentalManager.h"
#include <QFile>
#include <QTextStream>

RentalManager::RentalManager() : m_nextOrderId(1), m_nextContractId(1) {
    initConsoles();
    initGames();
}

void RentalManager::initConsoles() {
    m_consoles.append(ConsoleDevice(1, "PS5 Digital #1", 800.0));
    m_consoles.append(ConsoleDevice(2, "PS5 Digital #2", 800.0));
    m_consoles.append(ConsoleDevice(3, "PS5 Digital #3", 800.0));
}

void RentalManager::initGames() {
    m_games.append(Game(1, "Spider-Man 2",        200.0));
    m_games.append(Game(2, "God of War Ragnarök",  200.0));
    m_games.append(Game(3, "FIFA 25",             150.0));
    m_games.append(Game(4, "Mortal Kombat 1",     150.0));
    m_games.append(Game(5, "Gran Turismo 7",      180.0));
}

QVector<ConsoleDevice>& RentalManager::getConsoles() { return m_consoles; }
const QVector<ConsoleDevice>& RentalManager::getConsoles() const { return m_consoles; }
const QVector<Game>& RentalManager::getAvailableGames() const { return m_games; }
const QVector<RentalOrder>& RentalManager::getOrders() const { return m_orders; }

// ----------------------------------------------------------------
// Вспомогательные поиски
// ----------------------------------------------------------------

int RentalManager::findOrderIndex(int orderId) const {
    for (int i = 0; i < m_orders.size(); ++i) {
        if (m_orders[i].getOrderId() == orderId) return i;
    }
    return -1;
}

int RentalManager::findConsoleIndex(int consoleDeviceId) const {
    for (int i = 0; i < m_consoles.size(); ++i) {
        if (m_consoles[i].getId() == consoleDeviceId) return i;
    }
    return -1;
}

int RentalManager::findContractIndexByOrderId(int orderId) const {
    for (int i = 0; i < m_contracts.size(); ++i) {
        if (m_contracts[i].getOrderId() == orderId) return i;
    }
    return -1;
}

// ----------------------------------------------------------------
// Расчёт стоимости
// ----------------------------------------------------------------

double RentalManager::calculatePrice(int consoleIndex,
                                     const QDate& startDate,
                                     const QDate& endDate,
                                     int extraGamepads,
                                     const QVector<int>& selectedGameIndices) const
{
    if (consoleIndex < 0 || consoleIndex >= m_consoles.size()) return 0.0;

    QVector<Game> selectedGames;
    for (int idx : selectedGameIndices) {
        if (idx >= 0 && idx < m_games.size())
            selectedGames.append(m_games[idx]);
    }

    RentalOrder temp(0, Client(), m_consoles[consoleIndex],
                     startDate, endDate, extraGamepads, selectedGames);
    return temp.getTotalPrice();
}

// ----------------------------------------------------------------
// Создать заказ — консоль становится Reserved, создаётся договор
// ----------------------------------------------------------------

bool RentalManager::createOrder(const Client& client,
                                int consoleIndex,
                                const QDate& startDate,
                                const QDate& endDate,
                                int extraGamepads,
                                const QVector<int>& selectedGameIndices)
{
    if (consoleIndex < 0 || consoleIndex >= m_consoles.size()) return false;
    if (m_consoles[consoleIndex].getStatus() != ConsoleStatus::Free) return false;
    if (!startDate.isValid() || !endDate.isValid()) return false;
    if (startDate >= endDate) return false;

    QVector<Game> selectedGames;
    for (int idx : selectedGameIndices) {
        if (idx >= 0 && idx < m_games.size())
            selectedGames.append(m_games[idx]);
    }

    RentalOrder order(m_nextOrderId++, client, m_consoles[consoleIndex],
                      startDate, endDate, extraGamepads, selectedGames);
    m_orders.append(order);

    m_consoles[consoleIndex].setStatus(ConsoleStatus::Reserved);

    createContractForOrder(m_orders.last());

    return true;
}

void RentalManager::createContractForOrder(const RentalOrder& order) {
    QString number = QString("C-%1").arg(m_nextContractId, 4, 10, QChar('0'));
    RentalContract contract(m_nextContractId++,
                            order.getOrderId(),
                            number,
                            order.getClient().getFullName(),
                            QDate::currentDate());
    m_contracts.append(contract);
}

// ----------------------------------------------------------------
// Жизненный цикл заказа
// ----------------------------------------------------------------

bool RentalManager::startOrder(int orderId) {
    int oi = findOrderIndex(orderId);
    if (oi < 0) return false;
    if (m_orders[oi].getRentalStatus() != RentalStatus::Created) return false;

    m_orders[oi].setRentalStatus(RentalStatus::Active);

    int ci = findConsoleIndex(m_orders[oi].getConsole().getId());
    if (ci >= 0) m_consoles[ci].setStatus(ConsoleStatus::Rented);

    int ci2 = findContractIndexByOrderId(orderId);
    if (ci2 >= 0) m_contracts[ci2].activate();

    return true;
}

bool RentalManager::sendToReturnCheck(int orderId) {
    int oi = findOrderIndex(orderId);
    if (oi < 0) return false;
    if (m_orders[oi].getRentalStatus() != RentalStatus::Active) return false;

    m_orders[oi].setRentalStatus(RentalStatus::CheckReturn);

    int ci = findConsoleIndex(m_orders[oi].getConsole().getId());
    if (ci >= 0) m_consoles[ci].setStatus(ConsoleStatus::CheckReturn);

    return true;
}

bool RentalManager::completeOrder(int orderId) {
    int oi = findOrderIndex(orderId);
    if (oi < 0) return false;
    if (m_orders[oi].getRentalStatus() != RentalStatus::CheckReturn) return false;

    m_orders[oi].setRentalStatus(RentalStatus::Completed);

    int ci = findConsoleIndex(m_orders[oi].getConsole().getId());
    if (ci >= 0) m_consoles[ci].setStatus(ConsoleStatus::Free);

    return true;
}

bool RentalManager::cancelOrder(int orderId) {
    int oi = findOrderIndex(orderId);
    if (oi < 0) return false;

    RentalStatus s = m_orders[oi].getRentalStatus();
    if (s != RentalStatus::Created && s != RentalStatus::Active) return false;

    m_orders[oi].setRentalStatus(RentalStatus::Canceled);

    int ci = findConsoleIndex(m_orders[oi].getConsole().getId());
    if (ci >= 0) m_consoles[ci].setStatus(ConsoleStatus::Free);

    return true;
}

// ----------------------------------------------------------------
// Управление договорами
// ----------------------------------------------------------------

void RentalManager::closeContractForOrder(int orderId) {
    int ci = findContractIndexByOrderId(orderId);
    if (ci >= 0) m_contracts[ci].close();
}

void RentalManager::sendContractToManualReview(int orderId) {
    int ci = findContractIndexByOrderId(orderId);
    if (ci >= 0) m_contracts[ci].sendToManualReview();
}

const RentalContract* RentalManager::getContractByOrderId(int orderId) const {
    int ci = findContractIndexByOrderId(orderId);
    if (ci >= 0) return &m_contracts[ci];
    return nullptr;
}

// ----------------------------------------------------------------
// Экспорт в CSV
// ----------------------------------------------------------------

bool RentalManager::exportOrdersToCsv(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "ID;Клиент;Телефон;Консоль;Дата начала;Дата окончания;"
           "Сумма;Статус;Номер договора;Статус договора\n";

    for (const RentalOrder& order : m_orders) {
        QString contractNum;
        QString contractStatus;
        const RentalContract* c = getContractByOrderId(order.getOrderId());
        if (c) {
            contractNum    = c->getContractNumber();
            contractStatus = c->getStatusText();
        }

        out << order.getOrderId() << ";"
            << order.getClient().getFullName() << ";"
            << order.getClient().getPhone() << ";"
            << order.getConsole().getName() << ";"
            << order.getStartDate().toString("dd.MM.yyyy") << ";"
            << order.getEndDate().toString("dd.MM.yyyy") << ";"
            << QString::number(order.getTotalPrice(), 'f', 2) << ";"
            << order.getRentalStatusText() << ";"
            << contractNum << ";"
            << contractStatus << "\n";
    }

    file.close();
    return true;
}
