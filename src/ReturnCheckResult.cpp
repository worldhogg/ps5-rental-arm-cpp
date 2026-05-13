#include "ReturnCheckResult.h"

ReturnCheckResult::ReturnCheckResult()
    : m_powerOk(false), m_hdmiOk(false), m_controllerOk(false),
      m_storageOk(false), m_accessoriesOk(false)
{}

ReturnCheckResult::ReturnCheckResult(bool powerOk, bool hdmiOk, bool controllerOk,
                                     bool storageOk, bool accessoriesOk)
    : m_powerOk(powerOk), m_hdmiOk(hdmiOk), m_controllerOk(controllerOk),
      m_storageOk(storageOk), m_accessoriesOk(accessoriesOk)
{}

bool ReturnCheckResult::isPassed() const {
    return m_powerOk && m_hdmiOk && m_controllerOk && m_storageOk && m_accessoriesOk;
}

QString ReturnCheckResult::getSummary() const {
    auto ok = [](bool v) { return v ? "OK" : "ОШИБКА"; };
    return QString(
        "Питание:      %1\n"
        "HDMI:         %2\n"
        "Геймпад:      %3\n"
        "Система:      %4\n"
        "Комплектность:%5\n"
        "Итог: %6"
    ).arg(ok(m_powerOk))
     .arg(ok(m_hdmiOk))
     .arg(ok(m_controllerOk))
     .arg(ok(m_storageOk))
     .arg(ok(m_accessoriesOk))
     .arg(isPassed() ? "проверка пройдена" : "проверка НЕ пройдена");
}

bool ReturnCheckResult::isPowerOk()       const { return m_powerOk; }
bool ReturnCheckResult::isHdmiOk()        const { return m_hdmiOk; }
bool ReturnCheckResult::isControllerOk()  const { return m_controllerOk; }
bool ReturnCheckResult::isStorageOk()     const { return m_storageOk; }
bool ReturnCheckResult::isAccessoriesOk() const { return m_accessoriesOk; }
