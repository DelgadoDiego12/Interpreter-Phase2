#ifndef EXPRINTER_EXPR_HPP
#define EXPRINTER_EXPR_HPP

#include "SymbolTable.hpp"
#include "Token.hpp"
#include "ValueDescriptor.hpp"
#include <vector>

class ExprNode {
public:
    explicit ExprNode(Token token);
    virtual ~ExprNode() = default;

    [[nodiscard]] const Token &token() const;
    virtual void print() const = 0;
    [[nodiscard]] virtual ValueDescriptor *evaluate(const SymbolTable &symbolTable) const = 0;

private:
    Token expressionToken;
};

class BinaryExprNode final : public ExprNode {
public:
    BinaryExprNode(Token token, ExprNode *left, ExprNode *right);
    ~BinaryExprNode() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *leftOperand;
    ExprNode *rightOperand;
};

class UnaryExprNode final : public ExprNode {
public:
    UnaryExprNode(Token token, ExprNode *operand);
    ~UnaryExprNode() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *expression;
};

class IntegerLiteral final : public ExprNode {
public:
    explicit IntegerLiteral(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};

class IntegerConversion final : public ExprNode {
public:
    explicit IntegerConversion(ExprNode *expression);
    ~IntegerConversion() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *expression;
};

class DoubleLiteral final : public ExprNode {
    public:
        explicit DoubleLiteral(Token token);

        void print() const override;
        [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};

class BooleanLiteral final : public ExprNode {
    public:
        explicit BooleanLiteral(Token token);

        void print() const override;
        [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};

class StringLiteral final : public ExprNode {
    public:
        explicit StringLiteral(Token token);

        void print() const override;
        [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};

class StringConversion final : public ExprNode {
    public:
        explicit StringConversion(ExprNode *expression);
        ~StringConversion() override;

        void print() const override;
        [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

    private:
        ExprNode *expression;
};

class FloatConversion final : public ExprNode {
public:
    explicit FloatConversion(ExprNode *expression);
    ~FloatConversion() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *expression;
};

class LengthConversion final : public ExprNode {
public:
    explicit LengthConversion(ExprNode *expression);
    ~LengthConversion() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
private:
    ExprNode *expression;
};

class SplitConversion final : public ExprNode {
public:
    explicit SplitConversion(ExprNode *expression);
    ~SplitConversion() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *expression;
};

class PopConversion final : public ExprNode {
public:
    explicit PopConversion(ExprNode *expression);
    ~PopConversion() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *expression;
};

class Variable final : public ExprNode {
public:
    explicit Variable(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};


class EvaluatedRange {
public:
    EvaluatedRange(int start, int stop, int step);
    [[nodiscard]] int start() const;
    [[nodiscard]] int stop() const;
    [[nodiscard]] int step() const;
    [[nodiscard]] bool hasIteration() const;
    [[nodiscard]] bool shouldContinue(int nextValue) const;
private:
    int start_;
    int stop_;
    int step_;
};

class RangeExpression {
public:
    explicit RangeExpression(ExprNode* stop); //first form of range
    RangeExpression(ExprNode* start, ExprNode* stop); // second form of range
    RangeExpression(
        ExprNode* start,
        ExprNode* stop,
        ExprNode* step
    ); //third form of range
    ~RangeExpression();
    RangeExpression(const RangeExpression&) = delete;
    RangeExpression& operator=(const RangeExpression&) = delete;
    [[nodiscard]] EvaluatedRange evaluate(
        const SymbolTable& symbolTable
    ) const;
    //void print(std::ostream& output) const; //the actual
    void print() const;
private:
    ExprNode* startExpression;
    ExprNode* stopExpression;
    ExprNode* stepExpression;
};

class ListLiteral final : public ExprNode {
public:
    explicit ListLiteral(std::vector<ExprNode *> elements);
    ~ListLiteral() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
private:
    std::vector<ExprNode *> elements;
};

class SubscriptExpression final : public ExprNode {
public:
    SubscriptExpression(ExprNode *listExpression, ExprNode *indexExpression);
    ~SubscriptExpression() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
private:
    ExprNode *listExpression;
    ExprNode *indexExpression;
};


#endif // EXPRINTER_EXPR_HPP
