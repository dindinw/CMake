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
#include "cmDSPMakefile.h"
#include "cmStandardIncludes.h"
#include "cmSystemTools.h"
#include "cmRegularExpression.h"
#include "cmCacheManager.h"

cmDSPMakefile::~cmDSPMakefile()
{
}


cmDSPMakefile::cmDSPMakefile(cmMakefile*mf)
{
  m_Makefile = mf;
}

void cmDSPMakefile::OutputDSPFile()
{ 
  // If not an in source build, then create the output directory
  if(strcmp(m_Makefile->GetStartOutputDirectory(),
            m_Makefile->GetHomeDirectory()) != 0)
    {
    if(!cmSystemTools::MakeDirectory(m_Makefile->GetStartOutputDirectory()))
      {
      cmSystemTools::Error("Error creating directory ",
                           m_Makefile->GetStartOutputDirectory());
      }
    }

  // Setup /I and /LIBPATH options for the resulting DSP file
  std::vector<std::string>& includes = m_Makefile->GetIncludeDirectories();
  std::vector<std::string>::iterator i;
  for(i = includes.begin(); i != includes.end(); ++i)
    {
    m_IncludeOptions +=  "/I \"";
    m_IncludeOptions += *i;
    m_IncludeOptions += "\" ";
    }
  
  // Create the DSP or set of DSP's for libraries and executables
  m_LibraryBuildType = STATIC_LIBRARY;
  if(cmCacheManager::GetInstance()->IsOn("BUILD_SHARED_LIBS"))
    {
    m_LibraryBuildType = DLL;
    }

  // clear project names
  m_CreatedProjectNames.clear();

  // build any targets
  cmTargets &tgts = m_Makefile->GetTargets();
  for(cmTargets::iterator l = tgts.begin(); 
      l != tgts.end(); l++)
    {
    if (l->second.IsALibrary())
      {
      this->SetBuildType(m_LibraryBuildType, l->first.c_str());
      }
    else
      {
      this->SetBuildType(EXECUTABLE,l->first.c_str());
      }
    this->CreateSingleDSP(l->first.c_str(),l->second);
    }
}

void cmDSPMakefile::CreateSingleDSP(const char *lname, cmTarget &target)
{
  std::string fname;
  fname = m_Makefile->GetStartOutputDirectory();
  fname += "/";
  fname += lname;
  fname += ".dsp";
  std::string pname = lname;
  m_CreatedProjectNames.push_back(pname);
  std::ofstream fout(fname.c_str());
  if(!fout)
    {
    cmSystemTools::Error("Error Writing ", fname.c_str());
    }
  this->WriteDSPFile(fout,lname,target);
}

void cmDSPMakefile::WriteDSPBuildRule(std::ostream& fout)
{
  std::string dspname = *(m_CreatedProjectNames.end()-1);
  dspname += ".dsp";
  std::string makefileIn = "\"";
  makefileIn += m_Makefile->GetStartDirectory();
  makefileIn += "/";
  makefileIn += "CMakeLists.txt\"";
  std::string dsprule = "\"";
  dsprule += m_Makefile->GetHomeDirectory();
  dsprule += "/CMake/Source/CMakeSetupCMD\" ";
  dsprule += makefileIn;
  dsprule += " -DSP -H\"";
  dsprule += m_Makefile->GetHomeDirectory();
  dsprule += "\" -S\"";
  dsprule += m_Makefile->GetStartDirectory();
  dsprule += "\" -O\"";
  dsprule += m_Makefile->GetStartOutputDirectory();
  dsprule += "\" -B\"";
  dsprule += m_Makefile->GetHomeOutputDirectory();
  dsprule += "\"";

  std::set<std::string> depends;
  std::set<std::string> outputs;
  outputs.insert(outputs.begin(), dspname);
  fout << "# Begin Source File\n\n";
  fout << "SOURCE=" << makefileIn.c_str() << "\n\n";
  this->WriteCustomRule(fout, dsprule.c_str(), depends, outputs);
  fout << "# End Source File\n";
}


