#pragma once

#include <iostream>
#include <fstream>
#include <string>

struct Settings
{
    uint32_t width = 1024;
    uint32_t height = 768;
    uint32_t maxFPS = 60;
    uint32_t reflectionDepth = 5;
    bool fullscreen = false;
    float fovY = 45.0f;
    float velocityRotate = 0.5f;
    float velocityTranslate = 0.2f;
};

class ConfigLoader {
public:
    /*
    Format of the config file is rough and unforgivable! Small deviations will fail!
    (No need for sophisticated parsing for such a small config ;) )

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
                else if (descr == "velocityRotate"){ m_settings.velocityRotate = std::stof(value); }
                else if (descr == "velocityTranslate"){ m_settings.velocityTranslate = std::stof(value); }
            }
        }
    }

    Settings getSettings() const { return m_settings; }

private:
    Settings m_settings;
};