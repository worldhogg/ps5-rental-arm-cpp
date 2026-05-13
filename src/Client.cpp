#include "Client.h"

Client::Client() {}

Client::Client(const QString& fullName, const QString& phone, const QString& documentNumber)
    : m_fullName(fullName), m_phone(phone), m_documentNumber(documentNumber) {}

QString Client::getFullName() const { return m_fullName; }
QString Client::getPhone() const { return m_phone; }
QString Client::getDocumentNumber() const { return m_documentNumber; }

void Client::setFullName(const QString& fullName) { m_fullName = fullName; }
void Client::setPhone(const QString& phone) { m_phone = phone; }
void Client::setDocumentNumber(const QString& documentNumber) { m_documentNumber = documentNumber; }
