/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef cmUtilitySourceCommand_h
#define cmUtilitySourceCommand_h

#include "cmStandardIncludes.h"
#include "cmCommand.h"

/** \class cmUtilitySourceCommand
 * \brief A command to setup a cache entry with the location of a third-party
 * utility's source.
 *
 * cmUtilitySourceCommand is used when a third-party utility's source is
 * included in the project's source tree.  It specifies the location of
 * the executable's source, and any files that may be needed to confirm the
 * identity of the source.
 */
class cmUtilitySourceCommand : public cmCommand
{
public:
  /**
   * This is a virtual constructor for the command.
   */
  virtual cmCommand* Clone() 
    {
    return new cmUtilitySourceCommand;
    }

  /**
   * This is called when the command is first encountered in
   * the CMakeLists.txt file.
   */
  virtual bool Invoke(std::vector<std::string>& args);

  /**
   * This determines if the command gets propagated down
   * to makefiles located in subdirectories.
   */
  virtual bool IsInherited() { return true;  }

  /**
   * The name of the command as specified in CMakeList.txt.
   */
  virtual const char* GetName() { return "UTILITY_SOURCE";}

  /**
   * Succinct documentation.
   */
  virtual const char* GetTerseDocumentation() 
    {
    return "Specify the source tree of a third-party utility.";
    }
  
  /**
   * More documentation.
   */
  virtual const char* GetFullDocumentation()
    {
    return
      "UTILITY_SOURCE(cache_entry executable_name path_to_source [file1 file2 ...])\n"
      "When a third-party utility's source is included in the distribution,\n"
      "this command specifies its location and name.  The cache entry will\n"
      "not be set unless the path_to_source and all listed files exist.  It\n"
      "is assumed that the source tree of the utility will have been built\n"
      "before it is needed.";
    }
  
  cmTypeMacro(cmUtilitySourceCommand, cmCommand);
};



#endif
