#include <iostream>
#include <fstream>   // For file reading
#include <sstream>   // For reading file into string
#include "DuelScriptScanner.h"

// Helper function to read a file into a string
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(74); // Exit code for I/O error
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    std::string sourceFile = "test.duelscript";
    
    // You could also allow passing a file as an argument
    if (argc == 2) {
        sourceFile = argv[1];
    }
    
    std::cout << "--- Scanning DuelScript file: " << sourceFile << " ---" << std::endl;
    
    std::string sourceCode = readFile(sourceFile);
    
    Scanner scanner(sourceCode);
    std::vector<Token> tokens = scanner.scanTokens();

    // Print all the tokens
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }

    std::cout << "--- Scanning Complete ---" << std::endl;
    return 0;
}
