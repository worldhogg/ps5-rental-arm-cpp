#pragma once
#include <QString>
#include <QDate>

enum class ContractStatus {
    Signed,       // Подписан при создании заказа
    Active,       // Активен (консоль выдана)
    Closed,       // Закрыт после успешного возврата
    ManualReview  // Отправлен на ручную проверку
};

class RentalContract {
public:
    RentalContract();
    RentalContract(int id, int orderId,
                   const QString& contractNumber,
                   const QString& clientName,
                   const QDate& signedDate);

    int getId() const;
    int getOrderId() const;
    QString getContractNumber() const;
    QString getClientName() const;
    QDate getSignedDate() const;
    QDate getClosedDate() const;
    ContractStatus getStatus() const;
    QString getStatusText() const;

    void activate();
    void close();
    void sendToManualReview();

    bool isClosed() const;

private:
    int m_id;
    int m_orderId;
    QString m_contractNumber;
    QString m_clientName;
    QDate m_signedDate;
    QDate m_closedDate;
    ContractStatus m_status;
};
