#ifndef PRODUCT
#define PRODUCT

#include "macros.hpp"
#include "user.entity.hpp"

ENTITY
struct Product{
    ID
    int id;

    MANY_TO_ONE
    User user;
};


#endif