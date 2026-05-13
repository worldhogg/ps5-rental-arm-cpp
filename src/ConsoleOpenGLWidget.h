#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector>
#include "ConsoleDevice.h"

// Виджет OpenGL — отображает карточки консолей с цветом по статусу и силуэтом PS5
class ConsoleOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit ConsoleOpenGLWidget(QWidget* parent = nullptr);

    // Обновить данные о консолях для перерисовки
    void updateConsoles(const QVector<ConsoleDevice>& consoles);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QVector<ConsoleDevice> m_consoles;

    // Нарисовать фон и рамку карточки
    void drawConsoleCard(float x, float y, float width, float height, ConsoleStatus status);

    // Нарисовать упрощённый силуэт PS5 внутри карточки
    void drawPS5Silhouette(float cardX, float cardY, float cardW, float cardH);

    // Установить цвет по статусу
    void setColorByStatus(ConsoleStatus status);
};
