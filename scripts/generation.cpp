//_ CRUSADIA - World Generation CPP FILE _//_ COPYRIGHT (C) 2024 JFLX STUDIO - ALL RIGHTS RESERVED _//

#include <iostream>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <memory>
#include <cmath>
#include <algorithm>

#include <JFLX/logging.hpp>
#include <JFLX/perlinNoise.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <STB/stb_image_write.h>

#include "tiles.hpp"
#include "colorStruct.hpp"

namespace fs = std::filesystem;

std::string path = fs::current_path().string() + "/";

void generateAndSaveNoisePreviewImage(int* perlinNoiseMap, std::string imageName, int sizeX, int sizeY, const std::string& worldName) {
    uint8_t* img = new uint8_t[sizeX * sizeY * 3];

    for (int i = 0; i < sizeX * sizeY; ++i) {
        int v = perlinNoiseMap[i];

        // if your noise is (-1..1), convert:
        // int g = int((v + 1.0f) * 0.5f * 255);
        // If already 0..255:
        int g = std::clamp(v, 0, 255);

        img[i * 3 + 0] = g;
        img[i * 3 + 1] = g;
        img[i * 3 + 2] = g;
    }

    std::string pngPath = path + "/worlds/" + worldName + "/worldData/" + imageName + "Preview.png";
    if (!stbi_write_png(pngPath.c_str(), sizeX, sizeY, 3, img, sizeX * 3)) {
        JFLX::log("World Generation: ", "Failed to save PNG!", JFLX::LOGTYPE::ERROR);
    } else {
        JFLX::log("World Generation: ", "Saved " + imageName + " preview image (.png)", JFLX::LOGTYPE::SUCCESS);
    }
}

void generateAndSaveTilePreviewImage(Tile* tileMap, int sizeX, int sizeY, const std::string& worldName) {
    uint8_t* img = new uint8_t[sizeX * sizeY * 3];

    for (int i = 0; i < sizeX * sizeY; ++i) {
        Tile t = tileMap[i];
        if (t.blockID == TILES::BLOCKS::AIR) {
            img[i * 3 + 0] = std::clamp((TILES::COLORVALUES::COLORS[t.wallID].c_r - 25), 0, 255);;
            img[i * 3 + 1] = std::clamp((TILES::COLORVALUES::COLORS[t.wallID].c_g - 25), 0, 255);;
            img[i * 3 + 2] = std::clamp((TILES::COLORVALUES::COLORS[t.wallID].c_b - 25), 0, 255);;
            continue;
        } else {
            img[i * 3 + 0] = TILES::COLORVALUES::COLORS[t.blockID].c_r;
            img[i * 3 + 1] = TILES::COLORVALUES::COLORS[t.blockID].c_g;
            img[i * 3 + 2] = TILES::COLORVALUES::COLORS[t.blockID].c_b;
            continue;
        }
    }

    std::string pngPath = path + "/worlds/" + worldName + "/worldData/tilePreview.png";
    if (!stbi_write_png(pngPath.c_str(), sizeX, sizeY, 3, img, sizeX * 3)) {
        JFLX::log("World Generation: ", "Failed to save PNG!", JFLX::LOGTYPE::ERROR);
    } else {
        JFLX::log("World Generation: ", "Saved tileMap preview image (.png)", JFLX::LOGTYPE::SUCCESS);
    }
}


void fillAreaRecursion(Tile* tileMap, int* noiseMap, int targetX, int targetY, int limitMin, int limitMax, int sizeX, int sizeY, uint16_t blockID, uint16_t wallID = TILES::WALLS::NOCHANGE, bool replaceAir = false, bool airWithWalls = true, std::array<bool,4> directions = {true, true, true, true}) {
    if (targetX < 0 || targetX >= sizeX || targetY < 0 || targetY >= sizeY) {
        return;
    }

    int index = targetY * sizeX + targetX;
    uint16_t targetBlockID = tileMap[index].blockID;
    uint16_t targetWallID = tileMap[index].wallID;
    int noiseValue = noiseMap[index];


    if ((noiseValue < limitMin) || (noiseValue > limitMax)) {
        return;
    }

    if (!replaceAir && targetBlockID == TILES::BLOCKS::AIR || replaceAir && airWithWalls && targetWallID == wallID || targetBlockID == blockID) {
        return;
    }

    if (replaceAir) {
        if (airWithWalls) {
            if (targetBlockID == TILES::BLOCKS::AIR) {
                setTileIDS(tileMap[index], TILES::BLOCKS::NOCHANGE, wallID);
            } else {
                setTileIDS(tileMap[index], blockID);
            }
        } else {
            setTileIDS(tileMap[index], blockID);
        }
    } else {
        setTileIDS(tileMap[index], blockID);
    }

    // Recursively fill neighboring tiles
    if (directions[2]) {fillAreaRecursion(tileMap, noiseMap, targetX + 1, targetY, limitMin, limitMax, sizeX, sizeY, blockID, wallID, replaceAir, airWithWalls, directions);} // Right
    if (directions[1]) {fillAreaRecursion(tileMap, noiseMap, targetX - 1, targetY, limitMin, limitMax, sizeX, sizeY, blockID, wallID, replaceAir, airWithWalls, directions);} // Left
    if (directions[3]) {fillAreaRecursion(tileMap, noiseMap, targetX, targetY + 1, limitMin, limitMax, sizeX, sizeY, blockID, wallID, replaceAir, airWithWalls, directions);} // Down
    if (directions[0]) {fillAreaRecursion(tileMap, noiseMap, targetX, targetY - 1, limitMin, limitMax, sizeX, sizeY, blockID, wallID, replaceAir, airWithWalls, directions);} // Up
}

