#ifndef IDGENERATOR_HPP
#define IDGENERATOR_HPP
class IdGenerator
{
private:
    int id = 0;

public:
    int GetUniqueId() { return id++; };
};
#endif