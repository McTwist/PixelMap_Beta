

#include <memory>

#include "DataValues.hpp"

Color DataValues::colorData[DATAVALUES_AMOUNT];

// Make default colors
void DataValues::MakeColor()
{
	// Clear
	for (int i = 0; i < DATAVALUES_AMOUNT; ++i)
		colorData[i] = Color();

	colorData[AIR] = Color(255,255,255,0);
	colorData[STONE] = Color(120,120,120,255);
	colorData[GRASS] = Color(100, 150, 62, 255);
	colorData[DIRT] = Color(134,96,67,255);
	colorData[COBBLESTONE] = Color(115,115,115,255);
	colorData[WOOD] = Color(157,128,79,255);
	colorData[SAPLING] = Color(120,120,120,0);
	colorData[BEDROCK] = Color(84,84,84,255);
	colorData[WATER] = Color(38, 65, 128, 50);
	colorData[STATIONARYWATER] = colorData[WATER];
	colorData[LAVA] = Color(255,90,0,255);
	colorData[STATIONARYLAVA] = colorData[LAVA];
	colorData[SAND] = Color(218,210,158,255);
	colorData[GRAVEL] = Color(136,126,126,255);
	colorData[GOLDORE] = Color(143,140,125,255);
	colorData[IRONORE] = Color(136,130,127,255);
	colorData[COALORE] = Color(115,115,115,255);
	colorData[LOG] = Color(102,81,51,255);
	colorData[LEAVES] = Color(60,192,41,100);
	colorData[GLASS] = Color(255,255,255,64);

	colorData[WOOL] = Color(222,222,222,255);
	colorData[YELLOWFLOWER] = Color(255,255,0,255);
	colorData[REDROSE] = Color(255,0,0,255);
	colorData[GOLDBLOCK] = Color(231,165,45,255);
	colorData[IRONBLOCK] = Color(191,191,191,255);
	colorData[DOUBLESLAB_STONE] = Color(200,200,200,255);
	colorData[SLAB_STONE] = colorData[DOUBLESLAB_STONE];
	colorData[BRICK] = Color(170,86,62,255);
	colorData[TNT] = Color(160,83,65,255);
	colorData[MOSSYCOBBLESTONE] = Color(115,115,115,255);
	colorData[OBSIDIAN] = Color(26,11,43,255);
	colorData[TORCH] = Color(245,220,50,200);
	colorData[FIRE] = Color(255,170,30,200);
	colorData[WOODENSTAIRS] = Color(157,128,79,255);
	colorData[CHEST] = Color(125,91,38,255);
	colorData[DIAMONDORE] = Color(129,140,143,255);
	colorData[DIAMONDBLOCK] = Color(45,166,152,255);
	colorData[WORKBENCH] = Color(114,88,56,255);
	colorData[CROPS] = Color(146,192,0,255);
	colorData[SOIL] = Color(95,58,30,255);
	colorData[FURNACE] = Color(96,96,96,255);
	colorData[BURNINGFURNACE] = colorData[FURNACE];
	colorData[SIGNPOST] = Color(111,91,54,255);
	colorData[WOODENDOOR] = Color(136,109,67,255);
	colorData[LADDER] = Color(181,140,64,32);
	colorData[MINECARTTRACK] = Color(150,134,102,180);
	colorData[COBBLESTONESTAIRS] = Color(115,115,115,255);
	colorData[IRONDOOR] = Color(191,191,191,255);
	colorData[REDSTONEORE] = Color(131,107,107,255);
	colorData[GLOWINGREDSTONEORE] = colorData[REDSTONEORE];
	colorData[REDSTONETORCHON] = Color(181,140,64,32);
	colorData[REDSTONETORCHOFF] = Color(255,0,0,200);
	colorData[SNOW] = Color(255,255,255,255);
	colorData[ICE] = Color(83,113,163,51);
	colorData[SNOWBLOCK] = Color(250,250,250,255);
	colorData[CACTUS] = Color(25,120,25,255);
	colorData[CLAY] = Color(151,157,169,255);
	colorData[REED] = Color(193,234,150,255);
	colorData[JUKEBOX] = Color(100,67,50,255);

	colorData[PUMPKIN] = Color(192,181,21,255);
	colorData[BLOODSTONE] = Color(110,53,51,255);
	colorData[SLOWSAND] = Color(84,64,51,255);
	colorData[LIGHTSTONE] = Color(137,112,64,255);
	colorData[PORTAL] = Color(201,119,239,255);
	colorData[JACKOLANTERN] = colorData[PUMPKIN];

	colorData[LAPIZLAZULIORE] = Color(20,69,143,255);
	colorData[LAPIZLAZULIBLOCK] = Color(27,75,180,255);
	colorData[DISPENSER] = colorData[FURNACE];
	colorData[SANDSTONE] = colorData[SAND];
	colorData[NOTEBLOCK] = colorData[JUKEBOX];
	colorData[CAKE] = Color(229,201,202,255);

	colorData[REPEATERON] = colorData[REDSTONETORCHON];
	colorData[REPEATEROFF] = colorData[REDSTONETORCHOFF];

	colorData[BED] = Color(142, 22, 22, 255);
	// colorData[NEWORE] =  Color(25, 29, 70, 256);

	// Wools color
	colorData[WOOLWHITE] = colorData[WOOL];
	colorData[WOOLORANGE] = Color(235,128,56,255);
	colorData[WOOLMAGENTA] = Color(192,76,202,255);
	colorData[WOOLLIGHTBLUE] = Color(105,140,210,255);
	colorData[WOOLYELLOW] = Color(195,182,29,255);
	colorData[WOOLLIMEGREEN] = Color(60,189,48,255);
	colorData[WOOLPINK] = Color(218,133,156,255);
	colorData[WOOLGRAY] = Color(67,67,67,255);
	colorData[WOOLLIGHTGRAY] = Color(159,166,166,255);
	colorData[WOOLCYAN] = Color(40,117,150,255);
	colorData[WOOLBLUE] = Color(39,52,155,255);
	colorData[WOOLBROWN] = Color(86,52,28,255);
	colorData[WOOLGREEN] = Color(56,77,25,255);
	colorData[WOOLRED] = Color(165,45,41,255);
	colorData[WOOLBLACK] = Color(28,24,24,255);
	colorData[WOOLPURPLE] = Color(130,54,197,255);
	colorData[WOOLUNKNOWN] = colorData[WOOL];

	// Log color
	colorData[LOGNORMAL] = colorData[LOG];
	colorData[LOGREDWOOD] = Color(45,28,12,255);
	colorData[LOGBIRCH] = Color(206,206,200,255);

	// Leaf color
	colorData[LEAFNORMAL] = colorData[LEAVES];
	colorData[LEAFREDWOOD] = colorData[LEAVES];
	colorData[LEAFBIRCH] = colorData[LEAVES];

	// Slab color
	colorData[SLABSTONE] = colorData[STONE];
	colorData[SLABSAND] = colorData[SAND];
	colorData[SLABWOOD] = colorData[WOOD];
	colorData[SLABCOBBLE] = colorData[COBBLESTONE];
}

