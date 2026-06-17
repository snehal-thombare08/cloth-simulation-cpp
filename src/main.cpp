#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>

const int WIDTH = 1200, HEIGHT = 800;
const int CLOTH_W = 60, CLOTH_H = 40;
const float SPACING = 16.f;
const float GRAVITY = 1200.f;
const float DAMPING = 0.99f;
const int CONSTRAINT_ITER = 5;
const float TEAR_DIST = SPACING * 2.8f;

struct Point {
    float x, y, px, py;
    bool pinned, active;
    Point(float x, float y, bool pinned=false)
        : x(x), y(y), px(x), py(y), pinned(pinned), active(true) {}
};

struct Stick {
    int a, b;
    float length;
    bool active;
    Stick(int a, int b, float len) : a(a), b(b), length(len), active(true) {}
};

std::vector<Point> points;
std::vector<Stick> sticks;

void createCloth() {
    points.clear(); sticks.clear();
    float startX = (WIDTH - (CLOTH_W-1)*SPACING) / 2.f;
    float startY = 80.f;
    for(int y=0; y<CLOTH_H; y++)
        for(int x=0; x<CLOTH_W; x++) {
            bool pin = (y==0) && (x % 8 == 0);
            points.emplace_back(startX + x*SPACING, startY + y*SPACING, pin);
        }
    for(int y=0; y<CLOTH_H; y++)
        for(int x=0; x<CLOTH_W; x++) {
            if(x<CLOTH_W-1) sticks.emplace_back(y*CLOTH_W+x, y*CLOTH_W+x+1, SPACING);
            if(y<CLOTH_H-1) sticks.emplace_back(y*CLOTH_W+x, (y+1)*CLOTH_W+x, SPACING);
        }
}

void update(float dt, float windX) {
    for(auto& p : points) {
        if(!p.active || p.pinned) continue;
        float vx = (p.x - p.px) * DAMPING;
        float vy = (p.y - p.py) * DAMPING;
        p.px = p.x; p.py = p.y;
        p.x += vx + windX * dt;
        p.y += vy + GRAVITY * dt * dt;
        if(p.y > HEIGHT-5) { p.y = HEIGHT-5; p.py = p.y + vy*0.3f; }
        if(p.x < 0) { p.x = 0; }
        if(p.x > WIDTH) { p.x = (float)WIDTH; }
    }
    for(int iter=0; iter<CONSTRAINT_ITER; iter++) {
        for(auto& s : sticks) {
            if(!s.active) continue;
            auto& a = points[s.a]; auto& b = points[s.b];
            float dx = b.x-a.x, dy = b.y-a.y;
            float dist = std::sqrt(dx*dx+dy*dy);
            if(dist > TEAR_DIST) { s.active=false; continue; }
            if(dist < 0.0001f) continue;
            float diff = (dist - s.length) / dist * 0.5f;
            float ox = dx*diff, oy = dy*diff;
            if(!a.pinned) { a.x+=ox; a.y+=oy; }
            if(!b.pinned) { b.x-=ox; b.y-=oy; }
        }
    }
}

void tearCloth(float mx, float my, float radius) {
    for(auto& s : sticks) {
        if(!s.active) continue;
        auto& a = points[s.a]; auto& b = points[s.b];
        float cx=(a.x+b.x)*0.5f, cy=(a.y+b.y)*0.5f;
        float dx=cx-mx, dy=cy-my;
        if(dx*dx+dy*dy < radius*radius) s.active=false;
    }
}

