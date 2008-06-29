#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstring>
#include <cstdio>

#include "sboxwrit.h"

using namespace std;

void copy_file_to_sbox(SboxWriteHandle * out, FILE * f)
{
    char buffer[4096];
    int size;
    while(!feof(f))
    {
        size = fread(buffer, 1, 4096, f);
        if(size)
            SboxWriteData(out, buffer, size);
    }
}

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        cerr << "Usage: buildbox <file.sbox> [file ...]\n";
        return 1;
    }

    SboxWriteHandle * sbox;
    SboxWriteOpenFilename(&sbox, argv[1], "arithmetix-files");

    for(char ** file = argv+2; *file; ++file)
    {
        if(FILE * infile = fopen(*file,"rb"))
        {
            cout << "Adding " << *file << endl;
            SboxWriteStartItemNamed(sbox, *file, strlen(*file));
            copy_file_to_sbox(sbox, infile);
            SboxWriteEndItem(sbox);
        }
        else
        {
            cerr << "Failed to open " << *file << " for reading.\n";
        }
    }

    SboxWriteClose(sbox);
}
