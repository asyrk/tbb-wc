#ifndef FILEINFO_H
#define FILEINFO_H

class ChunkInfo
{
private:
    char* filename;
    char* buffer;

public:
    int offset;
    int words;
    int chars;
    int bytes;
    int lines;
    int maxLine;

    ChunkInfo();

    char *getBuffer() const;
    void setBuffer(char* value);

    char *getFilename() const;
    void setFilename(char *value);
};

#endif // FILEINFO_H
