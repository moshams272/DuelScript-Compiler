#ifndef DUELSCRIPT_ASTNODES_H
#define DUELSCRIPT_ASTNODES_H

#include "DuelScriptScanner.h"
#include <memory>
#include <vector>
#include <any>

// --- Forward Declarations ---
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;
struct VariableExpr;
struct AssignExpr;
struct CallExpr;
struct GetExpr;
struct SetExpr;

// --- ExprVisitor ---
template<typename R>
class ExprVisitor {
public:
    virtual R visitBinaryExpr(const BinaryExpr& expr) = 0;
    virtual R visitGroupingExpr(const GroupingExpr& expr) = 0;
    virtual R visitLiteralExpr(const LiteralExpr& expr) = 0;
    virtual R visitUnaryExpr(const UnaryExpr& expr) = 0;
    virtual R visitVariableExpr(const VariableExpr& expr) = 0;
    virtual R visitAssignExpr(const AssignExpr& expr) = 0;
    virtual R visitCallExpr(const CallExpr& expr) = 0;
    virtual R visitGetExpr(const GetExpr& expr) = 0;
    virtual R visitSetExpr(const SetExpr& expr) = 0;
    virtual ~ExprVisitor() = default;
};

// --- Base Expr ---
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor<std::any>& visitor) const = 0;
};

// --- Forward Declarations (Stmts) ---
struct ExpressionStmt;
struct SummonStmt;
struct DrawStmt;
struct VarDeclStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct FunctionStmt;
struct ReturnStmt;
struct ClassStmt;
struct StructStmt;
struct IncludeStmt;
struct UsingStmt; // <-- إضافة جديدة

// --- StmtVisitor ---
template<typename R>
class StmtVisitor {
public:
    virtual R visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual R visitSummonStmt(const SummonStmt& stmt) = 0;
    virtual R visitDrawStmt(const DrawStmt& stmt) = 0;
    virtual R visitVarDeclStmt(const VarDeclStmt& stmt) = 0;
    virtual R visitBlockStmt(const BlockStmt& stmt) = 0;
    virtual R visitIfStmt(const IfStmt& stmt) = 0;
    virtual R visitWhileStmt(const WhileStmt& stmt) = 0;
    virtual R visitFunctionStmt(const FunctionStmt& stmt) = 0;
    virtual R visitReturnStmt(const ReturnStmt& stmt) = 0;
    virtual R visitClassStmt(const ClassStmt& stmt) = 0;
    virtual R visitStructStmt(const StructStmt& stmt) = 0;
    virtual R visitIncludeStmt(const IncludeStmt& stmt) = 0;
    virtual R visitUsingStmt(const UsingStmt& stmt) = 0; // <-- إضافة جديدة
    virtual ~StmtVisitor() = default;
};

// --- Base Stmt ---
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor<void>& visitor) const = 0;
};

// --- Expression Node Definitions ---

struct BinaryExpr : public Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
            : left(std::move(left)), op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

struct GroupingExpr : public Expr {
    GroupingExpr(std::unique_ptr<Expr> expression)
            : expression(std::move(expression)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }

    std::unique_ptr<Expr> expression;
};

struct LiteralExpr : public Expr {
    LiteralExpr(std::any value) : value(std::move(value)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }

    std::any value;
};

struct UnaryExpr : public Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
            : op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }

    Token op;
    std::unique_ptr<Expr> right;
};

struct VariableExpr : public Expr {
    VariableExpr(Token name) : name(name) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitVariableExpr(*this);
    }

    Token name;
};

struct AssignExpr : public Expr {
    AssignExpr(Token name, std::unique_ptr<Expr> value)
            : name(name), value(std::move(value)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitAssignExpr(*this);
    }

    Token name;
    std::unique_ptr<Expr> value;
};

struct CallExpr : public Expr {
    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
            : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitCallExpr(*this);
    }

    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;
};

