//miniGit.hpp
#ifndef MINIGIT_HPP
#define MINIGIT_HPP

#include <string>
#include <unordered_map>
std::string readHEAD(); 
void writeHEAD(const std::string& branch); 
std::string readBranch(const std::string& branch); 
void writeBranch(const std::string& branch, const std::string& 
commitHash); 
void loadBranches(); 
std::unordered_map<std::string, std::string> stagingArea; 
std::string hashFile(const std::string& content); 
void writeBlob(const std::string& hash, const std::string& 
filename); 
std::string createCommit(const std::string& message, const 
std::string& parentHash);