/*
++ Flood Fill Algorithm to fill areas based on noise values

Directions array:
[0] = Up
[1] = Left
[2] = Right
[3] = Down

!! Filling Caves with water should only go down, left and right, not up.!!
*/
void fillArea(Tile* tileMap, int* noiseMap, int targetX, int targetY, int limitMin, int limitMax, int sizeX, int sizeY, uint16_t blockID, uint16_t wallID = TILES::WALLS::NOCHANGE, bool replaceAir = false, bool airWithWalls = true, std::array<bool,4> directions = {true, true, true, true}) {
    fillAreaRecursion(tileMap, noiseMap, targetX, targetY, limitMin, limitMax, sizeX, sizeY, blockID, wallID, replaceAir, airWithWalls, directions);
}

void fillCircularAreaFillCall(Tile* tileMap, int targetX, int targetY, int sizeX, int sizeY, std::unordered_map<uint16_t, std::array<uint16_t, 2>>* replaceMapBlock, std::unordered_map<uint16_t, std::array<uint16_t, 2>>* replaceMapWall) {
    if (targetX < 0 || targetX >= sizeX || targetY < 0 || targetY >= sizeY) {
        return;
    }

    int index = targetY * sizeX + targetX;
    uint16_t targetBlockID = tileMap[index].blockID;
    uint16_t targetWallID = tileMap[index].wallID;

    if (targetBlockID == TILES::BLOCKS::AIR && targetWallID == TILES::WALLS::AIR) {
        return;
    }

    if (replaceMapBlock->find(targetBlockID) != replaceMapBlock->end()) {
        uint16_t blockID = replaceMapBlock->at(targetBlockID)[0];
        uint16_t wallID = replaceMapBlock->at(targetBlockID)[1]; 
        setTileIDS(tileMap[index], blockID, wallID);
    } else if (replaceMapWall->find(targetWallID) != replaceMapWall->end()) {
        uint16_t blockID = replaceMapWall->at(targetWallID)[0]; 
        uint16_t wallID = replaceMapWall->at(targetWallID)[1]; 
        setTileIDS(tileMap[index], blockID, wallID);
    }
}

/*
? No longer used yet still helpful Full Explanation of the Midpoint Circle Algorithm: https://www.youtube.com/watch?v=hpiILbMkF9w
*/
void fillCircularArea(Tile* tileMap, int centerX, int centerY, int radius, int sizeX, int sizeY, std::unordered_map<uint16_t, std::array<uint16_t, 2>>* replaceMapBlock, std::unordered_map<uint16_t, std::array<uint16_t, 2>>* replaceMapWall) {
    int r2 = radius * radius;

    for (int y = -radius; y <= radius; ++y) {
        int y2 = y * y;

        for (int x = -radius; x <= radius; ++x) {
            if (x*x + y2 <= r2) {
                fillCircularAreaFillCall(tileMap, centerX + x, centerY + y, sizeX, sizeY, replaceMapBlock, replaceMapWall);
            }
        }
    }
}

