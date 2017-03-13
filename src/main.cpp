
#include "main.hpp"
#include <vector>
#include <SFML/System/Clock.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include "mingw.thread.h"
#include "mingw.mutex.h"
#include <queue>

int main()
{
    gameLoop();
    return 0;
}
