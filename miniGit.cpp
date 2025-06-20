//MiniGit.cpp
#include "MiniGit.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <unordered_map>  

namespace fs = std::filesystem;
MiniGit::MiniGit() { 
    if (!fs::exists(".minigit")) { 
        fs::create_directory(".minigit"); 
    } 
    if (!fs::exists(".minigit/objects")) { 
        fs::create_directory(".minigit/objects"); 
    } 
    if (!fs::exists(".minigit/branches")) { 
        fs::create_directory(".minigit/branches"); 
    } 
 
    if (fs::exists(".minigit/HEAD")) { 
        currentBranch = readHEAD(); 
    } else { 
        currentBranch = "main"; 
    } 
 
    loadBranches(); 
} 
 
void MiniGit::init() { 
    if (!fs::exists(".minigit")) { 
        fs::create_directory(".minigit"); 
        fs::create_directory(".minigit/objects"); 
        fs::create_directory(".minigit/branches"); 
        writeHEAD("main"); 
        writeBranch("main", ""); 
        std::cout << "Initialized empty MiniGit repository.\n"; 
    } else { 
        std::cout << "MiniGit repository already initialized.\n"; 
    } 
} 
 
std::string MiniGit::readHEAD() { 
    std::ifstream headFile(".minigit/HEAD"); 
    std::string branch; 
    std::getline(headFile, branch); 
    return branch; 
} 
 
void MiniGit::writeHEAD(const std::string& branch) { 
    std::ofstream headFile(".minigit/HEAD"); 
    headFile << branch; 
} 
 
std::string MiniGit::readBranch(const std::string& branch) { 
    std::ifstream file(".minigit/branches/" + branch); 
    std::string hash; 
    std::getline(file, hash); 
    return hash; 
} 
 
void MiniGit::writeBranch(const std::string& branch, const 
std::string& commitHash) { 
    std::ofstream file(".minigit/branches/" + branch); 
    file << commitHash; 
    branches[branch] = commitHash; 
} 
 
void MiniGit::loadBranches() { 
    branches.clear(); 
    for (const auto& file : 
fs::directory_iterator(".minigit/branches")) { 
        std::ifstream f(file.path()); 
        std::string hash; 
        std::getline(f, hash); 
        branches[file.path().filename().string()] = hash; 
    } 
} 
void MiniGit::add(const std::string& filename) { 
    if (!fs::exists(filename)) { 
        std::cout << "File does not exist: " << filename << "\n"; 
        return; 
    } 
 
    std::ifstream file(filename); 
    std::stringstream buffer; 
    buffer << file.rdbuf(); 
    std::string contents = buffer.str(); 
 
    std::string hash = hashFile(contents); 
    stagingArea[filename] = hash; 
    std::cout << "Staged " << filename << "\n"; 
} 
 
void MiniGit::commit(const std::string& message) { 
    std::string parentHash = readBranch(currentBranch); 
    std::string commitHash = createCommit(message, parentHash); 
 
    for (auto& [filename, hash] : stagingArea) { 
        writeBlob(hash, filename); 
    } 
 
    writeBranch(currentBranch, commitHash); 
    writeHEAD(currentBranch); 
    stagingArea.clear(); 
    std::cout << "Committed as " << commitHash << "\n"; 
} 
 
std::string MiniGit::hashFile(const std::string& content) { 
    std::hash<std::string> hasher; 
    return std::to_string(hasher(content)); 
} 
 
void MiniGit::writeBlob(const std::string& hash, const std::string& 
filename) { 
    std::ifstream src(filename); 
    std::ofstream dest(".minigit/objects/" + hash); 
    dest << src.rdbuf(); 
} 
 
std::string MiniGit::createCommit(const std::string& message, const 
std::string& parentHash) { 
    std::ostringstream ss; 
    ss << message << std::time(nullptr); 
 
    for (const auto& [file, hash] : stagingArea) { 
        ss << file << hash; 
    } 
 
    std::string combined = ss.str(); 
    std::string commitHash = hashFile(combined); 
 
    std::ofstream commitFile(".minigit/objects/" + commitHash); 
    commitFile << "parent:" << parentHash << "\n"; 
    commitFile << "message:" << message << "\n"; 
 
    for (const auto& [file, hash] : stagingArea) { 
        commitFile << "file:" << file << " " << hash << "\n"; 
    } 
 
    return commitHash; 
}
