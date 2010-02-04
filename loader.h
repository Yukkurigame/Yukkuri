#ifndef LOADER_H
#define LOADER_H

class Loader
{
    public:
        void load(const char* filename);        
    private:
        void _cload();
}

#endif