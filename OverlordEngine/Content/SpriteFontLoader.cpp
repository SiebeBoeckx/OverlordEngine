#include "stdafx.h"
#include "SpriteFontLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if (!pReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}

	TODO_W7(L"Implement SpriteFontLoader >> Parse .fnt file")
	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	const byte b = pReader->Read<byte>();
	const byte m = pReader->Read<byte>();
	const byte f = pReader->Read<byte>();
	const byte version = pReader->Read<byte>();
	//If Identification bytes doesn't match B|M|F,
	if (b != 'B' || m != 'M' || f != 'F')
	{
		//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		//return nullptr
		return nullptr;
	}

	//Parse the version (version 3 required)
	//If version is < 3,
	if (version < 3)
	{
		//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		//return nullptr
		return nullptr;
	}

	//Valid .fnt file >> Start Parsing!
	//use this SpriteFontDesc to store all relevant information (used to initialize a SpriteFont object)
	SpriteFontDesc fontDesc{};

	
	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	byte blockID = pReader->Read<byte>();
	int blockSize = pReader->Read<int>();
	//Retrieve the FontSize [fontDesc.fontSize]
	fontDesc.fontSize = pReader->Read<short>();
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pReader->MoveBufferPosition(12);
	//Retrieve the FontName [fontDesc.fontName]
	fontDesc.fontName = pReader->ReadNullString();


	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	blockID = pReader->Read<byte>();
	blockSize = pReader->Read<int>();
	//Retrieve Texture Width & Height [fontDesc.textureWidth/textureHeight]
	pReader->MoveBufferPosition(4);
	fontDesc.textureWidth = pReader->Read<short>();
	fontDesc.textureHeight = pReader->Read<short>();
	//Retrieve PageCount
	//> if pagecount > 1
	if (pReader->Read<short>() > 1)
	{
		//	> Log Error (Only one texture per font is allowed!)
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");
	}
	//Advance to Block2 (Move Reader)
	pReader->MoveBufferPosition(5);


	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	blockID = pReader->Read<byte>();
	blockSize = pReader->Read<int>();
	//Retrieve the PageName (BinaryReader::ReadNullString)
	const std::wstring pageName = pReader->ReadNullString();
	if (pageName.empty())
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
	}
	//Construct the full path to the page texture file
	//	>> page texture should be stored next to the .fnt file, pageName contains the name of the texture file
	//	>> full texture path = asset parent_path of .fnt file (see loadInfo.assetFullPath > get parent_path) + pageName (filesystem::path::append)
	std::wstring path = loadInfo.assetFullPath.parent_path().wstring();
	path += L"\\";
	path += pageName;
	//	>> Load the texture (ContentManager::Load<TextureData>) & Store [fontDesc.pTexture]
	fontDesc.pTexture = ContentManager::Load<TextureData>(path);


	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	blockID = pReader->Read<byte>();
	blockSize = pReader->Read<int>();
	//Retrieve Character Count (see documentation)
	const auto charcount{ blockSize / 20 };
	//Create loop for Character Count, and:
	for (int i = 0; i < charcount; ++i)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		const auto id = static_cast<wchar_t>(pReader->Read<int>());
		//> Create instance of FontMetric (struct)
		FontMetric& metric = fontDesc.metrics[id];
		//	> Set Character (CharacterId) [FontMetric::character]
		metric.character = id;
		//	> Retrieve Xposition (store Local)
		float xPosition = pReader->Read<short>();
		//	> Retrieve Yposition (store Local)
		float yPosition = pReader->Read<short>();
		//	> Retrieve & Set Width [FontMetric::width]
		metric.width = pReader->Read<short>();
		//	> Retrieve & Set Height [FontMetric::height]
		metric.height = pReader->Read<short>();
		//	> Retrieve & Set OffsetX [FontMetric::offsetX]
		metric.offsetX = pReader->Read<short>();
		//	> Retrieve & Set OffsetY [FontMetric::offsetY]
		metric.offsetY = pReader->Read<short>();
		//	> Retrieve & Set AdvanceX [FontMetric::advanceX]
		metric.advanceX = pReader->Read<short>();
		//	> Retrieve & Set Page [FontMetric::page]
		metric.page = pReader->Read<byte>();
		//	> Retrieve Channel (BITFIELD!!!) 
		//		> See documentation for BitField meaning [FontMetrix::channel]
		const byte channelBitmask = pReader->Read<byte>();
		if (channelBitmask & 1)
		{
			metric.channel = 2; //B
		}
		else if (channelBitmask & 2)
		{
			metric.channel = 1; //G
		}
		else if (channelBitmask & 4)
		{
			metric.channel = 0; //R
		}
		else if (channelBitmask & 8)
		{
			metric.channel = 3; //A
		}
		else if (channelBitmask & 16)
		{
			metric.channel = 4; //RGBA
		}
		//	> Calculate Texture Coordinates using Xposition, Yposition, fontDesc.TextureWidth & fontDesc.TextureHeight [FontMetric::texCoord]
		metric.texCoord = { xPosition / fontDesc.textureWidth, yPosition / fontDesc.textureHeight };
		//> Insert new FontMetric to the metrics [font.metrics] map
		//	> key = (wchar_t) charId
		//	> value = new FontMetric
		fontDesc.metrics.insert(std::pair<const wchar_t, FontMetric&>(id, metric));
		//(loop restarts till all metrics are parsed)
	}
	//Done!
	delete pReader;
	return new SpriteFont(fontDesc);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
