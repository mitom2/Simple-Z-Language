#include "Object/Object.hpp"
#include "Object.hpp"

int szl::Object::getSize() const
{
    int res = 0;
    for (const auto &it : szl::Object::variables)
    {
        auto type = it.second.getType();
        if (type == "int" || type == "uint" || type == "char" || type == "bool")
            res += 2;
        else if (type == "long" || type == "ulong")
            res += 4;
        else
        {
            if (!szl::objectTypes.count(type))
                throw szl::SZLException("Object '" + name + "' is declared with member named '" + it.first + "' that is is of unknown type '" + type + "'");
            res += szl::objectTypes[type].getSize();
        }
    }
    return res;
}

int szl::Object::getVariableOffset(const int objectOffset, const std::string &name) const
{
    for (const auto &it : variables)
    {
        if (it.first == name)
        {
            return objectOffset - it.second.getOffset() + 1;
        }
    }
    throw szl::SZLException("Unknown variable '" + name + "'");
}
