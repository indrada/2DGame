#include "map.hpp"
#include "water.hpp"
#include <ctime>

void drawMap(sf::RenderWindow& win, worldMap toDisplay, mapMode& mode){
    auto size = win.getView().getSize();
    float rowH = size.y/toDisplay.verticalSize;
    float colW = size.x/toDisplay.horizontalSize;
    sf::RectangleShape * rectangles = (sf::RectangleShape *) malloc(toDisplay.horizontalSize*toDisplay.verticalSize*sizeof(sf::RectangleShape));
    for(int i = 0; i < toDisplay.verticalSize; i++)
    {
        for(int j = 0; j < toDisplay.horizontalSize; j++)
        {
            new (rectangles + i*toDisplay.horizontalSize + j) sf::RectangleShape({colW,rowH});
            (*(rectangles + i*toDisplay.horizontalSize + j)).setPosition({colW*j,rowH*i});
            (*(rectangles + i*toDisplay.horizontalSize + j)).setFillColor(mode.getTileColor(j,i,toDisplay));
        }
    }
    
    for(int i = 0; i < toDisplay.horizontalSize * toDisplay.verticalSize; i++)
    {
        win.draw(rectangles[i]);
    }
}

std::string getInfoString(worldMap map, sf::Vector2f size)
{
    auto mousePos = sf::Mouse::getPosition();
    int xPos = (int) (map.horizontalSize * mousePos.x/size.x);
    int yPos = (int)(map.verticalSize * mousePos.y / size.y);
    tile currentTile = map.mapTiles[xPos + yPos * map.horizontalSize];
    std::string retval = "Elevation: " + std::to_string(currentTile.elevation);
    retval += "\nWater Depth: " + std::to_string(currentTile.waterDepth());
    if (map.resourceNames.size() > 0)
    {
        retval += "\nResources:";
        for (int i = 0; i < map.resourceNames.size(); i++)
        {
            retval += "\n  " + map.resourceNames.at(i) +": " + std::to_string(currentTile.resourceQuantity.at(i));
        }
    }
    return retval;
}

int main()
{
    srand(time(0));
    auto window = sf::RenderWindow(sf::VideoMode::getFullscreenModes()[0], "Test1",sf::Style::None);
    window.setFramerateLimit(144);
    worldMap myMap(100,100,10.0f);
    rain(myMap, 0.5f);
    Resource iron("iron", 1.0f);
    iron.registerResource(&myMap);
    person newPerson(20, 20, &myMap);
    addPerson(&newPerson);
    mapMode * myMapMode = new defaultMap();
    auto size = window.getView().getSize();
    sf::Font font("Assets/arial.ttf");
    sf::Text infoOverlay(font);
    infoOverlay.setCharacterSize(120);
    infoOverlay.setFillColor(sf::Color::Red);
    infoOverlay.setStyle(sf::Text::Bold);
    infoOverlay.setPosition({0.75f*size.x,0.0f});
    infoOverlay.setScale({0.25f,0.25f});
    std::string infoString = "";
    infoOverlay.setString(infoString);
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        drawMap(window, myMap, *myMapMode);
        
        infoString = getInfoString(myMap,size);
        infoOverlay.setString(infoString);
        window.draw(infoOverlay);
        window.display();
        window.clear();

    }
}

