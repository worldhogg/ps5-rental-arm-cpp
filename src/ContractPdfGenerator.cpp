#include "ContractPdfGenerator.h"
#include <QDir>
#include <QStandardPaths>
#include <QTextDocument>
#include <QPrinter>
#include <QPageSize>

// ----------------------------------------------------------------
// Публичный метод — создаёт PDF и возвращает путь к файлу
// ----------------------------------------------------------------

QString ContractPdfGenerator::generatePdf(const RentalOrder& order,
                                           const RentalContract& contract)
{
    QString dir = ensureContractsDir();
    if (dir.isEmpty()) return {};

    QString fileName = QString("contract_%1.pdf").arg(contract.getContractNumber());
    QString filePath = dir + "/" + fileName;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(20, 15, 15, 15), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setDefaultFont(QFont("Arial", 10));
    doc.setHtml(buildHtml(order, contract));
    doc.print(&printer);

    return filePath;
}

// ----------------------------------------------------------------
// Создаёт папку contracts/ рядом с исполняемым файлом
// ----------------------------------------------------------------

QString ContractPdfGenerator::ensureContractsDir()
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (base.isEmpty()) return {};

    QDir dir(base);
    if (!dir.exists("ps5_contracts"))
        dir.mkdir("ps5_contracts");
    return dir.filePath("ps5_contracts");
}

// ----------------------------------------------------------------
// Строит HTML-представление договора
// ----------------------------------------------------------------

