#ifndef _EAU_ERROR_H_
#define _EAU_ERROR_H_

#define EAU_S_OK                0
#define EAU_S_FALSE             -1

#define EAU_E_INVALIDPTR        1
#define EAU_E_INVALIDARG        2
#define EAU_E_EXIST             3
#define EAU_E_FAIL              4

#define return_if_fail(p)       {if(!(p)) {return;}}
#define returnv_if_fail(p, v)   {if(!(p)) {return (v);}}
#define break_if_fail(p)        {if(!(p)) {break;}}

#endif
