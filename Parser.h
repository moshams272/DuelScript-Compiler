#ifndef DUELSCRIPT_PARSER_H
#define DUELSCRIPT_PARSER_H

#include "DuelScriptScanner.h"
#include "AstNodes.h"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    class ParseError : public std::runtime_error {
    public:
        // --- (الإصلاح) ---
        // 'stda' -> 'std'
        ParseError(const std::string& message) : std::runtime_error(message) {}
    };

    Parser(const std::vector<Token>& tokens);

    std::vector<std::unique_ptr<Stmt>> parse();
    bool hadError = false;

private:
    // --- دوال مساعدة ---
    bool isAtEnd();
    Token peek();
    Token peekNext(); // <-- (الإصلاح) إضافة الدالة
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool checkNext(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    // --- التحقق من النوع ---
    bool isTypeKeyword();

    // --- دوال القواعد (Grammar Rules) ---
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> includeDeclaration();
    std::unique_ptr<Stmt> usingDeclaration();
    std::unique_ptr<Stmt> classDeclaration();
    std::unique_ptr<Stmt> structDeclaration();
    std::unique_ptr<FunctionStmt> ritualDeclaration(std::string kind);
    std::vector<FunctionParameter> parseParameters();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> summonStatement();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<BlockStmt> block();

    // --- التعبيرات (Expressions) ---
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> logicOr();
    std::unique_ptr<Expr> logicAnd();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee);

    // --- الأخطاء ---
    ParseError error(Token token, const std::string& message);
    void synchronize();

    const std::vector<Token>& tokens;
    int current = 0;
};

#endif // DUELSCRIPT_PARSER_H


