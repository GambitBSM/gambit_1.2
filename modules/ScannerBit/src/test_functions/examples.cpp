//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Base functions objects for use in GAMBIT.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Gregory Martinez
///           (gregory.david.martinez@gmail.com)
///  \date 2014 MAY
///
///  *********************************************

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <algorithm>

#include "function_plugin.hpp"
#include "cholesky.hpp"

function_plugin(uniform, version(1,0,0))
{
        double plugin_main (const std::vector<double> &vec)
        {
                std::vector<std::string> &keys     = get_keys();
                std::cout << "unform info:" <<std::endl;
                std::vector<double> params = prior_transform(vec);
                std::cout << "param = " << get_inifile_value<int>("some_param") << std::endl;
                std::cout << "dim = " << params.size() << std::endl;
                std::cout << "keys = " << keys[0] << "   " << keys[1] << std::endl;
                std::cout << "pts = " << params[0] << "   " << params[1] << std::endl;
                std::cout << "vec size = " << vec.size() << "   " << vec[0] << "   " << vec[1] << std::endl;
                getchar();
                
                return 0;
        }
}

function_plugin(gaussian, version(1, 0, 0))
{
        Gambit::Cholesky chol;
        std::vector <double> mean;
        std::vector<std::vector<double>> cov;
        unsigned int dim;
        
        plugin_constructor
        {
                dim = get_keys().size();
                std::vector<std::vector<double>> cov(dim, std::vector<double>(dim, 0));
                        
                cov = get_inifile_value<std::vector<std::vector<double>>> ("cov", cov);
                
                if (cov.size() != dim)
                {
                        //NOTE:  put error here.
                }
                
                for (std::vector<std::vector<double>>::iterator it = cov.begin(); it != cov.end(); it++)
                {
                        if (it->size() != cov.size())
                        {
                                //Note:  put error here.
                        }
                }
                
                if (!chol.EnterMat(cov))
                {
                        std::vector <double> sigs = get_inifile_value <std::vector <double>> ("sigs", std::vector<double>(dim, 1));
                        if (sigs.size() != dim)
                        {
                                //NOTE:  put error message here.
                        }
                        else
                        {
                                for (int i = 0, end = sigs.size(); i < end; i++)
                                {
                                        cov[i][i] = sigs[i]*sigs[i];
                                }
                        }
                }
                else
                {
                        //NOTE:  put error message here.
                }
                
                mean = get_inifile_value<std::vector <double>> ("mean", std::vector <double>(dim, 0));
                
                if (!chol.EnterMat(cov))
                {
                        //Note:  put error message here: "Gaussian (test):  Covariance matrix is not postive definite."       
                }
        }
        
        double plugin_main(const std::vector<double> &vec)
        {
                std::vector<double> &params = prior_transform(vec);
                
                return -chol.Square(params, mean)/2.0;
        }
}

function_plugin(EggBox, version(1, 0, 0))
//function_plugin(EggBox, version(1, 0, 0), external_library_required)
{

        std::vector <double> params;
        std::pair <double, double> length;
        unsigned int dim;
        
        plugin_constructor
        {
                dim = get_keys().size();
                if (dim != 2)
                {
                        //NOTE:  put error message here.
                }
                length = get_inifile_value<std::pair<double, double> > ("length", std::pair<double, double>(10.0, 10.0));
        }
        
        double plugin_main(const std::vector<double> &unit)
        {
                std::vector<double> &params = prior_transform(unit);
                params[0] *= length.first;
                params[1] *= length.second;
                
                return pow((2.0 + cos(params[0]*M_PI_2)*cos(params[1]*M_PI_2)), 5.0);
        }
}
