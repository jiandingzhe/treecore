//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    BestParam.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/02/27
//! @brief   
//!********************************************************************************

#ifndef ____BESTPARAM__12C4C41E_93A0_45C7_8656_F9D6F9262E4F
#define ____BESTPARAM__12C4C41E_93A0_45C7_8656_F9D6F9262E4F

#include "TypeIf.h"

namespace treecore {

///! ���ģ������"����"�ķ�ʽ���ݲ���:
///! @code
///! template< typename T >
///! void foo( T bar ); //����ô�ֵ? ������?
///! @endcode
///! һЩ�����������ʹ����ñȽ��˷�,��ֵ����,����һЩ
///! ��һ�����,�����޷���������ֻ�ܴ�����.��͵�����
///! ģ�庯���Ĳ���������ѡ��.
///! ʹ��BestParam<T>::type���������"����"�ķ�ʽ����.
///! ʹ�÷�������:
///! @code
///! template< typename T >
///! void foo( BestParam<T>::type bar ); //�Զ�ѡ��ֵ���ͻ�����������
///! @endcode 
///! ��ν"�����"�ķ�ʽ����:
///! + ��������(int,float,double...)->ֵ����
///! + ָ������->ֵ����
///! + ��ƽ������������->����4��8(ȡ����32λ����64λ),const���ô���,����ֵ����.
///! + ���ɿ���������->const���ô���
///! + �ɿ�����������ƽ����->const���ô���
///! + ��������->ʹ��ָ�봫��
///! @warning ��˵һ��,��һ������"�ҷ�Ҫ���޸ĵ����ô�����ô��?" ֱ��д��......
template <typename T>
struct BestParam
{
    typedef typename TypeIf<
        std::is_arithmetic<T>::value ||
        std::is_enum<T>::value ||
        std::is_pointer<T>::value ||
        ( std::is_trivially_copyable<T>::value && sizeof(T)<=sizeof(void*) ) ,
        T
        const T&
    >::type type;
};

#define BEST_PARAM_MACRO(srcT,targetT,...)\
template <typename T ,##__VA_ARGS__ >\
struct BestParam<srcT>\
{\
    typedef targetT type;\
}

BEST_PARAM_MACRO( T& , T& );
BEST_PARAM_MACRO( const T& , const T& );
BEST_PARAM_MACRO( T&& , T&& );
BEST_PARAM_MACRO( T[N] , T* , std::size_t N );
BEST_PARAM_MACRO( const T[N] , const T* , std::size_t N );

����ָ��Ҫ��һ��,ֵ���ݷ�����inline

#undef BEST_PARAM_MACRO

} //namespace treecore

#endif // ____BESTPARAM__12C4C41E_93A0_45C7_8656_F9D6F9262E4F