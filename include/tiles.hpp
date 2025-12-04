//_ CRUSADIA - Tile HPP FILE _//_ COPYRIGHT (C) 2024 JFLX STUDIO - ALL RIGHTS RESERVED _//

#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <JFLX/logging.hpp>
#include "colorStruct.hpp"

namespace TILES {
    namespace BLOCKS {
        enum ID : uint16_t{
            NOCHANGE        = 0,

            //++ Atmosphere
                AIR             = 1,
                CLOUD           = 2,

            //++ Surface
                GRASS           = 3,
                JUNGLEGRASS     = 4,
                DIRT            = 5,
                CLAY            = 6,
                MUD             = 7,
                STONE           = 8,
                SAND            = 9,
                GRAVEL          = 10,
                SANDSTONE       = 11,

            //++ Ground
                //-> Rocks
                CHALK           = 12,
                SHALE           = 13,
                LIMESTONE       = 14,

                //-> Ores / Minerals
                COAL            = 15,
                COALORE         = 16,
                LEAD            = 17,
                LEADORE         = 18,
                BISMUTH         = 19,
                BISMUTHORE      = 20,
                //-> Gemstones
                QUARTZ          = 21,
                AMETHYST        = 22,


            //++ Caves Layer
                //-> Rocks
                ANDESITE        = 23,
                DIORITE         = 24,
                GRANITE         = 25,
                
                //-> Ores / Minerals
                ZINC            = 26,
                ZINCORE         = 27,
                COPPER          = 28,
                COPPERORE       = 29,
                ALUMINIUM       = 30,
                ALUMINIUMORE    = 31,
                BRONZE          = 32,
                BRONZEORE       = 33,
                IRON            = 34,
                IRONORE         = 35,

                //-> Gemstones
                ONYX            = 36,

            //++ Deep Caves Layer
                //-> Rocks
                SLATE           = 37,
                MARBLE          = 38,
                SERPENTINITE    = 39,
                
                //-> Ores / Minerals
                SILVER          = 40,
                SILVERORE       = 41,
                TUNGSTEN        = 42,
                TUNGSTENORE     = 43,
                GOLD            = 44,
                GOLDORE         = 45,
                PLATINUM        = 46,
                PLATINUMORE     = 47,

                //-> Gemstones
                AQUAMARINE      = 48,
                TOPAZ           = 49,
            

            //++ Compression Layer
                //-> Rocks
                QUARTZITE       = 50,
                GNEISS          = 51,
                
                //-> Ores / Minerals
                GALENA          = 52,
                GALENAORE       = 53,
                HEMATITE        = 54,
                HEMATITEORE     = 55,
                COBALT          = 56,
                COBALTORE       = 57,
                TITANIUM        = 58,
                TITANIUMORE     = 59,

                //-> Gemstones
                SAPPHIRE        = 60,
                RUBY            = 61,
                EMERALD         = 62,
            

            //++ Outer Core Layer
                //-> Rocks
                BASALT          = 63,
                
                //-> Ores / Minerals
                ADAMANTITE      = 64,
                ADAMANTITEORE   = 65,
                MITHRILITE      = 66,
                MITHRILITEORE   = 67,
                ORICHALCUM      = 68,
                ORICHALCUMORE   = 69,

                //-> Gemstones
                TANZANITE       = 70,
            

            //++ Inner Core Layer
                //-> Rocks
                KIMBERLITE      = 71,
                
                //-> Ores / Minerals
                OSMIUM          = 72,
                OSMIUMORE       = 73,
                IRIDIUM         = 74,
                IRIDIUMORE      = 75,
                URANIUM         = 76,
                URANIUMORE      = 77,
                PLUTONIUM       = 78,
                PLUTONIUMORE    = 79,

                //-> Gemstones
                DIAMOND         = 80,
            



            BEDROCK         = UINT16_MAX
        };
    }

    namespace WALLS {
        enum ID : uint16_t{
            NOCHANGE        = 0,

            //++ Atmosphere
                AIR             = 1,
                CLOUD           = 2,

            //++ Surface
                GRASS           = 3,
                JUNGLEGRASS     = 4,
                DIRT            = 5,
                CLAY            = 6,
                MUD             = 7,
                STONE           = 8,
                SAND            = 9,
                GRAVEL          = 10,
                SANDSTONE       = 11,

            //++ Ground
                //-> Rocks
                CHALK           = 12,
                SHALE           = 13,
                LIMESTONE       = 14,

                //-> Ores / Minerals
                COAL            = 15,
                COALORE         = 16,
                LEAD            = 17,
                LEADORE         = 18,
                BISMUTH         = 19,
                BISMUTHORE      = 20,
                //-> Gemstones
                QUARTZ          = 21,
                AMETHYST        = 22,


            //++ Caves Layer
                //-> Rocks
                ANDESITE        = 23,
                DIORITE         = 24,
                GRANITE         = 25,
                
                //-> Ores / Minerals
                ZINC            = 26,
                ZINCORE         = 27,
                COPPER          = 28,
                COPPERORE       = 29,
                ALUMINIUM       = 30,
                ALUMINIUMORE    = 31,
                BRONZE          = 32,
                BRONZEORE       = 33,
                IRON            = 34,
                IRONORE         = 35,

