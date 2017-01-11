#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <queue>
#include <memory>
#include "GUI/Interface.hpp"
#include "Nodes.hpp"

#define ID_TYPE unsigned int

class Entity;
class Human;
class Chunk;

enum chunkState{
    VISIBLE,
    HIDDEN,
    NONEXISTING
};

enum edit_type{
    MENU,
    PATHS
};

class System{
public:
    System();
    ~System();

    //sf::RenderWindow getRenderWindow() const;

    void updateMouseButton();
    void updateKeyboardButton();

    void manageWindowEvent(sf::Event event, sf::RenderWindow& window, int FPS);//Processes event in the window (zooms, moving...)
    void writeDebugInfo(sf::RenderWindow& window, int FPS) const;//Used for debug. Write info on the screen about the current system, window...

    bool checkKeyboardButtonMaintained(sf::Keyboard::Key key) const;
    bool checkKeyboardButtonPushed(sf::Keyboard::Key key) const;
    void nullifyPush(sf::Keyboard::Key key);//For example, when the key's push has already been used (less useful than click)

    bool checkMouseButtonMaintained(sf::Mouse::Button button) const;
    bool checkMouseButtonClicked(sf::Mouse::Button button) const;
    void nullifyClick(sf::Mouse::Button button);//For example, when the click has already been registered and used, or used on something forefront (interfaces...)
    void nullifyClickMaintained(sf::Mouse::Button button);

    double getXUp() const;
    double getYUp() const;

    bool isDebugDisplayed() const;

    void updateChunksList(sf::RenderWindow& window);
    void displayChunks(sf::RenderWindow& window) const;

    int visibleChunkExist(int xChunkToTest, int yChunkToTest) const;//Returns the position of the chunk is found, -1 if not
    int hiddenChunkExist(int xChunkToTest, int yChunkToTest) const;//Same as written above
    chunkState giveChunkState(int xChunkToCheck, int yChunkToCheck) const;//Says whether the chunk is hidden, visible or non-existing

    void removeEntityIDFromChunk(int xChunkToTarget, int yChunkToTarget, ID_TYPE ID);
    void addEntityIDToChunk(int xChunkToTarget, int yChunkToTarget, ID_TYPE ID);

    void addHumanToSystem(Human toAdd);//Adds an human to the system (bounds to chunks & adds it in vector)
    void removeHumanFromSystem(ID_TYPE ID);
    Human returnHumanOfID(ID_TYPE ID) const;

    sf::RenderWindow* returnRenderWindowPointer() const;

    void blockFPS(sf::Clock &fpsClock, int &currentFrameNumber, const int wishedFPS) const;
    void editMenu();

    std::map<ID_TYPE, Human>* returnHumanVectorPointer() const;
    std::map<ID_TYPE, Human> returnHumanVector() const;

    void console();

    template<class T> Widget* addWidget(T toAdd, std::string name);
    void removeWidget(std::string name);
    Widget* giveWidgetPointer(std::string widgetName) const;//Matches the name and the ID, to then get pointer from ID

    void updateWidgets();
    void renderWidgets();

    void Update();
    void Render();
private://Functions private

    void unloadHiddenChunks(int fitOnScreenWidth, int fitOnScreenHeight);//Unload the chunks still marked as 'visible', even though they're really hidden
    void addNewChunks(int fitOnScreenWidth, int fitOnScreenHeight);//Add the new chunks necessary to the screen being filled
    void transferChunkVisibleToHidden(int positionInVisible);
    void transferChunkHiddenToVisible(int positionInHidden);

private:
    std::vector<Chunk> visibleChunks;//Displayed to the player
    std::vector<Chunk> hiddenChunks;//Used by the AI, if needed

    std::map<ID_TYPE, Human> allHumans;
    ID_TYPE currentHumanID;
    std::queue<ID_TYPE> vacantHumanID;

    std::map<sf::Vector2i, Node> allNodes;

    Container primaryContainer;

    double xUp, yUp;
    int xChunk, yChunk;

    sf::Font debug_font;
    edit_type currentType;

    sf::RenderWindow windowMain;

    short consoleFontSize;

    bool mouseButtonsMaintained[sf::Mouse::ButtonCount];
    bool ignoreMouseMaintained[sf::Mouse::ButtonCount];
    bool mouseButtonClick[sf::Mouse::ButtonCount];
    bool ignoreMouseClick[sf::Mouse::ButtonCount];

    bool keyboardButtonsMaintained[sf::Keyboard::KeyCount];
    bool keyboardButtonPushed[sf::Keyboard::KeyCount];

    bool displayConsole;
    std::string consoleString;

    bool displayDebug;
    bool displayChunksBoundaries;

    bool pathChecked;

    bool addingNewNodes;

    sf::Event event;

