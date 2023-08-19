#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <unordered_set>

#define PI 3.14159f

struct Intersect
{
    bool result;
    float t;
    sf::Vector2f pos;

    Intersect() {};
    Intersect(bool r, float tVal, sf::Vector2f p)
    {
        result = r;
        t = tVal;
        pos = p;
    }
};

Intersect LineIntersect(sf::Vertex a, sf::Vertex b, sf::Vertex c, sf::Vertex d)
{
    sf::Vector2f r = (b.position - a.position);
    sf::Vector2f s = (d.position - c.position);
    float rXs = (r.x * s.y) - (r.y * s.x);
    sf::Vector2f cMa = c.position - a.position;
    float t = ((cMa.x * s.y) - (cMa.y * s.x)) / rXs;
    float u = ((cMa.x * r.y) - (cMa.y * r.x)) / rXs;

    if (t >= 0 and u >=0 and u <= 1)
    {
        return Intersect(true, t, sf::Vector2f(a.position.x + t*r.x, a.position.y + t*r.y));
    }

    return Intersect(false, -1, sf::Vector2f(0, 0));
}

//Intersect LineIntersect(sf::Vertex a, sf::Vertex b, sf::Vertex c, sf::Vertex d)
//{
//    float r_px = a.position.x; 
//    float r_dx = b.position.x - a.position.x;
//    float s_px = c.position.x;
//    float s_dx = d.position.x - c.position.x; 
//
//    float r_py = a.position.y;
//    float r_dy = b.position.y - a.position.y; 
//    float s_py = c.position.y;
//    float s_dy = d.position.y - c.position.y; 
//
//    float T2 = (r_dx*(s_py - r_py) + r_dy*(r_px-s_px)) / (s_dx*r_dy - s_dy*r_dx);
//    float T1 = (s_px + s_dx * T2 - r_px) / r_dx;
//
//    // Parrallel check
//    float r_mag = sqrt(pow(r_dx, 2) + pow(r_dy, 2));
//    float s_mag = sqrt(pow(s_dx, 2) + pow(s_dy, 2));
//    if (r_dx/r_mag == s_dx/s_mag and r_dy/r_mag == s_dy/s_mag) { return Intersect(false, -1, sf::Vector2f(0, 0)); }
//
//    if (T1 > 0 and T2 > 0 and T2 < 1)
//    {
//        //std::cout << "T1: " << T1 << std::endl;
//        return Intersect(true, T1, sf::Vector2f(r_px + T1*r_dx, r_py + T1*r_dy));
//    }
//
//    return Intersect(false, -1, sf::Vector2f(0, 0));
//}
//

bool compareAngle(const sf::VertexArray& v1, const sf::VertexArray& v2)
{
    sf::Vertex p1 = v1[1];
    sf::Vertex p2 = v2[1];
    float a1 = atan(p1.position.y / p1.position.x);
    float a2 = atan(p2.position.y / p2.position.x);

    return a1 < a2;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "RayCaster Demo");


    std::vector<std::tuple<sf::Vertex, sf::Vertex>> lineSegments;
    std::vector<sf::Vertex> vertexPoints;

    // polygons
    sf::Vertex line[] =
        {

            sf::Vertex(sf::Vector2f(30, 300)),
            sf::Vertex(sf::Vector2f(400, 50)),
            sf::Vertex(sf::Vector2f(200, 400)),
            sf::Vertex(sf::Vector2f(30, 300))

        };

    sf::Vertex line2[] =
        {

            sf::Vertex(sf::Vector2f(500, 500)),
            sf::Vertex(sf::Vector2f(500, 590)),
            sf::Vertex(sf::Vector2f(750, 350)),
            sf::Vertex(sf::Vector2f(500, 500))
        };

    // bounds
    sf::Vertex bounds[] =
        {
            sf::Vertex(sf::Vector2f(0, 0)),
            sf::Vertex(sf::Vector2f(window.getSize().x, 0)),
            sf::Vertex(sf::Vector2f(window.getSize().x, window.getSize().y)),
            sf::Vertex(sf::Vector2f(0, window.getSize().y)),
            sf::Vertex(sf::Vector2f(0, 0))
        };

    for (int i=0;  i < 4; ++i)
    {
        line[i].color = sf::Color::White;
        vertexPoints.push_back(line[i]);
        lineSegments.push_back(std::make_pair(line[i], line[(i + 1) % 4]));
    }

    for (int i=0;  i < 4; ++i)
    {
        line2[i].color = sf::Color::White;
        vertexPoints.push_back(line2[i]);
        lineSegments.push_back(std::make_pair(line2[i], line2[(i + 1) % 4]));
    }

    for (int i=0;  i < 5; ++i)
    {
        bounds[i].color = sf::Color::White;
        vertexPoints.push_back(bounds[i]);
        lineSegments.push_back(std::make_pair(bounds[i], bounds[(i + 1) % 5]));
    }

    // debug text
    sf::Font font;
    if (!font.loadFromFile("fonts/tech.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
    }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(12);
    text.setColor(sf::Color::Black);

    float wiggle[3] = {-0.00001f, 0, 0.00001f};

    sf::Event event;
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        sf::Vector2f pos(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

        std::string debugString = "X: " + std::to_string((int)pos.x) + " Y: " + std::to_string((int)pos.y);
        text.setString(debugString);

        // create rays
        sf::Vertex centre(pos);
        std::vector<sf::VertexArray> rays;

        std::unordered_set<std::string> seen;
        for (int i=0; i < 360; i+=(360/100))
        {
            for (auto dTheta : wiggle)
            {
                float bestT = std::numeric_limits<float>::max();
                sf::Vector2f intersectPoint;
                // degree to radians conversion
                float a = (i*PI) / 180;
                sf::Vertex dir(sf::Vector2f(centre.position.x + cos(a), centre.position.y + sin(a)));

                for (auto t : lineSegments)
                {
                    sf::Vertex p1 = std::get<0>(t);
                    sf::Vertex p2 = std::get<1>(t);

                    Intersect r = LineIntersect(centre, dir, p1, p2);
                    if (r.result and r.t < bestT)
                    {
                        intersectPoint = r.pos;
                        bestT = r.t;
                    }
                }

                sf::VertexArray ray(sf::Lines, 2);
                ray[0] = sf::Vertex(centre);
                ray[1] = sf::Vertex(intersectPoint);

                ray[0].color = sf::Color::White;
                ray[1].color = sf::Color::White;

                rays.push_back(ray);
            }
        }
        std::sort(rays.begin(), rays.end(), compareAngle);

        // render
        window.clear(sf::Color::Black);
        window.draw(line, 4, sf::LinesStrip);
        window.draw(line2, 4, sf::LinesStrip);

        for (auto &ray : rays)
        {
           window.draw(ray);
        }

        window.draw(text);
        window.display();
    }

    return 0;
}