# Prerequisite : Options.cmake should run first
   
   SET(HEADER  "/** ==========================================================================
 * Original code made by Robert Engeln. Given as a PUBLIC DOMAIN dedication for
 * the benefit of g3log.  It was originally published at:
 * http://code-freeze.blogspot.com/2012/01/generating-stack-traces-from-c.html

 * 2014-2015: adapted for g3log by Kjell Hedstrom (KjellKod).
 *
 * This is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 *
 * For more information see g3log/LICENSE or refer refer to http://unlicense.org
 * ============================================================================*/")




MESSAGE("COMPILE_DEFINITIONS:  ${G3_DEFINITIONS}")
MESSAGE("End of COMPILE_DEFINITIONS")
FILE(WRITE g3log/definitions.hpp "// AUTO GENERATED MACRO DEFINITIONS FOR G3LOG\n\n")
FILE(APPEND g3log/definitions.hpp ${HEADER}"\n")
FILE(APPEND g3log/definitions.hpp "#pragma once\n\n")

FOREACH(definition ${G3_DEFINITIONS} )
   FILE(APPEND g3log/definitions.hpp "#define ${definition}\n")
ENDFOREACH(definition)
