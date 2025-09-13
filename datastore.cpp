#include "datastore.h"
#include <iostream>

bool DataStore::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        data_[key] = value;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error setting key-value pair: " << e.what() << std::endl;
        return false;
    }
}

std::optional<std::string> DataStore::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = data_.find(key);
    if (it != data_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool DataStore::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = data_.find(key);
    if (it != data_.end()) {
        data_.erase(it);
        return true;
    }
    return false;
}

std::unordered_map<std::string, std::string> DataStore::getAllData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_;
}

void DataStore::loadData(const std::unordered_map<std::string, std::string>& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_ = data;
}

size_t DataStore::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_.size();
}
