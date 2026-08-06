#include "saveMap.h"
#include <asserts.h>

bool saveBlockDataToFile(std::vector<Block>& blocks, int w, int h, const char* fileName)
{

	std::ofstream f(fileName, std::ios::binary);	// open a file with binary mode

	if (!f.is_open()) { return false; }		// if can't open it

	permaAssertCommentDevelopement(blocks.size() == w * h);
	permaAssertCommentDevelopement(blocks.size() != 0);
	if (blocks.size() != w * h) { return false; }
	if (blocks.size() == 0) { return false; }

	f.write((const char*)&w, sizeof(w));
	f.write((const char*)&h, sizeof(h));

	f.write((const char*)blocks.data(), sizeof(Block) * blocks.size());

	f.close();

	return true;

}

bool loadBlockDataFromFile(std::vector<Block>& blocks, int& w, int& h, const char* fileName)
{

	blocks.clear();
	w = 0;
	h = 0;

	std::ifstream f(fileName, std::ios::binary);

	if (!f.is_open()) { return false; }

	// Read demensions
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
	size_t blockCount = w * h;
	blocks.resize(blockCount);

	f.read((char*)blocks.data(), sizeof(Block) * blockCount);

	if (!f)
	{
		blocks.clear();
		w = 0;
		h = 0;
		f.close();
		return false;
	}

	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i].sanitize();
	}

	f.close();
	return false;
}