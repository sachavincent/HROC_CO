#ifndef IDGENERATOR_HPP
#define IDGENERATOR_HPP
class IdGenerator
{
private:
    static int id;

public:
    static int GetUniqueId() { return id++; };
};
#endif