void setLayerAt(int x, int y, Tile* tileMap, int* noiseMapA, int* noiseMapB, int sizeX, int sizeY, int index) { //! Maybe Use 2nd map for the rock choices !!!
    int noiseValueA = noiseMapA[y * sizeX + x];
    int noiseValueB = noiseMapB[y * sizeX + x];

    int percent = (y * 100) / sizeY;

    //++ Height Distribution: noiseValue < Value = Air/Block, noiseValue >= Value = Wall
    switch (percent) {
        case 0 ... 5: { //++ space Layer
            setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::AIR);
            break;
        }
        case 6 ... 15: { //++ sky Layer
            setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::AIR);
            break;
        }
        case 16 ... 20: { //++ surface Layer
            if (noiseValueA < 155) {
                setTileIDS(tileMap[index], TILES::BLOCKS::DIRT, TILES::WALLS::DIRT);
            } else {
                setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::DIRT);
            }
            break;
        }
        case 21 ... 25: { //++ ground Layer
            if (noiseValueA < 143) {
                if (noiseValueB < 80) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::CHALK, TILES::WALLS::CHALK);
                } else if (noiseValueB < 110) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::LIMESTONE, TILES::WALLS::LIMESTONE);
                } else {
                    setTileIDS(tileMap[index], TILES::BLOCKS::SHALE, TILES::WALLS::SHALE);
                }
            } else {
                if (noiseValueB < 80) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::CHALK);
                } else if (noiseValueB < 110) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::LIMESTONE);
                } else {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::SHALE);
                }
            }
            break;
        }
        case 26 ... 35: { //++ caves Layer
            if (noiseValueA < 143) {
                if (noiseValueB < 80) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::GRANITE, TILES::WALLS::GRANITE);
                } else if (noiseValueB < 100) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::DIORITE, TILES::WALLS::DIORITE);
                } else {
                    setTileIDS(tileMap[index], TILES::BLOCKS::ANDESITE, TILES::WALLS::ANDESITE);
                }
            } else {
                if (noiseValueB < 80) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::GRANITE);
                } else if (noiseValueB < 100) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::DIORITE);
                } else {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::ANDESITE);
                }
            }
            break;
        }
        case 36 ... 50: { //++ deep Caves Layer
            if (noiseValueA < 143) {
                if (noiseValueB < 70) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::SERPENTINITE, TILES::WALLS::SERPENTINITE);
                } else if (noiseValueB < 90) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::MARBLE, TILES::WALLS::MARBLE);
                } else {
                    setTileIDS(tileMap[index], TILES::BLOCKS::SLATE, TILES::WALLS::SLATE);
                }
            } else {
                if (noiseValueB < 70) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::SERPENTINITE);
                } else if (noiseValueB < 90) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::MARBLE);
                } else {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::SLATE);
                }
            }
            break;
        }
        case 51 ... 68: { //++ Compression Layer
            if (noiseValueA < 143) {
                if (noiseValueB < 95) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::QUARTZITE, TILES::WALLS::QUARTZITE);
                }  else {
                    setTileIDS(tileMap[index], TILES::BLOCKS::GNEISS, TILES::WALLS::GNEISS);
                }
            } else {
                if (noiseValueB < 95) {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::QUARTZITE);
                }  else {
                    setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::GNEISS);
                }
            }
            break;
        }
        case 69 ... 73: { //++ outer Core Layer
            if (noiseValueA < 143) {
                setTileIDS(tileMap[index], TILES::BLOCKS::BASALT, TILES::WALLS::BASALT);
            } else {
                setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::BASALT);
            }
            break;
        }
        case 74 ... 88: {//++ inner Core Layer
            if (noiseValueA < 143) {
                setTileIDS(tileMap[index], TILES::BLOCKS::KIMBERLITE, TILES::WALLS::KIMBERLITE);
            } else {
                setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::KIMBERLITE);
            }
            break;
        }
        case 89 ... 101: { //++ bedrock Layer
            if (noiseValueA < 143) {
                setTileIDS(tileMap[index], TILES::BLOCKS::KIMBERLITE, TILES::WALLS::KIMBERLITE);
            } else {
                setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::KIMBERLITE);
            }
            break;
        }
        default: {
            setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::AIR);
            break;
        }
    }
}

void setTilesByLayer(Tile* tileMap, int* noiseMapA, int* noiseMapB, int sizeX, int sizeY) {

    for (int y = 0; y < sizeY; ++y) {
        for (int x = 0; x < sizeX; ++x) {
            int index = y * sizeX + x;
            setLayerAt(x, y, tileMap, noiseMapA, noiseMapB, sizeX, sizeY, index);
        }
    }
}

/*
++ Complex Wave Function for surfacelevel variation
*/
double complexWave(double x) {
    // first part
    double part1 = (1.0 + 0.33 * sin(2.0 * x)) * sin(x + 0.5 * sin(M_PI * x)) + 0.6 * sin(M_E * x) + 0.25 * sin(pow(x, 1.33));

    // envelope/multiplier
    double envelope = -2.0 + 0.2 * sin(M_PI * x) + 0.3 * sin(0.05 * x * x);

    // final result
    return part1 - envelope * part1 - M_PI;
}

