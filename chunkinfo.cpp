#include "chunkinfo.h"

ChunkInfo::ChunkInfo() :
    words(0),
    chars(0),
    bytes(0),
    lines(0),
    maxLine(0),
    filename(0),
    offset(0)
{
}

char *ChunkInfo::getFilename() const
{
    return filename;
}

void ChunkInfo::setFilename(char *value)
{
    filename = value;
}

char *ChunkInfo::getBuffer() const
{
    return buffer;
}

void ChunkInfo::setBuffer(char* value)
{
    buffer = value;
}
