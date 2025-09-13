#pragma once

#include <unordered_map>
#include <mutex>
#include <string>
#include <optional>

/**
 * Thread-safe in-memory key-value data store
 * Uses std::unordered_map with mutex protection for concurrent access
 */
class DataStore {
private:
    std::unordered_map<std::string, std::string> data_;
    mutable std::mutex mutex_;

public:
    /**
     * Store a key-value pair
     * @param key The key to store
     * @param value The value to associate with the key
     * @return true if successful
     */
    bool set(const std::string& key, const std::string& value);

    /**
     * Retrieve a value by key
     * @param key The key to look up
     * @return Optional containing the value if found, empty if not found
     */
    std::optional<std::string> get(const std::string& key) const;

    /**
     * Delete a key-value pair
     * @param key The key to delete
     * @return true if key was deleted, false if key didn't exist
     */
    bool del(const std::string& key);

    /**
     * Get all key-value pairs (for persistence)
     * @return Copy of the entire data map
     */
    std::unordered_map<std::string, std::string> getAllData() const;

    /**
     * Load data from a map (for persistence)
     * @param data The data to load
     */
    void loadData(const std::unordered_map<std::string, std::string>& data);

    /**
     * Get the number of stored key-value pairs
     * @return Number of entries
     */
    size_t size() const;
};
