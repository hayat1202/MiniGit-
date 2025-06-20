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
 
    for (const auto& [filename, hash] : files) { 
        std::ifstream src(".minigit/objects/" + hash); 
        std::ofstream dest(filename); 
        dest << src.rdbuf(); 
    } 
 
    writeHEAD(currentBranch); 
    std::cout << "Checked out to " << target << "\n"; 
} 
 
void MiniGit::merge(const std::string& branch) { 
    std::string currentHash = readBranch(currentBranch); 
    std::string otherHash = readBranch(branch); 
 
    if (otherHash.empty()) { 
        std::cout << "Branch not found.\n"; 
return; 
} 
std::string mergedHash = createCommit("Merged branch " + branch, 
currentHash + "," + otherHash); 
writeBranch(currentBranch, mergedHash); 
std::cout << "Merged branch " << branch << " into " << 
currentBranch << "\n";