void grabCloth(float mx, float my, float radius, float fx, float fy) {
    for(auto& p : points) {
        if(!p.active || p.pinned) continue;
        float dx=p.x-mx, dy=p.y-my;
        float dist=std::sqrt(dx*dx+dy*dy);
        if(dist < radius) {
            float influence = 1.f - dist/radius;
            p.x += fx * influence;
            p.y += fy * influence;
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned)WIDTH,(unsigned)HEIGHT}),
        "Cloth Simulation | LMB: Grab | RMB: Tear | W: Wind | S: Scissors | R: Reset");
    window.setFramerateLimit(60);

    createCloth();

    float windX = 0.f, windTarget = 0.f;
    bool windOn = false, scissorMode = false, lmbHeld = false;

    sf::Font font;
    bool hasFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    sf::Text hud(font);
    hud.setCharacterSize(15);
    hud.setFillColor(sf::Color::White);
    hud.setOutlineColor(sf::Color::Black);
    hud.setOutlineThickness(1.5f);
    hud.setPosition({10.f, 10.f});

    sf::Clock clock;
    sf::Clock totalClock;
    sf::Vector2f lastMouse;

    while(window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.033f);
        auto mpos = sf::Mouse::getPosition(window);
        sf::Vector2f mouse((float)mpos.x, (float)mpos.y);

        while(auto ev = window.pollEvent()) {
            if(ev->is<sf::Event::Closed>()) window.close();
            if(auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if(k->code == sf::Keyboard::Key::Escape) window.close();
                if(k->code == sf::Keyboard::Key::R) createCloth();
                if(k->code == sf::Keyboard::Key::W) {
                    windOn = !windOn;
                    windTarget = windOn ? 15000.f : 0.f;
                }
                if(k->code == sf::Keyboard::Key::S) scissorMode = !scissorMode;
                if(k->code == sf::Keyboard::Key::G)
                    for(auto& p : points) p.pinned = false;
            }
            if(auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if(mb->button == sf::Mouse::Button::Left) lmbHeld = true;
                if(mb->button == sf::Mouse::Button::Right)
                    tearCloth(mouse.x, mouse.y, 40.f);
            }
            if(auto* mb = ev->getIf<sf::Event::MouseButtonReleased>())
                if(mb->button == sf::Mouse::Button::Left) lmbHeld = false;
        }

        windX += (windTarget - windX) * 3.f * dt;
        if(windOn)
            windTarget = 14000.f + std::sin(totalClock.getElapsedTime().asSeconds()*1.3f)*5000.f;

        if(lmbHeld) {
            sf::Vector2f delta = mouse - lastMouse;
            if(scissorMode)
                tearCloth(mouse.x, mouse.y, 25.f);
            else
                grabCloth(mouse.x, mouse.y, 60.f, delta.x*8.f, delta.y*8.f);
        }
        lastMouse = mouse;

        update(dt, windX);

        window.clear(sf::Color(20, 20, 35));

        // Draw cloth lines
        sf::VertexArray lines(sf::PrimitiveType::Lines);
        for(auto& s : sticks) {
            if(!s.active) continue;
            auto& a = points[s.a]; auto& b = points[s.b];
            float dx=b.x-a.x, dy=b.y-a.y;
            float strain = std::sqrt(dx*dx+dy*dy) / s.length;
            float t = std::min(1.f, (strain - 1.f) * 5.f);
            uint8_t r = (uint8_t)(100 + t * 155);
            uint8_t g2 = (uint8_t)(180 - t * 160);
            uint8_t b2 = (uint8_t)(255 - t * 200);
            sf::Color col(r, g2, b2, 230);
            lines.append(sf::Vertex{sf::Vector2f(a.x, a.y), col});
            lines.append(sf::Vertex{sf::Vector2f(b.x, b.y), col});
        }
        window.draw(lines);

        // Draw pins
        for(auto& p : points) {
            if(p.pinned) {
                sf::CircleShape pin(6.f);
                pin.setFillColor(sf::Color(255, 80, 80));
                pin.setOutlineColor(sf::Color::White);
                pin.setOutlineThickness(1.5f);
                pin.setPosition({p.x-6.f, p.y-6.f});
                window.draw(pin);
            }
        }

        if(hasFont) {
            hud.setString(
                "LMB: Grab  | RMB: Tear  | W: Wind " +
                std::string(windOn?"(ON) ":"(OFF)") +
                " | S: Scissors " + std::string(scissorMode?"(ON) ":"(OFF)") +
                " | G: Unpin All | R: Reset | Esc: Quit"
            );
            window.draw(hud);
        }

        window.display();
    }
    return 0;
}
