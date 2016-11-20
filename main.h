#ifndef __LCB_H__
#define __LCB_H__

#include <windows.h>
#include <string>

using namespace std;

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

string DLL_EXPORT DllAdd(string,string);
string DLL_EXPORT DllSub(string,string);
string DLL_EXPORT DllMul(string,string);
string DLL_EXPORT Butaf(string,string);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
