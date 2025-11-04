#ifndef DUELSCRIPT_ASTPRINTER_H
#define DUELSCRIPT_ASTPRINTER_H

#include "AstNodes.h"
#include <iostream>
#include <string>

// كلاس لطباعة شجرة الـ AST (للاختبار)
class AstPrinter : public ExprVisitor<std::any>, public StmtVisitor<void> {
public:
    void print(const std::vector<std::unique_ptr<Stmt>>& statements) {
        std::cout << "(Program" << std::endl;
        indent = "  "; // (إضافة مسافة بادئة للبرنامج)
        for (const auto& stmt : statements) {
            if (stmt) {
                stmt->accept(*this);
            }
        }
        indent = "";
        std::cout << ")" << std::endl;
    }

    // --- زيارة الجمل (Statements) ---

    void visitExpressionStmt(const ExpressionStmt& stmt) override {
        // --- (الإصلاح) ---
        // (إضافة std::cout لطباعة الناتج)
        std::cout << indent << parenthesize("ExpressionStmt", std::vector<Expr*>{stmt.expression.get()}) << std::endl;
    }

    void visitSummonStmt(const SummonStmt& stmt) override {
        // --- (الإصلاح) ---
        // (إضافة std::cout لطباعة الناتج)
        std::cout << indent << parenthesize("Summon", std::vector<Expr*>{stmt.expression.get()}) << std::endl;
    }

    void visitDrawStmt(const DrawStmt& stmt) override {
        parenthesize("Draw", std::vector<Token*>{const_cast<Token*>(&stmt.name)});
    }

    void visitVarDeclStmt(const VarDeclStmt& stmt) override {
        std::cout << indent << "(VarDecl " << tokenTypeToString(stmt.type.type) << " " << stmt.name.lexeme;
        if (stmt.initializer) {
            std::cout << " = ";
            std::cout << std::any_cast<std::string>(stmt.initializer->accept(*this));
        }
        std::cout << ")" << std::endl;
    }

    void visitBlockStmt(const BlockStmt& stmt) override {
        std::cout << indent << "(Block" << std::endl;
        std::string oldIndent = indent;
        indent += "  ";
        for (const auto& statement : stmt.statements) {
            statement->accept(*this);
        }
        indent = oldIndent;
        std::cout << indent << ")" << std::endl;
    }

    void visitIfStmt(const IfStmt& stmt) override {
        std::cout << indent << "(If ";
        std::cout << std::any_cast<std::string>(stmt.condition->accept(*this));
        std::cout << std::endl;

        std::string oldIndent = indent;
        indent += "  ";
        std::cout << indent << "(Then" << std::endl;
        indent += "  ";
        stmt.thenBranch->accept(*this);
        indent = oldIndent + "  ";
        std::cout << indent << ")" << std::endl;

        if (stmt.elseBranch) {
            std::cout << indent << "(Else" << std::endl;
            indent += "  ";
            stmt.elseBranch->accept(*this);
            indent = oldIndent + "  ";
            std::cout << indent << ")" << std::endl;
        }
        indent = oldIndent;
        std::cout << indent << ")" << std::endl;
    }

    void visitWhileStmt(const WhileStmt& stmt) override {
        // ... (لم يُنفذ بعد)
    }

    void visitFunctionStmt(const FunctionStmt& stmt) override {
        std::cout << indent << "(Function " << stmt.name.lexeme << " (";
        for (size_t i = 0; i < stmt.params.size(); ++i) {
            std::cout << tokenTypeToString(stmt.params[i].type.type) << " " << stmt.params[i].name.lexeme;
            if (i < stmt.params.size() - 1) std::cout << ", ";
        }
        std::cout << ")" << std::endl;

        std::string oldIndent = indent;
        indent += "  ";
        stmt.body->accept(*this);
        indent = oldIndent;
        std::cout << indent << ")" << std::endl;
    }

    void visitReturnStmt(const ReturnStmt& stmt) override {
        if (stmt.value) {
            // --- (الإصلاح) ---
            // (إضافة std::cout لطباعة الناتج)
            std::cout << indent << parenthesize("Tribute (Return)", std::vector<Expr*>{stmt.value.get()}) << std::endl;
        } else {
            // --- (الإصلاح) ---
            // (إضافة std::cout لطباعة الناتج)
            std::cout << indent << parenthesize("Tribute (Return)", std::vector<Expr*>{}) << std::endl;
        }
    }

