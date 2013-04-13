#include <iostream>
#include <vector>
#include <getopt.h>
#include <cmath>
#include <cstdio>
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_vector.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "chunkinfo.h"
#include "tbb/mutex.h"
#include "tbb/concurrent_hash_map.h"



using namespace tbb;

struct FileInfo {
    FileInfo() :
        words(0),
        lines(0),
        chars(0)
    {}
    int words;
    int lines;
    int chars;
    char* filename;
    int idx;
};

char* flags = "wlmchv";
void print_help_and_exit(char* name)
{
    std::cout << "usage: " << name << "-[" << flags << "] file1 [file2 ...]" << std::endl;
    exit(EXIT_SUCCESS);
}

void print_version_and_exit()
{
    std::cout << "wc-boost v.0.0, author Adam Syrek" <<std::endl;
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    bool w = false, l = false, c = false;
    bool flag_set = false;
    int n_flags = 1;
    char flag;
    while((flag = getopt(argc, argv, flags)) != -1)
    {
        switch(flag)
        {
        case 'm':
        case 'c':
            c = true;
            n_flags++;
            flag_set = true;
            break;
        case 'l':
            l = true;
            n_flags ++;
            flag_set = true;
            break;
        case 'w':
            w = true;
            n_flags ++;
            flag_set = true;
            break;
        case 'h':
            print_help_and_exit(argv[0]);
            break;
        case 'v':
            print_version_and_exit();
            break;
        }
    }
    if(!flag_set)
    {
        w = true;
        l = true;
        c = true;
        argv++;
        argc--;
    }
    else{
        argv += optind;
        argc -= optind;
    }
    const int BUF_SIZE = 500;
    concurrent_vector<FileInfo*> files;
    parallel_for(blocked_range<size_t>(0, argc, 1),
                 [&](const blocked_range<size_t>& r)
    {
        for(int i = r.begin(); i < r.end(); ++i)
        {
            mutex m;
            concurrent_vector<ChunkInfo*> chunks;
            FileInfo* tmpf = new FileInfo();
            tmpf->filename = argv[i];
            tmpf->idx = i;
            files.push_back(tmpf);
            FILE* file = fopen(argv[i], "rb");
            if(!file)
                return false;
            fseek(file, 0, SEEK_END);
            int n_chunks = std::ceil((double)ftell(file) / (double)BUF_SIZE);
            rewind(file);
            for(int k = 0; k < n_chunks; ++k)
            {
                ChunkInfo* ch = new ChunkInfo();
                ch->setFilename(argv[i]);
                ch->offset = k * BUF_SIZE;
                chunks.push_back(ch);
            }
            parallel_for(blocked_range<size_t>(0, n_chunks, 1),
                         [&](const blocked_range<size_t>& r2)
            {
                for(int j = r2.begin(); j < r2.end(); ++j)
                {
                    ChunkInfo* chunk = chunks[j];
                    char* buf = new char[BUF_SIZE];
                    m.lock();
                    fseek(file, chunk->offset, SEEK_SET);
                    int read_bytes = fread(buf, sizeof(char), BUF_SIZE, file);
                    m.unlock();
                    chunk->bytes = read_bytes;
                    chunk->setBuffer(buf);
                    char* tmp = chunk->getBuffer();
                    for(int n = 0; n < chunk->bytes; ++n)
                    {
                        chunk->chars++;
                        if(isspace(tmp[n]) && (n > 0 ? !isspace(tmp[n-1]) : 0))
                        {
                            chunk->words++;
                        }
                        if(tmp[n] == '\n')
                        {
                            chunk->lines++;
                        }
                    }
                    m.lock();
                    tmpf->words += chunk->words;
                    tmpf->chars += chunk->chars;
                    tmpf->lines += chunk->lines;
                    m.unlock();
               }
          });
        }
    });
    std::for_each(files.begin(), files.end(),
                  [](FileInfo* ch)
    {
        std::cout << ch->filename << " " << ch->lines << " " << ch->words << " " << ch->chars << '\n';
    });
exit(EXIT_SUCCESS);
}