                //-> Gemstones
                ONYX            = 36,

            //++ Deep Caves Layer
                //-> Rocks
                SLATE           = 37,
                MARBLE          = 38,
                SERPENTINITE    = 39,
                
                //-> Ores / Minerals
                SILVER          = 40,
                SILVERORE       = 41,
                TUNGSTEN        = 42,
                TUNGSTENORE     = 43,
                GOLD            = 44,
                GOLDORE         = 45,
                PLATINUM        = 46,
                PLATINUMORE     = 47,

                //-> Gemstones
                AQUAMARINE      = 48,
                TOPAZ           = 49,
            

            //++ Compression Layer
                //-> Rocks
                QUARTZITE       = 50,
                GNEISS          = 51,
                
                //-> Ores / Minerals
                GALENA          = 52,
                GALENAORE       = 53,
                HEMATITE        = 54,
                HEMATITEORE     = 55,
                COBALT          = 56,
                COBALTORE       = 57,
                TITANIUM        = 58,
                TITANIUMORE     = 59,

                //-> Gemstones
                SAPPHIRE        = 60,
                RUBY            = 61,
                EMERALD         = 62,
            

            //++ Outer Core Layer
                //-> Rocks
                BASALT          = 63,
                
                //-> Ores / Minerals
                ADAMANTITE      = 64,
                ADAMANTITEORE   = 65,
                MITHRILITE      = 66,
                MITHRILITEORE   = 67,
                ORICHALCUM      = 68,
                ORICHALCUMORE   = 69,

                //-> Gemstones
                TANZANITE       = 70,
            

            //++ Inner Core Layer
                //-> Rocks
                KIMBERLITE      = 71,
                
                //-> Ores / Minerals
                OSMIUM          = 72,
                OSMIUMORE       = 73,
                IRIDIUM         = 74,
                IRIDIUMORE      = 75,
                URANIUM         = 76,
                URANIUMORE      = 77,
                PLUTONIUM       = 78,
                PLUTONIUMORE    = 79,

                //-> Gemstones
                DIAMOND         = 80,
            



            BEDROCK         = UINT16_MAX
        };
    }

    namespace LIQUIDS {
        enum ID : uint16_t{
            NOCHANGE        = 0,
            AIR             = 1,
            WATER           = 2,
            LAVA            = 3
        };
    }

