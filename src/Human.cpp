
#include "Human.hpp"

int currentEntity = 0;
sf::Clock timeToDo;

Human::Human(System &mainSystem) : Entity(mainSystem)
{
    clicked = false;
    locked = false;


    color = sf::Color(0,0,0);
    width = 1;
    height = 1;
}

Human::Human(sf::Color rectColor, int widthInit, int heightInit, System &mainSystem) : Entity(mainSystem)
{
    clicked = false;
    locked = false;

    age = 0;
    color = rectColor;
    width = widthInit;
    height = heightInit;
}

Human::Human(sf::Color rectColor, int widthInit, int heightInit, Entity& initEntity, System &mainSystem) : Entity(initEntity)
{
    clicked = false;
    locked = false;

    age = 0;
    color = rectColor;
    width = widthInit;
    height = heightInit;

}

Human::Human()
{
    clicked = false;
    locked = false;

    age = 0;
    color = sf::Color(255,255,255);
    width = 1;
    height = 1;
}

Human::~Human()
{

}

static void Human::Update(std::map<unsigned int, Human>& allHumans, sf::RenderWindow &window, System &mainSystem)
{
    double speed(0);

    std::map<unsigned int, Human>::const_iterator it;
    Human currentHuman;

    sf::Vector2f previousPosition;
    //for (int i(currentEntity) ; i< allHumans.size() ;i++)

    /**
    Following pattern :
        Check stuff related to UI
        Make AI roll (update position etc.)
        Update chunk presence
    */

    for (it = allHumans.begin() ; it != allHumans.end() ; it++)
    {
        currentHuman = it->second;
        previousPosition = sf::Vector2f(it->second.getPosX(), it->second.getPosY());

        if (mainSystem.checkMouseButtonClicked(sf::Mouse::Left))
        {
            if (it->second.updateClick(sf::Mouse::getPosition(window),
                                     static_cast<int>(mainSystem.getXUp()),
                                     static_cast<int>(mainSystem.getYUp())))
            {
                mainSystem.nullifyClick(sf::Mouse::Left);
            }
        }

        if (it->second.isLocked())//If it is locked, we attach it to the mouse
        {
            it->second.setX(sf::Mouse::getPosition(window).x + mainSystem.getXUp());
            it->second.setY(sf::Mouse::getPosition(window).y + mainSystem.getYUp());
        }
        else
        {
            it->second.setX( - speed + it->second.getPosX());
            it->second.setY( - speed + it->second.getPosY());
        }

        it->second.manageHumanPositionInChunks(previousPosition, sf::Vector2f(it->second.getPosX(), it->second.getPosY()), mainSystem);
    }
}

bool collisionBetweenHumans(Human first, Human second)
{
    if ((first.getPosX() >= second.getPosX()) &&
        (first.getPosX() <= second.getPosX()) &&
        (first.getPosY() >= second.getPosY()) &&
        (first.getPosY() <= second.getPosY()))
        return true;
    else
        return false;
}

bool Human::isLocked() const
{
    return locked;
}

