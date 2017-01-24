#ifndef INTERFACE_HPP_INCLUDED
#define INTERFACE_HPP_INCLUDED

#define ID_TYPE unsigned int
#define BOUNDARIES_THICKNESS 5//In pixels

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <SFML/Graphics.hpp>

/**
Hello and welcome to this tutorial on how to use the basics of this GUI system

The system works on a simple inheritance system from a class called Widget.
Widget is very bare-boned and not meant to be used directly.
However what you can do is use the current inherited classes coded by myself,
or you can code yours.
Coding your own class is easy, but follows some strict rules that cannot be avoided.
The main concern is that you inherited class (let's call it yourAwesomeWidget) looks like
this :

class yourAwesomeWidget : public Widget{
public:
    yourAwesomeWidget();//An empty constructor
    yourAwesomeWidget(whatever youWant);//Any other constructor that your heart desires
    ~yourAwesomeWidget();//A destructor


    virtual void update(sf::RenderWindow& window, System* systemContainer);
    virtual void draw(sf::RenderWindow& window) const;

private:
    //What you want
};

Regarding updating :
    The library required you to call a static function called Widget::

*/

//We will set a basic GUI system relying on SFML, based on flexibility (read : mod-ability), and easiness to use
//All the system will be based on a big inheritance system from this class (widget) to create the whole system

//Not to be used directly, only a basis to build stuff on
class Widget{//Ok
public:

    bool isHovered(sf::RenderWindow& window, sf::FloatRect* widgetShape) const;//
    bool isClicked() const;//
    ID_TYPE getID() const;//

    Widget();
    Widget(int widgetX, int widgetY, bool setPosToAbsolute = false);
    virtual ~Widget();

    void setPosAbsolute(bool isPosAbsolute);
    void setDragable(bool isDragable);
    void setResizable(bool isResizable);
    void setID(ID_TYPE newID);

    void updateClick(sf::RenderWindow& window, sf::FloatRect* widgetShape);

    static ID_TYPE giveTextureID(std::string texturePath);//Simply gives the pointer if there's one, otherwise loads then give. One file path = one texture, two same textures with different path will occupy two spaces
    static ID_TYPE giveFontID(std::string fontPath);//Same
    static void removeTexture(ID_TYPE textureID);
    static void removeFont(ID_TYPE fontID);
    static sf::Texture* giveTexturePointerOfID(ID_TYPE textureID);
    static sf::Font* giveFontPointerOfID(ID_TYPE fontID);

    static bool Init(sf::Window const& mainWindow);
    static void UpdateEvents(sf::Window const& mainWindow);
    static bool isKeyPressed(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck);
    static bool isKeyPushed(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck);
    static bool isKeyMaintained(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck);
    static bool isButtonPressed(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck);
    static bool isButtonClicked(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck);
    static bool isButtonMaintained(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck);
    static void ignoreButtonClick(sf::Window const& mainWindow, sf::Mouse::Button toIgnore);
    static void ignoreButtonMaintained(sf::Window const& mainWindow, sf::Mouse::Button toIgnore);

    static bool isWidgetContainer(Widget* toCheck);

    virtual void update(sf::RenderWindow& window);
    virtual void draw(sf::RenderWindow& window) const;
protected:

    void setSize(int newWidth, int newHeight);

protected://All will be inherited
    bool posAbsolute;//If true, the position of the widget is absolute and can be anything within the universe of the game. Otherwise, it is screen-bounded
    bool dragable;//Defines whether you can change the widget's position by clicking on it or no
    bool resizable;//Defines if you can resize the widget by dragging its boundaries
    bool clicked;//True only on the event of a click, then it goes to click maintained
    bool clickMaintained;
    bool toClose;
    ID_TYPE ID;

private://Those two are essentially corresponding tables who will store all the necessary fonts/textures for the GUI
    static ID_TYPE currentFontID;
    static std::map<std::string, ID_TYPE> stringToIDTableFont;//Every single file is given an ID
    static std::map<ID_TYPE, std::unique_ptr<sf::Font>> widgetFonts;//The name of the texture file + a pointer
    static std::queue<ID_TYPE> vacantFontID;

    static ID_TYPE currentTextureID;
    static std::map<std::string, ID_TYPE> stringToIDTableTexture;
    static std::map<ID_TYPE, std::unique_ptr<sf::Texture>> widgetTextures;//Same
    static std::queue<ID_TYPE> vacantTextureID;

    //All the stuff for the own GUI event management
    static bool mouseButtonsMaintained[sf::Mouse::ButtonCount];
    static bool ignoreMouseMaintained[sf::Mouse::ButtonCount];
    static bool mouseButtonClick[sf::Mouse::ButtonCount];
    static bool ignoreMouseClick[sf::Mouse::ButtonCount];

    static bool keyboardButtonsMaintained[sf::Keyboard::KeyCount];
    static bool keyboardButtonPushed[sf::Keyboard::KeyCount];
};

//Basically just a graphical widget, that will be used by other ones to display their content
//It can be anything, a colored rectangle, a 2d texture, text etc.
class Dummy : public Widget{//Ok
public:
    /**
    Creates a Dummy Widget from an image file.
    This constructor also adds the loaded texture to
    the widgetTextures static container, in Widget

    @param Essentially the file's path, joined with
    the desired size of the dummy

    */
    Dummy(std::string imagePath, sf::Vector2f size, sf::Vector2f pos = sf::Vector2f(0,0));//Load a texture from a file

