/// This file is a part of Fasada toolbox
/// ---------------------------------------------------
/// @author Wojciech Borkowski <wborkowsk@gmail.com>
/// @footnote Technical mail: <fasada.wb@gmail.com>
/// @copyright 2019
/// @version 0.01
///
///  See CURRENT licence file!
///
#define UNIT_IDENTIFIER "view_processor"
#include "fasada.hpp"
#include "view_processor.h"
#include <boost/lexical_cast.hpp>

namespace fasada
{

view_processor::view_processor(const char* name):
    tree_processor(READER,name)
{

}

view_processor::~view_processor()
{}

void view_processor::_implement_read(ShmString& o,const pt::ptree& top,URLparser& request)
//THIS SHOULD BE REIMPLEMENTED IN DERIVED CLASSES
{
    throw(tree_processor_exception("PTREE PROCESSOR "+procName+" IS NOT IMPLEMENTED AS A READER!"));
}


void view_processor::_implement_write(ShmString& o,pt::ptree& top,URLparser& request)
//THIS SHOULD NOT BE REIMPLEMENTED!
{
    throw(tree_processor_exception("PTREE PROCESSOR "+procName+" IS REALLY NOT A WRITER!"));
}

}//namespace "fasada"
