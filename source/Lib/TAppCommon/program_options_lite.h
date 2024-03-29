/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.  
 *
* Copyright (c) 2010-2014, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include  "../TLibCommon/TypeDef.h"

#if H_MV
#include <vector>
#include <errno.h>
#include <cstring>

#ifdef WIN32
#define strdup _strdup
#endif
#endif
//! \ingroup TAppCommon
//! \{


namespace df
{
  namespace program_options_lite
  {
    struct Options;
    
    struct ParseFailure : public std::exception
    {
      ParseFailure(std::string arg0, std::string val0) throw()
      : arg(arg0), val(val0)
      {}

      ~ParseFailure() throw() {};

      std::string arg;
      std::string val;

      const char* what() const throw() { return "Option Parse Failure"; }
    };

    void doHelp(std::ostream& out, Options& opts, unsigned columns = 80);
    unsigned parseGNU(Options& opts, unsigned argc, const char* argv[]);
    unsigned parseSHORT(Options& opts, unsigned argc, const char* argv[]);
    std::list<const char*> scanArgv(Options& opts, unsigned argc, const char* argv[]);
    void scanLine(Options& opts, std::string& line);
    void scanFile(Options& opts, std::istream& in);
    void setDefaults(Options& opts);
    void parseConfigFile(Options& opts, const std::string& filename);
    bool storePair(Options& opts, const std::string& name, const std::string& value);
    
    /** OptionBase: Virtual base class for storing information relating to a
     * specific option This base class describes common elements.  Type specific
     * information should be stored in a derived class. */
    struct OptionBase
    {
#if H_MV      
      OptionBase(const std::string& name, const std::string& desc, bool duplicate = false)
        : opt_string(name), opt_desc(desc), opt_duplicate(duplicate)
#else
      OptionBase(const std::string& name, const std::string& desc)
      : opt_string(name), opt_desc(desc)
#endif
      {};
      
      virtual ~OptionBase() {}
      
      /* parse argument arg, to obtain a value for the option */
      virtual void parse(const std::string& arg) = 0;
      /* set the argument to the default value */
      virtual void setDefault() = 0;
      
      std::string opt_string;
      std::string opt_desc;
#if H_MV
      bool        opt_duplicate; 
#endif
    };
    
    /** Type specific option storage */
    template<typename T>
    struct Option : public OptionBase
    {
#if H_MV
      Option(const std::string& name, T& storage, T default_val, const std::string& desc, bool duplicate = false)
        : OptionBase(name, desc, duplicate), opt_storage(storage), opt_default_val(default_val)
#else
      Option(const std::string& name, T& storage, T default_val, const std::string& desc)
      : OptionBase(name, desc), opt_storage(storage), opt_default_val(default_val)
#endif
      {}
      
      void parse(const std::string& arg);
      
      void setDefault()
      {
        opt_storage = opt_default_val;
      }
      
      T& opt_storage;
      T opt_default_val;
    };
    
    /* Generic parsing */
    template<typename T>
    inline void
    Option<T>::parse(const std::string& arg)
    {
      std::istringstream arg_ss (arg,std::istringstream::in);
      arg_ss.exceptions(std::ios::failbit);
      try
      {
        arg_ss >> opt_storage;
      }
      catch (...)
      {
        throw ParseFailure(opt_string, arg);
      }
    }
    
    /* string parsing is specialized -- copy the whole string, not just the
     * first word */
    template<>
    inline void
    Option<std::string>::parse(const std::string& arg)
    {
      opt_storage = arg;
    }

#if H_MV    
    template<>
    inline void
      Option<char*>::parse(const std::string& arg)
    {
      opt_storage = arg.empty() ? NULL : strdup(arg.c_str()) ;
    }

    template<>
    inline void
      Option< std::vector<char*> >::parse(const std::string& arg)
    {
      opt_storage.clear(); 

      char* pcStart = (char*) arg.data();      
      char* pcEnd = strtok (pcStart," ");

      while (pcEnd != NULL)
      {
        size_t uiStringLength = pcEnd - pcStart;
        char* pcNewStr = (char*) malloc( uiStringLength + 1 );
        strncpy( pcNewStr, pcStart, uiStringLength); 
        pcNewStr[uiStringLength] = '\0'; 
        pcStart = pcEnd+1; 
        pcEnd = strtok (NULL, " ,.-");
        opt_storage.push_back( pcNewStr ); 
      }      
    }


    template<>    
    inline void
      Option< std::vector<double> >::parse(const std::string& arg)
    {
      char* pcNextStart = (char*) arg.data();
      char* pcEnd = pcNextStart + arg.length();

      char* pcOldStart = 0; 

      size_t iIdx = 0; 

      while (pcNextStart < pcEnd)
      {
        errno = 0; 

        if ( iIdx < opt_storage.size() )
        {
          opt_storage[iIdx] = strtod(pcNextStart, &pcNextStart);
        }
        else
        {
          opt_storage.push_back( strtod(pcNextStart, &pcNextStart)) ;
        }
        iIdx++; 

        if ( errno == ERANGE || (pcNextStart == pcOldStart) )
        {
          std::cerr << "Error Parsing Doubles: `" << arg << "'" << std::endl;
          exit(EXIT_FAILURE);    
        };   
        while( (pcNextStart < pcEnd) && ( *pcNextStart == ' ' || *pcNextStart == '\t' || *pcNextStart == '\r' ) ) pcNextStart++;  
        pcOldStart = pcNextStart; 

      }
    }

