#include "compte.h"
#include "compteimpl.h"
#include "error.h"

long GetCloudyInstance(eau::zeroptr<eau::ICloudy> &pCloudy)
{
    pCloudy = eau::CCloudy::inst();
    return EAU_S_OK;
}