void generateSurfaceLevel(Tile* tileMap, int sizeX, int sizeY) {
    for (int x = 0; x < sizeX; ++x) {
        double limit = sizeY*0.16;
        // Compute the target height for this column
        int targetHeight = 10+static_cast<int>(limit - 2.5*complexWave(x * 0.05));

        for (int y = targetHeight; y > 0; --y) {
            setTileIDS(tileMap[y * sizeX + x], TILES::BLOCKS::AIR, TILES::WALLS::AIR);
        }

        
        for (int i = 1; i <= 1 + (rand() % 4); ++i) {
            int index = (targetHeight + i) * sizeX + x;
            if (tileMap[index].blockID != TILES::BLOCKS::AIR) {
                setTileIDS(tileMap[index], TILES::BLOCKS::GRASS, TILES::WALLS::GRASS);
            } else {
                setTileIDS(tileMap[index], TILES::BLOCKS::AIR, TILES::WALLS::GRASS);
            }
        }
    }
}

void generateBedrockLevel(Tile* tileMap, int sizeX, int sizeY) {
    for (int x = 0; x < sizeX; ++x) {
        double limit = sizeY*0.98;
        // Compute the target height for this column
        int targetHeight = static_cast<int>(limit + complexWave(x));

        for (int y = sizeY - 1; y > targetHeight; --y) {
            setTileIDS(tileMap[y * sizeX + x], TILES::BLOCKS::BEDROCK, TILES::WALLS::BEDROCK);
        }
    }
}

