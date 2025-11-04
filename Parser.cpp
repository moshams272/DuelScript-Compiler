#include "Parser.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

//----------------------------------------------------------------//
// الدالة الرئيسية
//----------------------------------------------------------------//
std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (const ParseError& e) {
            std::cerr << e.what() << std::endl;
            hadError = true;
            synchronize(); // محاولة المزامنة
        }
    }
    return statements;
}

//----------------------------------------------------------------//
// قواعد الجمل (Statements)
//----------------------------------------------------------------//

// دالة مساعدة للتحقق من أنواع المتغيرات
bool Parser::isTypeKeyword() {
    return check(TokenType::KEYWORD_DARKMAGICIAN) ||
           check(TokenType::KEYWORD_BLUEEYESWHITEDRAGON) ||
           check(TokenType::KEYWORD_REDEYESBLACKDRAGON) ||
           check(TokenType::KEYWORD_TIMEWIZARD) ||
           check(TokenType::IDENTIFIER); // (للأنواع المخصصة مثل Duelist)
}

std::unique_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::KEYWORD_SETFIELD})) {
        return includeDeclaration();
    }
    if (match({TokenType::KEYWORD_KAIBA})) { // (using Kaiba...)
        return usingDeclaration();
    }
    if (match({TokenType::KEYWORD_LORDOFD})) {
        return classDeclaration();
    }
    if (match({TokenType::KEYWORD_TOONWORLD})) {
        return structDeclaration();
    }
    if (match({TokenType::KEYWORD_RITUAL})) {
        return ritualDeclaration("function");
    }

    // --- (الإصلاح) ---
    // (التحقق إذا كان تعريف متغير)
    if (isTypeKeyword()) {
        // (التحقق إذا كان ما يليه هو اسم، وليس قوسين '()')
        if (checkNext(TokenType::IDENTIFIER)) {
            // (التحقق إذا كان الاسم ليس 'Yugi'، لأن 'Yugi' دائماً دالة)
            if (peekNext().lexeme != "Yugi") {
                return varDeclaration();
            }
            // (التحقق إذا كان 'Yugi()' كاستدعاء دالة)
        } else if (check(TokenType::KEYWORD_YUGI) && checkNext(TokenType::LEFT_PAREN)) {
            // (هذا استدعاء دالة، اتركه لـ statement())
        }
            // (التحقق إذا كان تعريف متغير عادي)
        else if (checkNext(TokenType::IDENTIFIER)) {
            return varDeclaration();
        }
    }


    return statement();
}

std::unique_ptr<Stmt> Parser::includeDeclaration() {
    Token path = consume(TokenType::STRING, "Expect module name (string) after #SetField.");
    consume(TokenType::SEMICOLON, "Expect ';' after #SetField declaration.");
    return std::make_unique<IncludeStmt>(path);
}

std::unique_ptr<Stmt> Parser::usingDeclaration() {
    // "Kaiba" was already consumed
    Token keyword = previous();
    Token name = consume(TokenType::KEYWORD_JOEY, "Expect 'Joey' after 'Kaiba'.");
    consume(TokenType::SEMICOLON, "Expect ';' after 'using' declaration.");
    return std::make_unique<UsingStmt>(keyword, name);
}

std::unique_ptr<Stmt> Parser::classDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

    std::vector<std::unique_ptr<VarDeclStmt>> fields;
    std::vector<std::unique_ptr<FunctionStmt>> methods;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match({TokenType::KEYWORD_RITUAL})) {
            methods.push_back(ritualDeclaration("method"));
        }
        else if (isTypeKeyword()) {
            fields.push_back(std::unique_ptr<VarDeclStmt>(
                    static_cast<VarDeclStmt*>(varDeclaration().release())
            ));
        } else {
            // (السماح بأسطر فارغة أو تعليقات)
            if (peek().type == TokenType::SEMICOLON) {
                advance();
            } else {
                throw error(peek(), "Expect method (Ritual) or field declaration inside class.");
            }
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
    consume(TokenType::SEMICOLON, "Expect ';' after class declaration.");
    return std::make_unique<ClassStmt>(name, std::move(fields), std::move(methods));
}

std::unique_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");

    std::vector<std::unique_ptr<VarDeclStmt>> fields;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        fields.push_back(std::unique_ptr<VarDeclStmt>(
                static_cast<VarDeclStmt*>(varDeclaration().release())
        ));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    consume(TokenType::SEMICOLON, "Expect ';' after struct declaration.");
    return std::make_unique<StructStmt>(name, std::move(fields));
}

std::unique_ptr<FunctionStmt> Parser::ritualDeclaration(std::string kind) {
    Token name; // (هذا سليم الآن بسبب إصلاح Token struct)
    if (match({TokenType::KEYWORD_YUGI})) {
        name = previous();
    } else {
        name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    }

    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");

    std::vector<FunctionParameter> parameters = parseParameters();

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");

    std::unique_ptr<BlockStmt> body = block();
    return std::make_unique<FunctionStmt>(name, std::move(parameters), std::move(body));
}

