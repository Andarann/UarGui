
#include "game.hpp"
#include "Chunk.hpp"
#include "Entity.hpp"
#include "Human.hpp"
#include "Nodes.hpp"

#include "RichText+.hpp"

unsigned int Entity::currentID = 0;//Everytime the program is launched, currentID is initialized to 0
unsigned int wishedFPS = 60;
unsigned int Path::currentID = 0;

void gameLoop()
{
    System mainSystem;

    Entity temp;
    Widget::Init(*mainSystem.returnRenderWindowPointer());



    for (int i(0); i < 20 ; i++)
    {
        temp = Entity(i*200, i*200);
        mainSystem.addHumanToSystem(Human(sf::Color(255,255,255), 50, 50, temp, mainSystem));
    }

    sf::Color myColor(255,0,0);



    mainSystem.addWidget( InputString(sf::Vector2f(50,50), sf::Vector2f(100,100), "C:/Users/GUIGUI/Desktop/TeSFML/resources/window_background.png", sf::Vector2f(0,0), sf::Vector2f(1,1), 50), "Me");

    InputString *me = mainSystem.giveWidgetPointer("Me");

    do
    {
        mainSystem.Update();

        std::cout << me->returnValueAsUtf8() << '\n';

        mainSystem.Render();

    }while (1);
}

void System::Update()
{
    /**
    Order to keep:
    FPS count
    SFML updates
    Hardware updates (keyboard or mouse)
    GUI updates
    Game updates*/
    if (clockFPS.getElapsedTime().asSeconds() >= 1.0)
    {
        FPS = numberOfFrames;

        if (numberOfFrames == 0)
            FPS = 1;

        numberOfFrames = 0;
        clockFPS.restart();
    }

    windowMain.pollEvent(event);

    updateMouseButton();
    updateKeyboardButton();
    manageWindowEvent(event, windowMain, FPS);

    Widget::UpdateEvents(windowMain);

    updateWidgets();

    Human::Update(allHumans, windowMain, *this);

    updateChunksList(windowMain);
}

void System::Render()
{
    sf::Font font;
    font.loadFromFile("ariali.ttf");

    windowMain.clear();//Rendering part
    {
        displayChunks(windowMain);

        Human::Draw(allHumans, windowMain,
                    static_cast<int>(getXUp()),
                    static_cast<int>(getYUp()),
                    static_cast<int>(windowMain.getSize().x),
                    static_cast<int>(windowMain.getSize().y));


        writeDebugInfo(windowMain, FPS);
        //mainSystem.editMenu();
        console();

        renderWidgets();
//        ImGui::Render();
    }
    windowMain.display();

    numberOfFrames++;
}

void System::blockFPS(sf::Clock &fpsClock, int &currentFrameNumber, const int wishedFPS) const
{
    while (fpsClock.getElapsedTime().asSeconds() < static_cast<float>(currentFrameNumber)/static_cast<float>(wishedFPS));
}

void System::updateChunksList(sf::RenderWindow& window)
{
    xChunk = std::floor(xUp / (CHUNK_SIZE * TILE_SIZE));
    yChunk = std::floor(yUp / (CHUNK_SIZE * TILE_SIZE));

    //The number of chunks on the width
    double fitOnScreenWidth(static_cast<double>(window.getSize().x) / (CHUNK_SIZE * TILE_SIZE));

    if (fitOnScreenWidth > static_cast<int>(fitOnScreenWidth))
        fitOnScreenWidth = static_cast<int>(fitOnScreenWidth) + 1;

    if ((xChunk*CHUNK_SIZE*TILE_SIZE)
           + (fitOnScreenWidth*CHUNK_SIZE*TILE_SIZE) < static_cast<double>(window.getSize().x) + std::floor(xUp))//We check if the number of chunks fit the width
        fitOnScreenWidth++;

    //Same, the number of chunks on the height
    double fitOnScreenHeight(static_cast<double>(window.getSize().y) / (CHUNK_SIZE * TILE_SIZE));

    if (fitOnScreenHeight > static_cast<int>(fitOnScreenHeight))
        fitOnScreenHeight = static_cast<int>(fitOnScreenHeight) + 1;

    if ((yChunk*CHUNK_SIZE*TILE_SIZE)
           + (fitOnScreenHeight*CHUNK_SIZE*TILE_SIZE) < static_cast<double>(window.getSize().y) + std::floor(yUp))//If the whole screen is not filled
        fitOnScreenHeight++;

    unloadHiddenChunks(fitOnScreenWidth, fitOnScreenHeight);
    addNewChunks(fitOnScreenWidth, fitOnScreenHeight);

    //std::cout << visibleChunks.size() << "    " <<'\r';
}

