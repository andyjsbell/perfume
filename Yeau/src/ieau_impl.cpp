#include "ieau_api.h"

void CreateIEauApi(IEauApi ** pp)
{
    (*pp) = new CEauAPI();
}

void DestroyIEauApi(IEauApi ** pp)
{
    if (*pp) {
        delete (*pp);
        (*pp) = NULL;
    }
}
