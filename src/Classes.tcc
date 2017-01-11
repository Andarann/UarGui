template<class T>
T* System::addWidget(T&& toAdd, std::string name)
{
    auto findWidget = stringToIDTable.find(name);

    if (findWidget == stringToIDTable.end())
    {
        if (vacantWidgetID.empty())
        {
            toAdd.setID(Widget::currentWidgetID);
            Widget::currentWidgetID++;
        }
        else
        {
            toAdd.setID(vacantWidgetID.front());
            vacantWidgetID.pop();
        }

        stringToIDTable.insert(std::make_pair(name, toAdd.getID()));//We make the ID and the name correspond

        allWidgets.insert( std::make_pair(toAdd.getID(), std::unique_ptr<Widget>(new T(toAdd)) ));
        return toAdd.getID();
    }
    else
    {
        return giveWidgetPointer(name);
    }
}
