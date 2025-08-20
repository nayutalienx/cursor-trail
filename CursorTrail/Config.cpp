#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

// Global configuration instance
Config g_config;

bool Config::LoadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Config file '" << filename << "' not found, using defaults." << std::endl;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Remove comments and trim whitespace
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        // Parse key=value pairs
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            std::cout << "Warning: Invalid config line " << lineNumber << ": " << line << std::endl;
            continue;
        }
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Trim key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Convert key to lowercase for case-insensitive matching
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        
        // Parse configuration values
        try {
            if (key == "spritesize" || key == "sprite_size") {
                spriteSize = std::stof(value);
                if (spriteSize <= 0) {
                    std::cout << "Warning: spriteSize must be positive, using default." << std::endl;
                    spriteSize = 15.0f;
                }
            }
            else if (key == "texturepath" || key == "texture_path" || key == "texture") {
                texturePath = value;
                // Remove quotes if present
                if (texturePath.front() == '"' && texturePath.back() == '"') {
                    texturePath = texturePath.substr(1, texturePath.length() - 2);
                }
            }
            else if (key == "fadetime" || key == "fade_time") {
                fadeTime = std::stof(value);
                if (fadeTime <= 0) {
                    std::cout << "Warning: fadeTime must be positive, using default." << std::endl;
                    fadeTime = 1.0f;
                }
            }
            else if (key == "faderate" || key == "fade_rate") {
                fadeRate = std::stof(value);
                if (fadeRate <= 0 || fadeRate > 1.0f) {
                    std::cout << "Warning: fadeRate must be between 0 and 1, using default." << std::endl;
                    fadeRate = 0.05f;
                }
            }
            else if (key == "spawnfrequency" || key == "spawn_frequency" || key == "density") {
                spawnFrequency = std::stof(value);
                if (spawnFrequency <= 0) {
                    std::cout << "Warning: spawnFrequency must be positive, using default." << std::endl;
                    spawnFrequency = 6.0f;
                }
            }
            else if (key == "maxparticles" || key == "max_particles" || key == "particles") {
                maxParticles = std::stoi(value);
                if (maxParticles <= 0 || maxParticles > 10000) {
                    std::cout << "Warning: maxParticles must be between 1 and 10000, using default." << std::endl;
                    maxParticles = 2048;
                }
            }
            else {
                std::cout << "Warning: Unknown config key '" << key << "' on line " << lineNumber << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Warning: Failed to parse value for '" << key << "' on line " << lineNumber << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << "Configuration loaded from: " << filename << std::endl;
    return true;
}

bool Config::SaveToFile(const std::string& filename) const
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to save config to: " << filename << std::endl;
        return false;
    }
    
    file << "# Cursor Trail Configuration\n";
    file << "# Edit these values to customize your cursor trail\n\n";
    
    file << "# Trail appearance\n";
    file << "spriteSize=" << spriteSize << "     # Size of trail particles (pixels)\n";
    file << "texture=" << texturePath << "     # Path to trail texture image\n\n";
    
    file << "# Trail behavior\n";
    file << "fadeTime=" << fadeTime << "       # How long particles last (seconds)\n";
    file << "fadeRate=" << fadeRate << "       # How fast particles fade per frame (0.0-1.0)\n";
    file << "spawnFrequency=" << spawnFrequency << "   # Spawn interval - lower = denser trail (pixels)\n";
    file << "maxParticles=" << maxParticles << "     # Maximum number of particles\n";
    
    std::cout << "Configuration saved to: " << filename << std::endl;
    return true;
}

bool Config::ParseCommandLine(int argc, char* argv[])
{
    bool foundArgs = false;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Cursor Trail Configuration Options:\n";
            std::cout << "  --size <value>        Set sprite size (default: " << spriteSize << ")\n";
            std::cout << "  --texture <path>      Set texture path (default: " << texturePath << ")\n";
            std::cout << "  --fade-time <value>   Set fade time (default: " << fadeTime << ")\n";
            std::cout << "  --fade-rate <value>   Set fade rate (default: " << fadeRate << ")\n";
            std::cout << "  --density <value>     Set spawn density (default: " << spawnFrequency << ")\n";
            std::cout << "  --particles <value>   Set max particles (default: " << maxParticles << ")\n";
            std::cout << "  --config <file>       Load config from file\n";
            std::cout << "  --save-config <file>  Save current config to file\n";
            std::cout << "  --help, -h            Show this help\n";
            return false; // Indicate to exit after showing help
        }
        else if (arg == "--size" && i + 1 < argc) {
            spriteSize = std::stof(argv[++i]);
            foundArgs = true;
        }
        else if (arg == "--texture" && i + 1 < argc) {
            texturePath = argv[++i];
            foundArgs = true;
        }
        else if (arg == "--fade-time" && i + 1 < argc) {
            fadeTime = std::stof(argv[++i]);
            foundArgs = true;
        }
        else if (arg == "--fade-rate" && i + 1 < argc) {
            fadeRate = std::stof(argv[++i]);
            foundArgs = true;
        }
        else if (arg == "--density" && i + 1 < argc) {
            spawnFrequency = std::stof(argv[++i]);
            foundArgs = true;
        }
        else if (arg == "--particles" && i + 1 < argc) {
            maxParticles = std::stoi(argv[++i]);
            foundArgs = true;
        }
        else if (arg == "--config" && i + 1 < argc) {
            LoadFromFile(argv[++i]);
            foundArgs = true;
        }
        else if (arg == "--save-config" && i + 1 < argc) {
            SaveToFile(argv[++i]);
            foundArgs = true;
        }
    }
    
    return foundArgs;
}

void Config::PrintConfig() const
{
    std::cout << "\n=== Cursor Trail Configuration ===" << std::endl;
    std::cout << "Sprite Size:      " << spriteSize << " pixels" << std::endl;
    std::cout << "Texture Path:     " << texturePath << std::endl;
    std::cout << "Fade Time:        " << fadeTime << " seconds" << std::endl;
    std::cout << "Fade Rate:        " << fadeRate << " per frame" << std::endl;
    std::cout << "Spawn Frequency:  " << spawnFrequency << " pixels" << std::endl;
    std::cout << "Max Particles:    " << maxParticles << std::endl;
    std::cout << "=================================\n" << std::endl;
}

void Config::SetDefaults()
{
    spriteSize = 15.0f;
    texturePath = "cursortrail.png";
    fadeTime = 1.0f;
    fadeRate = 0.05f;
    spawnFrequency = 6.0f;
    maxParticles = 2048;
}