#ifndef INTERFACE_HPP_INCLUDED
#define INTERFACE_HPP_INCLUDED

#define ID_TYPE unsigned int
#define BOUNDARIES_THICKNESS 5//In pixels

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <typeinfo>
#include <windows.h>
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

    bool isHovered(sf::RenderWindow& window, sf::FloatRect* widgetShape);//
    bool isClicked() const;//
    ID_TYPE getID() const;//

    Widget();
    Widget(int widgetX, int widgetY, bool setPosToAbsolute = false);
    virtual ~Widget();

    void setID(ID_TYPE newID);

    void updateMouseInteraction(sf::RenderWindow& window, sf::FloatRect* widgetShape);//Updates the hovered, clicked, clickMaintained flags

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

    virtual void update(sf::RenderWindow& window) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;


protected://All will be inherited
    //bool posAbsolute;//If true, the position of the widget is absolute and can be anything within the universe of the game. Otherwise, it is screen-bounded
    //bool dragable;//Defines whether you can change the widget's position by clicking on it or no
    //bool resizable;//Defines if you can resize the widget by dragging its boundaries
    bool clicked;//True only on the event of a click, then it goes to click maintained
    bool clickMaintained;
    bool hovered;
    ID_TYPE ID;

    static std::vector<sf::Uint32> unicodeBuffer;
    static std::vector<sf::Keyboard::Key> keyBuffer;

private://Those two are essentially corresponding tables who will store all the necessary fonts/textures for the GUI
    static sf::Event events;


    static ID_TYPE currentFontID;
    static std::map<std::string, ID_TYPE> stringToIDTableFont;//Every single file is given an ID
    static std::map<ID_TYPE, std::unique_ptr<sf::Font>> widgetFonts;//The name of the texture file + a pointer
    static std::queue<ID_TYPE> vacantFontID;

    static ID_TYPE currentTextureID;
    static std::map<std::string, ID_TYPE> stringToIDTableTexture;
    static std::map<ID_TYPE, std::unique_ptr<sf::Texture>> widgetTextures;//Same
    static std::queue<ID_TYPE> vacantTextureID;

    //All the stuff for the own GUI event management
    static bool widgetAlreadyHovered;

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
    Dummy() {};
    Dummy(std::string imagePath, sf::Vector2f size, sf::Vector2f pos = sf::Vector2f(0,0));//Load a texture from a file
    Dummy(sf::Color const& fillColor, sf::Vector2f size, sf::Vector2f pos = sf::Vector2f(0,0));//Create a colored rectangle. Probably very slow
    ~Dummy() {};

    void setTexture(std::string imagePath);
    void setPosition(sf::Vector2f newPos);
    void setFillColor(sf::Color newColor);
    void setSize(sf::Vector2f newSize);

    sf::Vector2f getSize() const;
    sf::Vector2f getPosition() const;
    sf::Color getFillColor() const;

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
    Button(sf::Vector2f pos, sf::Vector2i boundariesSize, sf::Color buttonColoration, std::string buttonContent = "", short fontSize = 0, sf::Vector2f textPosPercent = sf::Vector2f(0.5, 0.5));//Create a button with a colored rectangle as background
    Button(sf::Vector2f pos, sf::Vector2i boundariesSize, sf::Color buttonColoration, std::string texturePath, std::string buttonContent = "", short fontSize = 0, sf::Vector2f textPosPercent = sf::Vector2f(0.5, 0.5));
    ~Button();

    void setString(std::string stringToPut);
    void setColor(sf::Color newColor);
    void setHoveredColor(sf::Color newColor);
    void setClickedColor(sf::Color newColor);

    sf::Color getColor() const;
    sf::Color getHoveredColor() const;
    sf::Color getClickedColor() const;
    std::string getButtonString() const;

    bool isPressed() const {return (clicked || clickMaintained);};

    virtual void update(sf::RenderWindow& window);
    virtual void draw(sf::RenderWindow& window) const;
