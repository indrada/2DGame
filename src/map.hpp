#ifndef worldMap_h
#define worldMap_h
#include <map>
#include <vector>
#include <string>
#include <math.h>
#include <random>
#include "util.hpp"
#include <algorithm>
#include <SFML/graphics.hpp>

class person;

class tile
{
public:
    int xPos;
    int yPos;
    float elevation;
    float waterLevel;
    person* personHere;
    std::vector<float> resourceQuantity;
    inline float waterDepth()
    {
        return waterLevel - elevation;
    }
    tile()
    {

    }
    tile(int x, int y, float elevation = 0.0f, float waterLevel = 0.0f, person* personHere = NULL)
    {
        xPos = x;
        yPos = y;
        this->elevation = elevation;
        this->personHere = personHere;
        this->waterLevel = waterLevel;
    }
};

class worldMap
{
public:
    tile* mapTiles;
    int horizontalSize;
    int verticalSize;
    std::vector<std::string> resourceNames;
    std::vector<int> shuffledIndices;
    worldMap(int horizontalSize, int verticalSize, float terrainElevation = 5.0f)
    {
        this->mapTiles = (tile*)malloc(horizontalSize * verticalSize * sizeof(tile));
        for (int i = 0; i < verticalSize * horizontalSize; i++)
        {
            new (mapTiles + i) tile(i%horizontalSize,i/horizontalSize);
        }
        this->horizontalSize = horizontalSize;
        this->verticalSize = verticalSize;
        generateTerrain(terrainElevation);
        resourceNames = {};
        for (int i = 0; i < verticalSize * horizontalSize; i++)
        {
            mapTiles[i].waterLevel = mapTiles[i].elevation;
            shuffledIndices.push_back(i);
        }
    }

    float generateResourceQuantity(float abundance, bool isClustered)
    {
        if (!isClustered) return RandomNumber(0, abundance);
    }

    void generateTerrain(float terrainHeight = 5.0f)
    {
        float* gradients = (float*)malloc(sizeof(float) * (verticalSize - 1) * (horizontalSize - 1) * 2);
        gradients[0] = RandomNumber(-terrainHeight, terrainHeight);
        gradients[1] = RandomNumber(-terrainHeight, terrainHeight);
        mapTiles[0].elevation = 0.0f;
        for (int i = 1; i < horizontalSize - 1; i++)
        {
            gradients[2 * i] = gradients[2 * (i - 1)] + RandomNumber(-terrainHeight, terrainHeight);
            gradients[2 * i + 1] = gradients[2 * (i - 1) + 1] + RandomNumber(-terrainHeight, terrainHeight);
            mapTiles[i].elevation = gradients[2 * (i - 1) + 1] + mapTiles[i - 1].elevation;
        }
        for (int j = 1; j < verticalSize - 1; j++)
        {
            gradients[2 * j * (horizontalSize - 1)] = gradients[2 * (j - 1) * (horizontalSize - 1)] + RandomNumber(-terrainHeight, terrainHeight);
            gradients[2 * j * (horizontalSize - 1) + 1] = gradients[2 * (j - 1) * (horizontalSize - 1) + 1] + RandomNumber(-terrainHeight, terrainHeight);
            mapTiles[j * horizontalSize].elevation = gradients[2 * (j - 1) * (horizontalSize - 1)] + mapTiles[(j - 1) * horizontalSize].elevation;

        }
        for (int i = 1; i < horizontalSize - 1; i++)
        {
            for (int j = 1; j < verticalSize - 1; j++)
            {

                gradients[2 * (j * (horizontalSize - 1) + i)] = RandomNumber(-terrainHeight, terrainHeight) + (gradients[2 * ((j - 1) * (horizontalSize - 1) + i)] + gradients[2 * (j * (horizontalSize - 1) + i - 1)]) / 2;

                gradients[2 * (j * (horizontalSize - 1) + i) + 1] = RandomNumber(-terrainHeight, terrainHeight) + (gradients[2 * ((j - 1) * (horizontalSize - 1) + i) + 1] + gradients[2 * (j * (horizontalSize - 1) + i - 1) + 1]) / 2;

                mapTiles[j * horizontalSize + i].elevation = (mapTiles[(j - 1) * horizontalSize + i].elevation + mapTiles[j * horizontalSize + i - 1].elevation + gradients[2 * ((j - 1) * (horizontalSize - 1) + i)] + gradients[2 * (j * (horizontalSize - 1) + i - 1) + 1]) / 2;

            }

            mapTiles[(verticalSize - 1) * horizontalSize + i].elevation = mapTiles[(verticalSize - 2) * horizontalSize + i].elevation + gradients[2 * ((verticalSize - 2) * (horizontalSize - 1) + i) + 1];

        }
        for (int j = 1; j < verticalSize - 1; j++)
        {

            mapTiles[((horizontalSize - 1) + horizontalSize * j)].elevation = mapTiles[((horizontalSize - 2) + horizontalSize * j)].elevation + gradients[2 * ((horizontalSize - 1) * j + horizontalSize - 2) + 1];
        }
        mapTiles[horizontalSize * verticalSize - 1].elevation = (mapTiles[horizontalSize * verticalSize - 2].elevation + mapTiles[horizontalSize * (verticalSize - 1) - 1].elevation) / 2;
        free(gradients);
    }





