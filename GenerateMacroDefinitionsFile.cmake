# ==========================================================================
# 2015 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
# with no warranties. This code is yours to share, use and modify with no
# strings attached and no restrictions or obligations.
# 
# For more information see g3log/LICENSE or refer refer to http://unlicense.org
# ============================================================================*/

# Prerequisite : Options.cmake should run first
   
   SET(HEADER  "/** ==========================================================================
* 2015 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* 
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/")




MESSAGE("COMPILE_DEFINITIONS:  ${G3_DEFINITIONS}")
MESSAGE("End of COMPILE_DEFINITIONS")
SET(GENERATED_G3_DEFINITIONS src/g3log/generated_definitions.hpp)
file(REMOVE ${GENERATED_G3_DEFINITIONS} )
FILE(WRITE ${GENERATED_G3_DEFINITIONS} "// AUTO GENERATED MACRO DEFINITIONS FOR G3LOG\n\n")
FILE(APPEND ${GENERATED_G3_DEFINITIONS} "${HEADER}\n")
FILE(APPEND ${GENERATED_G3_DEFINITIONS} "#pragma once\n\n")
FILE(APPEND ${GENERATED_G3_DEFINITIONS} "// CMake induced definitions below. See g3log/Options.cmake for details.\n\n")

FOREACH(definition ${G3_DEFINITIONS} )
   FILE(APPEND ${GENERATED_G3_DEFINITIONS} "#define ${definition}\n")
ENDFOREACH(definition)

MESSAGE("Generated ${GENERATED_G3_DEFINITIONS}")
file(READ ${GENERATED_G3_DEFINITIONS} generated_content)

MESSAGE("******************** START *************************")
MESSAGE(${generated_content})
MESSAGE("******************** END *************************")
