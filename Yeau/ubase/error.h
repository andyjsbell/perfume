#ifndef _EAU_ERROR_H_
#define _EAU_ERROR_H_

#include <stdio.h>
#include <iostream>

// For error code
#define EAU_S_OK                0
#define EAU_S_FALSE             -1

#define EAU_E_INVALIDPTR        1
#define EAU_E_INVALIDARG        2
#define EAU_E_EXIST             3
#define EAU_E_FAIL              4
#define EAU_E_UNIMPL            5

// For log output
#define CXX_FUNC_TAG            "["<<__FUNCTION__<<"]"
#define CXX_INFO_TAG            "["<<__FILE__<<":"<<__LINE__<<"]"

#define log_print(pp)                   {   std::cout << CXX_FUNC_TAG << \
                                            #pp"["<< pp << "] printed!" << \
                                            CXX_INFO_TAG << std::endl;}
#define log_print2(pp, pv)              {   std::cout << CXX_FUNC_TAG << \
                                            pp"["<< pv << "] printed!" << \
                                            CXX_INFO_TAG << std::endl;}
#define log_print4(p1, pv1, p2, pv2)    {   std::cout << CXX_FUNC_TAG << " " \
                                            p1"["<< pv1 << "] != " << p2"[" << pv2 << "]" << \
                                            CXX_INFO_TAG << std::endl;}

// To continue if (p) is true, not NULL, or (p1 == p2);
// p, p1 and p2 cannot be funtion, only one of value or expression
#define return_assert(pp)               {   long pv=long(pp); if(!pv) {log_print2(#pp, pv); return;}}
#define returnv_assert(pp, vv)          {   long pv=long(pp); if(!pv) {log_print2(#pp, pv); return (vv);}}
#define break_assert(pp)                {   long pv=long(pp); if(!pv) {log_print2(#pp, pv); break;}}
#define log_assert(pp)                  {   long pv=long(pp); if(!pv) {log_print2(#pp, pv);}}

#define return_assert2(p1, p2)          {   long pv1=long(p1);long pv2=long(p2); \
                                            if(pv1 != pv2) {log_print4(#p1, pv1, #p2, pv2); return;}}
#define returnv_assert2(p1, p2, vv)     {   long pv1=long(p1);long pv2=long(p2); \
                                            if(pv1 != pv2) {log_print4(#p1, pv1, #p2, pv2); return (vv);}}
#define break_assert2(p1, p2)           {   long pv1=long(p1);long pv2=long(p2); \
                                            if(pv1 != pv2) {log_print4(#p1, pv1, #p2, pv2); break;}}
#define log_assert2(p1, p2)             {   long pv1=long(p1);long pv2=long(p2); \
                                            if(pv1 != pv2) {log_print4(#p1, pv1, #p2, pv2);}}

#endif