    template<>
    inline void
      Option< std::vector<int> >::parse(const std::string& arg)
    {
      opt_storage.clear();


      char* pcNextStart = (char*) arg.data();
      char* pcEnd = pcNextStart + arg.length();

      char* pcOldStart = 0; 

      size_t iIdx = 0; 


      while (pcNextStart < pcEnd)
      {

        if ( iIdx < opt_storage.size() )
        {
          opt_storage[iIdx] = (int) strtol(pcNextStart, &pcNextStart,10);
        }
        else
        {
          opt_storage.push_back( (int) strtol(pcNextStart, &pcNextStart,10)) ;
        }
        iIdx++; 
        if ( errno == ERANGE || (pcNextStart == pcOldStart) )
        {
          std::cerr << "Error Parsing Integers: `" << arg << "'" << std::endl;
          exit(EXIT_FAILURE);
        };   
        while( (pcNextStart < pcEnd) && ( *pcNextStart == ' ' || *pcNextStart == '\t' || *pcNextStart == '\r' ) ) pcNextStart++;  
        pcOldStart = pcNextStart;
      }
    }


    template<>
    inline void
      Option< std::vector<bool> >::parse(const std::string& arg)
    {
      char* pcNextStart = (char*) arg.data();
      char* pcEnd = pcNextStart + arg.length();

      char* pcOldStart = 0; 

      size_t iIdx = 0; 

      while (pcNextStart < pcEnd)
      {
        if ( iIdx < opt_storage.size() )
        {
          opt_storage[iIdx] = (strtol(pcNextStart, &pcNextStart,10) != 0);
        }
        else
        {
          opt_storage.push_back(strtol(pcNextStart, &pcNextStart,10) != 0) ;
        }
        iIdx++; 

        if ( errno == ERANGE || (pcNextStart == pcOldStart) )
        {
          std::cerr << "Error Parsing Bools: `" << arg << "'" << std::endl;
          exit(EXIT_FAILURE);
        };   
        while( (pcNextStart < pcEnd) && ( *pcNextStart == ' ' || *pcNextStart == '\t' || *pcNextStart == '\r' ) ) pcNextStart++;  
        pcOldStart = pcNextStart;
      }
    }
#endif
    /** Option class for argument handling using a user provided function */
    struct OptionFunc : public OptionBase
    {
      typedef void (Func)(Options&, const std::string&);
      
      OptionFunc(const std::string& name, Options& parent_, Func *func_, const std::string& desc)
      : OptionBase(name, desc), parent(parent_), func(func_)
      {}
      
      void parse(const std::string& arg)
      {
        func(parent, arg);
      }
      
      void setDefault()
      {
        return;
      }
      
    private:
      Options& parent;
      void (*func)(Options&, const std::string&);
    };
    
    class OptionSpecific;
    struct Options
    {
      ~Options();
      
      OptionSpecific addOptions();
      
      struct Names
      {
        Names() : opt(0) {};
        ~Names()
        {
          if (opt)
            delete opt;
        }
        std::list<std::string> opt_long;
        std::list<std::string> opt_short;
        OptionBase* opt;
      };

      void addOption(OptionBase *opt);
      
      typedef std::list<Names*> NamesPtrList;
      NamesPtrList opt_list;
      
      typedef std::map<std::string, NamesPtrList> NamesMap;
      NamesMap opt_long_map;
      NamesMap opt_short_map;
    };
    
    /* Class with templated overloaded operator(), for use by Options::addOptions() */
    class OptionSpecific
    {
    public:
      OptionSpecific(Options& parent_) : parent(parent_) {}
      
      /**
       * Add option described by name to the parent Options list,
       *   with storage for the option's value
       *   with default_val as the default value
       *   with desc as an optional help description
       */
      template<typename T>
      OptionSpecific&
      operator()(const std::string& name, T& storage, T default_val, const std::string& desc = "")
      {
        parent.addOption(new Option<T>(name, storage, default_val, desc));
        return *this;
      }
      
#if H_MV
      template<typename T>
      OptionSpecific&
        operator()(const std::string& name, std::vector<T>& storage, T default_val, unsigned uiMaxNum, const std::string& desc = "" )
      {
        std::string cNameBuffer;
        std::string cDescBuffer;

        storage.resize(uiMaxNum);
        for ( unsigned int uiK = 0; uiK < uiMaxNum; uiK++ )
        {
          cNameBuffer       .resize( name.size() + 10 );
          cDescBuffer.resize( desc.size() + 10 );

          Bool duplicate = (uiK != 0); 
          // isn't there are sprintf function for string??
          sprintf((char*) cNameBuffer.c_str()       ,name.c_str(),uiK,uiK);

          if ( !duplicate )
          {          
            sprintf((char*) cDescBuffer.c_str(),desc.c_str(),uiK,uiK);
          }

          cNameBuffer.resize( std::strlen(cNameBuffer.c_str()) );  
          cDescBuffer.resize( std::strlen(cDescBuffer.c_str()) ); 
          

          parent.addOption(new Option<T>( cNameBuffer, (storage[uiK]), default_val, cDescBuffer, duplicate ));
        }

        return *this;
      }
#endif
      /**
       * Add option described by name to the parent Options list,
       *   with desc as an optional help description
       * instead of storing the value somewhere, a function of type
       * OptionFunc::Func is called.  It is upto this function to correctly
       * handle evaluating the option's value.
       */
      OptionSpecific&
      operator()(const std::string& name, OptionFunc::Func *func, const std::string& desc = "")
      {
        parent.addOption(new OptionFunc(name, parent, func, desc));
        return *this;
      }
    private:
      Options& parent;
    };
    
  }; /* namespace: program_options_lite */
}; /* namespace: df */

//! \}