private:
    sf::Vector2f textPos;//(0.5;0.5) would indicate middle, (1;0) top left etc.
    sf::Text buttonString;
    sf::Color buttonColor;
    sf::Color hoveredColor;
    sf::Color clickedColor;
    //sf::RectangleShape buttonShape;//To be replaced by a sf::Sprite, or sf::Texture, or sf::Whatever
    ID_TYPE fontID;
    Dummy buttonShape;
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
    T* addWidget(T toAdd, std::string name)
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
    //The default constructor of T will be used, no parameter sent
    InputValue() : background()
    {
        limits = false;
        aNumber = isNumber();
        selected = false;

        changeValue(T());

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("Cousine-Regular.TTF");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(T initValue) : background()
    {
        limits = false;
        aNumber = isNumber();
        selected = false;

        changeValue(initValue);

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("Cousine-Regular.TTF");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(T initValue, T minValue_, T maxValue_) : background()
    {
        limits = true;
        aNumber = isNumber();
        selected = false;

        minValue = minValue_;
        maxValue = maxValue_;

        changeValue(initValue);

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("Cousine-Regular.TTF");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize)//The default constructor of T will be used, no parameter sent
        : background(color, size, pos)
    {
        limits = false;
        aNumber = isNumber();
        selected = false;

        changeValue(T());

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("Cousine-Regular.TTF");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, T initValue)
            : background(color, size, pos)
    {
        limits = false;
        aNumber = isNumber();
        selected = false;

        changeValue(initValue);

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("Cousine-Regular.TTF");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, T initValue, T minValue_, T maxValue_)
            : background(color, size, pos)
    {
        limits = true;
        aNumber = isNumber();
        selected = false;

        minValue = minValue_;
        maxValue = maxValue_;

        changeValue(initValue);

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("Cousine-Regular.TTF");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize)//The default constructor of T will be used, no parameter sent
            : background(texturePath, size, pos)
    {
        limits = false;
        aNumber = isNumber();
        selected = false;

        changeValue(T());

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("Cousine-Regular.TTF");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, T initValue)
            : background(texturePath, size, pos)
    {
        limits = false;
        aNumber = isNumber();
        selected = false;

        changeValue(initValue);

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("Cousine-Regular.TTF");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, T initValue, T minValue_, T maxValue_)
            : background(texturePath, size, pos)
    {
        limits = true;
        aNumber = isNumber();
        selected = false;

        minValue = minValue_;
        maxValue = maxValue_;

        changeValue(initValue);

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("Cousine-Regular.TTF");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        cursorPos = valueContained.size();

        updateSprite();
    };

    ~InputValue() {};


    template<typename U>
    U returnValue() const
    {
        U toReturn;
        std::stringstream valueStr;

        valueStr << valueContained;
        valueStr >> toReturn;

        return toReturn;
    };

    std::string returnValue() const
    {
        return valueContained;
    };

    bool loadFont(std::string fontToLoad)
    {
        fontID = Widget::giveFontID(fontToLoad);
        contentText.setFont(*Widget::giveFontPointerOfID(fontID));
    };

    void changeValue(T toPut)
    {
        valueContained.clear();

        std::stringstream newValue;
        newValue << toPut;
        valueContained = newValue.str();
    };

    bool isNumber() const
    {
        std::stringstream myStream;
        T testType = T();
        T before(testType);
        myStream << "a";
        myStream >> testType;
        return (testType == before);
    }

    void adaptTextToBox()
    {
        sf::Text setHeight;
        setHeight.setString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890");
        setHeight.setFont(*contentText.getFont());
        setHeight.setCharacterSize(0);

        for (int i(0) ; textBoxSize.y - setHeight.getLocalBounds().top >= setHeight.getLocalBounds().height ; i++)
        {
            setHeight.setCharacterSize(i);
            contentText.setCharacterSize(i - 1);
        }
    }

    void updateSprite()
    {
        sf::RenderTexture tempText;

        tempText.create(textBoxSize.x, textBoxSize.y);

        sf::Vector2f tempPos(contentText.getPosition());
        this->contentText.setString(valueContained);

        double x(0.0);
        double y(0.0);

        if (contentText.getLocalBounds().width + contentText.getLocalBounds().left > tempText.getSize().x)
            x = -(contentText.getLocalBounds().width + contentText.getLocalBounds().left) + tempText.getSize().x;
        else
            x = 0.0;

        if (contentText.getLocalBounds().height + contentText.getLocalBounds().top > tempText.getSize().y)
            y = -(contentText.getLocalBounds().height + contentText.getLocalBounds().top) + tempText.getSize().y;
        else
            y = 0.0;

        this->contentText.setPosition(x,y);

        tempText.draw(contentText);
        tempText.display();

        myTexture = std::make_shared<sf::Texture>(tempText.getTexture());

        textToDisplay.setTexture(*myTexture);
        textToDisplay.setPosition(background.getPosition().x + textRelPos.x, background.getPosition().y + textRelPos.y);

        this->contentText.setPosition(tempPos);
    }

    void putValueBetweenLimits()
    {
        T currentValue = returnValue<T>();

        if (!limits)
        {
            if (currentValue > std::numeric_limits<T>::max() - 1)
            {
                changeValue(std::numeric_limits<T>::max());
            }
            else if (currentValue < std::numeric_limits<T>::min() + 1)
            {
                changeValue(std::numeric_limits<T>::min());
            }
        }
        else
        {
            if (currentValue > maxValue  - 1)
            {
                changeValue(maxValue);
            }
            else if (currentValue < minValue + 1)
            {
                changeValue(minValue);
            }
        }
    }

    virtual void update(sf::RenderWindow& window)
    {
        updateMouseInteraction(window, &background.getGlobalBounds());

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            if (hovered)
                selected = true;
            else
                selected = false;
        }

        if (selected)
        {
            if (Widget::unicodeBuffer.size())
            {
                std::string beforeModifText(valueContained);

                for (int i(0) ; i < Widget::unicodeBuffer.size() ; i++)
                {
                    std::cout << Widget::unicodeBuffer[i] << '\n';
                    switch (Widget::unicodeBuffer[i])
                    {
                    case 8://Delete

                        if (valueContained.size() > 0)
                            valueContained.erase(valueContained.begin()+(valueContained.size() - 1));

                        break;

                    case 13://Delete

                        if (!aNumber)
                            valueContained += '\n';

                        break;

                    case 45://Minus

                        if (aNumber)
                        {
                            if (valueContained.find((char)45) == std::string::npos)
                                valueContained += (char)Widget::unicodeBuffer[i];
                        }
                        else
                            valueContained += (char)Widget::unicodeBuffer[i];

                        break;

                    case 46://Comma

                        if (aNumber)
                        {
                            if (valueContained.find((char)46) == std::string::npos)
                                valueContained += (char)Widget::unicodeBuffer[i];
                        }
                        else
                            valueContained += (char)Widget::unicodeBuffer[i];

                        break;

                    case 48:
                    case 49:
                    case 50:
                    case 51:
                    case 52:
                    case 53:
                    case 54:
                    case 55:
                    case 56:
                    case 57://Numbers
                        valueContained += (char)Widget::unicodeBuffer[i];
                        break;

                    default:

                        if (!aNumber)
                            valueContained += (char)Widget::unicodeBuffer[i];

                        break;
                    }
                }

                if (valueContained != beforeModifText)//Update the sprite
                {
                    if (aNumber)
                        putValueBetweenLimits();

                    cursorPos += valueContained.size() - beforeModifText.size();

                    std::cout << "Cursos pos is now : " << cursorPos << '\n';

                    updateSprite();
                }
            }
        }
    };

    virtual void draw(sf::RenderWindow& window) const
    {
        background.draw(window);
        window.draw(textToDisplay);
    };

private:
    int cursorPos;

    bool limits;
    bool aNumber;
    bool selected;

    T minValue, maxValue;
    T currentValue;

    std::string valueContained;

    ID_TYPE fontID;
    sf::Text contentText;
    sf::Vector2f textRelPos;
    sf::Vector2f textBoxSize;
    sf::Sprite textToDisplay;

    std::shared_ptr<sf::Texture> myTexture;

    Dummy background;
};

