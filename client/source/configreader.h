#ifndef CONFIGREADER_H
#define CONFIGREADER_H
#include<iostream>
#include<fstream>
#include<unordered_map>
using namespace std;
class ConfigReader
{
private:
    static string SERVER_CONFIG_PATH;
public:
    ConfigReader();
    static unordered_map<string, string> readServerConfig();
};

#endif // CONFIGREADER_H
