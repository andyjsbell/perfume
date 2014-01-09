#include "ieau_api.h"
#include "eau_all.h"

void CreateIEauApi(eau::IEauApi ** pp)
{
    (*pp) = new eau::CEauApi();
}

void DestroyIEauApi(eau::IEauApi ** pp)
{
    eau::IEauApi *p = (eau::IEauApi *)(*pp);
    if (p) {
        delete p;
        (*pp) = NULL;
    }
}

