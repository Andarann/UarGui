
#include "Interface.hpp"

ID_TYPE Widget::currentFontID = 1;//1, because 0 means there was no texture initially bound
ID_TYPE Widget::currentTextureID = 1;
std::map<std::string, ID_TYPE> Widget::stringToIDTableFont;
std::map<ID_TYPE, std::unique_ptr<sf::Font>> Widget::widgetFonts;
std::queue<ID_TYPE> Widget::vacantFontID;
std::map<std::string, ID_TYPE> Widget::stringToIDTableTexture;
std::map<ID_TYPE, std::unique_ptr<sf::Texture>> Widget::widgetTextures;
std::queue<ID_TYPE> Widget::vacantTextureID;
bool Container::memberInteracted = false;

bool Widget::mouseButtonsMaintained[sf::Mouse::ButtonCount];
bool Widget::ignoreMouseMaintained[sf::Mouse::ButtonCount];
bool Widget::mouseButtonClick[sf::Mouse::ButtonCount];
bool Widget::ignoreMouseClick[sf::Mouse::ButtonCount];
bool Widget::keyboardButtonsMaintained[sf::Keyboard::KeyCount];
bool Widget::keyboardButtonPushed[sf::Keyboard::KeyCount];

Widget::Widget()
{
    posAbsolute = false;
    dragable = false;
    resizable = false;
    clicked = false;
    clickMaintained = false;
    toClose = false;
}

Widget::Widget(int widgetX, int widgetY, bool setPosToAbsolute)
{

}

Widget::~Widget()
{

}

bool Widget::isHovered(sf::RenderWindow& window, sf::FloatRect* widgetShape) const
{
    return widgetShape->contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
}

bool Widget::isClicked() const
{
    return clicked;
}

void Widget::update(sf::RenderWindow& window)
{
    //Nothing since you're not supposed to use a the Widget class directly
}

ID_TYPE Widget::getID() const
{
    return ID;
}

void Widget::draw(sf::RenderWindow& window) const
{
    //Nothing, you're not supposed to render a widget directly
}



void Widget::setPosAbsolute(bool isPosAbsolute)
{
    posAbsolute = isPosAbsolute;
}

void Widget::setDragable(bool isDragable)
{
    dragable = isDragable;
}

void Widget::setResizable(bool isResizable)
{
    resizable = isResizable;
}

void Widget::updateClick(sf::RenderWindow& window, sf::FloatRect* widgetShape)
{
    bool widgetHovered = isHovered(window, widgetShape);

    if (widgetHovered &&
        Widget::isButtonClicked(window, sf::Mouse::Left) &&
        (!clicked))
    {
        Widget::ignoreButtonClick(window, sf::Mouse::Left);
        clicked = true;
    }
    else if (widgetHovered &&
             Widget::isButtonMaintained(window, sf::Mouse::Left) &&
             clicked)
    {
        clicked = false;
        clickMaintained = true;
    }
    else if (!widgetHovered ||
             (!Widget::isButtonMaintained(window, sf::Mouse::Left) &&
              !Widget::isButtonClicked(window, sf::Mouse::Left)))
    {
        clicked = false;
        clickMaintained = false;
    }
}

void Widget::setID(ID_TYPE newID)
{
    ID = newID;
}

ID_TYPE Widget::giveTextureID(std::string texturePath)
{
    if (Widget::stringToIDTableTexture.find(texturePath) == Widget::stringToIDTableTexture.end())
    {
        sf::Texture loadingTexture;

        if (loadingTexture.loadFromFile(texturePath));
        else
        {
            std::cout << "Failed to load image of the following path : " << texturePath << '\n';
        }

        ID_TYPE givenID;

        if (Widget::vacantTextureID.empty())
        {
            givenID = Widget::currentTextureID;
            Widget::currentTextureID++;
        }
        else
        {
            givenID = Widget::vacantTextureID.front();
            Widget::vacantTextureID.pop();
        }

        Widget::stringToIDTableTexture.insert(std::make_pair(texturePath, givenID));
        Widget::widgetTextures.insert(std::make_pair(givenID, std::unique_ptr<sf::Texture>(new sf::Texture(loadingTexture)) ));
    }
    //else, the texture is loaded

    return Widget::stringToIDTableTexture.find(texturePath)->second;//Yummy yummy syntax
}