void cmDSPMakefile::AddDSPBuildRule(cmSourceGroup& sourceGroup)
{
  std::string dspname = *(m_CreatedProjectNames.end()-1);
  dspname += ".dsp";
  std::string makefileIn = "\"";
  makefileIn += m_Makefile->GetStartDirectory();
  makefileIn += "/";
  makefileIn += "CMakeLists.txt\"";
  std::string dsprule = "\"";
  dsprule += m_Makefile->GetHomeDirectory();
  dsprule += "/CMake/Source/CMakeSetupCMD\" ";
  dsprule += makefileIn;
  dsprule += " -DSP -H\"";
  dsprule += m_Makefile->GetHomeDirectory();
  dsprule += "\" -S\"";
  dsprule += m_Makefile->GetStartDirectory();
  dsprule += "\" -O\"";
  dsprule += m_Makefile->GetStartOutputDirectory();
  dsprule += "\" -B\"";
  dsprule += m_Makefile->GetHomeOutputDirectory();
  dsprule += "\"";

  std::vector<std::string> depends;
  std::vector<std::string> outputs;
  outputs.push_back(dspname);
  cmCustomCommand cc(makefileIn.c_str(), dsprule.c_str(),
                     depends, outputs);
  sourceGroup.AddCustomCommand(cc);
}


void cmDSPMakefile::WriteDSPFile(std::ostream& fout, 
                                 const char *libName,
                                 cmTarget &target)
{
  // Write the DSP file's header.
  this->WriteDSPHeader(fout, libName, target);
  
  // We may be modifying the source groups temporarily, so make a copy.
  std::vector<cmSourceGroup> sourceGroups = m_Makefile->GetSourceGroups();
  
  // get the classes from the source lists then add them to the groups
  target.GenerateSourceFilesFromSourceLists(*m_Makefile);
  std::vector<cmSourceFile> classes = target.GetSourceFiles();
  for(std::vector<cmSourceFile>::iterator i = classes.begin(); 
      i != classes.end(); i++)
    {
    if(!i->IsAHeaderFileOnly())
      {
      // Add the file to the list of sources.
      std::string source = i->GetFullPath();
      cmSourceGroup& sourceGroup = m_Makefile->FindSourceGroup(source.c_str(),
                                                               sourceGroups);
      sourceGroup.AddSource(source.c_str());
      }
    }
  
  // add any custom rules to the source groups
  for (std::vector<cmCustomCommand>::const_iterator cr = 
         target.GetCustomCommands().begin(); 
       cr != target.GetCustomCommands().end(); ++cr)
    {
    cmSourceGroup& sourceGroup = 
      m_Makefile->FindSourceGroup(cr->GetSourceName().c_str(),
                                  sourceGroups);
    cmCustomCommand cc(*cr);
    cc.ExpandVariables(*m_Makefile);
    sourceGroup.AddCustomCommand(cc);
    }
  
  // Find the group in which the CMakeLists.txt source belongs, and add
  // the rule to generate this DSP file.
  for(std::vector<cmSourceGroup>::reverse_iterator sg = sourceGroups.rbegin();
      sg != sourceGroups.rend(); ++sg)
    {
    if(sg->Matches("CMakeLists.txt"))
      {
      this->AddDSPBuildRule(*sg);
      break;
      }    
    }
  
  // Loop through every source group.
  for(std::vector<cmSourceGroup>::const_iterator sg = sourceGroups.begin();
      sg != sourceGroups.end(); ++sg)
    {
    const std::vector<std::string>& sources = sg->GetSources();
    const cmSourceGroup::CustomCommands& customCommands = sg->GetCustomCommands();
    // If the group is empty, don't write it at all.
    if(sources.empty() && customCommands.empty())
      { continue; }
    
    // If the group has a name, write the header.
    std::string name = sg->GetName();
    if(name != "")
      {
      this->WriteDSPBeginGroup(fout, name.c_str(), "");
      }
    
    // Loop through each source in the source group.
    for(std::vector<std::string>::const_iterator s = sources.begin();
        s != sources.end(); ++s)
      {
      this->WriteDSPBuildRule(fout, s->c_str());
      }    
    
    // Loop through each custom command in the source group.
    for(cmSourceGroup::CustomCommands::const_iterator cc =
          customCommands.begin(); cc != customCommands.end(); ++ cc)
      {
      std::string source = cc->first;
      const cmSourceGroup::Commands& commands = cc->second;

      fout << "# Begin Source File\n\n";
      fout << "SOURCE=" << source << "\n\n";
      
      // Loop through every command generating code from the current source.
      for(cmSourceGroup::Commands::const_iterator c = commands.begin();
          c != commands.end(); ++c)
        {
        std::string command = c->first;
        const cmSourceGroup::CommandFiles& commandFiles = c->second;
        this->WriteCustomRule(fout, command.c_str(), commandFiles.m_Depends,
                              commandFiles.m_Outputs);
        }      
      
      fout << "# End Source File\n";
      }
    
    // If the group has a name, write the footer.
    if(name != "")
      {
      this->WriteDSPEndGroup(fout);
      }
    }  

  // Write the DSP file's footer.
  this->WriteDSPFooter(fout);
}


