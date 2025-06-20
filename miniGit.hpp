//miniGit.hpp
//structured and reorganaised code final

#ifndef MINIGIT_HPP
#define MINIGIT_HPP

#include <string>
#include <unordered_map>
// This class handles the basic version control commands like commits, branches, and merges.

class MiniGit {
public:
    MiniGit();

    // These are the main commands we will actually use:
    void init();                      
    void add(const std::string& filename);  
    void commit(const std::string& message); 
    void log();                       

    // Branching and merging let us work on different versions at the same time at once:
    void createBranch(const std::string& branchName); 
    void checkout(const std::string& name);           
    void merge(const std::string& branch);            

private:
    // the things the  MiniGit keeps track of internally:
    std::unordered_map<std::string, std::string> stagingArea; 
    std::unordered_map<std::string, std::string> branches;    
    std::string currentBranch;                                

    // Helper functions for the behind-the-scenes stuff:
    std::string hashFile(const std::string& content);              
    void writeBlob(const std::string& hash, const std::string& filename); 
    std::string readHEAD();   

    void writeHEAD(const std::string& branch);// Set the current branch in HEAD
    std::string readBranch(const std::string& branch);  

    void writeBranch(const std::string& branch, const std::string& commitHash); // Update branch with new commit
    std::string createCommit(const std::string& message, const std::string& parentHash); // Make a new commit
    
    void loadBranches();                                           
};

#endif 



