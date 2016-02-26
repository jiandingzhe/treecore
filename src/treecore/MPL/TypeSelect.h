

template< bool useFirstType , typename T1 , typename T2 >
struct TypeIf
{
	typedef T1 type;
};

template< typename T1 , typename T2 >
struct TypeIf<false,T1,T2>
{
	typedef T2 type;
};