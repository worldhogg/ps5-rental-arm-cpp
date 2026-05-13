# UML-диаграммы проекта

## Файлы

| Файл | Описание |
|------|----------|
| `uml.puml` | Диаграмма классов всего проекта (PlantUML) |

## Как открыть диаграмму

### Вариант 1 — онлайн (быстро)

1. Открыть [https://www.plantuml.com/plantuml/uml/](https://www.plantuml.com/plantuml/uml/)
2. Вставить содержимое файла `uml.puml`
3. Нажать «Submit»

### Вариант 2 — VS Code

1. Установить расширение **PlantUML** (jebbs.plantuml)
2. Открыть файл `uml.puml`
3. Нажать `Alt+D` для предпросмотра

### Вариант 3 — локально через JAR

```bash
# Установить Java (если нет)
brew install openjdk

# Скачать plantuml.jar
curl -L https://github.com/plantuml/plantuml/releases/latest/download/plantuml.jar -o plantuml.jar

# Сгенерировать PNG
java -jar plantuml.jar diagrams/uml.puml
```

Результат: `diagrams/uml.png`

## Что изображено на диаграмме

- **Client** — данные клиента (ФИО, телефон, документ)
- **ConsoleDevice** — PS5 консоль с статусом (ConsoleStatus)
- **Game** — дополнительная игра для аренды
- **RentalOrder** — заказ аренды, объединяющий клиента, консоль и игры
- **RentalStatus** — жизненный цикл заказа (Created → Active → CheckReturn → Completed)
- **RentalManager** — менеджер данных, управляет всеми объектами
- **ConsoleOpenGLWidget** — OpenGL-виджет визуализации статусов консолей
- **MainWindow** — главное окно, связывает UI с бизнес-логикой