std::map<ID_TYPE, Human> System::returnHumanVector() const
{
    return allHumans;
}

std::map<ID_TYPE, Human>* System::returnHumanVectorPointer() const
{
    return &allHumans;
}

sf::RenderWindow* System::returnRenderWindowPointer() const
{
    return &windowMain;
}

Human System::returnHumanOfID(ID_TYPE ID) const
{
    return allHumans[ID];
}

void System::addNewChunks(int fitOnScreenWidth, int fitOnScreenHeight)
{
    if (visibleChunks.empty())
    {
        for (int i(0) ; i < static_cast<int>(fitOnScreenWidth) ; i++)
        {
            for (int j(0) ; j < static_cast<int>(fitOnScreenHeight) ; j++)
            {
                visibleChunks.push_back(Chunk(xChunk + i, yChunk + j));
            }
        }

    }
    else
    {
        const int toFitWidth(static_cast<int>(fitOnScreenWidth));
        const int toFitHeight(static_cast<int>(fitOnScreenHeight));

        bool toFitTable[toFitHeight][toFitWidth];

        for (int i(0) ; i < toFitHeight ; i++)
        {
            for (int j(0) ; j < toFitWidth ; j++)
            {
                toFitTable[i][j] = false;
            }
        }

        for (int i(0) ; i < visibleChunks.size() ; i++)//We mark the chunks already loaded
            toFitTable[visibleChunks[i].getY() - yChunk][visibleChunks[i].getX() - xChunk] = true;

        for (int i(0) ; i < hiddenChunks.size() ; i++)//We check if some hidden chunk became visible
        {
            if ((hiddenChunks[i].getX() >= xChunk)
                && (hiddenChunks[i].getX() < xChunk + toFitWidth)
                && (hiddenChunks[i].getY() >= yChunk)
                && (hiddenChunks[i].getY() < yChunk + toFitHeight))
            {
                //Then we transfer the chunk from hidden to visible
                //std::cout << "Hidden chunk (" << hiddenChunks[i].getX() << ';'<< hiddenChunks[i].getY() <<") now visible\n";
                toFitTable[hiddenChunks[i].getY() - yChunk][hiddenChunks[i].getX() - xChunk] = true;
                transferChunkHiddenToVisible(i);
            }
        }

        for (int i(0) ; i < toFitHeight ; i++)//And we load the missing chunks
        {
            for (int j(0) ; j < toFitWidth ; j++)
            {
                if (toFitTable[i][j] == false)
                {
                    visibleChunks.push_back(Chunk(j + xChunk, i + yChunk));

                }
            }
        }
    }
}