ID_TYPE Widget::giveFontID(std::string fontPath)
{
    if (Widget::stringToIDTableFont.find(fontPath) == Widget::stringToIDTableFont.end())
    {
        sf::Font loadingFont;

        if (loadingFont.loadFromFile(fontPath));
        else
        {
            std::cout << "Failed to load font of the following path : " << fontPath << '\n';
        }

        ID_TYPE givenID;

        if (Widget::vacantFontID.empty())
        {
            givenID = Widget::currentFontID;
            Widget::currentFontID++;
        }
        else
        {
            givenID = Widget::vacantFontID.front();
            Widget::vacantFontID.pop();
        }

        Widget::stringToIDTableFont.insert(std::make_pair(fontPath, givenID));
        Widget::widgetFonts.insert(std::make_pair(givenID, std::unique_ptr<sf::Font>(new sf::Font(loadingFont)) ));
    }
    //else, the font is already loaded

    return Widget::stringToIDTableFont.find(fontPath)->second;
}

sf::Texture* Widget::giveTexturePointerOfID(ID_TYPE textureID)
{
    if (Widget::widgetTextures.find(textureID) == Widget::widgetTextures.end())
        std::cout << "Error : There is no texture of ID " << textureID << " stored\n";
    else
        return Widget::widgetTextures.find(textureID)->second.get();
}

sf::Font* Widget::giveFontPointerOfID(ID_TYPE fontID)
{
    if (Widget::widgetFonts.find(fontID) == Widget::widgetFonts.end())
        std::cout << "Error : There is no font of ID " << fontID << " stored\n";
    else
        return Widget::widgetFonts.find(fontID)->second.get();
}

void Widget::removeTexture(ID_TYPE textureID)
{

}

void Widget::removeFont(ID_TYPE fontID)
{

}

bool Widget::Init(sf::Window const& mainWindow)
{
    for (int i(0) ; i < sf::Mouse::ButtonCount ; i++)
    {
        Widget::mouseButtonsMaintained[i] = false;
        Widget::ignoreMouseMaintained[i] = false;
        Widget::mouseButtonClick[i] = false;
        Widget::ignoreMouseClick[i] = false;
    }

    for (int i(0) ; i < sf::Keyboard::KeyCount ; i++)
    {
        keyboardButtonsMaintained[i] = false;
        keyboardButtonPushed[i] = false;
    }
}

void Widget::UpdateEvents(sf::Window const& mainWindow)
{
    if (mainWindow.hasFocus())//In case there are multiple windows, we ensure that we only update events in the right window
    {
        for (int i(0); i<sf::Keyboard::KeyCount ; i++)
        {
            if (sf::Keyboard::isKeyPressed(i))//If the button is pressed...
            {
                if (Widget::keyboardButtonPushed[i] == false && Widget::keyboardButtonsMaintained[i] == false)//We first for once set it on clicked...
                {
                    //std::cout << "Pushed key number : "<< i <<'\n';
                    Widget::keyboardButtonPushed[i] = true;
                }
                else if (Widget::keyboardButtonPushed[i] == true && Widget::keyboardButtonsMaintained[i] == false)//Then block it on maintained
                {
                    Widget::keyboardButtonPushed[i] = false;
                    Widget::keyboardButtonsMaintained[i] = true;
                }
            }
            else if (Widget::keyboardButtonPushed[i] || Widget::keyboardButtonsMaintained[i])//Release the button
            {
                //std::cout << "Released key number : "<< i <<'\n';
                Widget::keyboardButtonPushed[i] = false;
                Widget::keyboardButtonsMaintained[i] = false;
            }
        }

        for (int i(0);i<sf::Mouse::ButtonCount;i++)
        {
            if (sf::Mouse::isButtonPressed(i))//If the button is pressed...
            {
                if (Widget::mouseButtonClick[i] == false && Widget::mouseButtonsMaintained[i] == false)//We first for once set it on clicked...
                {
                    Widget::mouseButtonClick[i] = true;
                    Widget::ignoreMouseClick[i] = false;
                }
                else if (Widget::mouseButtonClick[i] == true && Widget::mouseButtonsMaintained[i] == false)//Then block it on maintained
                {
                    Widget::mouseButtonClick[i] = false;
                    Widget::mouseButtonsMaintained[i] = true;
                    Widget::ignoreMouseMaintained[i] = false;
                }
            }
            else if (Widget::mouseButtonClick[i] || Widget::mouseButtonsMaintained[i])//Release the button
            {
                //std::cout << "Released button number : "<< i <<'\n';
                Widget::mouseButtonClick[i] = false;
                Widget::mouseButtonsMaintained[i] = false;
                Widget::ignoreMouseClick[i] = false;
                Widget::ignoreMouseMaintained[i] = false;
            }
        }
    }
}

