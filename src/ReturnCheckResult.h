#pragma once
#include <QString>

// Результат автоматической технической проверки консоли проверочным боксом
class ReturnCheckResult {
public:
    ReturnCheckResult();
    ReturnCheckResult(bool powerOk, bool hdmiOk, bool controllerOk,
                      bool storageOk, bool accessoriesOk);

    bool isPassed() const;
    QString getSummary() const;

    bool isPowerOk() const;
    bool isHdmiOk() const;
    bool isControllerOk() const;
    bool isStorageOk() const;
    bool isAccessoriesOk() const;

private:
    bool m_powerOk;
    bool m_hdmiOk;
    bool m_controllerOk;
    bool m_storageOk;
    bool m_accessoriesOk;
};