void System::addHumanToSystem(Human toAdd)
{
    //We set the ID. Except if something else happened, we normally increment the variable currentID
    //However, we use a queue containing vacant IDs, to prevent overflows
    if (vacantHumanID.empty())
    {
        toAdd.setID(currentHumanID);
        currentHumanID++;
    }
    else
    {
        toAdd.setID(vacantHumanID.front());
        vacantHumanID.pop();
    }

    allHumans.insert(std::make_pair(toAdd.getID(), toAdd));

    double entityChunkX = std::floor(toAdd.getPosX() / (CHUNK_SIZE * TILE_SIZE));
    double entityChunkY = std::floor(toAdd.getPosY() / (CHUNK_SIZE * TILE_SIZE));
    double maxEntityChunkX = std::floor((toAdd.getPosX() + toAdd.getWidth()) / (CHUNK_SIZE * TILE_SIZE));
    double maxEntityChunkY = std::floor((toAdd.getPosY() + toAdd.getHeight()) / (CHUNK_SIZE * TILE_SIZE));
    //Most of the time, we'll have entityChunkX equal to maxEntityChunkX etc.
    //However, it can occur that a human is overlapping two chunks or more, depending on its size/position
    //We need to take account of that, especially since the chunk system helps calculating collisions

    for (int i(entityChunkX) ; i <= maxEntityChunkX ; i++)
    {
        for (int j(entityChunkY) ; j <= maxEntityChunkY ; j++)
        {
            addEntityIDToChunk(i,j,toAdd.getID());
        }
    }

}

void System::removeHumanFromSystem(ID_TYPE ID)
{
    Human toRemove = allHumans[ID];

    double entityChunkX = std::floor(toRemove.getPosX() / (CHUNK_SIZE * TILE_SIZE));
    double entityChunkY = std::floor(toRemove.getPosY() / (CHUNK_SIZE * TILE_SIZE));
    double maxEntityChunkX = std::floor((toRemove.getPosX() + toRemove.getWidth()) / (CHUNK_SIZE * TILE_SIZE));
    double maxEntityChunkY = std::floor((toRemove.getPosY() + toRemove.getHeight()) / (CHUNK_SIZE * TILE_SIZE));
    //Most of the time, we'll have entityChunkX equal to maxEntityChunkX etc.
    //However, it can occur that a human is overlapping two chunks or more, depending on its size/position
    //We need to take account of that, especially since the chunk system helps calculating collisions

    for (int i(entityChunkX) ; i <= maxEntityChunkX ; i++)
    {
        for (int j(entityChunkY) ; j <= maxEntityChunkY ; j++)
        {
            removeEntityIDFromChunk(i,j,ID);
        }
    }

    vacantHumanID.push(ID);

    allHumans.erase(ID);
}

void System::removeEntityIDFromChunk(int xChunkToTarget, int yChunkToTarget, ID_TYPE ID)
{
    chunkState checkState = giveChunkState(xChunkToTarget, yChunkToTarget);

    if (checkState == chunkState::HIDDEN)
    {
        hiddenChunks[hiddenChunkExist(xChunkToTarget, yChunkToTarget)].removeEntityFromChunk(ID);
    }
    else if (checkState == chunkState::VISIBLE)
    {
        visibleChunks[visibleChunkExist(xChunkToTarget, yChunkToTarget)].removeEntityFromChunk(ID);
    }
}

void System::addEntityIDToChunk(int xChunkToTarget, int yChunkToTarget, ID_TYPE ID)
{
    chunkState checkState = giveChunkState(xChunkToTarget, yChunkToTarget);

    if (checkState == chunkState::HIDDEN)
    {
        hiddenChunks[hiddenChunkExist(xChunkToTarget, yChunkToTarget)].addEntity(ID);
    }
    else if (checkState == chunkState::VISIBLE)
    {
        visibleChunks[visibleChunkExist(xChunkToTarget, yChunkToTarget)].addEntity(ID);
    }
    else if (checkState == chunkState::NONEXISTING)
    {
        hiddenChunks.push_back(Chunk(xChunkToTarget, yChunkToTarget));
        hiddenChunks[hiddenChunks.size() - 1].addEntity(ID);
    }
}

