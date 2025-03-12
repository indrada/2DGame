#ifndef water_h
#define water_h
#include "map.hpp"
class waterPopulator
{
	public:
        void rain(worldMap map, float amount = 1.0f)
        {
            for (int i = 0; i < map.verticalSize * map.horizontalSize; i++)
            {
                map.mapTiles[i].waterLevel += amount;
            }
            waterFlow(map,50, 0.1f);
        }

        void waterFlow(worldMap map, int numIter = 25, float flowAmount = 0.5f)
        {
            float totalDifference;
            float northDifference;
            float southDifference;
            float eastDifference;
            float westDifference;
            float fractionOfOutflowAvailable;
            int i, j;
            std::random_device rd;
            std::mt19937 gen(rd());
            tile* thisTile, * westTile, * southTile, * eastTile, * northTile;
            for (int iter = 0; iter < numIter; iter++)
            {
                std::shuffle(map.shuffledIndices.begin(), map.shuffledIndices.end(), gen);
                for (int k = 0; k < map.horizontalSize * map.verticalSize; k++)
                {
                    k = map.shuffledIndices.at(k);
                    i = (k % map.horizontalSize);
                    j = (k / map.horizontalSize);
                    totalDifference = 0.0f;
                    northDifference = 0.0f;
                    southDifference = 0.0f;
                    eastDifference = 0.0f;
                    westDifference = 0.0f;
                    thisTile = map.mapTiles + (j * map.horizontalSize + i);
                    westTile = map.mapTiles + (j * map.horizontalSize + i - 1);
                    southTile = map.mapTiles + ((j - 1) * map.horizontalSize + i);
                    eastTile = map.mapTiles + (j * map.horizontalSize + i + 1);
                    northTile = map.mapTiles + ((j + 1) * map.horizontalSize + i);
                    if (i != 0)
                    {
                        westDifference = std::min(westTile->waterDepth(), flowAmount * (westTile->waterLevel - thisTile->waterLevel)) * flowAmount;
                        totalDifference += westDifference;
                    }
                    if (j != 0)
                    {
                        southDifference = std::min(southTile->waterDepth(), flowAmount * (southTile->waterLevel - thisTile->waterLevel)) * flowAmount;
                        totalDifference += southDifference;
                    }
                    if (i != map.horizontalSize - 1)
                    {
                        eastDifference = std::min(eastTile->waterDepth(), flowAmount * (eastTile->waterLevel - thisTile->waterLevel));
                        totalDifference += eastDifference;
                    }
                    if (j != map.verticalSize - 1)
                    {
                        northDifference = std::min(northTile->waterDepth(), flowAmount * (northTile->waterLevel - thisTile->waterLevel));
                        totalDifference += northDifference;
                    }
                    if (-totalDifference > thisTile->waterDepth())
                    {
                        fractionOfOutflowAvailable = -thisTile->waterDepth() / totalDifference;
                        totalDifference = -thisTile->waterDepth();
                        westDifference *= fractionOfOutflowAvailable;
                        southDifference *= fractionOfOutflowAvailable;
                        eastDifference *= fractionOfOutflowAvailable;
                        northDifference *= fractionOfOutflowAvailable;
                    }
                    if (i != 0)
                    {
                        equalize(thisTile, westTile, westDifference);
                    }
                    if (j != 0)
                    {
                        equalize(thisTile, southTile, southDifference);
                    }
                    if (i != map.horizontalSize - 1)
                    {
                        equalize(thisTile, eastTile, eastDifference);
                    }
                    if (j != map.verticalSize - 1)
                    {
                        equalize(thisTile, northTile, northDifference);
                    }
                }


            }
        }

        inline void equalize(tile* tile1, tile* tile2, float flowAmount)
        {
            tile1->waterLevel += flowAmount;
            tile2->waterLevel -= flowAmount;
        }
};

#endif