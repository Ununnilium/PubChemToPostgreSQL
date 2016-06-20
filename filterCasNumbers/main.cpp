#include <iostream>
#include <cstdio>
#include <cstring>

bool isCas(const char* cas);

int main()
{
    char line[4096];
    char *tabPos;
    while(true)
    {
        std::cin.getline(line, 4095);
        if(!std::cin.good())
            break;
        tabPos = std::strchr(line,'\t');
        if(tabPos == NULL)
            break;
        if(isCas(tabPos + 1))
            std::puts(line);
    }
}


inline bool isCas(const char* cas)
{
    const char *pch = std::strchr(cas,'-');
    if(pch == NULL)
        return false;
    long difference = (pch - cas);
    if(difference < 2 || difference > 7 || *(pch+3) != '-' || (*(pch+5) != '\0'))
        return false;
    int checkdigit = *(pch + 4) - 48;
    int calculated = (*(pch + 2) - 48) + (*(pch + 1) - 48) * 2;
    for(long i=1;i<=difference;i++)
        calculated += (*(pch - i) - 48) * (i + 2);
    return calculated % 10 == checkdigit;
}