    namespace COLORVALUES {
        std::unordered_map<uint16_t, Color> COLORS = {
            // Atmosphere
            {BLOCKS::AIR,          Color(255, 255, 255,   0)},
            {BLOCKS::CLOUD,        Color(200, 200, 200, 255)},

            // Surface
            {BLOCKS::GRASS,        Color(34, 139, 34, 255)},
            {BLOCKS::JUNGLEGRASS,  Color(17, 69, 17, 255)},
            {BLOCKS::DIRT,         Color(139, 69, 19, 255)},
            {BLOCKS::CLAY,         Color(190, 110, 60, 255)},
            {BLOCKS::MUD,          Color(60, 30, 10, 255)},
            {BLOCKS::STONE,        Color(128, 128, 128, 255)},
            {BLOCKS::SAND,         Color(237, 201, 175, 255)},
            {BLOCKS::GRAVEL,       Color(150, 150, 150, 255)},
            {BLOCKS::SANDSTONE,    Color(216, 180, 130, 255)},

            // Ground - Rocks
            {BLOCKS::CHALK,        Color(245, 245, 235, 255)},
            {BLOCKS::SHALE,        Color(70, 70, 60, 255)},
            {BLOCKS::LIMESTONE,    Color(200, 200, 180, 255)},

            // Ground - Ores / Minerals
            {BLOCKS::COAL,         Color(20, 20, 20, 255)},
            {BLOCKS::LEAD,         Color(90, 90, 110, 255)},
            {BLOCKS::BISMUTH,      Color(190, 125, 180, 255)},

            // Ground - Gemstones
            {BLOCKS::QUARTZ,       Color(230, 230, 255, 255)},
            {BLOCKS::AMETHYST,     Color(153, 102, 204, 255)},

            // Cave Layer - Rocks
            {BLOCKS::ANDESITE,     Color(112, 128, 144, 255)},
            {BLOCKS::DIORITE,      Color(220, 220, 220, 255)},
            {BLOCKS::GRANITE,      Color(169, 130, 110, 255)},

            // Cave Layer - Minerals
            {BLOCKS::ZINC,         Color(180, 180, 200, 255)},
            {BLOCKS::COPPER,       Color(184, 115, 51, 255)},
            {BLOCKS::ALUMINIUM,    Color(200, 200, 210, 255)},
            {BLOCKS::BRONZE,       Color(180, 130, 60, 255)},
            {BLOCKS::IRON,         Color(155, 90, 60, 255)},

            // Cave Layer - Gemstones
            {BLOCKS::ONYX,         Color(30, 30, 30, 255)},

            // Deep Caves - Rocks
            {BLOCKS::SLATE,        Color(70, 70, 80, 255)},
            {BLOCKS::MARBLE,       Color(240, 240, 240, 255)},
            {BLOCKS::SERPENTINITE, Color(60, 120, 80, 255)},

            // Deep Caves - Minerals
            {BLOCKS::SILVER,       Color(210, 210, 220, 255)},
            {BLOCKS::TUNGSTEN,     Color(50, 50, 60, 255)},
            {BLOCKS::GOLD,         Color(255, 215, 0, 255)},
            {BLOCKS::PLATINUM,     Color(225, 225, 235, 255)},

            // Deep Caves - Gemstones
            {BLOCKS::AQUAMARINE,   Color(70, 200, 200, 255)},
            {BLOCKS::TOPAZ,        Color(255, 180, 50, 255)},

            // Compression Layer - Rocks
            {BLOCKS::QUARTZITE,    Color(200, 190, 200, 255)},
            {BLOCKS::GNEISS,       Color(160, 150, 140, 255)},

            // Compression Layer - Minerals
            {BLOCKS::GALENA,       Color(110, 110, 130, 255)},
            {BLOCKS::HEMATITE,     Color(150, 30, 30, 255)},
            {BLOCKS::TITANIUM,     Color(180, 180, 190, 255)},
            {BLOCKS::COBALT,       Color(0, 70, 140, 255)},

            // Compression Layer - Gemstones
            {BLOCKS::SAPPHIRE,     Color(15, 65, 160, 255)},
            {BLOCKS::RUBY,         Color(180, 20, 20, 255)},
            {BLOCKS::EMERALD,      Color(20, 150, 80, 255)},

            // Outer Core Layer - Rocks
            {BLOCKS::BASALT,       Color(40, 40, 50, 255)},

            // Outer Core Layer - Minerals
            {BLOCKS::ADAMANTITE,   Color(255, 20, 20, 255)},
            {BLOCKS::MITHRILITE,   Color(110, 240, 190, 255)},
            {BLOCKS::ORICHALCUM,   Color(255, 140, 0, 255)},

            // Outer Core - Gemstones
            {BLOCKS::TANZANITE,    Color(75, 60, 140, 255)},

            // Inner Core Layer - Rocks
            {BLOCKS::KIMBERLITE,   Color(105, 105, 105, 255)},

            // Inner Core Layer - Minerals
            {BLOCKS::OSMIUM,       Color(30, 60, 120, 255)},
            {BLOCKS::IRIDIUM,      Color(200, 200, 220, 255)},
            {BLOCKS::URANIUM,      Color(140, 255, 60, 255)},
            {BLOCKS::PLUTONIUM,    Color(40, 255, 100, 255)},

            // Inner Core - Gemstones
            {BLOCKS::DIAMOND,      Color(180, 240, 255, 255)},

            // Bedrock
            {BLOCKS::BEDROCK,      Color(0, 0, 0, 255)}
        };

    }
}


struct Tile {
    uint16_t blockID;
    uint16_t liquidID;
    uint16_t wallID;

    /*
    ++ 8-bit to represent 8 neighbours

    ++Bit Positions:
    0,  0,  0,  0,  0,  0,  0,  0,  0
    TL, TC, TR, CR, CC, CL, BL, BC, BR

    T = Top
    C = Center
    B = Bottom
    L = Left
    R = Right

    ++Neighbour Indexes:
    [0] = block Neighbours
    [1] = wall Neighbours
    [2] = liquid Neighbours
    */
    std::array<uint8_t, 3> neightbours = {
        0b00000000,
        0b00000000,
        0b00000000
    };
};

struct TileDefinition {
    bool solid;
    bool blocksLight;
    bool interactable;
    std::string name;
    // optional: texture index, drop item, sound, etc.
};

// {
//     AIR         = {false, false, false, "Air"},
//     CLOUD       = {false, false, false, "Cloud"},
//     GRASS       = {true,  true,  false, "Grass Block"},
//     DIRT        = {true,  true,  false, "Dirt Block"},
//     CLAY        = {true,  true,  false, "Clay Block"},
//     STONE       = {true,  true,  false, "Stone Block"},
//     DIORITE     = {true,  true,  false, "Diorite Block"},
//     ANDESITE    = {true,  true,  false, "Andesite Block"},
//     GRANITE     = {true,  true,  false, "Granite Block"},
//     KIMBERLITE  = {true,  true,  false, "Kimberlite Block"},
//     BEDROCK     = {true,  true,  false, "Bedrock"}
// };

struct Item {

};

struct Chest {
    int x, y;
    std::vector<Item> items;
};

std::string getNameById(uint16_t b_id) {
    
    return "Unknown Tile";
}

void setTileIDS(Tile& tile, uint16_t blockID = 0, uint16_t wallID = 0, uint16_t liquidID = 0) {
    if (blockID  != 0) {tile.blockID = blockID;}
    if (wallID   != 0) {tile.wallID = wallID;}
    if (liquidID != 0) {tile.liquidID = liquidID;}
}


/*
Optional uses:
if (tileDefs[tile.id].interactable) {
    -> handle interaction
}
*/