QString ContractPdfGenerator::buildHtml(const RentalOrder& order,
                                         const RentalContract& contract)
{
    const Client&        client  = order.getClient();
    const ConsoleDevice& console = order.getConsole();

    QString clientName   = client.getFullName().toHtmlEscaped();
    QString clientPhone  = client.getPhone().toHtmlEscaped();
    QString clientDoc    = client.getDocumentNumber().toHtmlEscaped();
    QString contractNum  = contract.getContractNumber().toHtmlEscaped();
    QString consoleName  = console.getName().toHtmlEscaped();
    QString signedDate   = contract.getSignedDate().toString("dd.MM.yyyy");
    QString startDate    = order.getStartDate().toString("dd.MM.yyyy");
    QString endDate      = order.getEndDate().toString("dd.MM.yyyy");
    int     days         = order.getDaysCount();
    double  pricePerDay  = console.getPricePerDay();
    double  totalPrice   = order.getTotalPrice();

    // Строки оборудования: консоль
    QString equipRows;
    equipRows += QString(
        "<tr>"
        "<td>1</td>"
        "<td>Игровая консоль Sony PlayStation 5<br><small>(%1)</small></td>"
        "<td>Консоль PS5 • Кабель питания • Кабель HDMI • Геймпад DualSense • Кабель USB-C • Зарядная станция</td>"
        "<td>1</td>"
        "<td>%2</td>"
        "<td align='right'>%3 ₽/сут</td>"
        "<td align='right'>0 ₽</td>"
        "<td align='right'>40 000 ₽</td>"
        "</tr>")
        .arg(consoleName)
        .arg(days)
        .arg((int)pricePerDay);

    // Дополнительные геймпады
    if (order.getExtraGamepads() > 0) {
        double gpPrice = RentalOrder::gamepadPricePerDay();
        equipRows += QString(
            "<tr>"
            "<td>2</td>"
            "<td>Дополнительный геймпад DualSense</td>"
            "<td>&nbsp;</td>"
            "<td>%1</td>"
            "<td>%2</td>"
            "<td align='right'>%3 ₽/сут</td>"
            "<td align='right'>0 ₽</td>"
            "<td align='right'>&nbsp;</td>"
            "</tr>")
            .arg(order.getExtraGamepads())
            .arg(days)
            .arg((int)gpPrice);
    }

    // Игры
    const QVector<Game>& games = order.getSelectedGames();
    int rowNum = order.getExtraGamepads() > 0 ? 3 : 2;
    for (const Game& g : games) {
        equipRows += QString(
            "<tr>"
            "<td>%1</td>"
            "<td>Игра: %2</td>"
            "<td>&nbsp;</td>"
            "<td>1</td>"
            "<td>%3</td>"
            "<td align='right'>%4 ₽/сут</td>"
            "<td align='right'>0 ₽</td>"
            "<td align='right'>&nbsp;</td>"
            "</tr>")
            .arg(rowNum++)
            .arg(g.getTitle().toHtmlEscaped())
            .arg(days)
            .arg((int)g.getPricePerDay());
    }

    QString html = QString(R"(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8"/>
<style>
  body { font-family: Arial, sans-serif; font-size: 10pt; color: #000; }
  h2   { text-align: center; font-size: 13pt; margin-bottom: 4px; }
  .subtitle { text-align: center; font-size: 10pt; margin-bottom: 12px; }
  p    { text-align: justify; margin: 4px 0; }
  .section-title { font-weight: bold; text-align: center; margin-top: 12px; }
  table.equip {
    width: 100%; border-collapse: collapse; margin: 8px 0; font-size: 9pt;
  }
  table.equip th, table.equip td {
    border: 1px solid #555; padding: 3px 5px; vertical-align: top;
  }
  table.equip th { background: #e8e8e8; text-align: center; font-weight: bold; }
  table.sign {
    width: 100%; margin-top: 16px; border-collapse: collapse;
  }
  table.sign td { width: 50%; padding: 6px; vertical-align: top; }
  .underline { border-bottom: 1px solid #000; display: inline-block;
               min-width: 180px; }
  .bold { font-weight: bold; }
  .right { text-align: right; }
  .total-row { font-weight: bold; background: #f0f0f0; }
  hr { border: none; border-top: 1px solid #aaa; margin: 10px 0; }
  .page-break { page-break-after: always; }
</style>
</head>
<body>

<!-- ============================================================ -->
<!--  ДОГОВОР АРЕНДЫ                                              -->
<!-- ============================================================ -->
<h2>ДОГОВОР АРЕНДЫ</h2>
<p style="text-align:right;"><b>г. Пермь</b> &nbsp;&nbsp;&nbsp; «___» ____________ 20___ г. &nbsp;&nbsp;&nbsp; № <b>%CONTRACT_NUM%</b></p>

<p>
<span class="underline">%CLIENT_NAME%</span>,
зарегистрированный по адресу: <span class="underline">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>,
паспорт (документ): <span class="underline">%CLIENT_DOC%</span>,
именуемый в дальнейшем <b>«Арендатор»</b>, действующий от своего имени и в своих
интересах, с одной стороны, и <b>Лернер Егор Яковлевич</b>, именуемый в дальнейшем
<b>«Арендодатель»</b>, действующий от своего имени и в своих интересах, с другой
стороны, совместно именуемые — <b>Стороны</b>, заключили настоящий Договор о нижеследующем.
</p>

<!-- ---- 1. ПРЕДМЕТ ---- -->
<p class="section-title">1. Предмет и общие условия Договора</p>

<p>1.1 По настоящему Договору Арендодатель обязуется предоставить Арендатору за
плату во временное владение и пользование следующее движимое имущество (далее —
<b>Имущество</b>):</p>

<table class="equip">
  <tr>
    <th>№</th>
    <th>Наименование Имущества</th>
    <th>Комплектация</th>
    <th>Кол-во</th>
    <th>Период проката, сут.</th>
    <th>Аренд. плата в сутки</th>
    <th>Обеспечит. платёж</th>
    <th>Оценочная стоимость</th>
  </tr>
  %EQUIP_ROWS%
</table>

<p>1.2 Общая Оценочная стоимость Имущества составляет <b>40 000 рублей</b>.</p>
<p>1.3 Общая сумма Обеспечительного платежа составляет <b>0 рублей</b>.</p>
<p>1.4 Общая сумма Арендной платы за все арендуемое Имущество за одни сутки
   составляет <b>%PRICE_PER_DAY% рублей</b>.</p>
<p>1.5 Общая сумма Арендной платы за весь Период проката составляет
   <b>%TOTAL_PRICE% рублей</b>.</p>
<p>1.6 Имущество предоставляется Арендатору после оплаты Арендодателю Арендной
   платы за Период проката, что составляет <b>%TOTAL_PRICE% рублей</b>.</p>
<p>1.7 Передача и возврат Имущества осуществляются при наличии у Арендатора паспорта.</p>

<!-- ---- 2. СРОК ---- -->
<p class="section-title">2. Срок действия Договора</p>

<p>2.1 Срок действия настоящего Договора составляет <b>%DAYS% дней</b> с даты подписания.</p>
<p>2.2 Период проката: с <b>%START_DATE%</b> по <b>%END_DATE%</b> включительно.
   Неполные сутки округляются до полных.</p>
<p>2.3 Прекращение срока действия Договора не освобождает Стороны от выполнения
   неисполненных обязательств.</p>

<!-- ---- 3. ПРАВА И ОБЯЗАННОСТИ ---- -->
<p class="section-title">3. Права и обязанности Сторон</p>

<p><b>3.1 Арендодатель вправе:</b></p>
<p>3.1.1 Осуществлять взыскание задолженности по арендной плате в порядке,
   предусмотренном законодательством РФ.</p>
<p>3.1.2 Взимать штраф 1 000 ₽ за единицу Имущества при возврате грязного
   Имущества (загрязнение ≥ 50% поверхности).</p>
<p>3.1.3 Досрочно расторгнуть Договор при невыполнении Арендатором обязательств.</p>
<p>3.1.4 Удержать платежи Арендатора из суммы Обеспечительного взноса.</p>

<p><b>3.2 Арендодатель обязан:</b></p>
<p>3.2.1 Передать Имущество в исправном состоянии и в полной комплектации.</p>
<p>3.2.2 Ознакомить Арендатора с правилами эксплуатации.</p>
<p>3.2.3 Производить капитальный ремонт Имущества своими силами.</p>
<p>3.2.4 Гарантировать стабильную работу системного ПО консоли.</p>

<p><b>3.3 Арендатор вправе:</b></p>
<p>3.3.1 Отказаться от Договора в любое время; арендная плата начисляется по п. 2.2.</p>
<p>3.3.2 Продлить Период проката, предупредив Арендодателя не позднее чем за сутки.</p>

<p><b>3.4 Арендатор обязан:</b></p>
<p>3.4.1 Своевременно вносить Арендную плату и использовать Имущество по назначению.</p>
<p>3.4.2 Поддерживать Имущество в исправном состоянии.</p>
<p>3.4.3 Не сдавать Имущество в субаренду и не передавать права третьим лицам.</p>
<p>3.4.4 По истечении Периода проката вернуть Имущество в чистом, исправном и
   комплектном состоянии своими силами и за свой счёт.</p>
<p>3.4.5 Обращаться с Имуществом с чрезвычайной осторожностью (защита от падений,
   жидкостей, перегрева, пыли, прямых солнечных лучей).</p>
<p>3.4.6 Не предпринимать попыток взлома (jailbreak), прошивки или модификации консоли.</p>
<p>3.4.7 Не удалять и не изменять настройки Арендодателя на консоли без его согласия.</p>
<p>3.4.8 Не создавать новые пользовательские профили без согласования с Арендодателем.</p>
<p>3.4.9 Использовать штатные кабели, стабильное электроснабжение и хорошую вентиляцию.</p>
<p>3.4.10 Немедленно сообщать об ошибках, перегреве, неработоспособности геймпада,
   повреждении дисков с играми.</p>

<!-- ---- 4. ОТВЕТСТВЕННОСТЬ ---- -->
<p class="section-title">4. Ответственность Сторон</p>

<p>4.1 За просрочку — пени 1% от суммы невыполненных обязательств за каждый день.</p>
<p>4.2 При утрате или невозможности восстановления Имущества Арендатор выплачивает
   Оценочную стоимость; при частичном повреждении — оплачивает ремонт.</p>

<!-- ---- 5. ВЫХОД ИЗ СТРОЯ ---- -->
<p class="section-title">5. Действия Сторон при выходе Имущества из строя</p>

<p>5.1 При выходе из строя Арендатор немедленно уведомляет Арендодателя и возвращает
   Имущество. Арендная плата начисляется до момента фактического возврата.</p>
<p>5.2 Неисправное Имущество принимается по Акту с указанием технического состояния.</p>
<p>5.3 Диагностика проводится в течение 10 дней; Обеспечительный платёж удерживается
   до определения стоимости ремонта.</p>
<p>5.4 При недостаточности Обеспечительного платежа Арендатор возмещает остаток
   в течение 30 дней.</p>

<!-- ---- 6. СПОРЫ ---- -->
<p class="section-title">6. Разрешение споров</p>

<p>6.1 Все споры решаются путём переговоров.</p>
<p>6.2 При невозможности — рассматриваются в суде по месту регистрации Арендодателя.</p>

<!-- ---- 7. ЗАКЛЮЧИТЕЛЬНЫЕ ---- -->
<p class="section-title">7. Заключительные положения</p>

<p>7.1 Договор составлен в двух экземплярах, по одному для каждой из Сторон.</p>
<p>7.2 Изменения и дополнения действительны только при письменном оформлении и
   подписании обеими Сторонами.</p>
<p>7.3 Стороны признают юридическую силу за перепиской по электронной почте и
   телефону, указанным в Договоре.</p>
<p>7.4 Все положения Договора Сторонами поняты, возражений не имеется.</p>

<!-- ---- 8. КОНТАКТЫ ---- -->
<p class="section-title">8. Контактные данные Сторон</p>

<table class="sign">
  <tr>
    <td>
      <b>Арендодатель:</b><br>
      Лернер Егор Яковлевич<br>
      Адрес: г. Пермь, ул. Луначарского, 99<br>
      Телефон: 8 (951) 940-35-99<br>
      E-mail: art_shidichev07@mail.ru
    </td>
    <td>
      <b>Арендатор:</b><br>
      %CLIENT_NAME%<br>
      Телефон: %CLIENT_PHONE%<br>
      Документ: %CLIENT_DOC%<br>
      Дата подписания: %SIGNED_DATE%
    </td>
  </tr>
</table>

<table class="sign" style="margin-top:24px;">
  <tr>
    <td>
      <b>Арендодатель:</b><br><br>
      _________________ / Лернер Е.Я. /
    </td>
    <td>
      <b>Арендатор:</b><br><br>
      _________________ / %CLIENT_NAME% /
    </td>
  </tr>
</table>

<!-- ============================================================ -->
<!--  АКТ ПЕРЕДАЧИ                                                -->
<!-- ============================================================ -->
<div class="page-break"></div>

<h2>Акт передачи Имущества в прокат</h2>
<p class="subtitle">к Договору аренды № <b>%CONTRACT_NUM%</b> от <b>%SIGNED_DATE%</b></p>

<p><b>г. Пермь</b> &nbsp;&nbsp; ___ час ___ мин &nbsp; %START_DATE% г.</p>

<p>Арендодатель <b>Лернер Е.Я.</b> передал, а Арендатор
<b>%CLIENT_NAME%</b> принял в прокат следующее Имущество:</p>

<table class="equip">
  <tr>
    <th>№</th>
    <th>Наименование Имущества</th>
    <th>Кол-во</th>
    <th>Ед. изм.</th>
    <th>Отметки</th>
  </tr>
  <tr>
    <td>1</td>
    <td>%CONSOLE_NAME% (кабель HDMI, кабель питания, зарядка USB-C)</td>
    <td>1</td>
    <td>шт</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Геймпад DualSense (в комплекте)</td>
    <td>1</td>
    <td>шт</td>
    <td>&nbsp;</td>
  </tr>
  %ACT_EXTRA_ROWS%
  <tr>
    <td colspan="5">
      <p>Имущество передано в исправном состоянии, работоспособность проверена.</p>
      <p>Подписанием Акта Арендатор подтверждает ознакомление с правилами эксплуатации.</p>
    </td>
  </tr>
</table>

<table class="sign" style="margin-top:20px;">
  <tr>
    <td><b>Имущество передал:</b><br><br>_________________ / Лернер Е.Я. /</td>
    <td><b>Имущество принял:</b><br><br>_________________ / %CLIENT_NAME% /</td>
  </tr>
</table>

<!-- ============================================================ -->
<!--  АКТ ВОЗВРАТА                                                -->
<!-- ============================================================ -->
<div class="page-break"></div>

<h2>Акт о возврате Имущества из проката</h2>
<p class="subtitle">к Договору аренды № <b>%CONTRACT_NUM%</b> от <b>%SIGNED_DATE%</b></p>

<p><b>г. Пермь</b> &nbsp;&nbsp; ___ час ___ мин &nbsp; %END_DATE% г.</p>

<p>Арендодатель <b>Лернер Е.Я.</b> принял, а Арендатор
<b>%CLIENT_NAME%</b> вернул из проката следующее Имущество:</p>

<table class="equip">
  <tr>
    <th>№</th>
    <th>Наименование Имущества</th>
    <th>Кол-во</th>
    <th>Ед. изм.</th>
    <th>Отметки</th>
  </tr>
  <tr>
    <td>1</td>
    <td>%CONSOLE_NAME% (кабель HDMI, кабель питания, зарядка USB-C)</td>
    <td>1</td>
    <td>шт</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Геймпад DualSense (в комплекте)</td>
    <td>1</td>
    <td>шт</td>
    <td>&nbsp;</td>
  </tr>
  %ACT_EXTRA_ROWS%
</table>

<p>Имущество возвращено в следующем состоянии:
   ________________________________________________________________________</p>
<p>Фактический Период проката составил: <b>%DAYS%</b> суток.</p>
<p>Итого арендная плата за фактический Период проката: <b>%TOTAL_PRICE% рублей</b>.</p>

<table class="sign" style="margin-top:20px;">
  <tr>
    <td><b>Имущество принял:</b><br><br>_________________ / Лернер Е.Я. /</td>
    <td><b>Имущество передал:</b><br><br>_________________ / %CLIENT_NAME% /</td>
  </tr>
</table>

</body>
</html>
)");

    // Строки для актов: доп. геймпады и игры
    QString actExtraRows;
    int actRow = 3;
    if (order.getExtraGamepads() > 0) {
        actExtraRows += QString(
            "<tr><td>%1</td><td>Геймпад DualSense (дополнительный)</td>"
            "<td>%2</td><td>шт</td><td>&nbsp;</td></tr>")
            .arg(actRow++)
            .arg(order.getExtraGamepads());
    }
    for (const Game& g : games) {
        actExtraRows += QString(
            "<tr><td>%1</td><td>Игра: %2</td><td>1</td><td>шт</td><td>&nbsp;</td></tr>")
            .arg(actRow++)
            .arg(g.getTitle().toHtmlEscaped());
    }

    // Подставляем все переменные
    html.replace("%CONTRACT_NUM%",   contractNum);
    html.replace("%CLIENT_NAME%",    clientName);
    html.replace("%CLIENT_PHONE%",   clientPhone);
    html.replace("%CLIENT_DOC%",     clientDoc);
    html.replace("%SIGNED_DATE%",    signedDate);
    html.replace("%START_DATE%",     startDate);
    html.replace("%END_DATE%",       endDate);
    html.replace("%DAYS%",           QString::number(days));
    html.replace("%CONSOLE_NAME%",   consoleName);
    html.replace("%PRICE_PER_DAY%",  QString::number((int)pricePerDay));
    html.replace("%TOTAL_PRICE%",    QString::number((int)totalPrice));
    html.replace("%EQUIP_ROWS%",     equipRows);
    html.replace("%ACT_EXTRA_ROWS%", actExtraRows);

    return html;
}
