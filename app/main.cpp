#include <iostream>
#include "scanner.hpp"
int main(){
    HeaderScanner scanner;

    scanner.scanAllEntities("models");

    return 0;
}