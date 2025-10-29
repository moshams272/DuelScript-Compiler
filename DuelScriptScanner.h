#ifndef DUELSCRIPT_SCANNER_H
#define DUELSCRIPT_SCANNER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype> // For isdigit, isalpha

// Enum for all possible token types in DuelScript
enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    
    // DuelScript specific operators
    SUMMON_OP, // <<
    DRAW_OP,   // >>

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    KEYWORD_YUGI, KEYWORD_LORDOFD, KEYWORD_TOONWORLD, KEYWORD_RITUAL,
    KEYWORD_SETFIELD, KEYWORD_KAIBA, KEYWORD_JOEY, KEYWORD_SUMMON,
    KEYWORD_DRAW, KEYWORD_JUDGMENTOFANUBIS, KEYWORD_SOLEMNJUDGMENT,
    KEYWORD_FAIRYBOX, KEYWORD_SWORDSOFREVEALINGLIGHT, KEYWORD_TRIBUTE,
    
    // Data Type Keywords
    KEYWORD_DARKMAGICIAN, KEYWORD_BLUEEYESWHITEDRAGON,
    KEYWORD_REDEYESBLACKDRAGON, KEYWORD_TIMEWIZARD,

    // End of file.
    TOKEN_EOF
};

// Helper function to convert TokenType to a string for printing
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::MINUS: return "MINUS";
        case TokenType::PLUS: return "PLUS";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::SLASH: return "SLASH";
        case TokenType::STAR: return "STAR";
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::SUMMON_OP: return "SUMMON_OP (<<)";
        case TokenType::DRAW_OP: return "DRAW_OP (>>)";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::KEYWORD_YUGI: return "KEYWORD_YUGI";
        case TokenType::KEYWORD_LORDOFD: return "KEYWORD_LORDOFD";
        case TokenType::KEYWORD_TOONWORLD: return "KEYWORD_TOONWORLD";
        case TokenType::KEYWORD_RITUAL: return "KEYWORD_RITUAL";
        case TokenType::KEYWORD_SETFIELD: return "KEYWORD_SETFIELD";
        case TokenType::KEYWORD_KAIBA: return "KEYWORD_KAIBA";
        case TokenType::KEYWORD_JOEY: return "KEYWORD_JOEY";
        case TokenType::KEYWORD_SUMMON: return "KEYWORD_SUMMON";
        case TokenType::KEYWORD_DRAW: return "KEYWORD_DRAW";
        case TokenType::KEYWORD_JUDGMENTOFANUBIS: return "KEYWORD_JUDGMENTOFANUBIS";
        case TokenType::KEYWORD_SOLEMNJUDGMENT: return "KEYWORD_SOLEMNJUDGMENT";
        case TokenType::KEYWORD_FAIRYBOX: return "KEYWORD_FAIRYBOX";
        case TokenType::KEYWORD_SWORDSOFREVEALINGLIGHT: return "KEYWORD_SWORDSOFREVEALINGLIGHT";
        case TokenType::KEYWORD_TRIBUTE: return "KEYWORD_TRIBUTE";
        case TokenType::KEYWORD_DARKMAGICIAN: return "KEYWORD_DARKMAGICIAN";
        case TokenType::KEYWORD_BLUEEYESWHITEDRAGON: return "KEYWORD_BLUEEYESWHITEDRAGON";
        case TokenType::KEYWORD_REDEYESBLACKDRAGON: return "KEYWORD_REDEYESBLACKDRAGON";
        case TokenType::KEYWORD_TIMEWIZARD: return "KEYWORD_TIMEWIZARD";
        case TokenType::TOKEN_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

// Represents a single token
struct Token {
    TokenType type;
    std::string lexeme; // The actual text of the token
    int line;

    Token(TokenType type, std::string lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}

    std::string toString() const {
        return "Line " + std::to_string(line) + ": " + 
               tokenTypeToString(type) + " [" + lexeme + "]";
    }
};

class Scanner {
public:
    Scanner(const std::string& source)
        : source(source), start(0), current(0), line(1) {
        // Populate the keywords map
        keywords["Yugi"] = TokenType::KEYWORD_YUGI;
        keywords["LordOfD"] = TokenType::KEYWORD_LORDOFD;
        keywords["ToonWorld"] = TokenType::KEYWORD_TOONWORLD;
        keywords["Ritual"] = TokenType::KEYWORD_RITUAL;
        // #SetField is handled specially
        keywords["Kaiba"] = TokenType::KEYWORD_KAIBA;
        keywords["Joey"] = TokenType::KEYWORD_JOEY;
        keywords["Summon"] = TokenType::KEYWORD_SUMMON;
        keywords["Draw"] = TokenType::KEYWORD_DRAW;
        keywords["JudgmentOfAnubis"] = TokenType::KEYWORD_JUDGMENTOFANUBIS;
        keywords["SolemnJudgment"] = TokenType::KEYWORD_SOLEMNJUDGMENT;
        keywords["FairyBox"] = TokenType::KEYWORD_FAIRYBOX;
        keywords["SwordsOfRevealingLight"] = TokenType::KEYWORD_SWORDSOFREVEALINGLIGHT;
        keywords["Tribute"] = TokenType::KEYWORD_TRIBUTE;
        
        // Data types
        keywords["DarkMagician"] = TokenType::KEYWORD_DARKMAGICIAN;
        keywords["BlueEyesWhiteDragon"] = TokenType::KEYWORD_BLUEEYESWHITEDRAGON;
        keywords["RedEyesBlackDragon"] = TokenType::KEYWORD_REDEYESBLACKDRAGON;
        keywords["TimeWizard"] = TokenType::KEYWORD_TIMEWIZARD;
    }

