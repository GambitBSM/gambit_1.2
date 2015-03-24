//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  declaration for scanner module
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date 2013 August
///        2014 Feb
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)   
///  \date 2014 Dec
///
///  *********************************************

#ifndef GAMBIT_PLUGIN_DEFS_HPP
#define GAMBIT_PLUGIN_DEFS_HPP

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <typeinfo>

#include "gambit/ScannerBit/printer_interface.hpp"
#include "gambit/Utils/type_index.hpp"

namespace Gambit
{

        namespace Scanner
        {

                namespace Plugins
                {
                        using Gambit::type_index;
                        
                        class factoryBase
                        {
                        public:
                                virtual void *operator()() = 0;
                                virtual ~factoryBase() {}
                        };
                        
                        template <typename T>
                        class funcFactory : public factoryBase
                        {
                        private:
                                T *func;
                                
                        public:
                                funcFactory (T *in) : func(in) {}
                                void *operator()(){return *(void**)&func;}
                                ~funcFactory(){}
                        };
                        
                        template <typename T>
                        class classFactory : public factoryBase
                        {
                        private:
                                std::vector<T *> ptrs;
                                
                        public:
                                void *operator()()
                                {
                                        T *ptr = new T;
                                        ptrs.push_back(ptr);
                                        return (void*) ptr;
                                }
                                
                                ~classFactory()
                                {
                                        for (auto it = ptrs.begin(), end = ptrs.end(); it != end; it++)
                                                delete *it;
                                }
                        };
                        
                        /// Structure that holds all the data provided by plugins about themselves.
                        struct pluginData
                        {
                                std::string name;
                                std::string tag;
                                YAML::Node node;
                                printer_interface *printer;
                                std::vector <void *> inputData;
                                std::vector <void (*)(pluginData &)> inits;
                                std::map<std::string, factoryBase *> outputFuncs;
                                std::map<type_index, void *> plugin_mains;
                                void (*deconstructor)();
                                bool loaded;
                                
                                pluginData(std::string name) : name(name), deconstructor(NULL), loaded(false) {}
                                ~pluginData()
                                {
                                        if (deconstructor != NULL && loaded == true)
                                                deconstructor();
                                        
                                        for (auto it = outputFuncs.begin(), end = outputFuncs.end(); it != end; it++)
                                        {
                                                delete it->second;
                                        }
                                        
                                        loaded = false;
                                }
                        };  

                }

        }

}

#endif
