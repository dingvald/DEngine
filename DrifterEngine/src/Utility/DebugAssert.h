#pragma once

#pragma once

#ifdef DRFT_DEBUG
    #define DEBUG_ASSERT(expr) do { if (!(expr)) { __debugbreak(); } } while(0)
#else
    #define DEBUG_ASSERT(expr) ((void)0)
#endif
