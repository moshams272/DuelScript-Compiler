#include <iostream>
#include <fstream>
#include <sstream>
#include "DuelScriptScanner.h"
#include "Parser.h"
#include "AstNodes.h"
#include "AstPrinter.h" // <-- إضافة جديدة

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

    if (argc == 2) {
        sourceFile = argv[1];
    }

    std::string sourceCode = readFile(sourceFile);

    // --- 1. مرحلة الـ Scanner ---
    std::cout << "--- 1. Scanning DuelScript file: " << sourceFile << " ---" << std::endl;
    Scanner scanner(sourceCode);
    std::vector<Token> tokens = scanner.scanTokens();
    std::cout << "--- Scanning Complete (" << tokens.size() << " tokens) ---" << std::endl;


    // --- 2. مرحلة الـ Parser ---
    std::cout << "\n--- 2. Parsing Tokens into AST ---" << std::endl;
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    if (parser.hadError) {
        std::cout << "--- Parsing Failed (see errors above) ---" << std::endl;
        return 65; // Exit code for data error
    }

    std::cout << "--- Parsing Complete (AST generated) ---" << std::endl;


    // --- 3. مرحلة طباعة الـ AST (الجديدة) ---
    std::cout << "\n--- 3. Abstract Syntax Tree (AST) ---" << std::endl;
    AstPrinter printer;
    printer.print(statements);


    // --- 4. مرحلة الـ Interpreter (قادماً) ---
    // (هنا سنقوم بتنفيذ الـ "statements" قريباً)

    return 0;
}

