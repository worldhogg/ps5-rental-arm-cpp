#include "ReturnBoxSimulator.h"

// Для учебной демонстрации все технические проверки возвращают OK.
// В реальной системе здесь были бы запросы к физическому устройству.

ReturnCheckResult ReturnBoxSimulator::runFullCheck(const ConsoleDevice& console,
                                                   int extraControllersCount)
{
    Q_UNUSED(console)
    bool power       = checkPower();
    bool hdmi        = checkHdmi();
    bool controller  = checkController(extraControllersCount);
    bool storage     = checkStorage();
    bool accessories = checkAccessories();
    return ReturnCheckResult(power, hdmi, controller, storage, accessories);
}

bool ReturnBoxSimulator::checkPower()                         { return true; }
bool ReturnBoxSimulator::checkHdmi()                          { return true; }
bool ReturnBoxSimulator::checkController(int /*extra*/)       { return true; }
bool ReturnBoxSimulator::checkStorage()                       { return true; }
bool ReturnBoxSimulator::checkAccessories()                   { return true; }
