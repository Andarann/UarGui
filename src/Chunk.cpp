
#include "Chunk.hpp"

Chunk::Chunk()
{
    x = 0;
    y = 0;
    updated = false;

    for (int i(0) ; i < CHUNK_SIZE ; i++)
    {
        for (int j(0) ; j < CHUNK_SIZE ; j++)
        {
            arrayTiles[i][j] = 0;
        }
    }
}

Chunk::Chunk(int xChunk, int yChunk)
{
    x = xChunk;
    y = yChunk;
    updated = false;

    std::stringstream fileName;

    fileName << "Chunks/[" << x << ';' << y << "].cnk";

    std::ifstream saveFile(fileName.str().c_str());
    std::string readLine;

    if (saveFile)//If a saveFile was found...
    {

        //std::cout << "A save file was found !\n";
        getline(saveFile, readLine);
        getline(saveFile, readLine);

        int j(0);

        while (saveFile)
        {
            getline(saveFile, readLine);
            std::stringstream readInt(extract_ints(readLine));

            for (int i(0) ; i < CHUNK_SIZE ; i++)
            {
                readInt >> arrayTiles[i][j];
            }

            j++;
        }
    }
    else
    {
        for (int i(0) ; i < CHUNK_SIZE ; i++)
        {
            for (int j(0) ; j < CHUNK_SIZE ; j++)
            {
                arrayTiles[i][j] = i*j;
                //arrayTiles[i][j] = 0;
            }
        }
    }
}

std::string extract_ints(std::ctype_base::mask category, std::string str, std::ctype<char> const& facet)
{
    using std::strlen;

    char const *begin = &str.front(),
               *end   = &str.back();

    auto res = facet.scan_is(category, begin, end);

    begin = &res[0];
    end   = &res[strlen(res)];

    return std::string(begin, end);
}

std::string extract_ints(std::string str)
{
    return extract_ints(std::ctype_base::digit, str,
         std::use_facet<std::ctype<char>>(std::locale("")));
}

Chunk::Chunk(int toFill, int xChunk, int yChunk)
{
    x = xChunk;
    y = yChunk;
    updated = false;

    for (int i(0) ; i < CHUNK_SIZE ; i++)
    {
        for (int j(0) ; j < CHUNK_SIZE ; j++)
        {
            arrayTiles[i][j] = toFill;
        }
    }
}

Chunk::Chunk(std::string fileLoad, int xChunk, int yChunk)
{

}

void Chunk::fusionEntityList(Chunk& sender, Chunk& receiver)
{
    for (int i(0) ; i < sender.entityInChunkID.size() ; i++)
    {
        receiver.entityInChunkID.push_back(sender.entityInChunkID[i]);
    }
}

int Chunk::getX() const
{
    return x;
}

int Chunk::getY() const
{
    return y;
}

Chunk::~Chunk()
{

}



bool Chunk::isEmptyOfEnties() const
{
    return (entityInChunkID.empty());
}

int Chunk::getID(int x, int y) const
{
    return arrayTiles[x][y];
}

void Chunk::setID(int x, int y, ID_TYPE ID)
{
    if (arrayTiles[x][y] != ID)
        updated = true;

    arrayTiles[x][y] = ID;
}

void Chunk::addEntity(ID_TYPE entityToAddID)
{
    entityInChunkID.push_back(entityToAddID);
}

void Chunk::removeEntityFromChunk(ID_TYPE toRemoveID)
{
    for (ID_TYPE i(0) ; i < entityInChunkID.size() ; i++)
    {
        if (entityInChunkID[i] == toRemoveID)
        {
            entityInChunkID.erase(entityInChunkID.begin() + i);
            break;
        }
    }
}

int Chunk::getNumberOfEntities() const
{
    return entityInChunkID.size();
}

int Chunk::getID(int position) const
{
    return entityInChunkID[position];
}

bool Chunk::isEntityInChunk(ID_TYPE toCheckID) const
{
    bool found(false);

    for (ID_TYPE i(0) ; i < entityInChunkID.size() ; i++)
    {
        if (entityInChunkID[i] == toCheckID)
        {
            found = true;
            break;
        }
    }

    return found;
}

Chunk Chunk::loadChunk(std::string fileLoad) const
{

}

void Chunk::saveChunk() const
{
    if (updated)
    {
        //We want to save the chunk to wherever file he's supposed to be saved. TO OPTIMISE
        std::stringstream fileName;

        fileName << "Chunks/[" << x << ';' << y << "].cnk";

        std::ofstream saveFile(fileName.str());

        if (!saveFile)
        {
            std::cout << "File for chunk [" << x << ';' << y << "] couldn't be opened\n";
        }
        else
        {
            //The chunk format
            saveFile << "CHUNK_SIZE = " << CHUNK_SIZE << '\n';
            saveFile << "TILE_SIZE = " << TILE_SIZE << '\n';

            for (int i(0) ; i < CHUNK_SIZE ; i++)
            {
                for (int j(0) ; j < CHUNK_SIZE ; j++)
                {
                    saveFile << arrayTiles[j][i] << ' ';
                }
                saveFile << '\n';
            }
        }
    }
}
