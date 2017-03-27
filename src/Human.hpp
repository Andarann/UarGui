#ifndef HUMAN_HPP_INCLUDED
#define HUMAN_HPP_INCLUDED



#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <vector>

enum Sex{
    MALE,
    FEMALE
};

enum PERSONNALITY_TRAITS{
    TEMPERAMENT,//Describes whether the character is rather aggressive or calm
    /*Covered traits : */
    DISCIPLINE,//Involvement in what he prefers (hobbies, matters, domains...)
    /*Covered traits : */
    TOLERANCE,//Defines behavior toward people of different beliefs : religious, cultural, different groups...
    /*Covered traits : */
    CONFIDENCE,//Skill of the character to take decision easily, and to assume what he does (for example, his passions)...
    /*Covered traits : */
    AMBITION,//An ambitious character always wants more and is prone to jealousy, when a non-ambitious character is mostly happy of himself
    /*Covered traits : */
    FIDELITY//A fidel character
};

#include "Classes.h"

bool collisionBetweenHumans(Human first, Human second);

#endif // HUMAN_HPP_INCLUDED
