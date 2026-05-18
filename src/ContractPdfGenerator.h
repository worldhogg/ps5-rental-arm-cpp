#pragma once
#include <QString>
#include "RentalOrder.h"
#include "RentalContract.h"

class ContractPdfGenerator {
public:
    // Генерирует PDF и возвращает путь к файлу; пустая строка — ошибка
    static QString generatePdf(const RentalOrder& order,
                               const RentalContract& contract);

private:
    static QString buildHtml(const RentalOrder& order,
                              const RentalContract& contract);
    static QString ensureContractsDir();
};
