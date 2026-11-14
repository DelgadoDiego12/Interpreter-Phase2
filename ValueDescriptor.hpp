#ifndef VALUE_DESCRIPTOR_HPP
#define VALUE_DESCRIPTOR_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <cstddef>

class ValueDescriptor {
public:
    enum class Type {
        integer,
        doubleValue,
        boolean,
        string,
        list
    };

    virtual ~ValueDescriptor() = default;

    [[nodiscard]] virtual Type type() const = 0;
    [[nodiscard]] virtual std::string typeName() const = 0;
    [[nodiscard]] virtual std::string toString() const = 0;
    [[nodiscard]] virtual ValueDescriptor *clone() const = 0;
};

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

enum class ListElementType {
    unset,
    numeric,
    boolean,
    string
};

class ListObject {
public:
    explicit ListObject(ListElementType initialType = ListElementType::unset);
    ~ListObject();

    ListObject(const ListObject &other) = delete;
    ListObject &operator=(const ListObject &other) = delete;

    void retain();

    [[nodiscard]] bool release();
    [[nodiscard]] std::size_t referenceCount() const;
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] ListElementType elementType() const;
    [[nodiscard]] ValueDescriptor *cloneElementAt(std::size_t index) const;

    void setElement(std::size_t index, ValueDescriptor *value);
    void append(ValueDescriptor *value);

    [[nodiscard]] ValueDescriptor *pop();

private:
    std::size_t references{0};
    ListElementType category{ListElementType::unset};
    std::vector<ValueDescriptor *> elements;
};

class ListValueDescriptor final : public ValueDescriptor {
public:
    explicit ListValueDescriptor(ListObject *object);
    ~ListValueDescriptor() override;

    ListValueDescriptor(const ListValueDescriptor &other) = delete;
    ListValueDescriptor &operator=(const ListValueDescriptor &other) = delete;

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;

    [[nodiscard]] ListObject *object();
    [[nodiscard]] const ListObject *object() const;

private:
    ListObject *listObject;
};

#endif