void cmDSPMakefile::WriteCustomRule(std::ostream& fout,
                                    const char* command,
                                    const std::set<std::string>& depends,
                                    const std::set<std::string>& outputs)
{
  std::vector<std::string>::iterator i;
  for(i = m_Configurations.begin(); i != m_Configurations.end(); ++i)
    {
    if (i == m_Configurations.begin())
      {
      fout << "!IF  \"$(CFG)\" == " << i->c_str() << std::endl;
      }
    else 
      {
      fout << "!ELSEIF  \"$(CFG)\" == " << i->c_str() << std::endl;
      }
    fout << "# Begin Custom Build\n\n";
    
    // Write a rule for every output generated by this command.
    for(std::set<std::string>::const_iterator output = outputs.begin();
        output != outputs.end(); ++output)
      {
      fout << "\"" << output->c_str()
           << "\" :  \"$(SOURCE)\" \"$(INTDIR)\" \"$(OUTDIR)\"";
      // Write out all the dependencies for this rule.
      for(std::set<std::string>::const_iterator d = depends.begin();
          d != depends.end(); ++d)
        {
        fout << " \"" << d->c_str() << "\"";
        }
      fout << "\n  " << command << "\n\n";
      }
    
    fout << "# End Custom Build\n\n";
    }
  
  fout << "!ENDIF\n\n";
}


void cmDSPMakefile::WriteDSPBeginGroup(std::ostream& fout, 
					const char* group,
					const char* filter)
{
  fout << "# Begin Group \"" << group << "\"\n"
    "# PROP Default_Filter \"" << filter << "\"\n";
}


void cmDSPMakefile::WriteDSPEndGroup(std::ostream& fout)
{
  fout << "# End Group\n";
}




void cmDSPMakefile::SetBuildType(BuildType b, const char *libName)
{
  switch(b)
    {
    case STATIC_LIBRARY:
      m_DSPHeaderTemplate = m_Makefile->GetHomeDirectory();
      m_DSPHeaderTemplate += "/CMake/Source/staticLibHeader.dsptemplate";
      m_DSPFooterTemplate = m_Makefile->GetHomeDirectory();
      m_DSPFooterTemplate += "/CMake/Source/staticLibFooter.dsptemplate";
      break;
    case DLL:
      m_DSPHeaderTemplate =  m_Makefile->GetHomeDirectory();
      m_DSPHeaderTemplate += "/CMake/Source/DLLHeader.dsptemplate";
      m_DSPFooterTemplate =  m_Makefile->GetHomeDirectory();
      m_DSPFooterTemplate += "/CMake/Source/DLLFooter.dsptemplate";
      break;
    case EXECUTABLE:
      m_DSPHeaderTemplate = m_Makefile->GetHomeDirectory();
      m_DSPHeaderTemplate += "/CMake/Source/EXEHeader.dsptemplate";
      m_DSPFooterTemplate = m_Makefile->GetHomeDirectory();
      m_DSPFooterTemplate += "/CMake/Source/EXEFooter.dsptemplate";
      break;
    }

  // once the build type is set, determine what configurations are
  // possible
  std::ifstream fin(m_DSPHeaderTemplate.c_str());
  cmRegularExpression reg("# Name ");
  if(!fin)
    {
    cmSystemTools::Error("Error Reading ", m_DSPHeaderTemplate.c_str());
    }

  // reset m_Configurations
  m_Configurations.erase(m_Configurations.begin(), m_Configurations.end());
  // now add all the configurations possible
  char buffer[2048];
  while(fin)
    {
    fin.getline(buffer, 2048);
    std::string line = buffer;
    cmSystemTools::ReplaceString(line, "OUTPUT_LIBNAME",libName);
    if (reg.find(line))
      {
      m_Configurations.push_back(line.substr(reg.end()));
      }
    }
}
  
