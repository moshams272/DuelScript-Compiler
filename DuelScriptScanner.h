#ifndef DUELSCRIPT_SCANNER_H
#define DUELSCRIPT_SCANNER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>

enum class TokenType {
    // ... (نفس الـ Tokens) ...
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    SUMMON_OP,
    DRAW_OP,

    IDENTIFIER, STRING, NUMBER,

    KEYWORD_YUGI, KEYWORD_LORDOFD, KEYWORD_TOONWORLD, KEYWORD_RITUAL,
    KEYWORD_SETFIELD, KEYWORD_KAIBA, KEYWORD_JOEY, KEYWORD_SUMMON,
    KEYWORD_DRAW, KEYWORD_JUDGMENTOFANUBIS, KEYWORD_SOLEMNJUDGMENT,
    KEYWORD_FAIRYBOX, KEYWORD_SWORDSOFREVEALINGLIGHT, KEYWORD_TRIBUTE,

    KEYWORD_DARKMAGICIAN, KEYWORD_BLUEEYESWHITEDRAGON,
    KEYWORD_REDEYESBLACKDRAGON, KEYWORD_TIMEWIZARD,

    KEYWORD_TRUE, KEYWORD_FALSE,

    TOKEN_EOF
};

inline std::string tokenTypeToString(TokenType type) {
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
        case TokenType::KEYWORD_TRUE: return "KEYWORD_TRUE";
        case TokenType::KEYWORD_FALSE: return "KEYWORD_FALSE";
        case TokenType::TOKEN_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token() : type(TokenType::TOKEN_EOF), lexeme(""), line(0) {}

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
        keywords["Yugi"] = TokenType::KEYWORD_YUGI;
        keywords["LordOfD"] = TokenType::KEYWORD_LORDOFD;
        keywords["ToonWorld"] = TokenType::KEYWORD_TOONWORLD;
        keywords["Ritual"] = TokenType::KEYWORD_RITUAL;

        keywords["Kaiba"] = TokenType::KEYWORD_KAIBA;
        keywords["Joey"] = TokenType::KEYWORD_JOEY;
        keywords["Summon"] = TokenType::KEYWORD_SUMMON;
        keywords["Draw"] = TokenType::KEYWORD_DRAW;
        keywords["JudgmentOfAnubis"] = TokenType::KEYWORD_JUDGMENTOFANUBIS;
        keywords["SolemnJudgment"] = TokenType::KEYWORD_SOLEMNJUDGMENT;
        keywords["FairyBox"] = TokenType::KEYWORD_FAIRYBOX;
        keywords["SwordsOfRevealingLight"] = TokenType::KEYWORD_SWORDSOFREVEALINGLIGHT;
        keywords["Tribute"] = TokenType::KEYWORD_TRIBUTE;

        keywords["DarkMagician"] = TokenType::KEYWORD_DARKMAGICIAN;
        keywords["BlueEyesWhiteDragon"] = TokenType::KEYWORD_BLUEEYESWHITEDRAGON;
        keywords["RedEyesBlackDragon"] = TokenType::KEYWORD_REDEYESBLACKDRAGON;
        keywords["TimeWizard"] = TokenType::KEYWORD_TIMEWIZARD;

        keywords["true"] = TokenType::KEYWORD_TRUE;
        keywords["false"] = TokenType::KEYWORD_FALSE;
    }


    std::vector<Token> scanTokens() {
        while (!isAtEnd()) {
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

    char advance() {
        return source[current++];
    }

    // --- (الإصلاح) ---
    // إضافة دالة previous() التي كانت مفقودة
    Token previous() {
        return tokens.back();
    }

    void addToken(TokenType type) {
        std::string text = source.substr(start, current - start);
        tokens.push_back(Token(type, text, line));
    }

    char peek() {
        if (isAtEnd()) return '\0';
        return source[current];
    }

    char peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    }


    bool peekString(const std::string& str) {
        if (current + str.length() > source.length()) {
            return false;
        }
        return source.substr(current, str.length()) == str;
    }

    bool match(char expected) {
        if (isAtEnd()) return false;
        if (source[current] != expected) return false;
        current++;
        return true;
    }

    void stringLiteral() {
        // --- (الإصلاح) ---
        // 'advance()' هي التي استهلكت الـ '"'،
        // ونحتاج الدالة 'previous()' الحقيقية (التي تعيد آخر توكن مضاف)
        // ولكن الـ 'advance()' تُرجع الحرف، لذلك 'previous()' (كدالة توكن) ليست صحيحة هنا
        // سنستخدم 'source[current-1]' (الحرف السابق)
        char quoteType = source[current-1]; // (")

        while (peek() != quoteType && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();
        }

        if (isAtEnd()) {
            std::cerr << "Line " << line << ": Error! Unterminated string." << std::endl;
            return;
        }

        advance();

        // إزالة علامات التنصيص
        std::string value = source.substr(start + 1, (current - start) - 2);
        // (استدعاء 'addToken' مع 'value' بدلاً من 'text')
        tokens.push_back(Token(TokenType::STRING, value, line));
    }


    void numberLiteral() {
        while (isdigit(peek())) advance();

        if (peek() == '.' && isdigit(peekNext())) {
            advance();
            while (isdigit(peek())) advance();
        }
        addToken(TokenType::NUMBER);
    }

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


    void scanToken() {

        if (peekString("MillenniumEye:")) {
            current += 14;
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
            return;
        }


        if (peekString("ShadowRealm{")) {
            current += 12;
            while (!peekString("}") && !isAtEnd()) {
                if (peek() == '\n') line++;
                advance();
            }

            if (isAtEnd()) {
                std::cerr << "Line " << line << ": Error! Unterminated ShadowRealm block." << std::endl;
                return;
            }

            advance(); // ابلع '}'
            return;
        }

        if (peekString("#SetField")) {
            current += 9;
            addToken(TokenType::KEYWORD_SETFIELD);
            return;
        }


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

            case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
            case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
            case '<': addToken(match('<') ? TokenType::SUMMON_OP : (match('=') ? TokenType::LESS_EQUAL : TokenType::LESS)); break;
            case '>': addToken(match('>') ? TokenType::DRAW_OP : (match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER)); break;

            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n':
                line++;
                break;


            case '"': stringLiteral(); break;

            default:
                if (isdigit(c)) {
                    numberLiteral();
                } else if (isalpha(c) || c == '_') {

                    identifier();
                } else {
                    std::cerr << "Line " << line << ": Error! Unexpected character '" << c << "'" << std::endl;
                }
                break;
        }
    }
};

#endif