    void visitClassStmt(const ClassStmt& stmt) override {
        std::cout << indent << "(Class " << stmt.name.lexeme << std::endl;
        std::string oldIndent = indent;
        indent += "  ";

        if (!stmt.fields.empty()) {
            std::cout << indent << "(Fields" << std::endl;
            indent += "  ";
            for (const auto& field : stmt.fields) {
                field->accept(*this);
            }
            indent = oldIndent + "  ";
            std::cout << indent << ")" << std::endl;
        }

        if (!stmt.methods.empty()) {
            std::cout << indent << "(Methods" << std::endl;
            indent += "  ";
            for (const auto& method : stmt.methods) {
                method->accept(*this);
            }
            indent = oldIndent + "  ";
            std::cout << indent << ")" << std::endl;
        }

        indent = oldIndent;
        std::cout << indent << ")" << std::endl;
    }

    void visitStructStmt(const StructStmt& stmt) override {
        std::cout << indent << "(Struct " << stmt.name.lexeme << std::endl;
        std::string oldIndent = indent;
        indent += "  ";
        for (const auto& field : stmt.fields) {
            field->accept(*this);
        }
        indent = oldIndent;
        std::cout << indent << ")" << std::endl;
    }

    void visitIncludeStmt(const IncludeStmt& stmt) override {
        std::cout << indent << "($SetField \"" << stmt.path.lexeme << "\")" << std::endl;
    }

    void visitUsingStmt(const UsingStmt& stmt) override {
        std::cout << indent << "(Using " << stmt.keyword.lexeme << " " << stmt.name.lexeme << ")" << std::endl;
    }


    // --- زيارة التعبيرات (Expressions) ---

    std::any visitBinaryExpr(const BinaryExpr& expr) override {
        return parenthesize(expr.op.lexeme, std::vector<Expr*>{expr.left.get(), expr.right.get()});
    }

    std::any visitGroupingExpr(const GroupingExpr& expr) override {
        return parenthesize("group", std::vector<Expr*>{expr.expression.get()});
    }

    std::any visitLiteralExpr(const LiteralExpr& expr) override {
        if (expr.value.type() == typeid(std::string)) {
            return "\"" + std::any_cast<std::string>(expr.value) + "\"";
        }
        if (expr.value.type() == typeid(double)) {
            std::string num = std::to_string(std::any_cast<double>(expr.value));
            num.erase(num.find_last_not_of('0') + 1, std::string::npos);
            if(num.back() == '.') num.pop_back();
            return num;
        }
        if (expr.value.type() == typeid(bool)) {
            return std::string(std::any_cast<bool>(expr.value) ? "true" : "false");
        }
        return std::string("nil");
    }

    std::any visitUnaryExpr(const UnaryExpr& expr) override {
        return parenthesize(expr.op.lexeme, std::vector<Expr*>{expr.right.get()});
    }

    std::any visitVariableExpr(const VariableExpr& expr) override {
        return expr.name.lexeme;
    }

    std::any visitAssignExpr(const AssignExpr& expr) override {
        return parenthesize("= " + expr.name.lexeme, std::vector<Expr*>{expr.value.get()});
    }

    std::any visitCallExpr(const CallExpr& expr) override {
        std::vector<Expr*> args;
        for(const auto& arg : expr.arguments) {
            args.push_back(arg.get());
        }
        return parenthesize("call " + std::any_cast<std::string>(expr.callee->accept(*this)), args);
    }

    std::any visitGetExpr(const GetExpr& expr) override {
        // --- (الإصلاح) ---
        // (تحديد نوع الـ vector لإزالة الغموض)
        return parenthesize("." + expr.name.lexeme, std::vector<Expr*>{expr.object.get()});
    }

    std::any visitSetExpr(const SetExpr& expr) override {
        // --- (الإصلاح) ---
        // (تحديد نوع الـ vector لإزالة الغموض)
        return parenthesize("set ." + expr.name.lexeme, std::vector<Expr*>{expr.object.get(), expr.value.get()});
    }

private:
    std::string indent = "";

    std::string parenthesize(std::string name, const std::vector<Expr*>& exprs) {
        std::string result = "(" + name;
        for (const auto& expr : exprs) {
            result += " ";
            result += std::any_cast<std::string>(expr->accept(*this));
        }
        result += ")";
        return result;
    }

    void parenthesize(std::string name, const std::vector<Token*>& tokens) {
        std::cout << indent << "(" << name;
        for (const auto& token : tokens) {
            std::cout << " " << token->lexeme;
        }
        std::cout << ")" << std::endl;
    }
};

#endif // DUELSCRIPT_ASTPRINTER_H

