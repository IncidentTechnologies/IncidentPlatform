#include "valid.h"

using namespace dss;

valid::valid(bool valid) :
    m_fValid(valid)
{/*empty stub*/}

valid::~valid()
{/*empty stub*/}

bool valid::IsValid() {
    if(m_fValid == 1)
        return true;
    else
        return false;
}

int valid::GetValid(){
    return m_fValid;
}

RESULT valid::Validate(){
    m_fValid = true;
    return R_SUCCESS;
}

RESULT valid::Invalidate(){
    m_fValid = false;
    return R_SUCCESS;
}

bool CheckValid(void *obj)
{
    valid *pTempValid = reinterpret_cast<valid*>(obj);
    return pTempValid->IsValid();
}