template<>
class InputValue<sf::String> : public Widget{
public:
    //The default constructor of T will be used, no parameter sent
    InputValue() : background()
    {
        selected = false;
        renderFromLowerCursorBound = true;
        renderFromLowerLineBound = true;

        changeValue(std::string());

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        updateSprite(false);
    };

    InputValue(sf::String initValue) : background()
    {
        selected = false;
        renderFromLowerCursorBound = true;
        renderFromLowerLineBound = true;

        changeValue(initValue);

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        updateSprite(false);
    };

    InputValue(sf::String initValue, sf::String minValue_, sf::String maxValue_) : background()
    {
        selected = false;
        renderFromLowerCursorBound = true;
        renderFromLowerLineBound = true;

        changeValue(initValue);

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        updateSprite(false);
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize)//The default constructor of T will be used, no parameter sent
        : background(color, size, pos)
    {
        selected = false;
        renderFromLowerCursorBound = true;
        renderFromLowerLineBound = true;

        changeValue(std::string());

        textRelPos = sf::Vector2f(pos.x + relativeTextPosPer_.x * size.x, pos.y + relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite(false);
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, sf::String initValue)
            : background(color, size, pos)
    {
        selected = false;
        renderFromLowerCursorBound = true;
        renderFromLowerLineBound = true;

        changeValue(initValue);

        textRelPos = sf::Vector2f(pos.x + relativeTextPosPer_.x * size.x, pos.y + relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite(false);
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, sf::String initValue, sf::String minValue_, sf::String maxValue_)
            : background(color, size, pos)
    {

        selected = false;
        renderFromLowerCursorBound = true;
        renderFromLowerLineBound = true;

        changeValue(initValue);

        textRelPos = sf::Vector2f(pos.x + relativeTextPosPer_.x * size.x, pos.y + relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite(false);
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize)//The default constructor of T will be used, no parameter sent
            : background(texturePath, size, pos)
    {
        selected = false;
        renderFromLowerCursorBound = true;
        renderFromLowerLineBound = true;

        changeValue(std::string());

        textRelPos = sf::Vector2f(pos.x + relativeTextPosPer_.x * size.x, pos.y + relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite(false);
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, sf::String initValue)
            : background(texturePath, size, pos)
    {
        selected = false;
        renderFromLowerCursorBound = true;
        renderFromLowerLineBound = true;

        changeValue(initValue);

        textRelPos = sf::Vector2f(pos.x + relativeTextPosPer_.x * size.x, pos.y + relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite(false);
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, sf::String initValue, sf::String minValue_, sf::String maxValue_)
            : background(texturePath, size, pos)
    {
        selected = false;
        renderFromLowerCursorBound = true;
        renderFromLowerLineBound = true;

        changeValue(initValue);

        textRelPos = sf::Vector2f(pos.x + relativeTextPosPer_.x * size.x, pos.y + relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite(false);
    };

    ~InputValue() {};

    sf::String returnValue() const
    {
        sf::String toReturn;

        for (int i(0) ; i < valueContained.size() ; i++)
        {
            toReturn += valueContained[i];
            toReturn += '\n';
        }

        return toReturn;
    };

    bool loadFont(std::string fontToLoad)
    {
        fontID = Widget::giveFontID(fontToLoad);
        contentText.setFont(*Widget::giveFontPointerOfID(fontID));
    };

    void changeValue(sf::String toPut)
    {
        valueContained.clear();

        /*std::stringstream newValue;
        newValue << toPut;

        for (int i(0) ; !newValue.eof() ; i++)
        {
            valueContained.push_back("");
            std::getline(newValue, valueContained[i]);
        }*/

        std::size_t pos(0);

        do
        {
            valueContained.push_back(toPut.substring(pos, toPut.find('\n') - pos + 1));
        } while (toPut.find('\n') != sf::String::InvalidPos) ;

        currentLine = valueContained.size() - 1;
        cursorPos = 0;
    };

    void setTextRelPos()
    {

    }

    void adaptTextToBox()
    {
        sf::Text setHeight;
        setHeight.setString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890");
        setHeight.setFont(*contentText.getFont());
        setHeight.setCharacterSize(0);

        for (int i(0) ; textBoxSize.y - setHeight.getLocalBounds().top >= setHeight.getLocalBounds().height ; i++)
        {
            setHeight.setCharacterSize(i);
            contentText.setCharacterSize(i - 1);
        }
    }

    void updateSprite(bool lineChanged)
    {
        sf::RenderTexture tempText;

        tempText.create(textBoxSize.x, textBoxSize.y);

        sf::Vector2f tempPos(contentText.getPosition());

        double x(0.0);
        double y(0.0);

        contentText.setString(returnValue());

        std::size_t cursorAbsolutePos(0);

        for (int i(0) ; i < currentLine ; i++)
            cursorAbsolutePos += valueContained[i].getSize() + 1;

        cursorAbsolutePos += cursorPos;

        if (cursorAbsolutePos < currentCursorBounds.x && !lineChanged)//We set the characters that will be displayed
        {
            if (cursorPos >= 1)
                currentCursorBounds.x = cursorAbsolutePos - 1;
            else
                currentCursorBounds.x = cursorPos;

            currentCursorBounds.y = currentCursorBounds.x;
            //We now need to set the upper bound

            int numberOfCharacters(1);

            for ( ; contentText.findCharacterPos(currentCursorBounds.x + numberOfCharacters).x - contentText.findCharacterPos(currentCursorBounds.x).x <= tempText.getSize().x
                            && currentCursorBounds.x + numberOfCharacters < valueContained[currentLine].getSize() ; numberOfCharacters++)//While there are characters to treat
            {
                //contentText.setString(valueContained[currentLine].substr(currentCursorBounds.x, numberOfCharacters));
                //currentCursorBounds.y = currentCursorBounds.x + numberOfCharacters;

            }

            currentCursorBounds.y = currentCursorBounds.x + numberOfCharacters;

            renderFromLowerCursorBound = true;
            //std::cout << "rekt" << "]";
            //std::cout << "New bounds : [" << currentCursorBounds.x << ';' << currentCursorBounds.y << "]\n";
        }
        else if (((cursorAbsolutePos > currentCursorBounds.y || currentCursorBounds.y > valueContained[currentLine].getSize()) || lineChanged)
                 && cursorAbsolutePos > 0)
        {
            std::size_t minimumLowerBound(0);

            minimumLowerBound = cursorAbsolutePos - cursorPos;
            currentCursorBounds.y = cursorAbsolutePos;
            currentCursorBounds.x = currentCursorBounds.y;
            //We now need to set the upper bound

            int numberOfCharacters(1);

            for ( ; contentText.findCharacterPos(currentCursorBounds.y).x - contentText.findCharacterPos(currentCursorBounds.y - numberOfCharacters).x <= tempText.getSize().x
                            && currentCursorBounds.y - numberOfCharacters > minimumLowerBound ; numberOfCharacters++)//While there are characters to treat
            {
                //contentText.setString(valueContained[currentLine].substr(currentCursorBounds.y - numberOfCharacters, numberOfCharacters));
                //currentCursorBounds.x = currentCursorBounds.y - numberOfCharacters;
            }
            currentCursorBounds.x = currentCursorBounds.y - numberOfCharacters;
            //currentCursorBounds.x = currentCursorBounds.y - numberOfCharacters;
            renderFromLowerCursorBound = false;
            //std::cout << "New bounds : [" << currentCursorBounds.x << ';' << currentCursorBounds.y << "]\n";
        }

        if (contentText.getLocalBounds().height + contentText.getLocalBounds().top > tempText.getSize().y)
            y = -(contentText.getLocalBounds().height + contentText.getLocalBounds().top) + tempText.getSize().y;
        else
            y = 0.0;

        if (renderFromLowerCursorBound)
        {

            if (contentText.findCharacterPos(currentCursorBounds.y).x - contentText.findCharacterPos(currentCursorBounds.x).x > tempText.getSize().x)
                x = -contentText.findCharacterPos(currentCursorBounds.x).x + contentText.findCharacterPos(0).x;
        }
        else
        {
            if (contentText.findCharacterPos(currentCursorBounds.y).x - contentText.findCharacterPos(currentCursorBounds.x).x > tempText.getSize().x)
                x = -contentText.findCharacterPos(currentCursorBounds.y).x + tempText.getSize().x + contentText.findCharacterPos(0).x;
        }

        //std::cout << "Length : " << contentText.findCharacterPos(currentCursorBounds.y).x - contentText.findCharacterPos(currentCursorBounds.x).x << '\n';
        //std::cout << "Bounds : ["<< currentCursorBounds.x <<';'<< currentCursorBounds.y <<"]\n";

        this->contentText.setPosition(x,y);

        tempText.draw(contentText);
        tempText.display();

        myTexture.reset();
        myTexture = std::make_shared<sf::Texture>(tempText.getTexture());

        textToDisplay.setTexture(*myTexture);
        textToDisplay.setPosition(background.getPosition().x + textRelPos.x, background.getPosition().y + textRelPos.y);

        this->contentText.setPosition(tempPos);
    }

    virtual void update(sf::RenderWindow& window)
    {
        updateMouseInteraction(window, &background.getGlobalBounds());

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            if (hovered)
                selected = true;
            else
                selected = false;
        }

        if (selected)
        {
            std::size_t cursorPosBefore(cursorPos);
            std::size_t currentLineBefore(currentLine);

            std::size_t lettersCountBefore(0);

            for (int i(0) ; i < valueContained.size() ; i++)
                lettersCountBefore += valueContained[i].getSize();

            //std::cout << contentText.findCharacterPos(0).x <<'\n';
            if (Widget::unicodeBuffer.size())
            {
                for (int i(0) ; i < Widget::unicodeBuffer.size() ; i++)
                {
                    //std::cout << Widget::unicodeBuffer[i] << '\n';
                    switch (Widget::unicodeBuffer[i])
                    {
                    case 8://Delete

                        if (cursorPos > 0)
                        {
                            valueContained[currentLine].erase(/**valueContained[currentLine].begin() + */cursorPos - 1);
                            cursorPos--;
                        }
                        else//cursorPos == 0
                        {
                            if (currentLine != 0)
                            {
                                cursorPos = valueContained[currentLine - 1].getSize();//We put the cursor at the beginning of the upper line
                                valueContained[currentLine - 1] += valueContained[currentLine];//We add the older line to the upper
                                valueContained.erase(valueContained.begin() + currentLine);//We erase the previous line
                                currentLine--;//And we say that we're on a new line
                            }
                        }


                        break;

                    case 13://Enter

                        valueContained.insert(valueContained.begin()+currentLine+1, valueContained[currentLine].substring(cursorPos));
                        valueContained[currentLine] = valueContained[currentLine].substring(0,cursorPos);
                        cursorPos = 0;
                        currentLine++;

                        break;

                    default:

                        valueContained[currentLine].insert(/**valueContained[currentLine].begin()+*/cursorPos, Widget::unicodeBuffer[i]);
                        cursorPos++;
                        break;
                    }
                }
            }

            if (Widget::keyBuffer.size())
            {
                for (int i(0) ; i < Widget::keyBuffer.size() ; i++)
                {
                    switch (Widget::keyBuffer[i])
                    {
                    case 71://Left
                        if (cursorPos)
                            cursorPos--;
                        break;
                    case 72://Right
                        if (cursorPos < valueContained[currentLine].getSize())
                            cursorPos++;
                        break;
                    case 73://Up

                        if (currentLine > 0)
                        {
                            if (valueContained[currentLine - 1].getSize() < cursorPos)
                            {
                                cursorPos = valueContained[currentLine - 1].getSize();
                            }
                            //else, value unchanged
                            currentLine--;
                        }

                        break;
                    case 74://Down

                        if (currentLine < valueContained.size() - 1)
                        {
                            if (valueContained[currentLine + 1].getSize() < cursorPos)
                            {
                                cursorPos = valueContained[currentLine + 1].getSize();
                            }
                            //else, value unchanged
                            currentLine++;
                        }

                        break;
                    default:
                        break;
                    }
                }
            }

            std::size_t lettersCountAfter(0);

            for (int i(0) ; i < valueContained.size() ; i++)
                lettersCountAfter += valueContained[i].getSize();

            if (lettersCountBefore != lettersCountAfter || cursorPosBefore != cursorPos || currentLineBefore != currentLine)//Update the sprite
            {
                updateSprite(!(currentLineBefore == currentLine));


                /**std::cout << "Pos of the character is (" << contentText.findCharacterPos(cursorPos).x
                << ';' << contentText.findCharacterPos(cursorPos).y << ")\n";*/
                std::cout << "Number of lines now : "<< valueContained.size() <<'\n';
                std::cout << "Number of characters now : " << lettersCountAfter << '\n';
            }
        }
    };
    void Foo() {};
    virtual void draw(sf::RenderWindow& window) const
    {
        background.draw(window);
        window.draw(textToDisplay);
    };

private:
    std::size_t cursorPos;
    std::size_t currentLine;
    bool selected;

    sf::Vector2u currentCursorBounds;//the .x of this is the lower bound, .y is the upper bound
    bool renderFromLowerCursorBound;//If true, we render from the lower bound. Else, we render from the upper bound
    sf::Vector2u currentLineBounds;//Exactly the same as the two above, but on a vertical axis (on y)
    bool renderFromLowerLineBound;

    std::vector<sf::String> valueContained;

    ID_TYPE fontID;
    sf::Text contentText;
    sf::Vector2f textRelPos;
    sf::Vector2f textBoxSize;
    sf::Sprite textToDisplay;

    std::shared_ptr<sf::Texture> myTexture;

    Dummy background;
};

void updateWidgetDirection(Widget& toUpdate, sf::RenderWindow& window);
void renderWidgetDirection(Widget& toRender, sf::RenderWindow& window);

#include "Classes.h"

#endif // INTERFACE_HPP_INCLUDED