    int FPS;
    int numberOfFrames;
    sf::Clock clockFPS;
    sf::Clock imGuiClock;
};

//A simple class to symbolize and perform operation on vectors
class Vector{
public:
    Vector() {x=0; y=0;};
    Vector(float xToGive, float yToGive) {x=xToGive; y=yToGive;};
    ~Vector() {};

    Vector normalize() const {double length(sqrt((x*x) + (y*y))); return Vector(x / length, y / length);};
    static Vector sumVector(Vector *data, int dataSize)
    {
        Vector dummy(0,0);

        for (int i(0) ; i < dataSize ; i++)
            dummy += *(data + i);

        return dummy;
    };

    friend Vector operator+(const Vector& A, const Vector& B) {return Vector(A.x + B.x, A.y + B.y);};
    friend void operator+=(Vector& A, const Vector& B) {A.x += B.x; A.y += B.y;};
    friend std::ostream& operator<<(std::ostream& out, const Vector A)
    {
        out << '(' << A.x << ';' << A.y << ')';
        return out;
    };

private:
    float x, y;
};

class Entity{
public:

    Entity();//PREFERABLY NOT USED AT ALL (ALL BELOW WITHOUT SYSTEM)
    Entity(int xInit, int yInit);

    Entity(System& mainSystem);
    Entity(int xInit, int yInit, int widthInit, int heightInit, System& mainSystem);
    Entity(Entity& toCopy, System& mainSystem);
    ~Entity();

    double getPosX() const;
    double getPosY() const;
    unsigned int getID() const;
    int getHeight() const;
    int getWidth() const;

    void setX(double xNew);
    void setY(double yNew);
    void setID(unsigned int newID);

protected:

    //Not much too, a Entity is a very general thing
    int width, height;
    double x, y;
    ID_TYPE ID;
    static unsigned int currentID;

private:

    //Nothing yet

};

class Human : public Entity {
    public:
        Human(System &mainSystem);
        Human(sf::Color rectColor, int widthInit, int heightInit, System &mainSystem);
        Human(sf::Color rectColor,  int widthInit, int heightInit, Entity& initEntity, System &mainSystem);
        Human();
        ~Human();

        static void Update(std::map<unsigned int, Human>& allHumans, sf::RenderWindow &window, System &mainSystem);
        static void Draw(std::map<unsigned int, Human>& allHumans,sf::RenderTarget& window, int xWindow, int yWindow, int windowWidth, int windowHeight);

        void drawHuman(sf::RenderTarget& window, int xWindow, int yWindow);//Just draw the rectangle for now
        bool updateClick(sf::Vector2i mousePos, int xUp, int yUp);

        void manageHumanPositionInChunks(sf::Vector2f previousPos, sf::Vector2f actualPos, System& mainSystem);
        bool isLocked() const;
    private:
        /**
        Loads of physical caracteristics :
            Face
            Height
            Strength
            etc.
            For now, human are only shown as rectangles
        */

        int age;//Age in minutes
        sf::Color color;//For early testing
        sf::RectangleShape rectangleRaw;

        bool clicked;
        bool locked;

    protected:
        //Nothing should fall here, Human not being heritable (?)
};

#define CHUNK_SIZE 64
#define TILE_SIZE 64

//A chunk a data array containing information about the tiles of a certain area, and also containing the entities of this area
class Chunk{
public:
    Chunk();
    Chunk(int xChunk, int yChunk);
    Chunk(int toFill, int xChunk, int yChunk);
    Chunk(std::string fileLoad, int xChunk, int yChunk);
    ~Chunk();

    int getX() const;
    int getY() const;

    Chunk loadChunk(std::string fileLoad) const;

    void displayChunk(sf::RenderWindow& window) const;
    void fusionEntityList(Chunk& sender, Chunk& receiver);

    int getID(int x, int y) const;
    void setID(int x, int y, ID_TYPE ID);

    bool isEmptyOfEnties() const;//We check if the chunk has any entity inside it
    int getNumberOfEntities() const;

    void addEntity(ID_TYPE entityToAddID);

    bool isEntityInChunk(ID_TYPE toCheckID) const;//Checks whether an entity has its ID in the chunk
    int getID(int position) const;//Returns the ID at a given position in the std::vector
    void removeEntityFromChunk(ID_TYPE toRemoveID);//Kinda straightforward : removes an entity from a chunk
    void saveChunk() const;
private:
    int x, y;//Coordinates of the chunk
    int arrayTiles[CHUNK_SIZE][CHUNK_SIZE];
    bool updated;//If a chunk has been changed during its lifetime, then we will need to update the filesave once it is shut down

    //Contains pointers to every single entity contained in the chunk
    std::vector<ID_TYPE> entityInChunkID;
};

#endif // CLASSES_H_INCLUDED
