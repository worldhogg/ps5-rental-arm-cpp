#include "RentalContract.h"

RentalContract::RentalContract()
    : m_id(0), m_orderId(0), m_status(ContractStatus::Signed)
{}

RentalContract::RentalContract(int id, int orderId,
                               const QString& contractNumber,
                               const QString& clientName,
                               const QDate& signedDate)
    : m_id(id), m_orderId(orderId),
      m_contractNumber(contractNumber),
      m_clientName(clientName),
      m_signedDate(signedDate),
      m_status(ContractStatus::Signed)
{}

int RentalContract::getId() const { return m_id; }
int RentalContract::getOrderId() const { return m_orderId; }
QString RentalContract::getContractNumber() const { return m_contractNumber; }
QString RentalContract::getClientName() const { return m_clientName; }
QDate RentalContract::getSignedDate() const { return m_signedDate; }
QDate RentalContract::getClosedDate() const { return m_closedDate; }
ContractStatus RentalContract::getStatus() const { return m_status; }

QString RentalContract::getStatusText() const {
    switch (m_status) {
        case ContractStatus::Signed:       return "Подписан";
        case ContractStatus::Active:       return "Активен";
        case ContractStatus::Closed:       return "Закрыт";
        case ContractStatus::ManualReview: return "Ручная проверка";
    }
    return "";
}

void RentalContract::activate() {
    m_status = ContractStatus::Active;
}

void RentalContract::close() {
    m_status = ContractStatus::Closed;
    m_closedDate = QDate::currentDate();
}

void RentalContract::sendToManualReview() {
    m_status = ContractStatus::ManualReview;
}

bool RentalContract::isClosed() const {
    return m_status == ContractStatus::Closed;
}