bool Widget::isKeyPressed(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::keyboardButtonsMaintained[keyCheck] || Widget::keyboardButtonPushed[keyCheck]);
    else
        return false;//No interaction
}

bool Widget::isKeyPushed(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::keyboardButtonPushed[keyCheck]);
    else
        return false;//No interaction
}

bool Widget::isKeyMaintained(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::keyboardButtonsMaintained[keyCheck]);
    else
        return false;//No interaction
}

bool Widget::isButtonPressed(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck)
{
    if (mainWindow.hasFocus())
        return ((Widget::mouseButtonsMaintained[buttonCheck] && !Widget::ignoreMouseMaintained[buttonCheck]) ||
                (Widget::mouseButtonClick[buttonCheck] && !Widget::ignoreMouseClick[buttonCheck]));
    else
        return false;//No interaction
}

bool Widget::isButtonClicked(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::mouseButtonClick[buttonCheck] && !Widget::ignoreMouseClick[buttonCheck]);
    else
        return false;//No interaction
}

bool Widget::isButtonMaintained(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::mouseButtonsMaintained[buttonCheck] && !Widget::ignoreMouseMaintained[buttonCheck]);
    else
        return false;//No interaction
}

void Widget::ignoreButtonClick(sf::Window const& mainWindow, sf::Mouse::Button toIgnore)
{
    if (mainWindow.hasFocus())
        Widget::ignoreMouseClick[toIgnore] = true;
}

void Widget::ignoreButtonMaintained(sf::Window const& mainWindow, sf::Mouse::Button toIgnore)
{
    if (mainWindow.hasFocus())
        Widget::ignoreMouseMaintained[toIgnore] = true;
}


bool Widget::isWidgetContainer(Widget* toCheck)
{
    return (typeid(*toCheck) == typeid(Container));
}

Dummy::Dummy(std::string imagePath, sf::Vector2f size, sf::Vector2f pos)
{
    textureID = Widget::giveTextureID(imagePath);
    sprite_.setTexture(*Widget::giveTexturePointerOfID(textureID));
    sprite_.setScale(size.x / sprite_.getTextureRect().width, size.y / sprite_.getTextureRect().height);
    sprite_.setPosition(pos);
}

Dummy::Dummy(sf::Color const& fillColor, sf::Vector2f size, sf::Vector2f pos)
{

    textureID = 0;
    sf::Texture tempTexture;
    tempTexture.create(size.x, size.y);
    sprite_.setTexture(tempTexture);//Small hack : We purposely let it go out of scope so that we can color the sprite as we wish
    sprite_.setPosition(pos);
    sprite_.setColor(fillColor);
}

void Dummy::setPos(sf::Vector2f& newPos)
{
    sprite_.setPosition(newPos);
}

void Dummy::resizeDummy(sf::Vector2f& newSize)
{
    sprite_.setScale(newSize.x / sprite_.getTextureRect().width, newSize.y / sprite_.getTextureRect().height);
}

sf::FloatRect Dummy::getLocalBounds() const
{
    return sprite_.getLocalBounds();
}

sf::FloatRect Dummy::getGlobalBounds() const
{
    return sprite_.getGlobalBounds();
}

void Dummy::update(sf::RenderWindow& window)
{

}

void Dummy::draw(sf::RenderWindow& window) const
{
    //sprite_.setTexture(*Widget::giveTexturePointerOfID(textureID));
    window.draw(sprite_);
}

Button::Button()
{
    buttonShape.setSize(sf::Vector2f(1,1));
    buttonShape.setFillColor(sf::Color(255,255,255));
    buttonShape.setPosition(sf::Vector2f(0,0));
}

Button::Button(sf::Vector2f pos, sf::Color buttonColoration, std::string buttonContent, short fontSize, sf::Vector2f textPosPercent, short distTextBoundaries)
{
    sf::Font tempFont;
    tempFont.loadFromFile("Cousine-Regular.TTF");

    buttonFont = std::make_shared<sf::Font>(tempFont);

    sf::Sprite test;

    buttonString.setColor(sf::Color(0,0,0));
    buttonString.setCharacterSize(fontSize);

    buttonString.setFont(*buttonFont);
    buttonString.setString(buttonContent);

    buttonShape.setSize(sf::Vector2f(distTextBoundaries*2 + buttonString.getLocalBounds().width ,
                                     distTextBoundaries*2 + buttonString.getLocalBounds().height));
    buttonShape.setFillColor(buttonColoration);
    buttonShape.setPosition(pos);

    buttonString.setPosition(-buttonString.getGlobalBounds().left + buttonShape.getPosition().x + (float)textPosPercent.x * (buttonShape.getSize().x - buttonString.getLocalBounds().width) ,
                             -buttonString.getGlobalBounds().top + buttonShape.getPosition().y + (float)textPosPercent.y * (buttonShape.getSize().y - buttonString.getLocalBounds().height));
}

