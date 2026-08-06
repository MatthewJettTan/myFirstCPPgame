#include "saveMap.h"
#include <asserts.h>

struct BlockSaveRepresentation1
{
	std::uint16_t type = 0;

	// from representation to its real block
	Block toBlock()
	{
		Block b;
		b.type = type;
		return b;
	}
};

struct BlockSaveRepresentation2
{
	std::uint16_t type = 0;
	std::uint8_t durability = 0;

	// from representation to its real block
	Block toBlock()
	{
		Block b;
		b.type = type;
		b.durability = durability;
		return b;
	}
};

const int VERSION = 2;

// From real block to its representation
BlockSaveRepresentation2 toBlockRepresentation(Block b)
{
	BlockSaveRepresentation2 rez;
	rez.type = b.type;
	rez.durability = b.durability;
	return rez;
}


bool saveBlockDataToFile(std::vector<Block>& blocks, int w, int h, const char* fileName)
// the blocks here is the data source for save
{

	std::ofstream f(fileName, std::ios::binary);	// open a file with binary mode

	if (!f.is_open()) { return false; }		// if can't open it

	permaAssertCommentDevelopement(blocks.size() == w * h);
	permaAssertCommentDevelopement(blocks.size() != 0);
	if (blocks.size() != w * h) { return false; }
	if (blocks.size() == 0) { return false; }

	f.write((const char*)&VERSION, sizeof(VERSION));
	f.write((const char*)&w, sizeof(w));
	f.write((const char*)&h, sizeof(h));

	for (int i = 0; i < blocks.size(); i++)
	{
		auto b = toBlockRepresentation(blocks[i]);
		f.write((const char*)&b, sizeof(b));
	}
	// f.write((const char*)blocks.data(), sizeof(Block) * blocks.size());

	f.close();

	return true;

}


// the core of forward compability
bool loadBlockDataFromFile(std::vector<Block>& blocks, int& w, int& h, const char* fileName)
// the blocks here is the receiving buffer before writing into formal block
{

	blocks.clear();
	w = 0;
	h = 0;

	std::ifstream f(fileName, std::ios::binary);

	if (!f.is_open()) { return false; }

	// Read data
	int readVersion = 0;

	f.read((char*)&readVersion, sizeof(readVersion));		// copy 4 binary-bytes into the address of "readVersion"
	f.read((char*)&w, sizeof(w));
	f.read((char*)&h, sizeof(h));

	if (!f || w <= 0 || h <= 0)
	{
		f.close();
		return false;
	}

	if (w > 10000) { f.close(); return false; }	 // propably corrupt data
	if (h > 10000) { f.close(); return false; }	 // propably corrupt data

	// Read block data
	switch (readVersion)
	{
		case 1:
		{
			size_t blockCount = w * h;
			blocks.resize(blockCount);  // set enough space for storing Blocks

			// f.read((char*)blocks.data(), sizeof(Block) * blockCount);
			for (int i = 0; i < blockCount; i++)	// loop through and read each block in sequence
			{
				BlockSaveRepresentation1 read;
				f.read((char*)&read, sizeof(read));	// read the block and write it in temporary RepresentStructure

				if (!f)		// check the status of the file stream after reading
				{
					blocks.clear();
					w = 0;
					h = 0;
					f.close();
					return false;
				}

				blocks[i] = read.toBlock();		// convert the temporary structure to a formal block
			}

			break;
		}
		case 2:
		{
			size_t blockCount = w * h;
			blocks.resize(blockCount);

			// f.read((char*)blocks.data(), sizeof(Block) * blockCount);
			for (int i = 0; i < blockCount; i++)
			{
				BlockSaveRepresentation2 read;
				f.read((char*)&read, sizeof(read));

				if (!f)
				{
					blocks.clear();
					w = 0;
					h = 0;
					f.close();
					return false;
				}

				blocks[i] = read.toBlock();
			}

			break;
		}
		default:
		{
			// incorrect version
			blocks.clear();
			w = 0;
			h = 0;
			f.close();
			return false;
		}
	}

	// cleaning and vertification
	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i].sanitize();
	}

	f.close();
	return true;
}