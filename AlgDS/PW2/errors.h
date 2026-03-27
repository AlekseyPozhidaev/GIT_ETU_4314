#include <exception>
#include <string>

class ShapeException : public std::exception {
protected:
    std::string msg;
public:
    ShapeException(const std::string& m = "Shape error") : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

// Конкретные исключения
class OutOfScreenException : public ShapeException {
public:
    OutOfScreenException(const std::string& details = "")
        : ShapeException("Точка/фигура выходит за пределы экрана" + details) {}
};

class CantBeCreated : public OutOfScreenException {
public:
    CantBeCreated(const std::string& reason = "")
        : OutOfScreenException(" Невозможно создать фигуру: " + reason) {}
};

class CantBeRotated : public ShapeException {
public:
    CantBeRotated(const std::string& reason = "")
        : ShapeException("Невозможно повернуть фигуру: " + reason) {}
};

class CantBeReflected : public ShapeException {
public:
    CantBeReflected(const std::string& reason = "")
        : ShapeException("Невозможно отразить фигуру: " + reason) {}
};

// Дополнительно
class InvalidParameter : public ShapeException {
public:
    InvalidParameter(const std::string& reason = "")
        : ShapeException("Некорректные параметры фигуры: " + reason) {}
};

class CantBeEdited : public ShapeException {
public:
    CantBeEdited(const std::string& reason = "")
        : ShapeException("Невозможно изменить фигуру: " + reason) {}
};