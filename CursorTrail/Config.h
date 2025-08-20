#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Configuration structure for cursor trail customization
struct Config
{
    // Trail appearance
    float spriteSize;           // Size of trail particles (default: 15.0)
    std::string texturePath;    // Path to trail texture (default: "cursortrail.png")
    
    // Trail behavior
    float fadeTime;             // How long particles last (default: 1.0)
    float fadeRate;             // How fast particles fade per frame (default: 0.05)
    float spawnFrequency;       // Interpolation interval - lower = more dense trail (default: 6.0)
    int maxParticles;           // Maximum number of particles (default: 2048)
    
    // Default constructor with sensible defaults
    Config()
        : spriteSize(15.0f)
        , texturePath("cursortrail.png")
        , fadeTime(1.0f)
        , fadeRate(0.05f)
        , spawnFrequency(6.0f)  // SPRITE_SIZE / 2.5
        , maxParticles(2048)
    {
    }
    
    // Load configuration from file
    bool LoadFromFile(const std::string& filename);
    
    // Save configuration to file
    bool SaveToFile(const std::string& filename) const;
    
    // Parse command line arguments
    bool ParseCommandLine(int argc, char* argv[]);
    
    // Print current configuration
    void PrintConfig() const;
    
private:
    void SetDefaults();
};

// Global configuration instance
extern Config g_config;

#endif // CONFIG_H