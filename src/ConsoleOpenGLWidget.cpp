#include "ConsoleOpenGLWidget.h"
#include <QPainter>
#include <QFont>

ConsoleOpenGLWidget::ConsoleOpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent) {}

void ConsoleOpenGLWidget::updateConsoles(const QVector<ConsoleDevice>& consoles) {
    m_consoles = consoles;
    update();
}

void ConsoleOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.12f, 0.12f, 0.14f, 1.0f); // тёмный фон
}

void ConsoleOpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void ConsoleOpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_consoles.isEmpty()) return;

    int w = width();
    int h = height();

    int padding = 16;
    int count   = m_consoles.size();
    int cardWidth  = (w - padding * (count + 1)) / count;
    int cardHeight = h - padding * 2;

    // Ортогональная проекция: Y вниз, как в Qt
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (int i = 0; i < count; ++i) {
        float x = padding + i * (cardWidth + padding);
        float y = padding;
        drawConsoleCard(x, y, cardWidth, cardHeight, m_consoles[i].getStatus());
        drawPS5Silhouette(x, y, cardWidth, cardHeight);
    }

    // Текст поверх через QPainter
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont nameFont("Arial", 10, QFont::Bold);
    QFont statusFont("Arial", 8);

    for (int i = 0; i < count; ++i) {
        float x = padding + i * (cardWidth + padding);

        // Номер консоли вверху карточки
        painter.setFont(nameFont);
        painter.setPen(Qt::white);
        painter.drawText(QRect((int)x, (int)(padding + 6), cardWidth, 22),
                         Qt::AlignHCenter | Qt::AlignTop,
                         QString("PS5 #%1").arg(i + 1));

        // Статус внизу карточки
        painter.setFont(statusFont);
        painter.setPen(Qt::white);
        painter.drawText(QRect((int)x, (int)(padding + cardHeight - 28), cardWidth, 22),
                         Qt::AlignHCenter | Qt::AlignTop,
                         m_consoles[i].getStatusText());
    }

    painter.end();
}

// ----------------------------------------------------------------
// Фон карточки + белая рамка
// ----------------------------------------------------------------
void ConsoleOpenGLWidget::drawConsoleCard(float x, float y, float width, float height,
                                          ConsoleStatus status)
{
    // Цветной фон по статусу
    setColorByStatus(status);
    glBegin(GL_QUADS);
        glVertex2f(x,         y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x,         y + height);
    glEnd();

    // Белая рамка
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x + 1.f,         y + 1.f);
        glVertex2f(x + width - 1.f, y + 1.f);
        glVertex2f(x + width - 1.f, y + height - 1.f);
        glVertex2f(x + 1.f,         y + height - 1.f);
    glEnd();
}

// ----------------------------------------------------------------
// Упрощённый силуэт PS5 в центре карточки
// ----------------------------------------------------------------
void ConsoleOpenGLWidget::drawPS5Silhouette(float cx, float cy, float cw, float ch) {
    // Центр карточки, слегка смещаем вверх — оставляем место для текста сверху и снизу
    float centerX = cx + cw * 0.5f;
    float centerY = cy + ch * 0.5f + 4.0f;

    // Размеры основного корпуса
    float bodyW = cw * 0.44f;
    float bodyH = ch * 0.50f;
    float bx = centerX - bodyW * 0.5f;
    float by = centerY - bodyH * 0.5f;

    // --- Белый корпус ---
    glColor3f(0.90f, 0.90f, 0.90f);
    glBegin(GL_QUADS);
        glVertex2f(bx,          by);
        glVertex2f(bx + bodyW,  by);
        glVertex2f(bx + bodyW,  by + bodyH);
        glVertex2f(bx,          by + bodyH);
    glEnd();

    // --- Тёмная центральная полоса (середина корпуса по высоте) ---
    float bandY = by + bodyH * 0.36f;
    float bandH = bodyH * 0.26f;
    glColor3f(0.16f, 0.16f, 0.20f);
    glBegin(GL_QUADS);
        glVertex2f(bx,          bandY);
        glVertex2f(bx + bodyW,  bandY);
        glVertex2f(bx + bodyW,  bandY + bandH);
        glVertex2f(bx,          bandY + bandH);
    glEnd();

    // --- Синяя светодиодная линия в центре тёмной полосы ---
    float ledY = bandY + bandH * 0.40f;
    float ledH = (ch > 80) ? 3.0f : 2.0f;
    float ledInset = bodyW * 0.12f;
    glColor3f(0.10f, 0.55f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(bx + ledInset,          ledY);
        glVertex2f(bx + bodyW - ledInset,  ledY);
        glVertex2f(bx + bodyW - ledInset,  ledY + ledH);
        glVertex2f(bx + ledInset,          ledY + ledH);
    glEnd();

    // --- Контур корпуса ---
    glColor3f(0.25f, 0.25f, 0.30f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(bx,          by);
        glVertex2f(bx + bodyW,  by);
        glVertex2f(bx + bodyW,  by + bodyH);
        glVertex2f(bx,          by + bodyH);
    glEnd();

    // --- Небольшие «лапки» снизу корпуса (подставка) ---
    float legH  = bodyH * 0.08f;
    float legW  = bodyW * 0.18f;
    float legY  = by + bodyH;
    // левая ножка
    glColor3f(0.75f, 0.75f, 0.75f);
    glBegin(GL_QUADS);
        glVertex2f(bx + bodyW * 0.15f,         legY);
        glVertex2f(bx + bodyW * 0.15f + legW,  legY);
        glVertex2f(bx + bodyW * 0.15f + legW,  legY + legH);
        glVertex2f(bx + bodyW * 0.15f,         legY + legH);
    glEnd();
    // правая ножка
    glBegin(GL_QUADS);
        glVertex2f(bx + bodyW * 0.67f,         legY);
        glVertex2f(bx + bodyW * 0.67f + legW,  legY);
        glVertex2f(bx + bodyW * 0.67f + legW,  legY + legH);
        glVertex2f(bx + bodyW * 0.67f,         legY + legH);
    glEnd();
}

// ----------------------------------------------------------------
// Цвет по статусу консоли
// ----------------------------------------------------------------
void ConsoleOpenGLWidget::setColorByStatus(ConsoleStatus status) {
    switch (status) {
        case ConsoleStatus::Free:
            glColor3f(0.18f, 0.62f, 0.18f);  // зелёный
            break;
        case ConsoleStatus::Reserved:
            glColor3f(0.16f, 0.36f, 0.82f);  // синий
            break;
        case ConsoleStatus::Rented:
            glColor3f(0.85f, 0.44f, 0.06f);  // оранжевый
            break;
        case ConsoleStatus::CheckReturn:
            glColor3f(0.76f, 0.08f, 0.08f);  // красный
            break;
        case ConsoleStatus::Unavailable:
            glColor3f(0.36f, 0.36f, 0.36f);  // серый
            break;
        default:
            glColor3f(0.5f, 0.5f, 0.5f);
            break;
    }
}
