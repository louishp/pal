##
 #######################################################################################################################
 #
 #  Copyright (c) 2020 Advanced Micro Devices, Inc. All Rights Reserved.
 #
 #  Permission is hereby granted, free of charge, to any person obtaining a copy
 #  of this software and associated documentation files (the "Software"), to deal
 #  in the Software without restriction, including without limitation the rights
 #  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 #  copies of the Software, and to permit persons to whom the Software is
 #  furnished to do so, subject to the following conditions:
 #
 #  The above copyright notice and this permission notice shall be included in all
 #  copies or substantial portions of the Software.
 #
 #  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 #  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 #  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 #  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 #  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 #  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 #  SOFTWARE.
 #
 #######################################################################################################################

include(PalVersionHelper)
include(CheckCXXCompilerFlag)

pal_include_guard(PalCompilerWarnings)

# If the current compiler supports the flag, add it.
function(add_flag_if_exists flag cachevarname)
    # GCC emits no diagnostics on warning suppressions
    # So enable the warning for the feature check instead
    string(REGEX REPLACE "^-Wno-" "-W" testflag "${flag}")
    check_cxx_compiler_flag("${testflag}" ${cachevarname})
    if(${cachevarname})
        target_compile_options(pal PRIVATE "${flag}")
    endif()
endfunction()

function(pal_compiler_warnings_gnu_or_clang)
    target_compile_options(pal
    PRIVATE
        # This turns off a lot of warnings related to unused code
        # With PAL's heavy use of ifdefs, code will often go unused in some configurations
        # -Wunused-but-set-parameter
        # -Wunused-but-set-variable
        # -Wunused-function
        # -Wunused-label
        # -Wunused-local-typedefs
        # -Wunused-parameter
        # -Wno-unused-result
        # -Wunused-variable
        # -Wunused-const-variable
        # -Wunused-value
        -Wno-unused

        # Don't warn about unneccessary qualifiers on return types
        -Wno-ignored-qualifiers

        # Don't warn if a structure’s initializer has some fields missing.
        # PAL will often memset or similar in the constructor, and this warning doesn't always handle that
        -Wno-missing-field-initializers

        # Don't warn about malformed comments
        -Wno-comment
        # Make this warning not an error
        -Wno-error=comment

        # Disable warnings about bad/undefined pointer arithmetic
        -Wno-pointer-arith
        # Make this warning not an error
        -Wno-error=pointer-arith

        # Ignore warnings issues about strict ISO C/C++ related to MFC
        # Allow usage of nameless struct/union
        -fms-extensions
    )
    # Don't complain on asserts, we want to keep them
    add_flag_if_exists(-Wno-tautological-compare HAS_WARN_TAUTOLOGICAL)
    # Only has false positives for computing dword sizes
    add_flag_if_exists(-Wno-sizeof-array-div HAS_WARN_SIZEOF_DIV)
    # Don't warn on double parentheses in ifs, this is PAL's coding style
    add_flag_if_exists(-Wno-parentheses-equality HAS_WARN_PARENS)
endfunction()