chunkState System::giveChunkState(int xChunkToCheck, int yChunkToCheck) const
{
    chunkState toReturn = chunkState::NONEXISTING;

    for (int i(0) ; i < visibleChunks.size() ; i++)
    {
        if (visibleChunks[i].getX() == xChunkToCheck && visibleChunks[i].getY() == yChunkToCheck)
        {
            toReturn = chunkState::VISIBLE;
            break;
        }
    }

    for (int i(0) ; i < hiddenChunks.size() ; i++)
    {
        if (hiddenChunks[i].getX() == xChunkToCheck && hiddenChunks[i].getY() == yChunkToCheck)
        {
            toReturn = chunkState::HIDDEN;
            break;
        }
    }

    return toReturn;
}

int System::visibleChunkExist(int xChunkToTest, int yChunkToTest) const
{
    int position(-1);

    for (int i(0) ; i < visibleChunks.size() ; i++ )
    {
        if (visibleChunks[i].getX() == xChunkToTest && visibleChunks[i].getY() == yChunkToTest)
        {
            position = i;
            break;
        }
    }

    return position;
}

int System::hiddenChunkExist(int xChunkToTest, int yChunkToTest) const
{
    int position(-1);

    for (int i(0) ; i < hiddenChunks.size() ; i++ )
    {
        if (hiddenChunks[i].getX() == xChunkToTest && hiddenChunks[i].getY() == yChunkToTest)
        {
            position = i;
            break;
        }
    }

    return position;
}

void System::unloadHiddenChunks(int fitOnScreenWidth, int fitOnScreenHeight)
{
    for (int i(0) ; i < visibleChunks.size() ; i++)
    {
        //We test if some chunks need to be unloaded
        if ((visibleChunks[i].getX() < xChunk)
            || (visibleChunks[i].getX() >= xChunk + fitOnScreenWidth)
            || (visibleChunks[i].getY() < yChunk)
            || (visibleChunks[i].getY() >= yChunk + fitOnScreenHeight))
        {
            if (visibleChunks[i].isEmptyOfEnties())
            {
                //std::cout << "Chunk (" << visibleChunks[i].getX() << ';'<< visibleChunks[i].getY() <<") was empty : deleted\n";
                visibleChunks[i].saveChunk();//Not transfered, deleted
                visibleChunks.erase(visibleChunks.begin() + i);
            }
            else
            {
                //std::cout << "Chunk (" << visibleChunks[i].getX() << ';'<< visibleChunks[i].getY() <<") wasn't empty : transfered\n";
                transferChunkVisibleToHidden(i);
            }

            i--;
        }//We don't do anything else to prevent access mistakes
    }

    for (int i(0) ; i < hiddenChunks.size() ; i++)
    {
        if (hiddenChunks[i].isEmptyOfEnties())
        {
            hiddenChunks[i].saveChunk();//Same as above, deleted from the world, so to be saved
            hiddenChunks.erase(hiddenChunks.begin() + i);
            i--;
        }
    }
}

void System::transferChunkVisibleToHidden(int positionInVisible)
{
    int position = hiddenChunkExist(visibleChunks[positionInVisible].getX(), visibleChunks[positionInVisible].getY());

    if (position <= -1)//Chunk doesn't exist
    {
        hiddenChunks.push_back(visibleChunks[positionInVisible]);
    }
    else
        hiddenChunks[position] = visibleChunks[positionInVisible];

    visibleChunks.erase(visibleChunks.begin() + positionInVisible);
}

void System::transferChunkHiddenToVisible(int positionInHidden)
{
    int position = visibleChunkExist(hiddenChunks[positionInHidden].getX(), hiddenChunks[positionInHidden].getY());

    if (position <= -1)//Chunk doesn't exist
    {
        visibleChunks.push_back(hiddenChunks[positionInHidden]);

    }
    else
    {
        visibleChunks[position] = hiddenChunks[positionInHidden];

    }


    hiddenChunks.erase(hiddenChunks.begin() + positionInHidden);
}

