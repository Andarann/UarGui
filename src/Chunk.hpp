#ifndef CHUNK_HPP_INCLUDED
#define CHUNK_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <locale>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <SFML/Graphics.hpp>

#include "Classes.h"

std::string extract_ints(std::ctype_base::mask category, std::string str, std::ctype<char> const& facet);
std::string extract_ints(std::string str);

#endif // CHUNK_HPP_INCLUDED
