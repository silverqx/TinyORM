#pragma once
#ifndef ORM_TINY_MACROS_CRTPMODEL_HPP
#define ORM_TINY_MACROS_CRTPMODEL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#define TINY_CRTP_MODEL                                                           \
        /*! Static cast this to a child's instance type (CRTP). */                \
        inline Derived &model();                                                  \
        /*! Static cast this to a child's instance type (CRTP), const version. */ \
        inline const Derived &model() const;

#endif // ORM_TINY_MACROS_CRTPMODEL_HPP
