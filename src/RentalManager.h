#pragma once
#include <QVector>
#include <QString>
#include "ConsoleDevice.h"
#include "Game.h"
#include "RentalOrder.h"
#include "RentalContract.h"
#include "Client.h"

class RentalManager {
public:
    RentalManager();

    QVector<ConsoleDevice>& getConsoles();
    const QVector<ConsoleDevice>& getConsoles() const;

    const QVector<Game>& getAvailableGames() const;

    const QVector<RentalOrder>& getOrders() const;

    bool createOrder(const Client& client,
                     int consoleIndex,
                     const QDate& startDate,
                     const QDate& endDate,
                     int extraGamepads,
                     const QVector<int>& selectedGameIndices);

    double calculatePrice(int consoleIndex,
                          const QDate& startDate,
                          const QDate& endDate,
                          int extraGamepads,
                          const QVector<int>& selectedGameIndices) const;

    bool startOrder(int orderId);        // Created -> Active, консоль -> Rented
    bool sendToReturnCheck(int orderId); // Active  -> CheckReturn, консоль -> CheckReturn
    bool completeOrder(int orderId);     // CheckReturn -> Completed, консоль -> Free
    bool cancelOrder(int orderId);       // Created/Active -> Canceled, консоль -> Free

    // Управление договорами
    void closeContractForOrder(int orderId);
    void sendContractToManualReview(int orderId);

    // Получить договор по ID заказа (nullptr если нет)
    const RentalContract* getContractByOrderId(int orderId) const;

    bool exportOrdersToCsv(const QString& filePath) const;

private:
    QVector<ConsoleDevice> m_consoles;
    QVector<Game> m_games;
    QVector<RentalOrder> m_orders;
    QVector<RentalContract> m_contracts;
    int m_nextOrderId;
    int m_nextContractId;

    void initConsoles();
    void initGames();

    int findOrderIndex(int orderId) const;
    int findConsoleIndex(int consoleDeviceId) const;
    int findContractIndexByOrderId(int orderId) const;

    void createContractForOrder(const RentalOrder& order);
};
