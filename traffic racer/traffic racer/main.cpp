#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int displayWidth = 800;
const int displayHeight = 600;

class Car {
public:
    sf::Sprite sprite;
    float dx = 0;

    Car(sf::Texture& texture) {
        sprite.setTexture(texture);
        sprite.setScale(0.7f, 0.7f);
        sprite.setPosition(400, 450);
    }

    void update() {
        sprite.move(dx, 0);
        if (sprite.getPosition().x + sprite.getGlobalBounds().width > displayWidth - 100)
            sprite.setPosition(displayWidth - 100 - sprite.getGlobalBounds().width, sprite.getPosition().y);
        if (sprite.getPosition().x < 100)
            sprite.setPosition(100, sprite.getPosition().y);
    }
};

class Obstacle {
public:
    sf::Sprite sprite;
    float dy = 5;
    bool counted = false;

    Obstacle(sf::Texture& texture, int x, int y) {
        sprite.setTexture(texture);
        sprite.setScale(0.7f, 0.7f);
        sprite.setPosition(x, y);
    }

    void update() {
        sprite.move(0, dy);
        if (sprite.getPosition().y > displayHeight) {
            reset();
            counted = true;
        }
    }

    void reset() {
        int x_values[] = {100, 270, 350, displayWidth - 200};
        int y_values[] = {-200, 0};
        int x = x_values[rand() % 4];
        int y = y_values[rand() % 2];
        sprite.setPosition(x, y);
        counted = false;
    }
};

class Police : public Obstacle {
public:
    Police(sf::Texture& texture, int x, int y) : Obstacle(texture, x, y) {
        dy = 4;
    }

    void reset() {
        sprite.setPosition(150 + rand() % (displayWidth - 350), 0);
        counted = false;
    }
};

int main() {
    srand(static_cast<unsigned int>(time(0)));

    sf::RenderWindow window(sf::VideoMode(displayWidth, displayHeight), "Traffic Racer | Watch Out For Cops!");

    sf::Texture backgroundTexture, carTexture, policeTexture, obs1, obs2, obs3, gameOverTexture;
    backgroundTexture.loadFromFile("road.jpg");
    carTexture.loadFromFile("icons8-race-car-96.png");
    policeTexture.loadFromFile("icons8-police-64.png");
    obs1.loadFromFile("ob1.png");
    obs2.loadFromFile("ob2.png");
    obs3.loadFromFile("ob3.png");
    gameOverTexture.loadFromFile("game-over.png");

    sf::Sprite background(backgroundTexture);
    sf::Sprite gameOver(gameOverTexture);
    Car player(carTexture);

    std::vector<Obstacle> obstacles;
    std::vector<sf::Texture*> obsTextures = {&obs1, &obs2, &obs3};

    for (int i = 0; i < 6 + rand() % 3; i++) {
        Obstacle obs(*obsTextures[rand() % 3], 100 + rand() % 500, -200 + rand() % 200);
        obstacles.push_back(obs);
    }

    Police police(policeTexture, 150 + rand() % 400, 0);
    int score = 0;
    bool gameOverFlag = false;

    sf::Font font;
    font.loadFromFile("Burbank Big Condensed Font.otf");
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(25);
    scoreText.setFillColor(sf::Color::Black);

    sf::Clock clock;
    sf::Clock scoreClock; 
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) { // Restart feature
                score = 0;
                gameOverFlag = false;
                player.sprite.setPosition(400, 450);
                for (auto& o : obstacles) o.reset();
                police.reset();
                scoreClock.restart(); // Reset score timer
            }
            if (!gameOverFlag) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Right) player.dx = 5;
                    if (event.key.code == sf::Keyboard::Left)  player.dx = -5;
                } else if (event.type == sf::Event::KeyReleased) {
                    if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::Left) player.dx = 0;
                }
            }
        }

        player.update();
        if (!gameOverFlag) {
            for (auto& o : obstacles) {
                o.update();
                if (o.sprite.getPosition().y > displayHeight && !o.counted) {
                    score++;
                    o.counted = true;
                }
                if (player.sprite.getGlobalBounds().intersects(o.sprite.getGlobalBounds())) {
                    score -= 2;
                    o.reset();
                }
            }

            police.update();
            if (player.sprite.getGlobalBounds().intersects(police.sprite.getGlobalBounds())) {
                gameOverFlag = true;
            }

            // Increment score every 500ms
            if (scoreClock.getElapsedTime().asMilliseconds() >= 500) {
                score++;
                scoreClock.restart();
            }
        }

        scoreText.setString("SCORE: " + std::to_string(score));

        window.clear();
        window.draw(background);

        for (auto& o : obstacles)
            window.draw(o.sprite);

        window.draw(police.sprite);
        window.draw(player.sprite);
        window.draw(scoreText);

        if (gameOverFlag)
            window.draw(gameOver);

        window.display();
        sf::sleep(sf::milliseconds(16)); // ~60 FPS
    }

    return 0;
}