    Dummy(sf::Color const& fillColor, sf::Vector2f size, sf::Vector2f pos = sf::Vector2f(0,0));//Create a colored rectangle. Probably very slow
    ~Dummy() {};

    void setPos(sf::Vector2f& newPos);
    void resizeDummy(sf::Vector2f& newSize);

    sf::FloatRect getLocalBounds() const;
    sf::FloatRect getGlobalBounds() const;

    virtual void update(sf::RenderWindow& window);
    virtual void draw(sf::RenderWindow& window) const;
private:
    double posX, posY;
    double sizeX, sizeY;
    ID_TYPE textureID;
    sf::Sprite sprite_;
};

//A button is a (colored) rectangle with text centered in it. It reacts when clicked on
class Button : public Widget{//Ok
public:
    Button();
    Button(sf::Vector2f pos, sf::Color buttonColoration, std::string buttonContent, short fontSize = 0, sf::Vector2f textPosPercent = sf::Vector2f(0,0), short distTextBoundaries = 0);
    ~Button();

    void setString(std::string stringToPut);
    void setColor(sf::Color newColor);

    sf::Color getColor() const;
    std::string getButtonString() const;

    bool isPressed() const {return (clicked || clickMaintained);};

    virtual void update(sf::RenderWindow& window);
    virtual void draw(sf::RenderWindow& window) const;
private:
    sf::Vector2f textPos;//(0.5;0.5) would indicate middle, (1;0) top left etc.
    short distFromText;//Distance (in pixels) from the text
    sf::Text buttonString;
    std::shared_ptr<sf::Font> buttonFont;
    sf::RectangleShape buttonShape;//To be replaced by a sf::Sprite, or sf::Texture, or sf::Whatever
};

//Checkbox is a box that is whether true, or false. A click changes this state
class Checkbox : public Widget{//Ok
public:
    Checkbox();
    Checkbox(bool init, sf::Vector2f widSize, sf::Vector2f pos);
    ~Checkbox();

    bool getValue() const;

    virtual void update(sf::RenderWindow& window);
    virtual void draw(sf::RenderWindow& window) const;
private:
    bool value;
    Dummy checkBoxUnchecked;
    Dummy checkBoxChecked;
};

//Two rectangles, one smaller sliding in a bigger one
//It can also have two buttons on the extremities, with two arrows in it
class Slider : public Widget{
public:
    Slider();
    ~Slider();

    virtual void update(sf::RenderWindow& window);
    virtual void draw(sf::RenderWindow& window) const;
private:
    float vMin, vMax;//Boundaries
    float vCurrent;//Current value
    int floaterLength;

    Button floater;//The floating rectangle within the slider
    Button arrowOne;
    Button arrowTwo;

    sf::RectangleShape background;
    sf::Color backgroundColor;

    sf::RectangleShape sliderRect;
    sf::Color sliderRectColor;
};

//A big rectangle containing other widgets (can be recursive, e.g a container containing a container etc.)
//If needed, it will contain one or two sliders, depending on whether the container is too small or not
class Container : public Widget{//Ok
public:
    Container();
    ~Container();

    void setTitle(std::string newTitle);

    //Those functions are exactly the same as in system, because it is itself considered to be a container ;)
    template<class T>
T* Container::addWidget(T toAdd, std::string name)
{
    auto findWidget = stringToIDTable.find(name);

    if (findWidget == stringToIDTable.end())
    {
        if (vacantWidgetID.empty())
        {
            toAdd.setID(currentWidgetID);
            currentWidgetID++;
        }
        else
        {
            toAdd.setID(vacantWidgetID.front());
            vacantWidgetID.pop();
        }

        stringToIDTable.insert(std::make_pair(name, toAdd.getID()));//We make the ID and the name correspond

        allWidgets.insert( std::make_pair(toAdd.getID(), std::unique_ptr<Widget>(new T(toAdd)) ));

    }

        return giveWidgetPointer(name);

};
    void removeWidget(std::string name);
    Widget* giveWidgetPointer(std::string widgetName) const;//Matches the name and the ID, to then get pointer from ID
    Widget* returnWidgetPointerOfID(ID_TYPE ID) const;//Not meant to be used directly. Matches ID and Widget*

    virtual void update(sf::RenderWindow& window);
    virtual void draw(sf::RenderWindow& window) const;
private:
    static bool memberInteracted;

    ID_TYPE currentWidgetID;
    std::map<std::string, ID_TYPE> stringToIDTable;
    std::map<ID_TYPE, std::unique_ptr<Widget>> allWidgets;
    std::queue<ID_TYPE> vacantWidgetID;

    std::string title;
};

/**
A class in which you can input any value you want : int, double, float, std::string, any object.
The required operators, however, are the >, <, =, <=, >= and == operators, for comparison matter.
The << operator is also needed, otherwise the text will not be able to be displayed
*/
template<class T>
class InputValue : public Widget{
public:

    InputValue();//The default constructor of T will be used, no parameter sent
    InputValue(T initValue);
    InputValue(T initValue, T minValue, T maxValue);
    ~InputValue() {};

    T returnValue() const;
    void changeValue(T toPut);

    virtual void update(sf::RenderWindow& window);
    virtual void draw(sf::RenderWindow& window) const;
private:
    bool limits;
    T minValue, maxValue;
    T currentValue;
    ID_TYPE fontID;
    sf::Text contentText;
};

void updateWidgetDirection(Widget& toUpdate, sf::RenderWindow& window);
void renderWidgetDirection(Widget& toRender, sf::RenderWindow& window);

#include "Classes.h"

#endif // INTERFACE_HPP_INCLUDED
