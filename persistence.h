#pragma once

#include "datastore.h"
#include <string>
#include <fstream>
#include <thread>
#include <atomic>
#include <chrono>

/**
 * Handles persistence operations for the database
 * Manages saving data to disk and loading data on startup
 */
class PersistenceManager {
private:
    DataStore& dataStore_;
    std::string filename_;
    std::atomic<bool> shouldStop_;
    std::thread persistenceThread_;

    /**
     * Save data to disk in CSV format
     * @return true if successful, false otherwise
     */
    bool saveToDisk();

    /**
     * Load data from disk
     * @return true if successful, false otherwise
     */
    bool loadFromDisk();

    /**
     * Background thread function for periodic saving
     */
    void persistenceLoop();

public:
    /**
     * Constructor
     * @param dataStore Reference to the data store
     * @param filename Filename for persistence (default: "dump.bdb")
     */
    PersistenceManager(DataStore& dataStore, const std::string& filename = "dump.bdb");

    /**
     * Destructor - stops the persistence thread
     */
    ~PersistenceManager();

    /**
     * Initialize persistence - load data from disk if available
     * @return true if successful, false otherwise
     */
    bool initialize();

    /**
     * Start the background persistence thread
     * @param intervalSeconds Interval between saves in seconds (default: 60)
     */
    void startPersistence(int intervalSeconds = 60);

    /**
     * Stop the background persistence thread
     */
    void stopPersistence();

    /**
     * Force an immediate save to disk
     * @return true if successful, false otherwise
     */
    bool forceSave();
};