void decideOreAt(int x, int y, Tile* tileMap, int* noiseMap, int sizeX, int sizeY, int index) {
    int noiseValue = noiseMap[index];
    uint16_t currentBlockID = tileMap[index].blockID;
    
    int limitMin = 0;
    int limitMax = 75;

    if (noiseValue > limitMax || noiseValue < limitMin || currentBlockID == TILES::BLOCKS::AIR) {
        return;
    }

    //++ Getting Height For Layer Determination
    int percent = (y * 100) / sizeY;

    //++ Decide if ore or gems are choosen
    int isOre = rand() % 12;
    if (isOre > 2) {
        int oreChoice = rand() % 100;

        switch (percent) {
            case 0 ... 5: {   //++ space Layer
                break;
            }
            case 6 ... 16: {   //++ sky Layer
                break;
            }
            case 17 ... 20: { //++ surface Layer
                if (currentBlockID == TILES::BLOCKS::GRASS || currentBlockID == TILES::BLOCKS::DIRT) {
                    if (oreChoice <= 33) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 30), sizeX, sizeY, TILES::BLOCKS::MUD, TILES::WALLS::MUD, true);
                    } else if (oreChoice <= 66) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 20), sizeX, sizeY, TILES::BLOCKS::GRAVEL, TILES::WALLS::GRAVEL, true);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 40), sizeX, sizeY, TILES::BLOCKS::CLAY, TILES::WALLS::CLAY, true);
                    }
                }
                break;
            }
            case 21 ... 25: {  //++ ground Layer
                if (currentBlockID == TILES::BLOCKS::CHALK || currentBlockID == TILES::BLOCKS::SHALE || currentBlockID == TILES::BLOCKS::LIMESTONE) {
                    if (oreChoice <= 70) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 30), sizeX, sizeY, TILES::BLOCKS::COAL);
                    } else if (oreChoice <= 95) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 20), sizeX, sizeY, TILES::BLOCKS::LEAD);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::BISMUTH);
                    }
                }
                break;
            }
            case 26 ... 35: {  //++ caves Layer
                if (currentBlockID == TILES::BLOCKS::ANDESITE || currentBlockID == TILES::BLOCKS::DIORITE || currentBlockID == TILES::BLOCKS::GRANITE) {
                    if (oreChoice <= 20) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 30), sizeX, sizeY, TILES::BLOCKS::ZINC);
                    } else if (oreChoice <= 40) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 25), sizeX, sizeY, TILES::BLOCKS::COPPER);
                    } else if (oreChoice <= 45) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::ALUMINIUM);
                    } else if (oreChoice <= 60) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 15), sizeX, sizeY, TILES::BLOCKS::BRONZE);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 15), sizeX, sizeY, TILES::BLOCKS::IRON);
                    }
                }
                break;
            }
            case 36 ... 50: {  //++ deep Caves Layer
                if (currentBlockID == TILES::BLOCKS::SLATE || currentBlockID == TILES::BLOCKS::MARBLE || currentBlockID == TILES::BLOCKS::SERPENTINITE) {
                    if (oreChoice <= 30) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 30), sizeX, sizeY, TILES::BLOCKS::SILVER);
                    } else if (oreChoice <= 55) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 20), sizeX, sizeY, TILES::BLOCKS::TUNGSTEN);
                    } else if (oreChoice <= 75) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 15), sizeX, sizeY, TILES::BLOCKS::GOLD);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::PLATINUM);
                    }
                }
                break;
            }
            case 51 ... 68: {  //++ Compression Layer
                if (currentBlockID == TILES::BLOCKS::QUARTZITE || currentBlockID == TILES::BLOCKS::GNEISS) {
                    if (oreChoice <= 10) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 5), sizeX, sizeY, TILES::BLOCKS::GALENA);
                    } else if (oreChoice <= 25) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 25), sizeX, sizeY, TILES::BLOCKS::HEMATITE);
                    } else if (oreChoice <= 60) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 30), sizeX, sizeY, TILES::BLOCKS::COBALT);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::TITANIUM);
                    }
                }
                break;
            }
            case 69 ... 73: {  //++ outer Core Layer
                if (currentBlockID == TILES::BLOCKS::BASALT) {
                    if (oreChoice <= 10) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 40), sizeX, sizeY, TILES::BLOCKS::ADAMANTITE);
                    } else if (oreChoice <= 25) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 15), sizeX, sizeY, TILES::BLOCKS::MITHRILITE);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::ORICHALCUM);
                    }
                }
                break;
            }
            case 74 ... 88: {  //++ inner Core Layer
                if (currentBlockID == TILES::BLOCKS::KIMBERLITE) {
                    if (oreChoice <= 50) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 25), sizeX, sizeY, TILES::BLOCKS::OSMIUM);
                    } else if (oreChoice <= 90) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 15), sizeX, sizeY, TILES::BLOCKS::IRIDIUM);
                    } else if (oreChoice <= 95) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 7), sizeX, sizeY, TILES::BLOCKS::URANIUM);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 5), sizeX, sizeY, TILES::BLOCKS::PLUTONIUM);
                    }
                }
                break;
            }
            case 89 ... 101: { //++ bedrock Layer
                if (currentBlockID == TILES::BLOCKS::KIMBERLITE) {
                    if (oreChoice <= 20) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 15), sizeX, sizeY, TILES::BLOCKS::OSMIUM);
                    } else if (oreChoice <= 40) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::IRIDIUM);
                    } else if (oreChoice <= 80) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 15), sizeX, sizeY, TILES::BLOCKS::URANIUM);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 15), sizeX, sizeY, TILES::BLOCKS::PLUTONIUM);
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    } else {
        int gemChoice = rand() % 100;

        switch (percent) {
            case 0 ... 5: {   //++ space Layer
                break;
            }
            case 6 ... 16: {   //++ sky Layer
                break;
            }
            case 17 ... 20: { //++ surface Layer
                break;
            }
            case 21 ... 25: {  //++ ground Layer
                if (currentBlockID == TILES::BLOCKS::CHALK || currentBlockID == TILES::BLOCKS::SHALE || currentBlockID == TILES::BLOCKS::LIMESTONE) {
                    if (gemChoice <= 70) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 20), sizeX, sizeY, TILES::BLOCKS::QUARTZ);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::AMETHYST);
                    }
                }
                break;
            }
            case 26 ... 35: {  //++ caves Layer
                if (currentBlockID == TILES::BLOCKS::ANDESITE || currentBlockID == TILES::BLOCKS::DIORITE || currentBlockID == TILES::BLOCKS::GRANITE) {
                    fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 15), sizeX, sizeY, TILES::BLOCKS::ONYX);
                }
                break;
            }
            case 36 ... 50: {  //++ deep Caves Layer
                if (currentBlockID == TILES::BLOCKS::SLATE || currentBlockID == TILES::BLOCKS::MARBLE || currentBlockID == TILES::BLOCKS::SERPENTINITE) {
                    if (gemChoice <= 55) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 20), sizeX, sizeY, TILES::BLOCKS::AQUAMARINE);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::TOPAZ);
                    }
                }
                break;
            }
            case 51 ... 68: {  //++ Compression Layer
                if (currentBlockID == TILES::BLOCKS::QUARTZITE || currentBlockID == TILES::BLOCKS::GNEISS) {
                    if (gemChoice <= 10) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 5), sizeX, sizeY, TILES::BLOCKS::SAPPHIRE);
                    } else if (gemChoice <= 60) {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 30), sizeX, sizeY, TILES::BLOCKS::RUBY);
                    } else {
                        fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::EMERALD);
                    }
                }
                break;
            }
            case 69 ... 73: {  //++ outer Core Layer
                if (currentBlockID == TILES::BLOCKS::BASALT) {
                    fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 40), sizeX, sizeY, TILES::BLOCKS::TANZANITE);
                }
                break;
            }
            case 74 ... 88: {  //++ inner Core Layer
                if (currentBlockID == TILES::BLOCKS::KIMBERLITE) {
                    fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 5), sizeX, sizeY, TILES::BLOCKS::DIAMOND);
                }
                break;
            }
            case 89 ... 101: { //++ bedrock Layer
                if (currentBlockID == TILES::BLOCKS::KIMBERLITE) {
                    fillArea(tileMap, noiseMap, x, y, limitMin, limitMax+ (rand() % 10), sizeX, sizeY, TILES::BLOCKS::DIAMOND);
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

void generateOres(Tile* tileMap, int* noiseMap, int sizeX, int sizeY) {
    for (int y = 0; y < sizeY; ++y) {
        for (int x = 0; x < sizeX; ++x) {
            int index = y * sizeX + x;
            decideOreAt(x, y, tileMap, noiseMap, sizeX, sizeY, index);
        }
    }
}

void savingWoldFile(const std::string& worldName, Tile* tileMap, int sizeX, int sizeY, int seed, int* perlinNoiseMap) {
    std::string worldFilePath = path + "/worlds/" + worldName + "/worldData/map/world_" + std::to_string(sizeX) + "x" + std::to_string(sizeY) + "_seed" + std::to_string(seed) + ".wld";

    std::ofstream worldFile(worldFilePath);
    if (!worldFile.is_open()) {
        JFLX::log("World Generation: ", "Failed to open world file!", JFLX::LOGTYPE::ERROR);
    }

    for (int y = 0; y < sizeY; ++y) {
        for (int x = 0; x < sizeX; ++x) {
            worldFile << perlinNoiseMap[y * sizeX + x];
            if (x < sizeX - 1)
                worldFile << " ";
        }
        worldFile << "\n";
    }
    worldFile.close();
}

 /*
 * Creates A new world with params
 * Sizes (SizeTemplate, factor 2):
 * -1    test             1.000 x    750
 *  0    Tiny             2.000 x  1.500
 *  1    Small            4.000 x  3.000
 *  2    Medium           6.000 x  4.500
 *  3    Large            8.000 x  6.000
 *  4    Extra Large     10.000 x  7.500
 *  5    Huge            12.000 x  9.000
 *
 * Height Distribution:
 * 5% 	space       Layer
 * 10%	sky         Layer
 * 5%	surface     Layer
 * 5%	ground      Layer
 * 10%	caves       Layer
 * 15%	deep Caves  Layer
 * 18%	compression Layer
 * 5%	outer Core  Layer
 * 15%	inner Core  Layer
 * 2%	bedrock     Layer
 */
void generateWorld(const std::string worldName, int sizeTemplate, int seed, std::array<int, 4> previews) {
    //-> Some Insight: https://www.youtube.com/watch?v=Pgt82G4Jxac&t=448s

    JFLX::log("World Generation: ", "Generating world...", JFLX::LOGTYPE::SUCCESS);
    
    JFLX::log("World Generation: ", "Setting Up Random Seed..", JFLX::LOGTYPE::SUCCESS);
    srand(seed);

    //++ Determine World Size and Smoothness
    int sizeX, sizeY, smoothness;
    switch (sizeTemplate) {
        case -1:
            sizeX       = 1000;
            sizeY       = 750;
            smoothness  = 32;
            break;
        case 0:
            sizeX       = 2000;
            sizeY       = 1500;
            smoothness  = 64;
            break;
        case 1:
            sizeX       = 4000;
            sizeY       = 3000;
            smoothness  = 64;
            break;
        case 2:
            sizeX       = 6000;
            sizeY       = 4500;
            smoothness  = 192;
            break;
        case 3:
            sizeX       = 8000;
            sizeY       = 6000;
            smoothness  = 256;
            break;
        case 4:
            sizeX       = 10000;
            sizeY       = 7500;
            smoothness  = 320;
            break;
        case 5:
            sizeX       = 12000;
            sizeY       = 9000;
            smoothness  = 384;
            break;
        default:
            JFLX::log("World Generation: ", "Invalid size template specified. Using default (Medium).", JFLX::LOGTYPE::WARNING);
            sizeX       = 6000;
            sizeY       = 4500;
            smoothness  = 192;
            break;
    }

    //++ Create 2D Arrays
    JFLX::log("World Generation: ", "Creating 2d arrays.", JFLX::LOGTYPE::SUCCESS);
    Tile* tileMap       = new Tile[sizeX * sizeY];
    int* perlinNoiseMap = new int[sizeX * sizeY];
    int* rockMap        = new int[sizeX * sizeY];
    int* veinMap        = new int[sizeX * sizeY];

    //++ Generate Perlin Noise Map
    JFLX::log("World Generation: ", "Generating Perlin noise map.", JFLX::LOGTYPE::SUCCESS);
    JFLX::perlinNoise(perlinNoiseMap, sizeX, sizeY, seed, smoothness);
    JFLX::perlinNoise(rockMap, sizeX, sizeY, seed, (90+(rand()%10)));
    JFLX::perlinNoise(veinMap, sizeX, sizeY, seed, (125+(rand()%10)));
    JFLX::log("World Generation: ", "Completed Generating Perlin noise map.", JFLX::LOGTYPE::SUCCESS);

    
    if (tileMap == nullptr || perlinNoiseMap == nullptr || rockMap == nullptr || veinMap == nullptr) {
        JFLX::log("World Generation: ", "One of the pointers is null! Can not continue Generation!", JFLX::LOGTYPE::ERROR);
        return;
    }

    //++ Set Tiles by Layer
    JFLX::log("World Generation: ", "Generate Layers.", JFLX::LOGTYPE::SUCCESS);
    setTilesByLayer(tileMap, perlinNoiseMap, rockMap, sizeX, sizeY);
    JFLX::log("World Generation: ", "Finished Generating Layering.", JFLX::LOGTYPE::SUCCESS);

    //++ Generate Ores based on noise value and depth
    JFLX::log("World Generation: ", "Generating Ores.", JFLX::LOGTYPE::SUCCESS);
    generateOres(tileMap, veinMap, sizeX, sizeY);
    JFLX::log("World Generation: ", "Finished Generating Ores.", JFLX::LOGTYPE::SUCCESS);

    //++ Surface Level
    JFLX::log("World Generation: ", "Generating Surface level.", JFLX::LOGTYPE::SUCCESS);
    generateSurfaceLevel(tileMap, sizeX, sizeY);
    JFLX::log("World Generation: ", "Finished Generating Surface level.", JFLX::LOGTYPE::SUCCESS);

    //++ Create Jungle Biome
    JFLX::log("World Generation: ", "Generating Jungle Biome.", JFLX::LOGTYPE::SUCCESS);
    std::unordered_map<uint16_t, std::array<uint16_t, 2>> replaceMapBlock = {
        {TILES::BLOCKS::GRASS,      {TILES::BLOCKS::JUNGLEGRASS, TILES::WALLS::JUNGLEGRASS}},
        {TILES::BLOCKS::DIRT,       {TILES::BLOCKS::JUNGLEGRASS, TILES::WALLS::JUNGLEGRASS}},
        {TILES::BLOCKS::SHALE,      {TILES::BLOCKS::JUNGLEGRASS, TILES::WALLS::JUNGLEGRASS}},
        {TILES::BLOCKS::ANDESITE,   {TILES::BLOCKS::JUNGLEGRASS, TILES::WALLS::JUNGLEGRASS}},
        {TILES::BLOCKS::SLATE,      {TILES::BLOCKS::JUNGLEGRASS, TILES::WALLS::JUNGLEGRASS}},
        {TILES::BLOCKS::GNEISS,     {TILES::BLOCKS::JUNGLEGRASS, TILES::WALLS::JUNGLEGRASS}},
    };

    std::unordered_map<uint16_t, std::array<uint16_t, 2>> replaceMapWall = {
        {TILES::WALLS::GRASS,       {TILES::BLOCKS::NOCHANGE, TILES::WALLS::JUNGLEGRASS}},
        {TILES::WALLS::DIRT,        {TILES::BLOCKS::NOCHANGE, TILES::WALLS::JUNGLEGRASS}},
        {TILES::WALLS::SHALE,       {TILES::BLOCKS::NOCHANGE, TILES::WALLS::JUNGLEGRASS}},
        {TILES::WALLS::ANDESITE,    {TILES::BLOCKS::NOCHANGE, TILES::WALLS::JUNGLEGRASS}},
        {TILES::WALLS::SLATE,       {TILES::BLOCKS::NOCHANGE, TILES::WALLS::JUNGLEGRASS}},
        {TILES::WALLS::GNEISS,      {TILES::BLOCKS::NOCHANGE, TILES::WALLS::JUNGLEGRASS}},
    };

    int jungleRadius = static_cast<int>((sizeX/7)+(rand()%25));
    int jungleHalfRadius = static_cast<int>(0.5*jungleRadius);
    int jungleSecondRadius = static_cast<int>(0.85*jungleRadius);
    int jungleX = rand() % sizeX;
    int jungleY = static_cast<int>((sizeY/3) + (rand() % 25));

    fillCircularArea(tileMap, jungleX, jungleY, jungleRadius, sizeX, sizeY, &replaceMapBlock, &replaceMapWall);
    fillCircularArea(tileMap, jungleX, jungleY+jungleHalfRadius, jungleSecondRadius, sizeX, sizeY, &replaceMapBlock, &replaceMapWall);
    JFLX::log("World Generation: ", "Finished Generating Jungle Biome at: {" + std::to_string(jungleX) + ", " + std::to_string(jungleY) + "} with radius " + std::to_string(jungleRadius) + ".", JFLX::LOGTYPE::SUCCESS);

    //++ Bedrock Level
    JFLX::log("World Generation: ", "Generating Bedrock level.", JFLX::LOGTYPE::SUCCESS);
    generateBedrockLevel(tileMap, sizeX, sizeY);
    JFLX::log("World Generation: ", "Finished Generating Bedrock level.", JFLX::LOGTYPE::SUCCESS);

    //++ Save World Data
    JFLX::log("World Generation: ", "Saving world data (.wld).", JFLX::LOGTYPE::SUCCESS);

    //++ Create World Directory
    if (!fs::exists(path + "/worlds/" + worldName)) {
        JFLX::log("World Generation: ", "Creating World Directory.", JFLX::LOGTYPE::INFO);
        fs::create_directories(path + "/worlds/" + worldName + "/worldData/map/");
    } else {
        JFLX::log("World Generation: ", "World Directory already exists. Replacing Old World Data.", JFLX::LOGTYPE::WARNING);
    }

    //++ Save World File
    savingWoldFile(worldName, tileMap, sizeX, sizeY, seed, perlinNoiseMap);

    JFLX::log("World Generation: ", "Saved world data (.wld).", JFLX::LOGTYPE::SUCCESS);

    //++ Save Preview Images
    JFLX::log("World Generation: ", "Saving Preview Image(s) (.png).", JFLX::LOGTYPE::SUCCESS);
    if (previews[0] == 1) {generateAndSaveNoisePreviewImage(perlinNoiseMap, "perlinNoise", sizeX, sizeY, worldName);}
    if (previews[1] == 1) {generateAndSaveNoisePreviewImage(rockMap, "rockMap", sizeX, sizeY, worldName);}
    if (previews[2] == 1) {generateAndSaveNoisePreviewImage(veinMap, "veinMap", sizeX, sizeY, worldName);}
    if (previews[3] == 1) {generateAndSaveTilePreviewImage(tileMap, sizeX, sizeY, worldName);}

    //++ Clean Up
    delete[] tileMap;
    delete[] perlinNoiseMap;
    delete[] veinMap;
    JFLX::log("World Generation: ", "World generation completed.", JFLX::LOGTYPE::SUCCESS);
}

/*
++ Arguments:
 argv[1] = worldName (string)
 argv[2] = sizeTemplate (int)
 argv[3] = seed (int)
 argv[4] = preview Perlin Noise Map (bool) 0 / false, 1 / true
 argv[5] = preview rock Noise Map (bool) 0 / false, 1 / true
 argv[6] = preview vein Noise Map (bool) 0 / false, 1 / true
 argv[7] = preview Tile Map (bool) 0 / false, 1 / true
*/
int main(int argc, char* argv[]) {
    if (argc < 8) {
        JFLX::log("World Generation: ", "Insufficient arguments provided. Usage: <worldName> [sizeTemplate] [seed] [preview Perlin Noise Map] [preview rock Noise Map] [preview vein Noise Map] [preview Tile Map]", JFLX::LOGTYPE::ERROR);
        return 1;
    }
    
    std::string combined;
    for (int i = 0; i < argc; ++i) {
        combined += argv[i];
        if (i < argc - 1) combined += ", ";
    }

    JFLX::log("World Generation: ", "Running: " + combined, JFLX::LOGTYPE::SUCCESS);

    generateWorld(argv[1], (argv[2] ? std::stoi(argv[2]) : -1), (argv[3] ? std::stoi(argv[3]) : 12345), {std::stoi(argv[4]), std::stoi(argv[5]), std::stoi(argv[6]), std::stoi(argv[7])});

    return 0;
}
