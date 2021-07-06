/**
 * @file bindings/python/print_py_wrapper.cpp
 * @author Nippun Sharma
 *
 * Implementation of a function to generate a .py file that
 * contains the wrapper for a method.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#include "print_wrapper_py.hpp"
#include "get_methods_wrapper.hpp"
#include "get_class_name_wrapper.hpp"
#include "get_program_name.hpp"
#include "get_arma_type.hpp"
#include <mlpack/core/util/io.hpp>

using namespace mlpack::util;
using namespace std;

namespace mlpack {
namespace bindings {
namespace python {

void PrintWrapperPY(const bool hasScikit,
                    const std::string& category,
                    const std::string& groupName,
                    const std::string& validMethods)
{
  map<string, Params> params;
  set<string> serializable; // all serializable parameters.
  map<string, bool> hyperParams; // if a parameter is hyperparameter.
  map<string, bool> isBool; // if a parameter is a bool.
  vector<string> methods = GetMethods(validMethods);
  int indent = 0; // keeps track of indentation at each point.

  for(auto i: methods)
  {
    params[i] = IO::Parameters(groupName + "_" + i);
  }

  string importString = "";
  for(auto i: methods)
  {
    importString += "from mlpack." + groupName + "_" + i + " ";
		importString += "import " + groupName + "_" + i + "\n";
  }
  // If hasScikit is true, then add scikit base class imports.
  if(hasScikit)
  {
    if(category == "regression")
      importString += "from sklearn.base import BaseEstimator\n";
    else if(category == "classification")
      importString += "from sklearn.base import BaseEstimator, ClassifierMixin\n";
  }
  cout << importString << endl;

  for(auto i=params.begin(); i!=params.end(); i++)
  {
    map<string, ParamData> methodParams = i->second.Parameters();
    for(auto itr=methodParams.begin(); itr!=methodParams.end(); itr++)
    {
      bool isSerial;
      i->second.functionMap[itr->second.tname]["IsSerializable"](
          itr->second, NULL, (void*)& isSerial);
      if(isSerial)
      	serializable.insert(itr->second.cppType);

      bool isHyperParam = false;
      size_t foundArma = itr->second.cppType.find("arma");
      if(itr->second.input && foundArma == string::npos &&
          !isSerial)
        isHyperParam = true;
      hyperParams[itr->first] = isHyperParam;

      if(itr->second.cppType == "bool")
        isBool[itr->first] = true;
      else
        isBool[itr->first] = false;
    }
  }

  string className = GetClassName(groupName);

  // print class.
  if(hasScikit)
  {
    if(category == "regression")
      cout << "class " << className << "(BaseEstimator)" << ":" << endl;
    else if(category == "classification")
    {
      cout << "class " << className << "(BaseEstimator, ClassifierMixin)" <<
          ":" << endl;
    }
    else
      cout << "class " << className << ":" << endl;
  }
  else
    cout << "class " << className << ":" << endl;

  indent += 2;
	cout << string(indent, ' ') << "def __init__(self," << endl;
  for(auto itr=hyperParams.begin(); itr!=hyperParams.end(); itr++)
  {
    // indent + 13, here 13 -> def __init__(
    string validName = (itr->first == "lambda") ? "lambda_" : itr->first;
    if(itr->second && isBool[itr->first])
      cout << string(indent+13, ' ') << validName << " = False," << endl;
    else if(itr->second && !isBool[itr->first])
      cout << string(indent+13, ' ') << validName << " = None," << endl;
  }
  cout << string(indent+12, ' ') << "):" << endl;
  cout << endl;

  indent += 2;
  cout << string(indent, ' ') << "# serializable attributes." << endl;
  for(auto itr=serializable.begin(); itr!=serializable.end(); itr++)
  {
    cout << string(indent, ' ');
    cout << "self._" << *itr << " = None" << endl;
  }
  cout << endl;
  cout << string(indent, ' ') << "# hyper-parameters." << endl;
  for(auto itr=hyperParams.begin(); itr!=hyperParams.end(); itr++)
  {
    if(itr->second)
    {
      string validName = (itr->first == "lambda") ? "lambda_" : itr->first;
      cout << string(indent, ' ');
      cout << "self." << validName << " = " << validName << endl;
    }
  }
  cout << endl;

  indent -= 2;

  // print all method definitions.
  for(auto methodName: methods)
  {
    cout << string(indent, ' ') << "def " << methodName << "(self," << endl;
    int addIndent = 4 + methodName.size() + 1;
    map<string, ParamData> methodParams = params[methodName].Parameters();
    map<string, string> mapToScikitNames;

    for(auto itr=methodParams.begin(); itr!=methodParams.end(); itr++)
    {
      if(hasScikit)
      {
        
      }
    }
    // print input parameters.
    for(auto itr=methodParams.begin(); itr!=methodParams.end(); itr++)
    {
      string validName = (itr->first == "lambda") ? "lambda_" : itr->first;
      if(itr->second.input && serializable.find(itr->second.cppType) ==
          serializable.end() && !hyperParams[itr->first])
      {
        cout << string(indent + addIndent, ' ');
        cout << validName << " = None," << endl;
      }
    }
    cout << string(indent + addIndent - 1, ' ');
    cout << "):" << endl;
    cout << endl;

    // print definition.
    indent += 2;
    cout << string(indent, ' ') << "out = " << groupName + "_" + methodName +
        "(";
    addIndent = 6 + groupName.size() + 1 + methodName.size() + 1;
    for(auto itr=methodParams.begin(); itr!=methodParams.end(); itr++)
    {
      string validName = (itr->first == "lambda") ? "lambda_" : itr->first;
      if(itr->second.input)
      {
        if(itr != methodParams.begin())
          cout << string(indent + addIndent, ' ');
        cout << validName << " = ";

        if(serializable.find(itr->second.cppType) != serializable.end())
          cout << "self._" << itr->second.cppType << "," << endl;
        else if(hyperParams[itr->first])
          cout << "self." << validName << "," << endl;
        else
          cout << validName << "," << endl;
      }
    }
    cout << string(indent + addIndent - 1, ' ');
    cout << ")" << endl;
    cout << endl;

    // print output parameters.
    string returnString = string(indent, ' ') + "return ";
    bool outputsOnlySerial = true;
    for(auto itr=methodParams.begin(); itr!=methodParams.end(); itr++)
    {
      if(!itr->second.input)
      {
        if(serializable.find(itr->second.cppType) != serializable.end())
        {
          cout << string(indent, ' ');
          cout << "self._" << itr->second.cppType << " = out[\"" <<
              itr->first << "\"]" << endl;
        }
        else
        {
          outputsOnlySerial = false;
          returnString += "out[\"" + itr->first + "\"], ";
        }
      }
    }
    cout << endl;
    // return somethings.
    if(outputsOnlySerial)
      cout << returnString + "self" << endl;
    else
      cout << returnString.substr(0, returnString.size() - 2) << endl;

    cout << endl;
    indent -= 2;
  }
}

} // python
} // bindings
} // mlpack
