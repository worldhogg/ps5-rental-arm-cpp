#pragma once
#include "ReturnCheckResult.h"
#include "ConsoleDevice.h"

// Программная имитация внешнего диагностического бокса.
// В реальной системе каждый метод получал бы данные от физического устройства
// через последовательный порт или USB-HID протокол.
class ReturnBoxSimulator {
public:
    ReturnBoxSimulator() = default;

    // Запустить полную проверку консоли
    ReturnCheckResult runFullCheck(const ConsoleDevice& console, int extraControllersCount);

private:
    // Проверка питания (POWER LED, напряжение шины 12 В)
    bool checkPower();
    // Проверка HDMI (EDID-рукопожатие с тестовым монитором)
    bool checkHdmi();
    // Проверка геймпада (BT-соединение + тест осей)
    bool checkController(int extraControllersCount);
    // Проверка накопителя (SMART-статус SSD)
    bool checkStorage();
    // Проверка комплектности (сенсор наличия кабелей и аксессуаров)
    bool checkAccessories();
};
