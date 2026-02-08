#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QInputDialog>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <queue>
#include <random>

using namespace std;

// Структура, описывающая одну клетку поля
struct Cell {
    bool hasMine = false;
    bool isOpen = false;
    bool isFlagged = false;
    int neighborMines = 0;
};

// GAME MODEL
// Паттерн: Model
class GameModel {
private:
    int rows;
    int cols;
    int totalMines;
    bool gameOver;
    bool victory;
    vector<vector<Cell>> grid;

public:
    // Конструктор
    GameModel() : rows(10), cols(10), totalMines(10), gameOver(false), victory(false) {
        grid.resize(rows, vector<Cell>(cols));
    }

    // Геттеры
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    int getTotalMines() const { return totalMines; }
    bool isGameOver() const { return gameOver; }
    bool isVictory() const { return victory; }

    const Cell& getCell(int r, int c) const {
        return grid[r][c];
    }

    // Основная логика игры

    // Начинает новую игру с заданными параметрами
    void startNewGame(int r, int c, int m) {
        rows = r;
        cols = c;
        totalMines = m;
        gameOver = false;
        victory = false;

        // Пересоздаем сетку под новый размер и очищаем её
        grid.assign(rows, vector<Cell>(cols));

        // Расставляем мины и считаем цифры
        generateMines();
        calculateNeighbors();
    }

    // Обработка открытия клетки
    // Возвращает false, если игрок подорвался, иначе true
    bool openCell(int r, int c) {
        // Проверки на выход за границы массива и состояние игры
        if (gameOver || r < 0 || r >= rows || c < 0 || c >= cols) return true;

        Cell &cell = grid[r][c];

        // Если клетка уже открыта или помечена флагом - ничего не делаем
        if (cell.isFlagged || cell.isOpen) return true;

        cell.isOpen = true; // Открываем клетку

        // Если попали на мину
        if (cell.hasMine) {
            gameOver = true;
            victory = false;
            revealAll(); // Показываем игроку всё поле
            return false;
        }

        // Если клетка пустая (вокруг нет мин), запускаем авто-открытие соседей
        if (cell.neighborMines == 0) {
            floodFill(r, c);
        }

        checkWinCondition(); // Проверяем, не выиграл ли игрок
        return true;
    }

    // Установка/снятие флажка
    void toggleFlag(int r, int c) {
        if (gameOver || r < 0 || r >= rows || c < 0 || c >= cols) return;
        Cell &cell = grid[r][c];

        // Флаг можно ставить только на закрытую клетку
        if (!cell.isOpen) {
            cell.isFlagged = !cell.isFlagged;
            checkWinCondition(); // Победа может наступить, если помечены все мины
        }
    }

private: // Внутренние вспомогательные методы (скрыты от внешнего мира)

    // Открывает все клетки (вызывается при проигрыше)
    void revealAll() {
        for (auto& row : grid) {
            for (auto& cell : row) {
                cell.isOpen = true;
            }
        }
    }

    // Генерация мин случайным образом
    void generateMines() {
        // static mt19937 - генератор создается только ОДИН раз при первом запуске игры.
        // Это эффективно и дает качественные случайные числа.
        static mt19937 gen(random_device{}());

        // Распределения для получения случайных координат
        uniform_int_distribution<> disRow(0, rows - 1);
        uniform_int_distribution<> disCol(0, cols - 1);

        int minesPlaced = 0;
        while (minesPlaced < totalMines) {
            int r = disRow(gen);
            int c = disCol(gen);

            // Если в этой клетке еще нет мины, ставим её
            if (!grid[r][c].hasMine) {
                grid[r][c].hasMine = true;
                minesPlaced++;
            }
        }
    }

    // Подсчет количества мин вокруг каждой клетки
    void calculateNeighbors() {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (grid[r][c].hasMine) continue; // Минам цифры не нужны

                int count = 0;
                // Проходимся по всем 8 соседям
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        int nr = r + dr;
                        int nc = c + dc;
                        // Проверяем, что сосед внутри поля и является миной
                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && grid[nr][nc].hasMine) {
                            count++;
                        }
                    }
                }
                grid[r][c].neighborMines = count;
            }
        }
    }

    // Рекурсивно открывает пустые области
    void floodFill(int r, int c) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                int nr = r + dr;
                int nc = c + dc;
                // Проверка границ
                if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                    // Открываем соседа, если он закрыт и не мина
                    if (!grid[nr][nc].isOpen && !grid[nr][nc].hasMine) {
                        openCell(nr, nc); // openCell сам вызовет floodFill дальше, если нужно
                    }
                }
            }
        }
    }

    // Проверка условий победы
    void checkWinCondition() {
        int coveredSafeCells = 0;       // Сколько безопасных клеток еще закрыто
        int correctlyFlaggedMines = 0;  // Сколько мин правильно помечено
        int totalFlags = 0;             // Сколько всего флагов поставил игрок

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                const Cell &cell = grid[r][c];
                if (!cell.hasMine && !cell.isOpen) coveredSafeCells++;
                if (cell.isFlagged) totalFlags++;
                if (cell.hasMine && cell.isFlagged) correctlyFlaggedMines++;
            }
        }

        // Условие 1: Открыты все клетки, где нет мин
        if (coveredSafeCells == 0) {
            victory = true;
            gameOver = true;
            return;
        }

        // Условие 2: Все флаги стоят на минах, и лишних флагов нет
        if (totalFlags == totalMines && correctlyFlaggedMines == totalMines) {
            victory = true;
            gameOver = true;
        }
    }
};