void System::displayChunks(sf::RenderWindow& window) const
{
    sf::RectangleShape tileDummy;
    tileDummy.setSize(sf::Vector2f(TILE_SIZE,TILE_SIZE));

    double tilePosX(0.0);
    double tilePosY(0.0);
    int tileID(0);
    for (int i(0) ; i < visibleChunks.size() ; i++)
    {
        for (int j(0) ; j < CHUNK_SIZE ; j++)
        {
            for (int k(0) ; k < CHUNK_SIZE ; k++)
            {
                tilePosX = ((visibleChunks[i].getX() * CHUNK_SIZE * TILE_SIZE) - xUp) + TILE_SIZE * j;
                tilePosY = ((visibleChunks[i].getY() * CHUNK_SIZE * TILE_SIZE) - yUp) + TILE_SIZE * k;
                tileID = visibleChunks[i].getID(j,k);

                if ((tilePosX + TILE_SIZE >= 0) &&
                    (tilePosX <= window.getSize().x) &&
                    (tilePosY + TILE_SIZE >= 0) &&
                    (tilePosY <= window.getSize().y))
                {
                    tileDummy.setSize(sf::Vector2f(TILE_SIZE,TILE_SIZE));
                    tileDummy.setFillColor(sf::Color(tileID*1, tileID*1, tileID*1));
                    tileDummy.setPosition(sf::Vector2f(tilePosX,tilePosY));
                    window.draw(tileDummy);
                }
            }
        }
        if (displayChunksBoundaries)
        {
            tileDummy.setFillColor(sf::Color(255,0,0, 200));
            tileDummy.setPosition(sf::Vector2f((visibleChunks[i].getX() * CHUNK_SIZE * TILE_SIZE) - xUp,
                                              (visibleChunks[i].getY() * CHUNK_SIZE * TILE_SIZE) - yUp));
            tileDummy.setSize(sf::Vector2f(1, TILE_SIZE*CHUNK_SIZE));
            window.draw(tileDummy);
            tileDummy.setSize(sf::Vector2f(TILE_SIZE*CHUNK_SIZE, 1));
            window.draw(tileDummy);
        }
    }
}

void System::updateMouseButton()
{
    for (int i(0);i<sf::Mouse::ButtonCount;i++)
    {
        if (sf::Mouse::isButtonPressed(i))//If the button is pressed...
        {
            if (mouseButtonClick[i] == false && mouseButtonsMaintained[i] == false)//We first for once set it on clicked...
            {
                mouseButtonClick[i] = true;
                ignoreMouseClick[i] = false;
            }
            else if (mouseButtonClick[i] == true && mouseButtonsMaintained[i] == false)//Then block it on maintained
            {
                mouseButtonClick[i] = false;
                mouseButtonsMaintained[i] = true;
                ignoreMouseMaintained[i] = false;
            }
        }
        else if (mouseButtonClick[i] || mouseButtonsMaintained[i])//Release the button
        {
            //std::cout << "Released button number : "<< i <<'\n';
            mouseButtonClick[i] = false;
            mouseButtonsMaintained[i] = false;
            ignoreMouseClick[i] = false;
            ignoreMouseMaintained[i] = false;
        }
    }
}

