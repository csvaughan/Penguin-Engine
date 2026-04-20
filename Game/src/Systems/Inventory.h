#include "Penguin.h"


//string = item name
//int = quantity

class Inventory
{
public:
    Inventory(/* args */);
    ~Inventory();

    void addItem(std::string& name, int quantity = 1)
    {
        if(m_items.find(name) != m_items.end())
        {

        }
    }

    std::string removeItem(std::string& name, int quantity = 1)
    {
        if(m_items.find(name) != m_items.end())
        {
            return std::string();
        }
    }

    int getItemQuantity(std::string name)
    {

    }

private:

    std::map<std::string, int> m_items;

};