// GAME WIDGET
// Паттерн: View
// Отвечает за отрисовку (QPainter) и обработку мыши (Mouse Events)
class MinesweeperWidget : public QWidget {
    GameModel model;
    const int cellSize = 30; // Размер одной клетки в пикселях

public:
    MinesweeperWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Сапёр (Minesweeper)");
        setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

        configureGame(); // Запуск диалога настроек
    }

protected:
    // Событие отрисовки: вызывается Qt автоматически, когда нужно перерисовать окно
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing); // Сглаживание

        // Проходим по всем клеткам модели и рисуем их
        for (int r = 0; r < model.getRows(); ++r) {
            for (int c = 0; c < model.getCols(); ++c) {
                drawCell(painter, r, c);
            }
        }
    }

    // Событие нажатия мыши
    void mousePressEvent(QMouseEvent *event) override {
        // Если игра окончена, клик вызывает предложение рестарта
        if (model.isGameOver()) {
            askToRestart();
            return;
        }

        // Вычисляем координаты клетки по координатам клика мыши
        int c = event->x() / cellSize;
        int r = event->y() / cellSize;

        // Левая кнопка - открыть, Правая - флаг
        if (event->button() == Qt::LeftButton) {
            model.openCell(r, c);
        } else if (event->button() == Qt::RightButton) {
            model.toggleFlag(r, c);
        }

        update(); // Просим Qt перерисовать окно (вызовет paintEvent)

        if (model.isGameOver()) {
            showGameOverMessage();
        }
    }

private:
    // Диалог настройки перед игрой
    void configureGame() {
        bool ok;
        // Запрашиваем число мин у пользователя
        int mines = QInputDialog::getInt(this, "Настройки", "Количество мин:", 10, 1, 50, 1, &ok);

        // Если нажали Cancel -> выход
        if (!ok) {
            exit(0);
        }

        model.startNewGame(10, 10, mines);

        // Жестко задаем размер окна под размер поля
        setFixedSize(model.getCols() * cellSize, model.getRows() * cellSize);
    }

    // Отрисовка одной клетки
    void drawCell(QPainter &painter, int r, int c) {
        Cell cell = model.getCell(r, c); // Получаем данные из модели
        QRect rect(c * cellSize, r * cellSize, cellSize, cellSize);

        painter.setPen(Qt::black); // Границы всегда черные

        // Рисуем фон
        if (cell.isOpen) {
            painter.setBrush(Qt::lightGray); // Открытая - серая
        } else {
            painter.setBrush(QColor(200, 200, 255)); // Закрытая - голубая
        }
        painter.drawRect(rect);

        // Рисуем содержимое (если нужно)
        if (cell.isOpen) {
            if (cell.hasMine) {
                // Рисуем мину (красный круг)
                painter.setBrush(Qt::red);
                painter.drawEllipse(rect.adjusted(5, 5, -5, -5));
            } else if (cell.neighborMines > 0) {
                // Рисуем цифру
                painter.setPen(Qt::black);
                painter.drawText(rect, Qt::AlignCenter, QString::number(cell.neighborMines));
            }
        } else if (cell.isFlagged) {
            // Рисуем флаг
            painter.setPen(Qt::red);
            painter.drawText(rect, Qt::AlignCenter, "F");
        }
    }

    // Сообщение о конце игры
    void showGameOverMessage() {
        QString msg;
        if (model.isVictory()) {
            msg = QString("Победа!\nВсего мин: %1").arg(model.getTotalMines());
        } else {
            // Считаем статистику для проигравшего
            int foundMines = 0;
            for (int r = 0; r < model.getRows(); ++r) {
                for (int c = 0; c < model.getCols(); ++c) {
                    Cell cell = model.getCell(r, c);
                    if(cell.hasMine && cell.isFlagged) foundMines++;
                }
            }
            msg = QString("Вы проиграли.\nНайдено мин: %1\nВсего мин: %2")
                    .arg(foundMines).arg(model.getTotalMines());
        }

        QMessageBox::information(this, "Конец игры", msg);
    }

    // Диалог перезапуска
    void askToRestart() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Новая игра", "Хотите сыграть еще раз?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            configureGame();
            update();
        } else {
            QApplication::quit();
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv); // Инициализация Qt

    MinesweeperWidget window;
    window.show(); // Отображение окна

    return app.exec(); // Запуск цикла обработки событий
}
