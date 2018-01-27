/**********************************************************
 * @file   catch_string_conversions.h
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_CATCH_STRING_CONVERSIONS_H
#define NOVELIST_CATCH_STRING_CONVERSIONS_H

#include <iostream>
#include <QtCore/QString>

std::ostream& operator<<(std::ostream& os, QString const& value)
{
    os << value.toStdString();
    return os;
}

#endif //NOVELIST_CATCH_STRING_CONVERSIONS_H
