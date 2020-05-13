#if COMPILER
    #include "FileResolver.hpp"
    #define Resolver FileResolver
#else
    #include "VirtualResolver.hpp"
    #define Resolver VirtualResolver
#endif