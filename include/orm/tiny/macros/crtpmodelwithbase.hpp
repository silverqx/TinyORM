#pragma once
#ifndef ORM_TINY_MACROS_CRTPMODELWITHBASE_HPP
#define ORM_TINY_MACROS_CRTPMODELWITHBASE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#define TINY_CRTP_MODEL_WITH_BASE                                                 \
        /*! Static cast this to a child's instance type (CRTP). */                \
        inline Derived &model();                                                  \
        /*! Static cast this to a child's instance type (CRTP), const version. */ \
        inline const Derived &model() const;                                      \
        /*! Static cast this to a child's instance Model type. */                 \
        inline Model<Derived, AllRelations...> &basemodel();                      \
        /*! Static cast this to a child's instance Model type, const version. */  \
        inline const Model<Derived, AllRelations...> &basemodel() const;

#endif // ORM_TINY_MACROS_CRTPMODELWITHBASE_HPP