    tile* tileAt(int x, int y)
    {
        return mapTiles + (y * horizontalSize + x);
    }

    float maxElevation()
    {
        float retval = 0.0f;
        for (int i = 0; i < horizontalSize * verticalSize; i++)
        {
            if (abs(mapTiles[i].elevation) > retval) retval = abs(mapTiles[i].elevation);
        }
        return retval;
    }

    float maxResourceValue(int resourceIndex)
    {
        if (resourceIndex >= resourceNames.size())
        {
            return 0;
        }
        float retval = 0.0f;
        for (int i = 0; i < horizontalSize * verticalSize; i++)
        {
            if (mapTiles[i].resourceQuantity.at(resourceIndex) > retval)
            {
                retval = mapTiles[i].resourceQuantity.at(resourceIndex);
            }
        }
        return retval;
    }
};

class attribute
{
    public:
        int health;
};

class task;

class person
{
    public:
        int xPos;
        int yPos;
        std::vector<attribute> attributes;
        std::vector<task> tasks;
        worldMap * attachedMap;
        person();
        person(int xPos, int yPos, worldMap * attachedMap)
        {
            this->attachedMap = attachedMap;
            this->xPos = xPos;
            this->yPos = yPos;
        }

};

typedef enum direction
{
    NORTH,
    SOUTH,
    EAST,
    WEST
}direction;

class task
{
    public:
        int timeToComplete;
        person* assignedPerson;
        task() 
        {

        }
        task(int timeToComplete, person* assignedPerson)
        {
            this->timeToComplete = timeToComplete;
            this->assignedPerson = assignedPerson;
        }
        void progressTime(int timeToProgress)
        {
            timeToComplete -= timeToProgress;
            if (timeToComplete <= 0)
            {
                doTask();
            }
        }
        virtual void doTask() = 0;
};

class moveTask : virtual public task
{
    public:
        int timeToComplete;
        person* assignedPerson;
        direction directionToMove;

        moveTask(int timeToComplete, person* assignedPerson, direction directionToMove)
        {
            this->directionToMove = directionToMove;
            this->timeToComplete = timeToComplete;
            this->assignedPerson = assignedPerson;

        }

        void doTask() override
        {

            switch (directionToMove)
            {
            case NORTH:
                if (assignedPerson->yPos == 0) return;
                assignedPerson->attachedMap->tileAt(assignedPerson->xPos, assignedPerson->yPos)->personHere = NULL;
                assignedPerson->attachedMap->tileAt(assignedPerson->xPos, assignedPerson->yPos - 1)->personHere = assignedPerson;
                assignedPerson->yPos--;
                return;
                break;
            case SOUTH:
                if (assignedPerson->yPos == assignedPerson->attachedMap->verticalSize) return;
                assignedPerson->attachedMap->tileAt(assignedPerson->xPos, assignedPerson->yPos)->personHere = NULL;
                assignedPerson->attachedMap->tileAt(assignedPerson->xPos, assignedPerson->yPos + 1)->personHere = assignedPerson;
                assignedPerson->yPos++;
                return;
                break;
            case WEST:
                if (assignedPerson->xPos == 0) return;
                assignedPerson->attachedMap->tileAt(assignedPerson->xPos, assignedPerson->yPos)->personHere = NULL;
                assignedPerson->attachedMap->tileAt(assignedPerson->xPos - 1, assignedPerson->yPos)->personHere = assignedPerson;
                assignedPerson->xPos--;
                return;
                break;
            case EAST:
                if (assignedPerson->xPos == assignedPerson->attachedMap->verticalSize) return;
                assignedPerson->attachedMap->tileAt(assignedPerson->xPos, assignedPerson->yPos)->personHere = NULL;
                assignedPerson->attachedMap->tileAt(assignedPerson->xPos + 1, assignedPerson->yPos)->personHere = assignedPerson;
                assignedPerson->xPos++;
                return;
                break;
            default:
                return;
            }
        }
};