void Human::manageHumanPositionInChunks(sf::Vector2f previousPos, sf::Vector2f actualPos, System& mainSystem)
{
    int previousEntityChunkX = std::floor(previousPos.x / (CHUNK_SIZE * TILE_SIZE));
    int previousEntityChunkY = std::floor(previousPos.y / (CHUNK_SIZE * TILE_SIZE));
    int previousMaxEntityChunkX = std::floor((previousPos.x + getWidth()) / (CHUNK_SIZE * TILE_SIZE));
    int previousMaxEntityChunkY = std::floor((previousPos.y + getHeight()) / (CHUNK_SIZE * TILE_SIZE));

    int entityChunkX = std::floor(actualPos.x / (CHUNK_SIZE * TILE_SIZE));
    int entityChunkY = std::floor(actualPos.y / (CHUNK_SIZE * TILE_SIZE));
    int maxEntityChunkX = std::floor((actualPos.x + getWidth()) / (CHUNK_SIZE * TILE_SIZE));
    int maxEntityChunkY = std::floor((actualPos.y + getHeight()) / (CHUNK_SIZE * TILE_SIZE));

    int intersectionChunkX;
    int intersectionChunkY;
    int maxIntersectionChunkX;
    int maxIntersectionChunkY;

    if ((entityChunkX != previousEntityChunkX) ||
        (entityChunkY != previousEntityChunkY) ||
        (maxEntityChunkX != previousMaxEntityChunkX) ||
        (maxEntityChunkY != previousMaxEntityChunkY))
    {
        if(maxEntityChunkX < previousEntityChunkX || entityChunkX > previousMaxEntityChunkX ||
           maxEntityChunkY < previousEntityChunkY || entityChunkY > previousMaxEntityChunkY)//There is no intersection between the spaces
        {
            //We remove the entity from the previous chunks
            for (int j(previousEntityChunkX) ; j <= previousMaxEntityChunkX ; j++)
            {
                for (int k(previousEntityChunkY) ; k <= previousMaxEntityChunkY ; k++)
                {
                    mainSystem.removeEntityIDFromChunk(j,k, getID());
                }
            }

            //We then add the new chunks
            for (int j(entityChunkX) ; j <= maxEntityChunkX ; j++)
            {
                for (int k(entityChunkY) ; k <= maxEntityChunkY ; k++)
                {
                    mainSystem.addEntityIDToChunk(j,k,getID());
                }
            }

        }
        else
        {
            intersectionChunkX = 0;
            intersectionChunkY = 0;
            maxIntersectionChunkX = 0;
            maxIntersectionChunkY = 0;

            if (entityChunkX < previousEntityChunkX || maxEntityChunkX < previousMaxEntityChunkX)//Left
            {
                intersectionChunkX = previousEntityChunkX;
                maxIntersectionChunkX = maxEntityChunkX;
            }
            else if (entityChunkX > previousEntityChunkX || maxEntityChunkX > previousMaxEntityChunkX)//right
            {
                intersectionChunkX = entityChunkX;
                maxIntersectionChunkX = previousMaxEntityChunkX;
            }
            else if (entityChunkX == previousEntityChunkX && maxEntityChunkX == previousMaxEntityChunkX)//No left/right
            {
                intersectionChunkX = entityChunkX;
                maxIntersectionChunkX = maxEntityChunkX;
            }

            if (entityChunkY < previousEntityChunkY || maxEntityChunkY < previousMaxEntityChunkY)//Up
            {
                intersectionChunkY = previousEntityChunkY;
                maxIntersectionChunkY = maxEntityChunkY;
            }
            else if (entityChunkY > previousEntityChunkY || maxEntityChunkY > previousMaxEntityChunkY)//Down
            {
                intersectionChunkY = entityChunkY;
                maxIntersectionChunkY = previousMaxEntityChunkY;
            }
            else if (entityChunkY == previousEntityChunkY && maxEntityChunkY == previousMaxEntityChunkY)//No up/down
            {
                intersectionChunkY = entityChunkY;
                maxIntersectionChunkY = maxEntityChunkY;
            }


            //We first remove chunks that are not part of the intersection and that we covered by the previous coordinates
            for (int j(previousEntityChunkX) ; j <= previousMaxEntityChunkX ; j++)
                for (int k(previousEntityChunkY) ; k <= previousMaxEntityChunkY ; k++)
                    if ((!(j >= intersectionChunkX && j <= maxIntersectionChunkX)) ||
                        (!(k >= intersectionChunkY && k <= maxIntersectionChunkY)))
                        mainSystem.removeEntityIDFromChunk(j,k,getID());


            //Then, we add every chunk covered by the new coordinates and that is not part of the intersection
            for (int j(entityChunkX) ; j <= maxEntityChunkX ; j++)
                for (int k(entityChunkY) ; k <= maxEntityChunkY ; k++)
                    if ((!(j >= intersectionChunkX && j <= maxIntersectionChunkX)) ||
                        (!(k >= intersectionChunkY && k <= maxIntersectionChunkY)))
                        mainSystem.addEntityIDToChunk(j,k,getID());
        }
    }
}

static void Human::Draw(std::map<unsigned int, Human>& allHumans,sf::RenderTarget& window, int xWindow, int yWindow, int windowWidth, int windowHeight)
{
    for (int i(0); i<allHumans.size();i++)
    {
        if ((allHumans[i].x < xWindow + windowWidth)
            && (allHumans[i].x + allHumans[i].width > xWindow)
            && (allHumans[i].y < yWindow + windowHeight)
            && (allHumans[i].y + allHumans[i].height > yWindow))
        {
            allHumans[i].drawHuman(window, xWindow, yWindow);
        }
    }
}

bool Human::updateClick(sf::Vector2i mousePos, int xUp, int yUp)
{
    if ((mousePos.x + xUp >= x && mousePos.x + xUp <= x+width) && (mousePos.y + yUp >= y && mousePos.y + yUp <= y+height))
    {
        clicked = !clicked;
        if (clicked == false)
            std::cout << "Human #" <<  ID << " was unclicked\n";
        if (clicked == true)
            std::cout << "Human #" <<  ID << " was clicked\n";

        return true;
    }
    return false;
}

void Human::drawHuman(sf::RenderTarget& window, int xWindow, int yWindow)
{
    rectangleRaw.setFillColor(color);
    rectangleRaw.setPosition(x - static_cast<double>(xWindow) , y - static_cast<double>(yWindow));
    rectangleRaw.setSize(sf::Vector2f(width, height));
    window.draw(rectangleRaw);

    if (clicked)
    {
        std::stringstream idString;
        idString << "#" <<ID;

        /*ImGui::Begin(idString.str().c_str());

        ImGui::Text("Entity ID : %d", ID);
        ImGui::Text("Age : %d", age);
        ImGui::Text("Position : (%f;%f)", x, y);

        if (ImGui::Button("Lock human"))
            locked = !locked;

        if (ImGui::Button("Close"))
            clicked = false;



        ImGui::End();*/
    }
}