void cmDSPMakefile::WriteDSPHeader(std::ostream& fout, const char *libName,
                                   const cmTarget &target)
{
  // determine the link directories
  std::string libOptions;
  std::string libDebugOptions;
  std::string libOptimizedOptions;
  std::vector<std::string>::iterator i;
  std::vector<std::string>& libdirs = m_Makefile->GetLinkDirectories();
  for(i = libdirs.begin(); i != libdirs.end(); ++i)
    {
    libOptions += " /LIBPATH:\"";
    libOptions += *i;
    libOptions += "/$(OUTDIR)\" ";
    libOptions += " /LIBPATH:\"";
    libOptions += *i;
    libOptions += "\" ";
    }
  // find link libraries
  cmMakefile::LinkLibraries& libs = m_Makefile->GetLinkLibraries();
  cmMakefile::LinkLibraries::const_iterator j;
  for(j = libs.begin(); j != libs.end(); ++j)
    {
    // add libraries to executables and dlls (but never include
    // a library in a library, bad recursion)
    if (!target.IsALibrary() || 
        (m_LibraryBuildType == DLL && libName != j->first))
      {
      if (j->second == cmMakefile::GENERAL)
        {
        libOptions += " ";
        libOptions += j->first;
        libOptions += ".lib ";
        }
      if (j->second == cmMakefile::DEBUG)
        {
        libDebugOptions += " ";
        libDebugOptions += j->first;
        libDebugOptions += ".lib ";
        }
      if (j->second == cmMakefile::OPTIMIZED)
        {
        libOptimizedOptions += " ";
        libOptimizedOptions += j->first;
        libOptimizedOptions += ".lib ";
        }      
      }
    }
  libOptions += "/STACK:10000000 ";
  
  std::ifstream fin(m_DSPHeaderTemplate.c_str());
  if(!fin)
    {
    cmSystemTools::Error("Error Reading ", m_DSPHeaderTemplate.c_str());
    }
  char buffer[2048];

  while(fin)
    {
      fin.getline(buffer, 2048);
      std::string line = buffer;
      cmSystemTools::ReplaceString(line, "CM_LIBRARIES",
                                   libOptions.c_str());
      cmSystemTools::ReplaceString(line, "CM_DEBUG_LIBRARIES",
                                   libDebugOptions.c_str());
      cmSystemTools::ReplaceString(line, "CM_OPTIMIZED_LIBRARIES",
                                   libOptimizedOptions.c_str());
      cmSystemTools::ReplaceString(line, "BUILD_INCLUDES",
                                   m_IncludeOptions.c_str());
      cmSystemTools::ReplaceString(line, "OUTPUT_LIBNAME",libName);
      cmSystemTools::ReplaceString(line, 
                                   "EXTRA_DEFINES", 
				   m_Makefile->GetDefineFlags());
      fout << line.c_str() << std::endl;
    }
}


void cmDSPMakefile::WriteDSPFooter(std::ostream& fout)
{  
  std::ifstream fin(m_DSPFooterTemplate.c_str());
  if(!fin)
    {
    cmSystemTools::Error("Error Reading ",
                         m_DSPFooterTemplate.c_str());
    }
  char buffer[2048];
  while(fin)
    {
      fin.getline(buffer, 2048);
      fout << buffer << std::endl;
    }
}


void cmDSPMakefile::WriteDSPBuildRule(std::ostream& fout, const char* path)
{
  fout << "# Begin Source File\n\n";
  fout << "SOURCE=" 
       << path << "\n";
  fout << "# End Source File\n";
}

  
