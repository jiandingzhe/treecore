//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    strong_typedef.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************

#ifndef ____STRONG_TYPEDEF__6BBD0942_2250_4D00_A927_CAC665D561DB
#define ____STRONG_TYPEDEF__6BBD0942_2250_4D00_A927_CAC665D561DB

#define TREECORE_STRONG_TYPEDEF(tName,newName) \
    struct newName \
    { \
        tName t; \
        explicit newName(const tName& t_) : t(t_) {}; \
        newName() : t(){}; \
        newName(const newName& t_) : t(t_.t){}; \
        newName & operator=(const newName & rhs) { t = rhs.t; return *this;}; \
        newName & operator=(const tName& rhs) { t = rhs; return *this;}; \
        operator const tName&() const {return t; }; \
        operator tName&() { return t; }; \
        bool operator==(const newName & rhs) const { return t == rhs.t; }; \
        bool operator<(const newName & rhs) const { return t < rhs.t; }; \
        tName& get() { return t; };\
        const tName& get() const { return t; };\
    }

#endif // ____STRONG_TYPEDEF__6BBD0942_2250_4D00_A927_CAC665D561DB