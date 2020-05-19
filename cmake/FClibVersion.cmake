# This name will be used to install FCLib (library, headers, ...) and
# when another lib or soft will need to search for FCLib.
set(PACKAGE_NAME "FCLib")
# --- Set a version number for the package ---
set(MAJOR_VERSION 3)
set(MINOR_VERSION 1)
set(PATCH_VERSION 0)
set(${PACKAGE_NAME}_version 3.1.0)
set(PACKAGE_VERSION ${${PACKAGE_NAME}_version})
# ============= ABI version =============
### SOVERSION
#
# The ${${PACKAGE_NAME}_version} variable above indicates API
# compatibility, whereas the ${PACKAGE_NAME}_SOVERSION (or SONAME),
# below, indicates ABI compatibility.
#
# To be bumped at each release, by the following rules.  If you are
# not sure, likely API and ABI compatibility have both been
# sacrificed, so simply bump SO_current and set the others to zero.
# If an effort has been made to be backwards compatible on this
# release (e.g. bug fix release), continue with the rules outlined
# below.
#
### RULES for SONAME (borrowed from libtool)
### https://www.gnu.org/software/libtool/manual/html_node/Libtool-versioning.html
#
# If the library source code has changed at all since the last update, then
# increment revision (`c:r:a' becomes `c:r+1:a').
#
# If any interfaces have been added, removed, or changed since the last update,
# increment current, and set revision to 0.
#
# If any interfaces have been added since the last public release, then
# increment age.
#
# If any interfaces have been removed since the last public release, then set
# age to 0.

set(SO_current 0)
set(SO_revision 0)
set(SO_age 0)

# Aggregate variables, to be passed to linker.
# libraries will be named e.g.,
#   libfclib.so -> libfclib.so.5 -> libfclib.so.5.0.0
# Again: this is *not* the software release number!
set(SO_version_info "${SO_current}:${SO_revision}:${SO_age}")
math(EXPR SO_current_minus_age "(${SO_current}) - (${SO_age})")
set(${PACKAGE_NAME}_SOVERSION "${SO_current_minus_age}.${SO_revision}.${SO_age}" CACHE STRING "Fclib SONAME")
set(${PACKAGE_NAME}_SOVERSION_MAJOR "${SO_current_minus_age}" CACHE STRING "fclib SONAME current-minus-age")
