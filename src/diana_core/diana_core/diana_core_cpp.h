#ifndef DIANA_CORE_CPP_H
#define DIANA_CORE_CPP_H

extern "C"
{
#include "diana_core.h"
#include "diana_pe.h"
#include "diana_text_output_masm.h"
}
#include "stdexcept"
#include "sstream"
#include "string"
#include "algorithm"
#include "vector"
namespace diana
{

class CException:public std::runtime_error
{
    int m_errorCode;

    std::string ToText(int errorCode,
                       const std::string & text)
    {
        std::stringstream res;
        res<<text<<", errorCode = "<<errorCode;
        return res.str();
    }
public:
    CException(int errorCode,
               const std::string & text)
        :
            std::runtime_error(ToText(errorCode, text)),
            m_errorCode(errorCode)
    {
    }
};

template<class Strategy>
class Guard
{
    typename Strategy::ObjectType m_object;
    bool m_needToFree;
    Guard(const Guard &);
    Guard & operator =(const Guard &);
public:
    Guard()
        :
            m_needToFree(false)
    {
    }
    Guard(const typename Strategy::ObjectType & object)
        :
            m_object(object),
            m_needToFree(true)
    {
    }
    void reset(const typename Strategy::ObjectType & object)
    {
        Guard guard2(object);
        guard2.swap(*this);
    }
    void swap(Guard & other)
    {
        std::swap(other.m_object, m_object);
        std::swap(other.m_needToFree, m_needToFree);
    }
    ~Guard()
    {
        if (m_needToFree)
        {
            Strategy::Free(m_object);
        }
    }
};

struct PeFile
{
    typedef Diana_PeFile * ObjectType;
    static void Free(Diana_PeFile * pObject)
    {
        DianaPeFile_Free(pObject);
    }
};

struct InstructionsOwner
{
    typedef Diana_InstructionsOwner * ObjectType;
    static void Free(Diana_InstructionsOwner * pObject)
    {
        Diana_InstructionsOwner_Free(pObject);
    }
};
#define DIANA_DEF_ERR_STRING    "DiException"
#define DI_CHECK_CPP2(di____Expression, di____ErrorString) { int di____status = (di____Expression); if (di____status) { throw diana::CException(di____status, di____ErrorString); } }
#define DI_CHECK_CPP(di____Expression) DI_CHECK_CPP2(di____Expression, DIANA_DEF_ERR_STRING)


class CMasmString
{
    std::vector<char> m_buffer;
    DianaStringOutputContext m_context;
    CMasmString(const CMasmString&);
    CMasmString&operator = (const CMasmString&);
public:
    CMasmString(int maxSize = 1024, 
                int spacesCount = 1)
        :
            m_buffer(maxSize)
    {
        DianaStringOutputContext_Init(&m_context, 
                                      DianaTextOutput_String, 
                                      DianaOpOutput_String, 
                                      spacesCount,
                                      &m_buffer.front(), 
                                      m_buffer.size());
    }
    ~CMasmString()
    {
    }
    const char * Assign(DianaParserResult * pResult, 
                        OPERAND_SIZE instructionRIP)
    {
        DI_CHECK_CPP(DianaTextOutputContext_TextOut(&m_context.parent, pResult, instructionRIP));
        return &m_buffer.front();
    }
};


}


#ifdef DI_LINK_C_RUNTIME_CPP 
extern "C"
{
#include "diana_c_runtime.inc"
}
#endif

#endif