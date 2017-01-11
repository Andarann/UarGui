
#include "Entity.hpp"

Entity::Entity()
{

    x = 0;
    y = 0;


}

Entity::Entity(int xInit, int yInit)
{

    x = xInit;
    y = yInit;

}

Entity::Entity(System& mainSystem)
{
    x = 0;
    y = 0;

    //mainSystem.addEntityToSystem(this);
}

Entity::Entity(int xInit, int yInit, int widthInit, int heightInit, System& mainSystem)
{
    x = xInit;
    y = yInit;

    //mainSystem.addEntityToSystem(this);
}

Entity::Entity(Entity& toCopy, System& mainSystem)
{
    *this = toCopy;

    //mainSystem.addEntityToSystem(this);
}

Entity::~Entity()
{

}

int Entity::getWidth() const
{
    return width;
}

int Entity::getHeight() const
{
    return height;
}

double Entity::getPosX() const
{
    return x;
}

double Entity::getPosY() const
{
    return y;
}

unsigned int Entity::getID() const
{
    return ID;
}

void Entity::setID(unsigned int newID)
{
    ID = newID;
}

void Entity::setX(double xNew)
{
    x = xNew;
}

void Entity::setY(double yNew)
{
    y = yNew;
}
