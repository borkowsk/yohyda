#ifndef PROCESSOR_LS_H
#define PROCESSOR_LS_H

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif
#include "tree_processor.h"


namespace facjata
{

class processor_ls : public tree_processor //Category READER!
{
    public:
        processor_ls(const char* name="ls");
       ~processor_ls();
    protected://implementation
    virtual
        void _implement_read(ShmString& o,const pt::ptree& top,URLparser& request);
    virtual
        void _implement_write(ShmString& o,pt::ptree& top,URLparser& request);
};

}//namespace facjata
#endif // PROCESSOR_LS_H