void System::writeDebugInfo(sf::RenderWindow& window, int FPS) const
{
    if (displayDebug)
    {
        sf::Text debug_text;
        debug_text.setCharacterSize(16);
        debug_text.setColor(sf::Color(255,0,0));
        debug_text.setFont(debug_font);
        debug_text.setPosition(0,0);

        std::stringstream myStringStream;

        myStringStream << "Welcome to the debug menu (press F12 to leave ;) )\n";

            myStringStream << "Position of upper left corner : (" << xUp << ';' << yUp << ")\n";
            myStringStream << "Current FPS count : " << FPS << '\n';
            myStringStream << "Current chunk : (" <<
            xChunk << ';' <<
            yChunk << ")\n";
            myStringStream << "Number of visible chunks : " << visibleChunks.size() << '\n';
            myStringStream << "Number of hidden chunks : " << hiddenChunks.size() << '\n';
            myStringStream << "Number of entities in current chunk : " << (visibleChunks[visibleChunkExist(xChunk, yChunk)].getNumberOfEntities()) << '\n';

            myStringStream << "Position of mouse in chunk : (" << std::floor((sf::Mouse::getPosition(window).x + getXUp()) / TILE_SIZE) - CHUNK_SIZE * std::floor((sf::Mouse::getPosition(window).x + getXUp()) / (TILE_SIZE*CHUNK_SIZE))
                                                               << ';'
                                                               << std::floor((sf::Mouse::getPosition(window).y + getYUp()) / TILE_SIZE) - CHUNK_SIZE * std::floor((sf::Mouse::getPosition(window).y + getYUp()) / (TILE_SIZE*CHUNK_SIZE))<< ")\n";


        myStringStream << "Press 'F' to " << (displayChunksBoundaries ? "hide" : "see") << " boundaries between chunks\n";
        myStringStream << "That's all I'm allowed to say...\n";

        debug_text.setString(myStringStream.str());
        window.draw(debug_text);
    }
}

double System::getXUp() const
{
    return xUp;
}

double System::getYUp() const
{
    return yUp;
}

bool System::isDebugDisplayed() const
{
    return displayDebug;
}

bool System::checkMouseButtonClicked(sf::Mouse::Button button) const
{
    if (ignoreMouseClick[button])
        return false;
    else
        return mouseButtonClick[button];
}

bool System::checkMouseButtonMaintained(sf::Mouse::Button button) const
{
    if (ignoreMouseMaintained[button])
        return false;
    else
        return mouseButtonsMaintained[button];
}

void System::nullifyClick(sf::Mouse::Button button)
{
    ignoreMouseClick[button] = true;
}

void System::nullifyClickMaintained(sf::Mouse::Button button)
{
    ignoreMouseMaintained[button] = true;
}

void System::updateKeyboardButton()
{
    for (int i(0); i<sf::Keyboard::KeyCount ; i++)
    {
        if (sf::Keyboard::isKeyPressed(i))//If the button is pressed...
        {
            if (keyboardButtonPushed[i] == false && keyboardButtonsMaintained[i] == false)//We first for once set it on clicked...
            {
                //std::cout << "Pushed key number : "<< i <<'\n';
                keyboardButtonPushed[i] = true;
            }
            else if (keyboardButtonPushed[i] == true && keyboardButtonsMaintained[i] == false)//Then block it on maintained
            {
                keyboardButtonPushed[i] = false;
                keyboardButtonsMaintained[i] = true;
            }
        }
        else if (keyboardButtonPushed[i] || keyboardButtonsMaintained[i])//Release the button
        {
            //std::cout << "Released key number : "<< i <<'\n';
            keyboardButtonPushed[i] = false;
            keyboardButtonsMaintained[i] = false;
        }
    }

//    if (ImGui::IsMouseHoveringAnyWindow() && checkMouseButtonClicked(sf::Mouse::Left))
//           nullifyClick(sf::Mouse::Left);
}

void System::editMenu()
{
    /*ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextTreeNodeOpen(false);

    ImGui::Begin("Editor");

    ImGui::Text("Menus to show :");

    ImGui::Checkbox("Paths", &pathChecked);

    if (pathChecked)
    {
        ImGui::Begin("Paths");

        if (addingNewNodes)
        {
            std::stringstream displaySettings;//Mainly to help the player in correctly placing his new node

            displaySettings << "Left click to add\nthe node at coordinates ";
            displaySettings << '(' << static_cast<int>(sf::Mouse::getPosition(windowMain).x + xUp) << ';' << static_cast<int>(sf::Mouse::getPosition(windowMain).y + yUp) <<')';
            displaySettings << "\nRight click to cancel.";

            ImGui::Text(displaySettings.str().c_str());

            sf::CircleShape tempNode;
            tempNode.setRadius(5);
            tempNode.setPosition(sf::Mouse::getPosition(windowMain).x - tempNode.getRadius(), sf::Mouse::getPosition(windowMain).y - tempNode.getRadius());
            windowMain.draw(tempNode);

            if (checkMouseButtonClicked(sf::Mouse::Right))
                addingNewNodes = false;
        }
        else
        {
            if (ImGui::Button("Add a node"))
                addingNewNodes = true;
        }

        ImGui::End();
    }

    ImGui::End();*/
}

