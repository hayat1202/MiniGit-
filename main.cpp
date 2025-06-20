//main.cpp
//final
#include <iostream>
#include <string>
#include "MiniGit.hpp"
using namespace std; 

    // This function prints a list of all the commands the user can run.

void printHelp() {
    cout << "MiniGit Commands:\n"
         << "  init                         Start a new MiniGit repository\n"
         << "  add <filename>               Stage a file for commit\n"
         << "  commit -m <message>          Save staged changes with a message\n"
         << "  log                          Show commit history\n"
         << "  branch <branch-name>         Create a new branch\n"
         << "  checkout <branch/commit>     Switch to a branch or commit\n"
         << "  merge <branch>               Merge another branch into current one\n"
         << "  help                         Show available commands\n"
         << "  exit                         Exit the program\n";
}

int main() {
    MiniGit repo;      
    string userInput;   // Stores whatever command the user types

    cout << "Welcome to MiniGit!\nType 'help' to see available commands.\n";



    while (true) {
        cout << "\n> ";                  // ask the user to input one of the comands 
        getline(cin, userInput);         
        if (userInput == "exit") {
            break;

        // Start a new MiniGit repository
        } else if (userInput == "init") {
            repo.init();

    // Stage a file for the next commit
        } else if (userInput.rfind("add ", 0) == 0) {
            string filename = userInput.substr(4);
            repo.add(filename);

        // Save the current staged files with a message
        } else if (userInput.rfind("commit -m ", 0) == 0) {
            string message = userInput.substr(10); 
            repo.commit(message);

        // Show commit history 
        } else if (userInput == "log") {
            repo.log();

        // Create a new branch from where we are now
        } else if (userInput.rfind("branch ", 0) == 0) {
            string branchName = userInput.substr(7);
            repo.createBranch(branchName);

        // Switch to another branch or a specific commit
        } else if (userInput.rfind("checkout ", 0) == 0) {
            string target = userInput.substr(9);
            repo.checkout(target);

        // Merge another branch into the current one
        } else if (userInput.rfind("merge ", 0) == 0) {
            string branchName = userInput.substr(6);
            repo.merge(branchName);

        // Remind the user of all available commands
        } else if (userInput == "help") {
            printHelp();

// If the command wasn't recognized, let them know to try only from the above   listed options
        } else {
            cout << "  Unknown command. Type 'help' to see available options.\n";
        }
    }

    return 0; 
}

