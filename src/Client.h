#pragma once
#include <QString>

// Клиент — человек, который берёт консоль в аренду
class Client {
public:
    Client();
    Client(const QString& fullName, const QString& phone, const QString& documentNumber);

    QString getFullName() const;
    QString getPhone() const;
    QString getDocumentNumber() const;

    void setFullName(const QString& fullName);
    void setPhone(const QString& phone);
    void setDocumentNumber(const QString& documentNumber);

private:
    QString m_fullName;       // ФИО
    QString m_phone;          // Телефон
    QString m_documentNumber; // Номер документа
};
