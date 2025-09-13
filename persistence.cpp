#include "persistence.h"
#include <iostream>
#include <sstream>

PersistenceManager::PersistenceManager(DataStore& dataStore, const std::string& filename)
    : dataStore_(dataStore), filename_(filename), shouldStop_(false) {
}

PersistenceManager::~PersistenceManager() {
    stopPersistence();
}

bool PersistenceManager::initialize() {
    std::cout << "Initializing persistence manager..." << std::endl;
    return loadFromDisk();
}

bool PersistenceManager::saveToDisk() {
    try {
        std::ofstream file(filename_);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename_ << std::endl;
            return false;
        }

        auto data = dataStore_.getAllData();
        for (const auto& pair : data) {
            // Escape commas and newlines in the data
            std::string escapedKey = pair.first;
            std::string escapedValue = pair.second;
            
            // Simple escaping: replace commas with \c and newlines with \n
            size_t pos = 0;
            while ((pos = escapedKey.find(",", pos)) != std::string::npos) {
                escapedKey.replace(pos, 1, "\\c");
                pos += 2;
            }
            pos = 0;
            while ((pos = escapedKey.find("\n", pos)) != std::string::npos) {
                escapedKey.replace(pos, 1, "\\n");
                pos += 2;
            }
            
            pos = 0;
            while ((pos = escapedValue.find(",", pos)) != std::string::npos) {
                escapedValue.replace(pos, 1, "\\c");
                pos += 2;
            }
            pos = 0;
            while ((pos = escapedValue.find("\n", pos)) != std::string::npos) {
                escapedValue.replace(pos, 1, "\\n");
                pos += 2;
            }
            
            file << escapedKey << "," << escapedValue << "\n";
        }

        file.close();
        std::cout << "Data saved to " << filename_ << " (" << data.size() << " entries)" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving to disk: " << e.what() << std::endl;
        return false;
    }
}

bool PersistenceManager::loadFromDisk() {
    try {
        std::ifstream file(filename_);
        if (!file.is_open()) {
            std::cout << "No existing data file found: " << filename_ << std::endl;
            return true; // Not an error if file doesn't exist
        }

        std::unordered_map<std::string, std::string> loadedData;
        std::string line;
        int loadedCount = 0;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            size_t commaPos = line.find(',');
            if (commaPos == std::string::npos) {
                std::cerr << "Invalid line format: " << line << std::endl;
                continue;
            }

            std::string key = line.substr(0, commaPos);
            std::string value = line.substr(commaPos + 1);

            // Unescape the data
            size_t pos = 0;
            while ((pos = key.find("\\c", pos)) != std::string::npos) {
                key.replace(pos, 2, ",");
                pos += 1;
            }
            pos = 0;
            while ((pos = key.find("\\n", pos)) != std::string::npos) {
                key.replace(pos, 2, "\n");
                pos += 1;
            }
            
            pos = 0;
            while ((pos = value.find("\\c", pos)) != std::string::npos) {
                value.replace(pos, 2, ",");
                pos += 1;
            }
            pos = 0;
            while ((pos = value.find("\\n", pos)) != std::string::npos) {
                value.replace(pos, 2, "\n");
                pos += 1;
            }

            loadedData[key] = value;
            loadedCount++;
        }

        file.close();
        dataStore_.loadData(loadedData);
        std::cout << "Loaded " << loadedCount << " entries from " << filename_ << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading from disk: " << e.what() << std::endl;
        return false;
    }
}

void PersistenceManager::persistenceLoop() {
    while (!shouldStop_) {
        std::this_thread::sleep_for(std::chrono::seconds(60));
        if (!shouldStop_) {
            saveToDisk();
        }
    }
}

void PersistenceManager::startPersistence(int intervalSeconds) {
    if (persistenceThread_.joinable()) {
        std::cerr << "Persistence thread already running" << std::endl;
        return;
    }

    shouldStop_ = false;
    persistenceThread_ = std::thread([this, intervalSeconds]() {
        while (!shouldStop_) {
            std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
            if (!shouldStop_) {
                saveToDisk();
            }
        }
    });
    
    std::cout << "Persistence thread started (interval: " << intervalSeconds << "s)" << std::endl;
}

void PersistenceManager::stopPersistence() {
    if (persistenceThread_.joinable()) {
        shouldStop_ = true;
        persistenceThread_.join();
        std::cout << "Persistence thread stopped" << std::endl;
    }
}

bool PersistenceManager::forceSave() {
    return saveToDisk();
}
