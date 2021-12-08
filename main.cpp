#include <math.h>

#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <ctime>
#include <iostream>

std::vector<sf::CircleShape> copy(std::vector<sf::CircleShape> v, std::vector<int> order) {
    std::vector<sf::CircleShape> copied;

    for (int i = 0; i < v.size(); i++) {
        copied.push_back(v[order[i]]);
    }
    return copied;
}

void drawPoints(std::vector<sf::CircleShape> p, sf::RenderWindow &window) {
    for (int i = 0; i < p.size(); i++) {
        window.draw(p[i]);
    }
}

void fillVector(sf::CircleShape p, std::vector<sf::CircleShape> &pointHolder, std::vector<int> &order, sf::RenderWindow &window, int count) {
    int y;

    for (int i = 0; i < count; i++) {
        y = rand() % window.getSize().y;
        if (y < 100) y = 100;

        p.setRadius(4);
        p.setFillColor(sf::Color(255, 0, 0));
        p.setPosition(rand() % window.getSize().x, y);
        p.setOrigin(4, 4);
        pointHolder.push_back(p);
        order.push_back(i);
    }
}

int lexicOrder(std::vector<int> &order) {
    int largestI = -1;
    int largestJ = -1;

    // Step one

    for (int i = 0; i < order.size() - 1; i++) {
        if (order[i] < order[i + 1]) {
            largestI = i;
        }
    }

    if (largestI == -1) {
        return 0;
    }

    for (int j = 0; j < order.size(); j++) {
        if (order[largestI] < order[j]) {
            largestJ = j;
        }
    }

    std::iter_swap(order.begin() + largestI, order.begin() + largestJ);
    std::vector<int> endArr(order.begin() + largestI + 1, order.end());
    order.erase(order.begin() + largestI + 1, order.end());
    std::reverse(endArr.begin(), endArr.end());
    order.insert(order.end(), endArr.begin(), endArr.end());
    return 1;
}

int factorial(int n) {
    if (n == 1)
        return 1;
    else
        return n * factorial(n - 1);
}

void route(sf::RenderWindow &window, std::vector<sf::CircleShape> pointHolder, std::vector<int> order, sf::Event ev, sf::Text &best, sf::Text &perc) {
    int sum = 0;
    bool done = false;
    int bestEver = 100000;
    unsigned int count = 0;
    perc.setPosition(0, 30);
    std::vector<sf::CircleShape> bestRoute;
    unsigned long int totalPermutations = factorial(pointHolder.size());
    bool g = true;

    while (!done) {
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
            }
        }

        sum = 0;
        window.clear();

        drawPoints(pointHolder, window);

        for (int i = 0; i < pointHolder.size() - 1; i++) {
            sf::VertexArray lines(sf::LinesStrip, 2);
            lines[0].position = sf::Vector2f(pointHolder[order[i]].getPosition().x, pointHolder[order[i]].getPosition().y);
            lines[0].color = sf::Color(255, 255, 255);
            lines[1].position = sf::Vector2f(pointHolder[order[i + 1]].getPosition().x, pointHolder[order[i + 1]].getPosition().y);
            lines[1].color = sf::Color(255, 255, 255);
            window.draw(lines);

            auto v = lines[0].position - lines[1].position;
            sum += sqrt(v.x * v.x + v.y * v.y);
        }

        window.draw(perc);
        window.draw(best);
        window.display();
        window.clear();

        drawPoints(pointHolder, window);

        if (sum < bestEver) {
            bestEver = sum;
            bestRoute = copy(pointHolder, order);
            best.setString("Best: " + std::to_string(bestEver));
        }

        for (int i = 0; i < bestRoute.size() - 1; i++) {
            sf::VertexArray lines(sf::Lines, 2);
            lines[0].position = sf::Vector2f(bestRoute[i].getPosition().x, bestRoute[i].getPosition().y);
            lines[0].color = sf::Color(255, 0, 0);
            lines[1].position = sf::Vector2f(bestRoute[i + 1].getPosition().x, bestRoute[i + 1].getPosition().y);
            lines[1].color = sf::Color(255, 0, 0);
            window.draw(lines);

            auto v = lines[0].position - lines[1].position;
            sum += sqrt(v.x * v.x + v.y * v.y);
        }

        count++;

        int percent = ((float)count / totalPermutations) * 100;
        perc.setString(std::to_string(percent) + "% Completed");

        window.draw(perc);
        window.draw(best);
        window.display();

        if (lexicOrder(order) == 0) {
            done = true;
        }
        if (g==true) {
            std::this_thread::sleep_for(std::chrono::seconds(4));
            g = false;
        }
    }
}

int main() {
    srand(time(NULL));

    int width = 1000;
    int hight = 800;
    bool start = false;

    sf::Font font;
    font.loadFromFile("/path/to/your/favorite/font");  // You should add a
                                                                        // path inside the
    sf::RenderWindow window(sf::VideoMode(width, hight), "salesman");
    sf::Event ev;

    sf::Text best;
    sf::Text perc;

    best.setFont(font);
    perc.setFont(font);

    best.setCharacterSize(24);
    perc.setCharacterSize(24);

    best.setFillColor(sf::Color(255, 255, 255));
    perc.setFillColor(sf::Color(255, 255, 255));

    window.setFramerateLimit(50);

    std::vector<int> order;
    std::vector<sf::CircleShape> circleVector;
    sf::CircleShape circle;

    fillVector(circle, circleVector, order, window, 5);
    while (window.isOpen()) {
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
        }
        if (!start) {
            route(window, circleVector, order, ev, best, perc);
            start = true;
        }
    }

        return 0;