void System::console()
{
    if (displayConsole)
    {

    }
}

void System::manageWindowEvent(sf::Event event, sf::RenderWindow& window, int FPS)
{
    double speed((double)SCROLL_SPEED / FPS);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        xUp -= speed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        xUp += speed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        yUp -= speed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        yUp += speed;

    if (checkKeyboardButtonPushed(sf::Keyboard::F))
        displayChunksBoundaries = !displayChunksBoundaries;

    if (checkKeyboardButtonPushed(sf::Keyboard::F12))
        displayDebug = !displayDebug;

    if (checkKeyboardButtonPushed(sf::Keyboard::T))
        displayConsole = !displayConsole;
}

bool System::checkKeyboardButtonMaintained(sf::Keyboard::Key key) const
{
    return keyboardButtonsMaintained[key];
}

bool System::checkKeyboardButtonPushed(sf::Keyboard::Key key) const
{
    return keyboardButtonPushed[key];
}

void System::nullifyPush(sf::Keyboard::Key key)
{
    keyboardButtonPushed[key] = false;
    if (sf::Keyboard::isKeyPressed(key))
        keyboardButtonsMaintained[key] = true;
    else
        keyboardButtonsMaintained[key] = false;
}

System::System() : windowMain(sf::VideoMode(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT), "Game", sf::Style::Resize),
                    primaryContainer()
{
    for (int i(0) ; i < sf::Mouse::ButtonCount ; i++)
    {
        ignoreMouseClick[i] = false;
        ignoreMouseMaintained[i] = false;
    }

    debug_font.loadFromFile("arial.ttf");//Thanks to the author (REMIND : credit him)

    displayConsole = false;
    displayDebug = false;//Pretty self-explanatory : if true, debug infos are displayed
    displayChunksBoundaries = true;

    currentType = MENU;
    consoleFontSize = 40;

    currentHumanID = 0;

    clockFPS.restart();
    numberOfFrames = 0;
    FPS = wishedFPS;

    xUp = -100;
    yUp = -100;

    pathChecked = false;

    addingNewNodes = false;

    xChunk = std::floor(xUp / (CHUNK_SIZE * TILE_SIZE));
    yChunk = std::floor(yUp / (CHUNK_SIZE * TILE_SIZE));

    updateChunksList(windowMain);

    primaryContainer.setTitle("Main system primary container");

    for (int i(0) ; i<sf::Mouse::ButtonCount ; i++)//Initialize states of the mouse buttons
    {
        mouseButtonsMaintained[i] = false;
        mouseButtonClick[i] = false;
    }

    for (int i(0) ; i<sf::Keyboard::KeyCount ; i++)//And of the keyboard buttons
    {
        keyboardButtonPushed[i] = false;
        keyboardButtonsMaintained[i] = false;
    }
}

template<typename T>
Widget* System::addWidget(T toAdd, std::string name)
{
    return primaryContainer.addWidget(toAdd, name);
}

void System::removeWidget(std::string name)
{
    primaryContainer.removeWidget(name);
}

Widget* System::giveWidgetPointer(std::string widgetName) const
{
    return primaryContainer.giveWidgetPointer(widgetName);
}

void System::updateWidgets()
{
    updateWidgetDirection(primaryContainer, windowMain);
}

void System::renderWidgets()
{
    renderWidgetDirection(primaryContainer, windowMain);
}

System::~System(){};