#include "Image.hpp"

// Get image from an another image
Color GetColorFromImage(UINT x, UINT y, Image * image)
{
	UINT width = image->Width();
	UINT height = image->Height();
	x *= width >> 4;
	y *= height >> 4;
	Image temp = Image();
	for (UINT Y = 0; Y < 16; ++Y)
		for (UINT X = 0; X < 16; ++X)
			temp.SetPixel(X, Y, image->GetPixel(X + x, Y + y));

	return temp.Average();
}

// Make colors from file
void DataValues::MakeColor(STRING file)
{
	Image * image = new Image();
	if (!image->Load(file))
	{
		delete image;
		return;
	}
	
	// Clear
	memset(&colorData, 0, sizeof(Color) * DATAVALUES_AMOUNT);

	colorData[AIR] = Color(255,255,255,0);
	colorData[STONE] = GetColorFromImage(1, 0, image);
	colorData[GRASS] = GetColorFromImage(0, 0, image);
	colorData[DIRT] = GetColorFromImage(3, 0, image);
	colorData[COBBLESTONE] = GetColorFromImage(0, 1, image);
	colorData[WOOD] = GetColorFromImage(4, 0, image);
	colorData[SAPLING] = Color(120,120,120,0);
	colorData[BEDROCK] = GetColorFromImage(1, 1, image);
	colorData[WATER] = GetColorFromImage(13, 12, image);
	colorData[STATIONARYWATER] = colorData[WATER];
	colorData[LAVA] = GetColorFromImage(13, 14, image);
	colorData[STATIONARYLAVA] = colorData[LAVA];
	colorData[SAND] = GetColorFromImage(2, 1, image);
	colorData[GRAVEL] = GetColorFromImage(3, 1, image);
	colorData[GOLDORE] = GetColorFromImage(0, 2, image);
	colorData[IRONORE] = GetColorFromImage(1, 2, image);
	colorData[COALORE] = GetColorFromImage(2, 2, image);
	colorData[LOG] = GetColorFromImage(4, 1, image);
	colorData[LEAVES] = GetColorFromImage(4, 3, image);
	colorData[GLASS] = GetColorFromImage(1, 3, image);
	colorData[WOOL] = GetColorFromImage(0, 4, image);
	colorData[YELLOWFLOWER] = GetColorFromImage(13, 0, image);
	colorData[REDROSE] = GetColorFromImage(12, 0, image);
	colorData[GOLDBLOCK] = GetColorFromImage(7, 1, image);
	colorData[IRONBLOCK] = GetColorFromImage(6, 1, image);
	colorData[DOUBLESLAB_STONE] = GetColorFromImage(6, 0, image);
	colorData[SLAB_STONE] = colorData[DOUBLESLAB_STONE];
	colorData[BRICK] = GetColorFromImage(7, 0, image);
	colorData[TNT] = GetColorFromImage(8, 0, image);
	colorData[MOSSYCOBBLESTONE] = GetColorFromImage(4, 2, image);
	colorData[OBSIDIAN] = GetColorFromImage(5, 2, image);
	colorData[TORCH] = GetColorFromImage(0, 5, image);
	colorData[FIRE] = Color(255,170,30,200);
	colorData[WOODENSTAIRS] = GetColorFromImage(4, 0, image);
	colorData[CHEST] = GetColorFromImage(9, 1, image);
	colorData[DIAMONDORE] = GetColorFromImage(2, 3, image);
	colorData[DIAMONDBLOCK] = GetColorFromImage(8, 1, image);
	colorData[WORKBENCH] = GetColorFromImage(11, 2, image);
	colorData[CROPS] = GetColorFromImage(15, 5, image);
	colorData[SOIL] = GetColorFromImage(6, 5, image);
	colorData[FURNACE] = GetColorFromImage(12, 2, image);
	colorData[BURNINGFURNACE] = colorData[FURNACE];
	colorData[SIGNPOST] = GetColorFromImage(4, 0, image);
	colorData[WOODENDOOR] = GetColorFromImage(1, 6, image);
	colorData[LADDER] = GetColorFromImage(3, 5, image);
	colorData[MINECARTTRACK] = GetColorFromImage(0, 8, image);
	colorData[COBBLESTONESTAIRS] = GetColorFromImage(0, 1, image);
	colorData[IRONDOOR] = GetColorFromImage(2, 6, image);
	colorData[REDSTONEORE] = GetColorFromImage(3, 3, image);
	colorData[GLOWINGREDSTONEORE] = colorData[REDSTONEORE];
	colorData[REDSTONETORCHON] = GetColorFromImage(3, 6, image);
	colorData[REDSTONETORCHOFF] = GetColorFromImage(3, 7, image);
	colorData[SNOW] = GetColorFromImage(2, 4, image);
	colorData[ICE] = GetColorFromImage(3, 4, image);
	colorData[SNOWBLOCK] = GetColorFromImage(2, 4, image);
	colorData[CACTUS] = GetColorFromImage(5, 4, image);
	colorData[CLAY] = GetColorFromImage(8, 4, image);
	colorData[REED] = GetColorFromImage(9, 4, image);
	colorData[JUKEBOX] = GetColorFromImage(11, 4, image);

	colorData[PUMPKIN] = GetColorFromImage(6, 6, image);
	colorData[BLOODSTONE] = GetColorFromImage(7, 6, image);
	colorData[SLOWSAND] = GetColorFromImage(8, 6, image);
	colorData[LIGHTSTONE] = GetColorFromImage(9, 6, image);
	colorData[PORTAL] = GetColorFromImage(0, 14, image); // Temporarily
	colorData[JACKOLANTERN] = colorData[PUMPKIN];

	colorData[LAPIZLAZULIORE] = GetColorFromImage(0, 10, image);
	colorData[LAPIZLAZULIBLOCK] = GetColorFromImage(0, 9, image);
	colorData[DISPENSER] = colorData[FURNACE];
	colorData[SANDSTONE] = GetColorFromImage(0, 11, image);
	colorData[NOTEBLOCK] = GetColorFromImage(10, 4, image);
	colorData[CAKE] = GetColorFromImage(9, 7, image);

	colorData[BED] = GetColorFromImage(6, 8, image);
	// colorData[NEWORE] = GetColorFromImage(6, 2, image);

	// WOOLs color
	colorData[WOOLWHITE] = colorData[WOOL];
	colorData[WOOLORANGE] = GetColorFromImage(2, 13, image);
	colorData[WOOLMAGENTA] = GetColorFromImage(2, 12, image);
	colorData[WOOLLIGHTBLUE] = GetColorFromImage(2, 11, image);
	colorData[WOOLYELLOW] = GetColorFromImage(2, 10, image);
	colorData[WOOLLIMEGREEN] = GetColorFromImage(2, 9, image);
	colorData[WOOLPINK] = GetColorFromImage(2, 8, image);
	colorData[WOOLGRAY] = GetColorFromImage(2, 7, image);
	colorData[WOOLLIGHTGRAY] = GetColorFromImage(1, 14, image);
	colorData[WOOLCYAN] = GetColorFromImage(1, 13, image);
	colorData[WOOLBLUE] = GetColorFromImage(1, 11, image);
	colorData[WOOLBROWN] = GetColorFromImage(1, 10, image);
	colorData[WOOLGREEN] = GetColorFromImage(1, 9, image);
	colorData[WOOLRED] = GetColorFromImage(1, 8, image);
	colorData[WOOLBLACK] = GetColorFromImage(1, 7, image);
	colorData[WOOLPURPLE] = GetColorFromImage(1, 12, image);
	colorData[WOOLUNKNOWN] = colorData[WOOL];

	// Log color
	colorData[LOGNORMAL] = colorData[LOG];
	colorData[LOGREDWOOD] = GetColorFromImage(4, 7, image);
	colorData[LOGBIRCH] = GetColorFromImage(5, 7, image);

	// Leaf color
	colorData[LEAFNORMAL] = colorData[LEAVES];
	colorData[LEAFREDWOOD] = GetColorFromImage(4, 8, image);
	colorData[LEAFBIRCH] = GetColorFromImage(5, 8, image);

	// Slab color
	colorData[SLABSTONE] = colorData[STONE];
	colorData[SLABSAND] = colorData[SAND];
	colorData[SLABWOOD] = colorData[WOOD];
	colorData[SLABCOBBLE] = colorData[COBBLESTONE];

	delete image;
}

// Get color
Color DataValues::GetColor(unsigned char c)
{
	return colorData[c];
}