class mapMode
{
    public:
        virtual sf::Color getTileColor(int x, int y, worldMap toDisplay) = 0;
};

class elevationMap : virtual public mapMode
{
    public:
        sf::Color getTileColor(int x, int y, worldMap toDisplay) override
        {
            unsigned int colorScale = (unsigned int) (127 * (1+toDisplay.mapTiles[y*toDisplay.horizontalSize + x].elevation/(1+toDisplay.maxElevation())));
            return sf::Color(colorScale,colorScale,colorScale);
        }
};

class defaultMap : virtual public mapMode
{
    public:
        sf::Color getTileColor(int x, int y, worldMap toDisplay) override
        {
            if(toDisplay.tileAt(x,y)->personHere!=NULL) return sf::Color(0, 255, 0);
            if(toDisplay.mapTiles[y*toDisplay.horizontalSize + x].waterDepth()>0.01f) return sf::Color(62,164,240);
            unsigned int colorScale = (unsigned int) (127 * (1-toDisplay.mapTiles[y*toDisplay.horizontalSize + x].elevation/(1+toDisplay.maxElevation())));
            return sf::Color(colorScale,colorScale,colorScale);
        }
    
};

class resourceMap : virtual public mapMode
{
    public:
        int resourceIndex;
        sf::Color getTileColor(int x, int y, worldMap toDisplay) override
        {
            unsigned int colorScale = (unsigned int)(127 * (1 - toDisplay.mapTiles[y * toDisplay.horizontalSize + x].resourceQuantity.at(resourceIndex) / (1 + toDisplay.maxResourceValue(resourceIndex))));
            return sf::Color(colorScale, colorScale, colorScale);
        }
        resourceMap(int resourceIndex)
        {
            this->resourceIndex = resourceIndex;
        }
};

class Resource
{
public:
    std::string name;
    float abundance;
    bool isClustered;
    float clusterFrequency;
    Resource(std::string name, float abundance, bool isClustered = true, float clusterFrequency = 0.002f)
    {
        this->name = name;
        this->abundance = abundance;
        this->isClustered = isClustered;
        this->clusterFrequency = clusterFrequency;
    }
    void registerResource(worldMap * map)
    {
        printf("%d", map->resourceNames.size());
        map->resourceNames.push_back(name);



        int resourceIndex = map->resourceNames.size() - 1;

        for (int i = 0; i < map->verticalSize * map->horizontalSize; i++)
        {
            map->mapTiles[i].resourceQuantity.push_back(0.0f);
        }

        for (int i = 0; i < map->verticalSize * map->horizontalSize; i++)
        {
            if (RandomNumber(0.0f, 1.0f) < clusterFrequency)
            {
                for (int j = 0; j < map->verticalSize * map->horizontalSize; j++)
                {
                    if (i != j)
                    {
                        (map->mapTiles[j].resourceQuantity)[resourceIndex] += abundance / (abs((i % map->horizontalSize) - (j % map->horizontalSize)) + abs((i / map->horizontalSize) - (j / map->horizontalSize)));
                    }
                    else
                    {
                        (map->mapTiles[j].resourceQuantity)[resourceIndex] += abundance * 2;
                    }
                }
            }
        }

    }
};

void addPerson(person * personToAdd)
{
    personToAdd->attachedMap->tileAt(personToAdd->xPos, personToAdd->yPos)->personHere = personToAdd;
}










#endif