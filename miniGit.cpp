//minigit.cpp
//final , merged and  edited!!

#include "MiniGit.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <unordered_map>  

namespace fs = std::filesystem;



MiniGit::MiniGit() {
    // Ensure .minigit and subfolders exist before anything else
    if (!fs::exists(".minigit")) {
        fs::create_directory(".minigit");
    }
    if (!fs::exists(".minigit/objects")) {
        fs::create_directory(".minigit/objects");
    }
    if (!fs::exists(".minigit/branches")) {
        fs::create_directory(".minigit/branches");
    }

    // Load current branch (HEAD) or default to "main"
    if (fs::exists(".minigit/HEAD")) {
        currentBranch = readHEAD();
    } else {
        currentBranch = "main";
    }

    loadBranches(); // Now safe to load branches without crashing
}


// THE init command

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

// THE add command
// Add file to staging area by hashing the contents it have
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
    stagingArea[filename] = hash;            // Stage the file with its hash
    std::cout << "Staged " << filename << "\n";
}

// THE commit command 
// Commit the staged files with a commit message
void MiniGit::commit(const std::string& message) {
    std::string parentHash = readBranch(currentBranch); 

    std::string commitHash = createCommit(message, parentHash); 

    
    for (auto& [filename, hash] : stagingArea) {
        writeBlob(hash, filename);
    }

    writeBranch(currentBranch, commitHash); // Update branch to point to new commit
    writeHEAD(currentBranch);               
    stagingArea.clear();                    
    std::cout << "Committed as " << commitHash << "\n";
}

// THE log command 
// Show commit history by walking back through parents
void MiniGit::log() {
    std::string currentHash = readBranch(currentBranch);

    while (!currentHash.empty()) {
        std::ifstream commitFile(".minigit/objects/" + currentHash);
        std::string line;
        std::cout << "Commit " << currentHash << "\n";

        // Print commit message
        while (std::getline(commitFile, line)) {
            if (line.rfind("message:", 0) == 0) {
                std::cout << "Message: " << line.substr(8) << "\n";
            }
        }

        // Reset stream to find parent commit hash
        commitFile.clear();
        commitFile.seekg(0);
        while (std::getline(commitFile, line)) {
            if (line.rfind("parent:", 0) == 0) {
                currentHash = line.substr(7);
                break;
            }
        }

        if (currentHash.empty()) break;  // End if no more parents
        std::cout << "----\n";
    }
}

// THE  branch command 
// Create a new branch pointing to current commit
void MiniGit::createBranch(const std::string& branchName) {
    std::string headCommit = readBranch(currentBranch);  // Get current commit
    writeBranch(branchName, headCommit);                  // Create branch file
    std::cout << "Created branch '" << branchName << "'\n";
}

// THE checkout command 
// Checkout a branch or specific commit: update files accordingly
void MiniGit::checkout(const std::string& target) {
    std::string commitHash;

    if (branches.count(target)) {       
        currentBranch = target;
        commitHash = readBranch(target);
    } else if (fs::exists(".minigit/objects/" + target)) { 
        commitHash = target;
    } else {
        std::cout << "Invalid branch or commit.\n";
        return;
    }

    // Read commit file to find tracked files and their hashes
    std::ifstream commitFile(".minigit/objects/" + commitHash);
    std::string line;
    std::unordered_map<std::string, std::string> files;

    while (std::getline(commitFile, line)) {
        if (line.rfind("file:", 0) == 0) {
            std::istringstream iss(line.substr(5));
            std::string name, hash;
            iss >> name >> hash;
            files[name] = hash;
        }
    }

    // Overwrite working directory files with contents from blobs
    for (const auto& [filename, hash] : files) {
        std::ifstream src(".minigit/objects/" + hash);
        std::ofstream dest(filename);
        dest << src.rdbuf();
    }

    writeHEAD(currentBranch);  // Update HEAD to new branch
    std::cout << "Checked out to " << target << "\n";
}

// The merge command
// Merge another branch into current branch by creating a merge commit
void MiniGit::merge(const std::string& branch) {
    std::string currentHash = readBranch(currentBranch);
    std::string otherHash = readBranch(branch);

    if (otherHash.empty()) {
        std::cout << "Branch not found.\n";
        return;
    }

    // Create a commit with two parents 
    std::string mergedHash = createCommit("Merged branch " + branch, currentHash + "," + otherHash);
    writeBranch(currentBranch, mergedHash);
    std::cout << "Merged branch " << branch << " into " << currentBranch << "\n";
}

// Helper Functions we used 

// Simple hash function for file content
std::string MiniGit::hashFile(const std::string& content) {
    std::hash<std::string> hasher;
    return std::to_string(hasher(content));
}

// Write file content to .minigit/objects using hash as filename
void MiniGit::writeBlob(const std::string& hash, const std::string& filename) {
    std::ifstream src(filename);
    std::ofstream dest(".minigit/objects/" + hash);
    dest << src.rdbuf();
}

// Read the current branch name stored in HEAD file
std::string MiniGit::readHEAD() {
    std::ifstream headFile(".minigit/HEAD");
    std::string branch;
    std::getline(headFile, branch);
    return branch;
}

// Write current branch name to HEAD file
void MiniGit::writeHEAD(const std::string& branch) {
    std::ofstream headFile(".minigit/HEAD");
    headFile << branch;
}

// Read commit hash that a branch points to
std::string MiniGit::readBranch(const std::string& branch) {
    std::ifstream file(".minigit/branches/" + branch);
    std::string hash;
    std::getline(file, hash);
    return hash;
}

// Write commit hash for a branch
void MiniGit::writeBranch(const std::string& branch, const std::string& commitHash) {
    std::ofstream file(".minigit/branches/" + branch);
    file << commitHash;
    branches[branch] = commitHash;
}

// Load all branch names and their commit hashes into branches map
void MiniGit::loadBranches() {
    branches.clear();
    for (const auto& file : fs::directory_iterator(".minigit/branches")) {
        std::ifstream f(file.path());
        std::string hash;
        std::getline(f, hash);
        branches[file.path().filename().string()] = hash;
    }
}

// Create a commit file with data and tracked files
std::string MiniGit::createCommit(const std::string& message, const std::string& parentHash) {
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


