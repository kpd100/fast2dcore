//
// Created by kpd on 11/1/24.
//
#include <iostream>
#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int BALL_RADIUS = 10;
const int BRICK_WIDTH = 60;
const int BRICK_HEIGHT = 20;
const int BRICK_ROWS = 5;
const int BRICK_COLS = 10;


class GameObject {
public:
    virtual void update() = 0;                      // Обновление состояния объекта (виртуальный метод)
    virtual void draw(sf::RenderWindow& window) = 0;  // Отрисовка объекта на экране
    virtual sf::FloatRect getBounds() const = 0;  // Границы объекта для проверки столкновений
    virtual ~GameObject() = default;              // Виртуальный деструктор для корректного удаления
};

class Brick : public GameObject{

public:
    sf::RectangleShape shape;
    bool destroyed{false};
    Brick(float x, float y) {
        shape.setSize(sf::Vector2f(BRICK_WIDTH, BRICK_HEIGHT));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Blue);
    }
    void update() override {

    }
    void draw(sf::RenderWindow& window) override {
        window.draw(shape);
    }
    sf::FloatRect getBounds() const override {
        return shape.getGlobalBounds();
    }

    void setPosition(float x, float y) {
        shape.setPosition(x, y);
    }
};

class Paddle : public GameObject {
private:
    sf::RectangleShape shape;

public:
    Paddle(float x, float y) {
        shape.setSize(sf::Vector2f(100, 20));
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(x, y);
    }

    void update() override {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && shape.getPosition().x > 0) {
            shape.move(-0.5f, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && shape.getPosition().x < 800 - shape.getSize().x) {
            shape.move(0.5f, 0);
        }
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(shape);
    }

    sf::FloatRect getBounds() const override {
        return shape.getGlobalBounds();
    }

    void setPosition(float x, float y) {
        shape.setPosition(x, y);
    }
};

int main() {

    int lives = 3;
    // Создаем окно размером 800x600 пикселей с заголовком "Arkanoid"
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arkanoid");

    // Загрузка шрифта
    sf::Font font;
    if (!font.loadFromFile("/home/kpd/Documents/develop/fast2dcore/fonts/Arial.ttf")) { //TODO сделать относительным
        return -1; // Завершаем, если шрифт не удалось загрузить
    }
    //GameObject nub;
    //nub.shape() = sf::RectangleShape(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    //nub.shape.setFillColor(sf::Color::Green);
    //nub.setPosition(WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2, WINDOW_HEIGHT - 50);
    // Создаем текст для отображения жизней
    sf::Text livesText;
    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(10, 10);
    livesText.setString("Lives: " + std::to_string(lives)); // Начальное значение текста


    // Создаем платформу
    sf::RectangleShape paddle(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    paddle.setFillColor(sf::Color::Green);
    paddle.setPosition(WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2, WINDOW_HEIGHT - 50);

    // Создаем мяч
    sf::CircleShape ball(BALL_RADIUS);
    ball.setFillColor(sf::Color::Red);
    ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    // Скорость мяча
    sf::Vector2f ballVelocity(0.05f, -0.05f);

    // Создаем кирпичи
    std::vector<Brick> bricks;
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLS; ++j) {
            float x = j * (BRICK_WIDTH + 5) + 50;
            float y = i * (BRICK_HEIGHT + 5) + 50;
            bricks.push_back(Brick(x,y));
        }
    }
    bool start = false;
    // Отрисовка платформы, мяча и кирпичей
    window.draw(paddle);
    window.draw(ball);
    window.draw(livesText); // Отображение жизней на экране

    for (const auto& brick : bricks) {
        if (!brick.destroyed) {
            window.draw(brick.shape);
        }
    }

    // Отображение содержимого окна
    window.display();
    // Игровой цикл
    while (window.isOpen()) {
        // Обработка событий
        sf::Event event;


        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        if (!start)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                start = true;
            }
            continue;
        }
        // Управление платформой
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && paddle.getPosition().x > 0) {
            paddle.move(-0.5f, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && paddle.getPosition().x < WINDOW_WIDTH - PADDLE_WIDTH) {
            paddle.move(0.5f, 0);
        }

        // Движение мяча
        ball.move(ballVelocity);

        // Проверка столкновений мяча с границами окна
        if (ball.getPosition().x < 0 || ball.getPosition().x + 2 * BALL_RADIUS > WINDOW_WIDTH) {
            ballVelocity.x = -ballVelocity.x;
        }
        if (ball.getPosition().y < 0) {
            ballVelocity.y = -ballVelocity.y;
        }
        if (ball.getPosition().y + 2 * BALL_RADIUS > WINDOW_HEIGHT) {
            // Мяч упал ниже платформы - теряем жизнь
            lives--;
            livesText.setString("Lives: " + std::to_string(lives)); // Обновление текста
            if (lives <= 0) {
                // Если жизней больше нет, закрываем окно
                window.close();
            } else {
                // Сбрасываем позицию мяча и платформы
                ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
                ballVelocity = sf::Vector2f(0.05f, -0.05);
                paddle.setPosition(WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2, WINDOW_HEIGHT - 50);
            }
        }

        // Проверка столкновения мяча с платформой
        if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
            ballVelocity.y = -ballVelocity.y;
        }

        // Проверка столкновений мяча с кирпичами
        for (auto& brick : bricks) {
            if (!brick.destroyed && ball.getGlobalBounds().intersects(brick.shape.getGlobalBounds())) {
                brick.destroyed = true;
                ballVelocity.y = -ballVelocity.y;
                break;
            }
        }

        // Очистка окна
        window.clear(sf::Color::Black);

        // Отрисовка платформы, мяча и кирпичей
        window.draw(paddle);
        window.draw(ball);
        window.draw(livesText); // Отображение жизней на экране

        for (const auto& brick : bricks) {
            if (!brick.destroyed) {
                window.draw(brick.shape);
            }
        }

        // Отображение содержимого окна
        window.display();
    }
    return 0;
}