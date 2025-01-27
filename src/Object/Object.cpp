#include "Object/Object.hpp"
#include "Object.hpp"

int szl::Object::getSize() const
{
    // TODO
    return 0;
}

int szl::Object::getVariablePosition(const int objectPosition, const std::string &name) const
{
    for (const auto &it : variables)
    {
        if (it.first == name)
        {
            return it.second.getPosition(objectPosition);
        }
    }
}
