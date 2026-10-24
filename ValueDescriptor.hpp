#ifndef VALUE_DESCRIPTOR_HPP
#define VALUE_DESCRIPTOR_HPP

#include <cstdint>
#include <string>

class ValueDescriptor {
public:
    enum class Type {
        integer,
        doubleValue,
        boolean,
        string
    };

    virtual ~ValueDescriptor() = default;

    [[nodiscard]] virtual Type type() const = 0;
    [[nodiscard]] virtual std::string typeName() const = 0;
    [[nodiscard]] virtual std::string toString() const = 0;
    [[nodiscard]] virtual ValueDescriptor *clone() const = 0;
};

#endif

class IntegerValueDescriptor final : public ValueDescriptor {
public:
    explicit IntegerValueDescriptor(std::int64_t value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] std::int64_t value() const;

private:
    std::int64_t integerValue;
};

class DoubleValueDescriptor final : public ValueDescriptor {
public:
    explicit DoubleValueDescriptor(double value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] double value() const;

private:
    double doubleValue;
};

class BooleanValueDescriptor final : public ValueDescriptor {
public:
    explicit BooleanValueDescriptor(bool value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] bool value() const;

private:
    bool booleanValue;
};

class StringValueDescriptor final : public ValueDescriptor {
public:
    explicit StringValueDescriptor(std::string value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] const std::string &value() const;

private:
    std::string stringValue;
};