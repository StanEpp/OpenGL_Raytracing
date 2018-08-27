#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

struct Settings
{
    uint32_t width = 1024;
    uint32_t height = 768;
    uint32_t maxFPS = 60;
    uint32_t reflectionDepth = 5;
    bool fullscreen = false;
    float fovY = 45.0f;
    float cameraSensitivity = 0.005;
};

class ConfigLoader
{
public:
    /*
    Format Type:

    width 1024
    height 768
    maxFps 60
    fullscreen 1

    */
    ConfigLoader(const std::string &filename) {
        std::ifstream file(filename, std::ios::in);

        if(!file){
            std::cerr << "Error: Cannot open config file! Make sure there is a config file with the name \"" << filename << "\" in the same directory as the executable.\n";
            std::cerr << "Falling back to default values!\n";
            return;
        }

        std::string line, value, descr;
        std::istringstream token;

        while(!file.eof()){
            getline(file, line);
            if(!line.empty()){
                token = std::istringstream(line);
                getline(token, descr, ' ');
                getline(token, value, ' ');
                if (descr == "width"){ m_settings.width = std::stoi(value); }
                else if (descr == "height"){ m_settings.height = std::stoi(value); }
                else if (descr == "maxFPS"){ m_settings.maxFPS = std::stoi(value); }
                else if (descr == "reflectionDepth"){ m_settings.reflectionDepth = std::stoi(value); }
                else if (descr == "fullscreen"){ m_settings.fullscreen = std::stoi(value); }
                else if (descr == "cameraSensitivity"){ m_settings.cameraSensitivity = std::stof(value); }
            }
        }
    }

    Settings settings() const { return m_settings; }

private:
    Settings m_settings;
};