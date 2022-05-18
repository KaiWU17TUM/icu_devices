#ifndef HELPER_H
#define HELPER_H
#include "helper.h"
#include <QDebug>
#include <iostream>

namespace helper
{
    void print_array(const char *data, int length)
    {
        qDebug() << ("\n");

        for (int i = 0; i < length; i++)
        {
            std::cout << std::hex << data[i];

            if ((i + 1) % 16 == 0)
            {
                qDebug() << ("\n");
            }
        }

        qDebug() << ("\n");
    };

}

#endif // HELPER_H
