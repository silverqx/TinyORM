#ifndef UTILS_H
#define UTILS_H

#include <QDebug>

#include "utils_global.hpp"

class UTILS_EXPORT Utils
{
public:
    Utils();

    void xxx() {
        qDebug() << "Hello";
    }
};

#endif // UTILS_H