    // Main function to scan all tokens
    std::vector<Token> scanTokens() {
        while (!isAtEnd()) {
            // We are at the beginning of the next lexeme.
            start = current;
            scanToken();
        }

        tokens.push_back(Token(TokenType::TOKEN_EOF, "", line));
        return tokens;
    }

private:
    std::string source;
    std::vector<Token> tokens;
    std::map<std::string, TokenType> keywords;
    int start;
    int current;
    int line;

    bool isAtEnd() {
        return current >= source.length();
    }

    // Consumes the current character and returns it
    char advance() {
        return source[current++];
    }

    // Adds a token to the list
    void addToken(TokenType type) {
        std::string text = source.substr(start, current - start);
        tokens.push_back(Token(type, text, line));
    }

    // Looks at the current character without consuming it
    char peek() {
        if (isAtEnd()) return '\0';
        return source[current];
    }

    // Looks at the next character
    char peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    }

    // Checks if a string matches the source at the current position
    // Used for our special comments
    bool peekString(const std::string& str) {
        if (current + str.length() > source.length()) {
            return false;
        }
        return source.substr(current, str.length()) == str;
    }

    // Consumes the current character if it matches the expected one
    bool match(char expected) {
        if (isAtEnd()) return false;
        if (source[current] != expected) return false;
        current++;
        return true;
    }

    // Handles string literals: "..."
    void stringLiteral() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();
        }

        if (isAtEnd()) {
            std::cerr << "Line " << line << ": Error! Unterminated string." << std::endl;
            return;
        }

        advance(); // Consume the closing "
        std::string value = source.substr(start + 1, current - start - 2);
        addToken(TokenType::STRING);
    }

    // Handles number literals
    void numberLiteral() {
        while (isdigit(peek())) advance();

        // Look for a fractional part
        if (peek() == '.' && isdigit(peekNext())) {
            advance(); // Consume the "."
            while (isdigit(peek())) advance();
        }
        addToken(TokenType::NUMBER);
    }

    // Handles identifiers and keywords
    void identifier() {
        while (isalnum(peek()) || peek() == '_') advance();

        std::string text = source.substr(start, current - start);
        TokenType type;
        if (keywords.count(text)) {
            type = keywords[text];
        } else {
            type = TokenType::IDENTIFIER;
        }
        addToken(type);
    }

    // Main token-scanning logic
    void scanToken() {
        // --- Handle Comments First ---
        // These are multi-character tokens, so we peek for them.

        // Check for MillenniumEye:
        if (peekString("MillenniumEye:")) {
            current += 14; // Consume "MillenniumEye:"
            // A comment goes until the end of the line.
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
            return; // Don't add a token, just skip
        }

        // Check for ShadowRealm{
        if (peekString("ShadowRealm{")) {
            current += 12; // Consume "ShadowRealm{"
            while (!peekString("}") && !isAtEnd()) {
                if (peek() == '\n') line++;
                advance();
            }

            if (isAtEnd()) {
                std::cerr << "Line " << line << ": Error! Unterminated ShadowRealm block." << std::endl;
                return;
            }
            
            advance(); // Consume the closing "}"
            return; // Skip the comment block
        }
        
        // Check for #SetField
        if (peekString("#SetField")) {
            current += 9; // Consume "#SetField"
            addToken(TokenType::KEYWORD_SETFIELD);
            return;
        }


        // --- Handle Single/Double Characters ---
        char c = advance();
        switch (c) {
            case '(': addToken(TokenType::LEFT_PAREN); break;
            case ')': addToken(TokenType::RIGHT_PAREN); break;
            case '{': addToken(TokenType::LEFT_BRACE); break;
            case '}': addToken(TokenType::RIGHT_BRACE); break;
            case ',': addToken(TokenType::COMMA); break;
            case '.': addToken(TokenType::DOT); break;
            case '-': addToken(TokenType::MINUS); break;
            case '+': addToken(TokenType::PLUS); break;
            case ';': addToken(TokenType::SEMICOLON); break;
            case '*': addToken(TokenType::STAR); break;
            case '/': addToken(TokenType::SLASH); break; 
            
            // Operators
            case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
            case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
            case '<': addToken(match('<') ? TokenType::SUMMON_OP : (match('=') ? TokenType::LESS_EQUAL : TokenType::LESS)); break;
            case '>': addToken(match('>') ? TokenType::DRAW_OP : (match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER)); break;
            
            // Whitespace
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            case '\n':
                line++;
                break;

            // String literals
            case '"': stringLiteral(); break;

            default:
                if (isdigit(c)) {
                    numberLiteral();
                } else if (isalpha(c) || c == '_') {
                    // Check for our keywords/identifiers
                    identifier();
                } else {
                    std::cerr << "Line " << line << ": Error! Unexpected character '" << c << "'" << std::endl;
                }
                break;
        }
    }
};

#endif // DUELSCRIPT_SCANNER_H
