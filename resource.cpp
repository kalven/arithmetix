#include "resource.hpp"

#include "sboxread.h"
#include "sboxkit.h"

#include <stdexcept>
#include <cassert>
#include <string>

static SboxHandle * sbox = 0;

static void init_sbox()
{
    assert(sbox == 0);
    if(SboxReadOpenFilename(&sbox, "arithmetix.sbox", "arithmetix-files") != SBOX_OK)
        throw std::runtime_error("Arithmetix data file error");
}

FILE * get_resource_file(const char * resource)
{
    if(sbox == 0)
        init_sbox();

    unsigned sbox_item = SboxkitFindString(sbox, const_cast<char*>(resource));

    if(sbox_item == SBOXKIT_NOTFOUND)
        throw std::runtime_error(std::string(resource) + " not found in sbox");
    if(SboxSeekItem(sbox, sbox_item, 0) != SBOX_OK)
        throw std::runtime_error(std::string(resource) + " seek error");

    return SboxFileHandle(sbox);
}

void * get_resource_mem(const char * resource, int * length)
{
    assert(length);

    if(sbox == 0)
        init_sbox();

    void * res;
    unsigned item_length = SboxkitGetByString(&res, sbox, const_cast<char*>(resource));
    if(item_length == 0)
        throw std::runtime_error(std::string(resource) + " not found in sbox");

    *length = item_length;
    return res;
}
