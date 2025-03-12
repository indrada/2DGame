#include "map.hpp"
#include "water.hpp"
#include <ctime>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>

sf::RectangleShape * drawMap(sf::RenderWindow& win, worldMap toDisplay, mapMode& mode){
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
    return rectangles;
}

void updateRectangle(worldMap toDisplay, mapMode& mode, sf::RectangleShape* rectangles, int xPos, int yPos)
{
    int index = xPos + yPos * toDisplay.horizontalSize;
    rectangles[index].setFillColor(mode.getTileColor(xPos, yPos, toDisplay));
}

void drawRectangles(sf::RenderWindow& win, worldMap toDisplay, sf::RectangleShape* rectangles)
{
    for (int i = 0; i < toDisplay.horizontalSize * toDisplay.verticalSize; i++)
    {
        win.draw(rectangles[i]);
    }
}

tile* getTileAtMousePosition(worldMap map, sf::Vector2f size)
{

    auto mousePos = sf::Mouse::getPosition();
    int xPos = (int)(map.horizontalSize * mousePos.x / size.x);
    int yPos = (int)(map.verticalSize * mousePos.y / size.y);
    return map.tileAt(xPos, yPos);
}

std::string getInfoString(worldMap map, tile currentTile)
{
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
    person * newPerson= new person(20, 20, &myMap);
    addPerson(newPerson);
    mapMode * myMapMode = new defaultMap();
    auto size = window.getView().getSize();
    sf::Font font("Assets/arial.ttf");
    sf::Text infoOverlay(font);
    infoOverlay.setCharacterSize(120);
    infoOverlay.setFillColor(sf::Color::White);
    infoOverlay.setOutlineColor(sf::Color::Black);
    infoOverlay.setOutlineThickness(3);
    infoOverlay.setStyle(sf::Text::Bold);
    infoOverlay.setPosition({0.75f*size.x,0.0f});
    infoOverlay.setScale({0.25f,0.25f});
    std::string infoString = "";
    infoOverlay.setString(infoString);
    tile hoveredTile = *getTileAtMousePosition(myMap, size);
    tile selectedTile;
    person* selectedPerson = NULL;
    
    sf::RectangleShape* mapRectangles = drawMap(window, myMap, *myMapMode);
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::MouseButtonPressed>()) {
                if (event->getIf< sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                    selectedTile = *getTileAtMousePosition(myMap, size);
                    selectedPerson = selectedTile.personHere;
                    printf("Selected Tile at %d, %d\n", selectedTile.xPos, selectedTile.yPos);  
                }
            }

        }
        if (selectedPerson != NULL)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            {
                (new moveTask(0, selectedPerson, WEST))->doTask();
                updateRectangle(myMap, *myMapMode, mapRectangles, selectedPerson->xPos+1, selectedPerson->yPos);
                updateRectangle(myMap, *myMapMode, mapRectangles, selectedPerson->xPos, selectedPerson->yPos);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            {
                (new moveTask(0, selectedPerson, EAST))->doTask();
                updateRectangle(myMap, *myMapMode, mapRectangles, selectedPerson->xPos - 1, selectedPerson->yPos);
                updateRectangle(myMap, *myMapMode, mapRectangles, selectedPerson->xPos, selectedPerson->yPos);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            {
                (new moveTask(0, selectedPerson, NORTH))->doTask();
                updateRectangle(myMap, *myMapMode, mapRectangles, selectedPerson->xPos, selectedPerson->yPos + 1);
                updateRectangle(myMap, *myMapMode, mapRectangles, selectedPerson->xPos, selectedPerson->yPos);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            {
                (new moveTask(0, selectedPerson, SOUTH))->doTask();
                updateRectangle(myMap, *myMapMode, mapRectangles, selectedPerson->xPos, selectedPerson->yPos - 1);
                updateRectangle(myMap, *myMapMode, mapRectangles, selectedPerson->xPos, selectedPerson->yPos);
            }
        }

        drawRectangles(window, myMap, mapRectangles);
        hoveredTile = *getTileAtMousePosition(myMap, size);
        infoString = getInfoString(myMap,hoveredTile);
        infoOverlay.setString(infoString);
        window.draw(infoOverlay);
        window.display();
        window.clear();

    }
}

