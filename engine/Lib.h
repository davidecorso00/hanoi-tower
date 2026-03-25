#pragma once

// Configuration for API export/import
#ifdef _WINDOWS
    // Export/import for Visual C++
    #ifdef ENGINE_EXPORTS
        #define ENG_API __declspec(dllexport)  ///< Export symbols when building the DLL
    #else
        #define ENG_API __declspec(dllimport)  ///< Import symbols when using the DLL
    #endif

    /**
    * @brief Disables MSVC warning C4251.
    *
    * Warning C4251 warns about exporting classes that contain STL members.
    * Since this is common and often safe, the warning is disabled to avoid noise.
    */
    #pragma warning(disable : 4251) 

#else
    /**
     * @def ENG_API
     * @brief Empty macro on Linux/macOS since no explicit export/import specifiers are required.
     */
    #define ENG_API
#endif

/**
* @brief Engine rendering mode flags.
*
* These flags can be combined using bitwise operators
* to enable or disable specific rendering features.
*/
#define ENG_RENDER_NORMAL   0x0001