#.rst:
# CPackBundle
# -----------
#
# CPack Bundle generator (Mac OS X) specific options
#
# #section Variables specific to CPack Bundle generator #end #module
#
# Installers built on Mac OS X using the Bundle generator use the
# aforementioned DragNDrop (CPACK_DMG_xxx) variables, plus the following
# Bundle-specific parameters (CPACK_BUNDLE_xxx).  #end
#
# #variable
#
# ::
#
#   CPACK_BUNDLE_NAME - The name of the generated bundle. This
#   appears in the OSX finder as the bundle name. Required.
#
# #end
#
# #variable
#
# ::
#
#   CPACK_BUNDLE_PLIST - Path to an OSX plist file that will be used
#   for the generated bundle. This assumes that the caller has generated
#   or specified their own Info.plist file. Required.
#
# #end
#
# #variable
#
# ::
#
#   CPACK_BUNDLE_ICON - Path to an OSX icon file that will be used as
#   the icon for the generated bundle. This is the icon that appears in the
#   OSX finder for the bundle, and in the OSX dock when the bundle is opened.
#   Required.
#
# #end
#
# #variable
#
# ::
#
#   CPACK_BUNDLE_STARTUP_COMMAND - Path to a startup script. This is a path to
#   an executable or script that will be run whenever an end-user double-clicks
#   the generated bundle in the OSX Finder. Optional.
#
# #end

#=============================================================================
# Copyright 2006-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

#Bundle Generator specific code should be put here
