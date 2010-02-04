#include <stdio.h>
/*This is unless module for debug messages*/
//It is the simple 'hello world'

/*By default: Information messages*/
void print_(const char* string, const char* prefix)
{
    printf("%s: %s\n", prefix, string);
}

/*Warnings*/
void print_w(const char* string)
{
    print_(string, "W");
}

/*Debug information*/
void print_d(const char* string)
{
    //add debug mode test
    // print only if debug mode is on
    print_(string, "D");
}

/*Errors*/
void print_e(const char* string)
{
    //This should cause an exception and exit or something like that.    
    print_(string, "E");
}