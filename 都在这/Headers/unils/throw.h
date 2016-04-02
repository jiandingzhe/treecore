

#define TREECORE_THROW(...) \
    tassertfalse;\
    throw __VA_ARGS__
