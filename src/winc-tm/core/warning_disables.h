#pragma once

#ifdef _MSC_VER

#pragma warning(disable : 4505) /* unreferenced local function has been removed */
#pragma warning(disable : 4514) /* unreferenced inline function has been removed */
#pragma warning(disable : 4625) /* copy constructor was implicitly defined as deleted */
#pragma warning(disable : 4571) /* Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught */
#pragma warning(disable : 4668) /* 'xxx' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif' */
#pragma warning(disable : 4710) /* function not inlined */
#pragma warning(disable : 4711) /* function 'xxx' selected for automatic inline expansion */
#pragma warning(disable : 4820) /* 'n' bytes padding added after data member 'xxx' */
#pragma warning(disable : 4996) /* This function or variable may be unsafe. */
#pragma warning(disable : 5039) /* pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception. */ 
#pragma warning(disable : 5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */

#endif