Button::~Button()
{

}

void Button::setString(std::string stringToPut)
{

}

void Button::setColor(sf::Color newColor)
{

}

sf::Color Button::getColor() const
{

}

std::string Button::getButtonString() const
{
    return buttonString.getString();
}

void Button::update(sf::RenderWindow& window)
{
    updateClick(window, &buttonShape.getGlobalBounds());
}

void Button::draw(sf::RenderWindow& window) const
{
    if (clicked || clickMaintained)
    {
        sf::Color temp(buttonShape.getFillColor());
        sf::Color tempTwo(temp.r +50, temp.g +50, temp.b +50);
        buttonShape.setFillColor(tempTwo);

        window.draw(buttonShape);
        window.draw(buttonString);

        buttonShape.setFillColor(temp);
    }
    else if (isHovered(window, &buttonShape.getGlobalBounds()))
    {
        sf::Color temp(buttonShape.getFillColor());
        sf::Color tempTwo(temp.r + 25, temp.g + 25, temp.b + 25);
        buttonShape.setFillColor(tempTwo);

        window.draw(buttonShape);
        window.draw(buttonString);

        buttonShape.setFillColor(temp);
    }
    else
    {
        window.draw(buttonShape);
        window.draw(buttonString);
    }

}

Slider::Slider()
{

}

Slider::~Slider()
{

}

void Slider::update(sf::RenderWindow& window)
{

}

void Slider::draw(sf::RenderWindow& window) const
{

}

Checkbox::Checkbox() : checkBoxChecked("checkboxcheck.png", sf::Vector2f(50,50)), checkBoxUnchecked("checkboxuncheck.png", sf::Vector2f(50,50))
{
    value = false;

}

Checkbox::Checkbox(bool init, sf::Vector2f widSize, sf::Vector2f pos) : checkBoxChecked("checkboxcheck.png", widSize, pos), checkBoxUnchecked("checkboxuncheck.png", widSize, pos)
{
    value = init;
}

Checkbox::~Checkbox()
{

}

bool Checkbox::getValue() const
{
    return value;
}

void Checkbox::update(sf::RenderWindow& window)
{
    updateClick(window, &checkBoxChecked.getGlobalBounds());

    if (clicked)
        value = !value;
}

void Checkbox::draw(sf::RenderWindow& window) const
{
    if (value)
    {
        checkBoxChecked.draw(window);
    }
    else
    {
        checkBoxUnchecked.draw(window);
    }
}

Container::Container()
{
    currentWidgetID = 0;
}

Container::~Container()
{

}

void Container::setTitle(std::string newTitle)
{

}



void Container::update(sf::RenderWindow& window)
{
    Container::memberInteracted = false;
    //First we update the container

    //Then we update every widget contained
    for (std::map<ID_TYPE, std::unique_ptr<Widget>>::reverse_iterator it = allWidgets.rbegin() ; it != allWidgets.rend() ; it++)
        updateWidgetDirection(*(it->second), window);
}

void Container::draw(sf::RenderWindow& window) const
{
    //Render container

    //Then render every widget
    for (auto it = allWidgets.begin() ; it != allWidgets.end() ; it++)
        renderWidgetDirection(*(it->second), window);
}

void Container::removeWidget(std::string name)
{
    ID_TYPE ID = stringToIDTable[name];

    vacantWidgetID.push(ID);

    stringToIDTable.erase(name);
    allWidgets.erase(ID);
}

Widget* Container::giveWidgetPointer(std::string widgetName) const
{
    auto findID = stringToIDTable.find(widgetName);

    if (findID == stringToIDTable.end())
        std::cout << "Widget of name '" << widgetName << "' does not exist in this system\n";
    else
        return returnWidgetPointerOfID(findID->second);
}

Widget* Container::returnWidgetPointerOfID(ID_TYPE ID) const
{
    auto findWidget = allWidgets.find(ID);

    if (findWidget == allWidgets.end())
        std::cout << "Widget of ID #" << ID << " does not exist in this system\n";
    else
        return findWidget->second.get();//Footnote : this syntax is horrible
}

void updateWidgetDirection(Widget& toUpdate, sf::RenderWindow& window)
{
    toUpdate.update(window);
}

void renderWidgetDirection(Widget& toRender, sf::RenderWindow& window)
{
    toRender.draw(window);
}