std::vector<FunctionParameter> Parser::parseParameters() {
    std::vector<FunctionParameter> parameters;

    if (check(TokenType::RIGHT_PAREN)) {
        return parameters; // لا يوجد بارامترات
    }

    do {
        if (parameters.size() >= 255) {
            error(peek(), "Can't have more than 255 parameters.");
        }

        Token type; // (هذا سليم الآن)
        if (isTypeKeyword()) {
            type = advance();
        } else {
            throw error(peek(), "Expect parameter type (e.g., DarkMagician).");
        }

        Token name = consume(TokenType::IDENTIFIER, "Expect parameter name.");

        parameters.push_back({type, name});

    } while (match({TokenType::COMMA}));

    return parameters;
}


std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token type; // (هذا سليم الآن)

    // (التحقق إذا كانت 'declaration()' قد استهلكت النوع بالفعل)
    if (previous().type == TokenType::KEYWORD_DARKMAGICIAN ||
        previous().type == TokenType::KEYWORD_BLUEEYESWHITEDRAGON ||
        previous().type == TokenType::KEYWORD_REDEYESBLACKDRAGON ||
        previous().type == TokenType::KEYWORD_TIMEWIZARD ||
        previous().type == TokenType::IDENTIFIER)
    {
        type = previous();
    }
        // (إذا لم تُستهلك، استهلكها الآن)
    else if(isTypeKeyword()) {
        type = advance();
    } else {
        throw error(peek(), "Expect variable type.");
    }


    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<VarDeclStmt>(type, name, std::move(initializer));
}


std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::KEYWORD_JUDGMENTOFANUBIS})) {
        return ifStatement();
    }
    if (match({TokenType::KEYWORD_SUMMON})) {
        return summonStatement();
    }
    if (match({TokenType::KEYWORD_TRIBUTE})) {
        return returnStatement();
    }
    if (match({TokenType::LEFT_BRACE})) {
        return block();
    }

    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'JudgmentOfAnubis'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    std::unique_ptr<Stmt> thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::KEYWORD_SOLEMNJUDGMENT})) {
        elseBranch = statement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after 'Tribute' value.");
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::unique_ptr<Stmt> Parser::summonStatement() {
    consume(TokenType::SUMMON_OP, "Expect '<<' after 'Summon'.");

    std::unique_ptr<Expr> expr = expression();

    while (match({TokenType::SUMMON_OP})) {
        Token op = previous();
        std::unique_ptr<Expr> right = expression();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    consume(TokenType::SEMICOLON, "Expect ';' after 'Summon' statement.");
    return std::make_unique<SummonStmt>(std::move(expr));
}


std::unique_ptr<BlockStmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

//----------------------------------------------------------------//
// قواعد التعبيرات (Expressions)
//----------------------------------------------------------------//

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = equality();

    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment();

        if (VariableExpr* varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            Token name = varExpr->name;
            return std::make_unique<AssignExpr>(name, std::move(value));
        }
        else if (GetExpr* getExpr = dynamic_cast<GetExpr*>(expr.get())) {
            return std::make_unique<SetExpr>(std::move(getExpr->object), getExpr->name, std::move(value));
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicOr() {
    return logicAnd();
}

std::unique_ptr<Expr> Parser::logicAnd() {
    return equality();
}


std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    return call();
}

std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr));
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_unique<GetExpr>(std::move(expr), name);
        } else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments));
}


std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::KEYWORD_FALSE})) return std::make_unique<LiteralExpr>(false);
    if (match({TokenType::KEYWORD_TRUE})) return std::make_unique<LiteralExpr>(true);

    if (match({TokenType::NUMBER})) {
        return std::make_unique<LiteralExpr>(std::stod(previous().lexeme));
    }

    if (match({TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous().lexeme);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(previous());
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}


//----------------------------------------------------------------//
// دوال مساعدة (Helper Functions)
//----------------------------------------------------------------//

bool Parser::isAtEnd() {
    return peek().type == TokenType::TOKEN_EOF;
}

Token Parser::peek() {
    return tokens[current];
}

// --- (الإصلاح) ---
Token Parser::peekNext() {
    if (current + 1 >= tokens.size()) {
        // (إرجاع آخر توكن وهو EOF)
        return tokens.back();
    }
    return tokens[current + 1];
}


Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::checkNext(TokenType type) {
    if (isAtEnd()) return false;
    // (التأكد أننا لسنا في نهاية الملف تماماً)
    if (tokens[current + 1].type == TokenType::TOKEN_EOF) return false;
    return tokens[current + 1].type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

Parser::ParseError Parser::error(Token token, const std::string& message) {
    std::string errorMessage = "[Line " + std::to_string(token.line) + "] Error";
    if (token.type == TokenType::TOKEN_EOF) {
        errorMessage += " at end";
    } else {
        errorMessage += " at '" + token.lexeme + "'";
    }
    errorMessage += ": " + message;
    return ParseError(errorMessage);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::KEYWORD_RITUAL:
            case TokenType::KEYWORD_LORDOFD:
            case TokenType::KEYWORD_TOONWORLD:
            case TokenType::KEYWORD_JUDGMENTOFANUBIS:
            case TokenType::KEYWORD_FAIRYBOX:
            case TokenType::KEYWORD_TRIBUTE:
            case TokenType::KEYWORD_SETFIELD:
                return;
        }
        advance();
    }
}

