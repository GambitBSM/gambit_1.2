//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  General small utility functions.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Pat Scott  
///          (patscott@physics.mcgill.ca)
///  \date 2013 Apr, July, Aug, Sep
///  \date 2014 Mar
///
///  \author Ben Farmer
///          (benjamin.farmer@monash.edu.au)
///  \date 2013 May, June, July
///
///  *********************************************
  
#include <cstring>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <string>  // string

#include "gambit/Utils/util_functions.hpp"


namespace Gambit
{
  
  namespace Utils
  {
  
    const char* whitespaces[] = {" ", "\t", "\n", "\f", "\r"};
  
    /// Split a string into a vector of strings using a delimiter,
    /// and remove any whitespace around the delimiters.
    std::vector<str> delimiterSplit(str s, str delim)
    {
      std::vector<str> vec;
      // Get rid of any whitespace around the delimiters
      #if GAMBIT_CONFIG_FLAG_use_regex     // Using regex :D 
        regex rgx1("\\s+"+delim), rgx2(delim+"\\s+");
        s = regex_replace(s, rgx1, delim);
        s = regex_replace(s, rgx2, delim);
      #else                                // Using lame-o methods >:(
        str previous = s+".";
        while (s != previous) 
        {
          previous = s;
          for (int i = 0; i != 5; i++)
          {
            boost::replace_all(s, whitespaces[i]+delim, delim);
            boost::replace_all(s, delim+whitespaces[i], delim);
          }
        }
      #endif
      if (s == "") return vec;
      // Split up the list of versions by the delimiters
      boost::split(vec, s, boost::is_any_of(delim), boost::token_compress_on);
      return vec;
    }
  
    /// Strips namespace from the start of a string, or after "const".
    str strip_leading_namespace(str s, str ns)
    {
      #if GAMBIT_CONFIG_FLAG_use_regex     // Using regex :D
        regex expression("(^|[\\s\\*\\&\\(\\,\\[])"+ns+"::");
        s = regex_replace(s, expression, str("\\1"));
      #else                                // Using lame-o methods >:(
        int len = ns.length() + 2;
        if (s.substr(0,len) == ns+str("::")) s.replace(0,len,"");
        boost::replace_all(s, str(",")+ns+"::", str(","));
        boost::replace_all(s, str("*")+ns+"::", str("*"));
        boost::replace_all(s, str("&")+ns+"::", str("&"));
        boost::replace_all(s, str("(")+ns+"::", str("("));
        boost::replace_all(s, str("[")+ns+"::", str("["));
        for (int i = 0; i != 5; i++)
        {
          boost::replace_all(s, whitespaces[i]+ns+"::", whitespaces[i]);
        }
      #endif
      return s;
    }
  
    /// Strips all whitespaces from a string, but re-inserts a single regular space after "const".
    str strip_whitespace_except_after_const(str s)
    {
      str tempstr("__TEMP__"), empty(""), constdec2("const ");
      #if GAMBIT_CONFIG_FLAG_use_regex     // Using regex :D
        regex constdec1("const\\s+"), temp(tempstr), whitespace("\\s+");
        s = regex_replace(s, constdec1, tempstr);
        s = regex_replace(s, whitespace, empty);
        s = regex_replace(s, temp, constdec2); 
      #else                                // Using lame-o methods >:(
        str previous = s+".";
        while (s != previous) 
        {
          previous = s;
          for (int i = 0; i != 5; i++)
          {
            boost::replace_all(s, str("const")+whitespaces[i], tempstr);
            s.erase(std::remove(s.begin(), s.end(), *whitespaces[i]), s.end());
          }
        }
        boost::replace_all(s, tempstr, constdec2);
      #endif
      return s;
    }
  
    /// Strips leading and/or trailing parentheses from a string.
    void strip_parentheses(str &s)
    {
      if (s.at(0) == '(')       s = s.substr(1, s.size());
      if (*s.rbegin() == ')')   s = s.substr(0, s.size()-1);
    }

    /// Ensure that a path exists (and then return the path, for chaining purposes)
    const std::string& ensure_path_exists(const std::string& path)
    { 
       // Boost was causing some people problems: now using J Leffler's 'mkdir'
       // boost::filesystem::path dir(path);
       // if( !( boost::filesystem::exists( dir.parent_path() ) )) 
       // { boost::filesystem::create_directories( dir.parent_path() ); }
       // return path;

       // Split off potential filename
       // If only path is provided, it must end in a slash!!!
       size_t found = path.find_last_of("/\\");
       std::string prefix = path.substr(0,found);

       recursive_mkdir( prefix.c_str() );

       std::cout << "path: " << path << std::endl;

       return path;
    }

    /// Get current system clock time
    time_point get_clock_now()
    {
        return std::chrono::system_clock::now();
    }

    /// Return (locally defined) date and time corresponding to time_point
    std::string return_time_and_date(const time_point& in)
    {
        std::time_t t = std::chrono::system_clock::to_time_t(in);

        std::string ts = std::ctime(&t); // for example : Tue Sep 27 14:21:13 2011\n
        ts.resize(ts.size()-1); // Remove the annoying trailing newline
        return ts;
    }

    /// Check if two strings are a "close" match
    /// Used for "did you mean?" type checking during command line argument processing
    bool are_similar(const std::string& s1, const std::string& s2)
    {
      if(check1(s1,s2) or check1(s2,s1)){ return true; }
      else if(check2(s1,s2)){ return true; } // symmetric
      else{ return false; }
      //TODO: Add more checks? These ones are pretty minimal. Maybe something that computes percentage match between strings...
    }

    /// true if s1 can be obtained by deleting one character from s2
    bool check1(const std::string& s1, const std::string& s2)
    {
      if(s2.length() - s1.length() != 1){ return false; }
      int i,j;
      for(i=0,j=0; i<s2.length(); i++,j++)
      {
          if(s2[i] == s1[j])
          {/*do  nothing*/}
          else if(i == j)
          { j++;}
          else
          {return false;}
      }
      return true;
    }

    /// true if s1 can be obtained from s2 by changing no more than X characters (X=2 for now)
    bool check2(const std::string& s1, const std::string& s2)
    {
      int error_limit = 2;
      int number_of_errors = 0;

      if(s2.length() != s1.length()){ return false; }
      int i,j;
      for(i=0,j=0; i<s2.length(); i++,j++)
      {
          if(s2[i] == s1[j])
          {/*do  nothing*/}
          else if(number_of_errors <= error_limit)
          { number_of_errors++;}
          else
          {return false;}
      }
      return true;
    }

     
  }

}
