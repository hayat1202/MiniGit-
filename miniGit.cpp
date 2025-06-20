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