struct GetExpr : public Expr {
    GetExpr(std::unique_ptr<Expr> object, Token name)
            : object(std::move(object)), name(name) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitGetExpr(*this);
    }

    std::unique_ptr<Expr> object;
    Token name;
};

struct SetExpr : public Expr {
    SetExpr(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value)
            : object(std::move(object)), name(name), value(std::move(value)) {}

    std::any accept(ExprVisitor<std::any>& visitor) const override {
        return visitor.visitSetExpr(*this);
    }

    std::unique_ptr<Expr> object;
    Token name;
    std::unique_ptr<Expr> value;
};


// --- Statement Node Definitions ---

struct ExpressionStmt : public Stmt {
    ExpressionStmt(std::unique_ptr<Expr> expression)
            : expression(std::move(expression)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitExpressionStmt(*this);
    }

    std::unique_ptr<Expr> expression;
};

struct SummonStmt : public Stmt {
    SummonStmt(std::unique_ptr<Expr> expression)
            : expression(std::move(expression)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitSummonStmt(*this);
    }

    std::unique_ptr<Expr> expression;
};

struct DrawStmt : public Stmt {
    DrawStmt(Token name) : name(name) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitDrawStmt(*this);
    }

    Token name;
};

struct VarDeclStmt : public Stmt {
    VarDeclStmt(Token type, Token name, std::unique_ptr<Expr> initializer)
            : type(type), name(name), initializer(std::move(initializer)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitVarDeclStmt(*this);
    }

    Token type;
    Token name;
    std::unique_ptr<Expr> initializer;
};

struct BlockStmt : public Stmt {
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
            : statements(std::move(statements)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitBlockStmt(*this);
    }

    std::vector<std::unique_ptr<Stmt>> statements;
};

struct IfStmt : public Stmt {
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
            : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitIfStmt(*this);
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
};

struct WhileStmt : public Stmt {
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
            : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitWhileStmt(*this);
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

// --- بنية البارامتر (جديدة) ---
struct FunctionParameter {
    Token type;
    Token name;
};

struct FunctionStmt : public Stmt {
    FunctionStmt(Token name, std::vector<FunctionParameter> params, std::unique_ptr<BlockStmt> body)
            : name(name), params(std::move(params)), body(std::move(body)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitFunctionStmt(*this);
    }

    Token name;
    std::vector<FunctionParameter> params;
    std::unique_ptr<BlockStmt> body;
};

struct ReturnStmt : public Stmt {
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
            : keyword(keyword), value(std::move(value)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitReturnStmt(*this);
    }

    Token keyword;
    std::unique_ptr<Expr> value;
};

struct ClassStmt : public Stmt {
    // الكلاس يحتوي على حقول ودوال
    ClassStmt(Token name,
              std::vector<std::unique_ptr<VarDeclStmt>> fields,
              std::vector<std::unique_ptr<FunctionStmt>> methods)
            : name(name), fields(std::move(fields)), methods(std::move(methods)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitClassStmt(*this);
    }

    Token name;
    std::vector<std::unique_ptr<VarDeclStmt>> fields;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
};

struct StructStmt : public Stmt {
    StructStmt(Token name, std::vector<std::unique_ptr<VarDeclStmt>> fields)
            : name(name), fields(std::move(fields)) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitStructStmt(*this);
    }

    Token name;
    std::vector<std::unique_ptr<VarDeclStmt>> fields;
};

struct IncludeStmt : public Stmt {
    IncludeStmt(Token path) : path(path) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitIncludeStmt(*this);
    }

    Token path;
};

// "using Kaiba Joey;"
struct UsingStmt : public Stmt {
    UsingStmt(Token keyword, Token name) : keyword(keyword), name(name) {}

    void accept(StmtVisitor<void>& visitor) const override {
        visitor.visitUsingStmt(*this);
    }

    Token keyword; // Kaiba
    Token name;    // Joey
};


#endif // DUELSCRIPT_ASTNODES_H

