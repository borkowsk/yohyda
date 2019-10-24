#include "tree_types.h"
#include "tree_consts.h"
#include "processor_add.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp> ///https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c

namespace fasada
{

//default HTML form for this processor
std::string processor_add::Form=
        "<form action=\"$fullpath!$proc\" class=\"fasada_form\">"
        "Node with name "
        "<input type=\"text\" name=\"name\" size=\"12\"><br>"
        "and value"
        "<input type=\"text\" name=\"value\" size=\"80\"><br>"
        "will be added to '$path' <br>"
        "<input type=\"submit\" value=\"Submit\">"
        "</form>";

processor_add::processor_add(const char* name):
    tree_processor(WRITER_READER,name) //also READER because is able to create its own FORM
{
}

processor_add::~processor_add()
{}

void processor_add::_implement_read(ShmString& o,const pt::ptree& top,URLparser& request)
//Implement_read WRITER'a powinno przygotować FORM jeśli jest to format "html"
{
    std::string fullpath=request.getFullPath();
            //request["&protocol"]+"://"+request["&domain"]+':'+request["&port"]+request["&path"];
    std::string tmp=top.get_value<std::string>();
    unsigned    noc=top.size();//czy ma elementy składowe?
    bool html=request["html"]!="false";

    if(html)//TYPE HEADER AND HTML HEADER
    {
         o+=ipc::string(EXT_PRE)+"htm\n";
         o+=getHtmlHeaderDefaults(fullpath)+"\n";
         if(top.data()=="")
         {
             //Podmienić ścieżkę i wartość domyślną
             std::string ReadyForm=Form;
             boost::replace_all(ReadyForm,"$proc",procName);
             boost::replace_all(ReadyForm,"$fullpath",fullpath);
             boost::replace_all(ReadyForm,"$path",request["&path"]);
             o+=ReadyForm;
         }
         else
         {
             o+="<H2>WARNING!</H2><P>Only not-leaf type nodes could be modified by '"+procName+"'</P>";
         }
         o+=getHtmlClosure();
    }
    else
    {
        o+=ipc::string(EXT_PRE)+"txt\n";
        o+="FOR ADD USE: "+fullpath+"?!"+procName+"&name=[NEW_NAME]&value=[NEW_VALUE]";
    }
}

void processor_add::_implement_write(ShmString& o,pt::ptree& top,URLparser& request)
//Implement_write WRITER'a powinno zmienić wartości na powstawie FORMularza z method==GET
{
    if(top.data()!="")//Jeśli ma wartość własną to jest liściem
    {
        throw(tree_processor_exception("PTREE PROCESSOR "+procName+" CANNOT ADD NODE INTO LEAF NODE!"));
    }

    top.add_child(request["name"],pt::ptree{request["value"]});
    o+=ipc::string(EXT_PRE)+"txt\n";
    o+="DONE";
//    top.data()=request["value"];
//    o+="'"+top.data()+"'";
}

}//namespace "fasada"



