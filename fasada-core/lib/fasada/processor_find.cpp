/// This file is a part of Fasada toolbox
/// ---------------------------------------------------
/// @author Wojciech Borkowski <wborkowsk@gmail.com>
/// @footnote Technical mail: <fasada.wb@gmail.com>
/// @copyright 2019
/// @version 0.01
///
///  See CURRENT licence file!
///
#define UNIT_IDENTIFIER "processor_find"
#include "fasada.hpp"
#include "processor_find.h"
#include "tree/ptree_foreach.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp> ///https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c
#include <iostream>

namespace fasada
{

processor_find::processor_find(const char* name):
    form_processor(name)
{

}

processor_find::~processor_find()
{}

//default HTML form for this processor
std::string processor_find::Form=
        "<form action=\"${fullpath}!${proc}\" class=\"fasada_form\">"
        "\n<input name=\"html\"   type=\"hidden\" >"
        "\n<input name=\"long\"   type=\"hidden\" >"
        "\n<input name=\"ready\"  type=\"hidden\"   value=\"$is_ready\" >"
        "\n&#x1F50D; &#x1F50D; &#x1F50D; &#x1F50D; &#x1F50D; &#x1F50D;"
        "\n<BR>SUBPATH:     "
        "   <input name=\"subpath\" type=\"$input_of_subpath\" onclick=\"this.select()\" size=\"$size_of_subpath\"   value=\"$subpath\">"
        "\n<BR>FIELD NAME: "
        "   <input name=\"field\"   type=\"$input_of_field\" onclick=\"this.select()\" size=\"$size_of_field\"   value=\"$field\">"
        "\n<BR>FIELD VALUE: "
        "   <input name=\"value\"   type=\"$input_of_value\" onclick=\"this.select()\" size=\"$size_of_value\"   value=\"$value\">"

        "\n&nbsp;<input name=\"regex\"  type=\"checkbox\" value=\"true\" ${regex_checked} ><a href=\"https://regexr.com/\">Use regular expressions?</a>"

        "\n<BR>WILL BE FIND IN <B class=fasada_path>'${&path}'</B> "
        "\n<BR> LIMIT: <input type=\"number\" name=\"limit\" value=\"${limit}\" min=\"25\" max=\"99999\"> "
        "\n<BR><input type=\"submit\" value=\"OK\">"
        "\n&nbsp;<input type=\"button\" value=\"CANCEL\" onclick=\"window.history.back();\" >"
        "\n&nbsp;<a class=\"fasada_action\" href=\"${fullpath}?ls&html&long\">LSL</A>&nbsp;&nbsp; "
        "\n<a class=\"fasada_action\" href=\"${fullpath}?dfs&html&long\">TREE</A>&nbsp;&nbsp; \n"
        "</form>";

/*
"<select name=\"limit\" >"
"<option value=\"256\" >256</option>"
"<option value=\"1024\" selected >1024</option>"
"<option value=\"2048\" >2048</option>"
"<option value=\"4096\" >4096</option>"
"</select>"
*/

void processor_find::_implement_read(ShmString& o,const pt::ptree& top,URLparser& request)
{
    bool        html=request.asHTML();
    unsigned    noc=top.size();//czy ma jakieś elementy składowe?
    std::string fullpath=request.getFullPath();

    if(noc<1)
    {
        throw(tree_processor_exception("PTREE PROCESSOR "+procName+" HAS NOTHING TO DO WITH LEAF NODE "+request["&path"]));
    }

    if(request["ready"]=="true")//<FORM> is filled completelly
    {
        _implement_substring_find(o,top,request);
    }
    else
    {
        if(!html)
            throw(tree_processor_exception("PTREE PROCESSOR "+procName+" CANNOT PREPARE FORM OTHER THAN HTML!"));

        o+=ipc::string(EXT_PRE)+"htm\n";
        o+=getPageHeader(fullpath)+"\n";

        if(request.find("limit")==request.end())
             request["limit"]="1024";

        if(request.find("regex")==request.end())
             request["regex_checked"]="";
        else
             request["regex_checked"]="checked";

        std::string ReadyForm=replace_all_variables(Form,request);

        //boost::replace_all(ReadyForm,"$path",request["&path"]);

        if( (request.find("subpath") != request.end() && request["subpath"]!="" )
        &&  (request.find( "field" ) != request.end() && request["field"]!="" )
        &&  (request.find( "value" ) != request.end() && request["value"]!="" )
                )
        {
            boost::replace_all(ReadyForm,"$is_ready","true");
        }

        if( request.find("sugestion") != request.end())//This is valid only at the begining of setting search!
        {
            auto Sugestion=request["sugestion"];
            if(request.find("field") ==  request.end()  )
                boost::replace_all(ReadyForm,"$value",Sugestion);
            if(request.find("value") ==  request.end()  )
                boost::replace_all(ReadyForm,"$field",Sugestion);
        }
        //else ???
        {
            ///<input name="subpath" type="$input_of_subpath"  value="$subpath" size="$size_of_subpath">
            if( request.find("subpath") == request.end() || request["subpath"]=="" )
            {
                boost::replace_all(ReadyForm,"$input_of_subpath","text");
                boost::replace_all(ReadyForm,"$subpath",STR_DEFAULT_FILTER);
                boost::replace_all(ReadyForm,"$size_of_subpath",STR_DEFAULT_LEN_OF_SUBPATH);
            }
            else
            {
                boost::replace_all(ReadyForm,"$input_of_subpath","hidden");
                std::string replacer=(request["subpath"]+"\"><I class=\"fasada_val\">"+request["subpath"]+"</I>");
                boost::replace_all(ReadyForm,"$subpath\">",replacer);
                boost::replace_all(ReadyForm,"$size_of_subpath","1");        boost::replace_all(ReadyForm,"$proc",procName);
                boost::replace_all(ReadyForm,"${fullpath}",fullpath);
            }

            ///<input name="field"   type="$input_of_field"    value="$field"   size="$size_of_field">
            if( request.find("field") == request.end() || request["field"]=="" )
            {
                boost::replace_all(ReadyForm,"$input_of_field","text");
                boost::replace_all(ReadyForm,"$field",STR_DEFAULT_FILTER);
                boost::replace_all(ReadyForm,"$size_of_field",STR_DEFAULT_LEN_OF_NAME);
            }
            else
            {
                boost::replace_all(ReadyForm,"$input_of_field","hidden");
                std::string replacer=(request["field"]+"\"><I class=\"fasada_val\">"+request["field"]+"</I>");
                boost::replace_all(ReadyForm,"$field\">",replacer);
                boost::replace_all(ReadyForm,"$size_of_field","1");
            }

            ///<input name="value"   type="$input_of_value"    value="$value"   size="$size_of_value">"
            if( request.find("value") == request.end() || request["value"]=="" )
            {
                boost::replace_all(ReadyForm,"$input_of_value","text");
                boost::replace_all(ReadyForm,"$value",STR_DEFAULT_FILTER);
                boost::replace_all(ReadyForm,"$size_of_value","24");
            }
            else
            {
                boost::replace_all(ReadyForm,"$input_of_value","hidden");
                std::string replacer=(request["value"]+"\"><I class=\"fasada_val\">"+request["value"]+"</I>");
                boost::replace_all(ReadyForm,"$value\">",replacer);
                boost::replace_all(ReadyForm,"$size_of_value","1");
            }
        }

        o+=ReadyForm;
        o+=getPageClosure(_compiled);
    }
}

void processor_find::_implement_regex_find(ShmString& o,const pt::ptree& top,URLparser& request)
{
    unsigned counter=0,limit=1024;
    if(request.find("limit")!=request.end())
        limit=boost::lexical_cast<unsigned>(request["limit"]);

    std::string fullpath=request.getFullPath();
    if( *(--fullpath.end())!='/' )
        fullpath+="/";

    bool defret=(request["return"]!="false");
    bool   html=request.asHTML();

    std::string  subpath=request["subpath"];
    std::string    field=request["field"];
    std::string    value=request["value"];

    bool all_spaths=(subpath=="*");
    bool all_fields=(field=="*");
    bool all_values=(value=="*");

    boost::regex rx_spaths;
    boost::regex rx_fields;
    boost::regex rx_values;

    if(html)//TYPE HEADER AND HTML HEADER
    {
        o+=ipc::string(EXT_PRE)+"htm\n";
        o+=getPageHeader(request["&path"])+"\n";
        o+=getActionsPanel(request);
        o+="\n<OL>\n";
    }
    else
        o+=ipc::string(EXT_PRE)+"txt\n";//TYPE HEADER

    if(!all_spaths)
        rx_spaths.assign(subpath);
    if(!all_fields)
        rx_fields.assign(field);
    if(!all_values)
        rx_values.assign(value);

    auto subpath_lambda= [rx_spaths](const ptree& t,std::string k)
                                {
                                    return boost::regex_match(k.c_str(),rx_spaths);
                                };

    auto field_lambda= [rx_fields](const ptree& t,std::string k)
                                {
                                    auto lslas=k.rfind('/');
                                    if(lslas==k.npos)
                                        return false;
                                    return boost::regex_match(k.c_str()+lslas,rx_fields);
                                };

    auto value_lambda= [rx_values](const ptree& t,std::string k)
                                {
                                    return boost::regex_match(t.data().c_str(),rx_values);
                                };

    auto print_lambda=[&o,defret,html,fullpath,&request,&counter,limit,this](const ptree& t,std::string k)
                                {
                                    counter++;
                                    if(counter<limit)
                                    {
                                        o+=(html?"<LI>":"* ");
                                        std::string pathk=k;
                                        if(html) o+="<B class=fasada_path><A HREF=\""
                                                +fullpath+pathk+"?ls&html&long\">";
                                        o+=pathk;
                                        if(html)
                                            o+="</A></B> : <I class=\"fasada_val\"> <A HREF=\""
                                             +fullpath+pathk+"?get&html&long\">'";
                                            else o+="' : '";
                                        o+=t.data();
                                        o+="'";
                                        if(html)
                                        {
                                            o+="</A></I>&nbsp; "+getNodePanel(t,fullpath+pathk,request);
                                            if(t.data()=="")
                                            {
                                                auto pos=k.rfind('/');
                                                _implement_attributes(o,t,request,k.substr(pos+1));
                                            }
                                            o+="\n";
                                        }
                                        else o+="\n";
                                    } else { o+='.';if(counter%100==0) o+="<BR>\n";}
                                    return defret;
                                };

    auto and_lambda=[all_fields,field_lambda,all_values,value_lambda,print_lambda](const ptree& t,std::string k)
    {
        return (all_fields || field_lambda(t,k))
                &&
               (all_values || value_lambda(t,k))
                &&
                print_lambda(t,k);
    };

//  It would be easier, but compiler don't cooperate :-/ ;-)
//      "error: operands to ?: have different types" ?????????
//  auto real_lambda=( all_spaths ? always_lambda : subpath_lambda );

    if(all_spaths)
        foreach_node( top,"",always, and_lambda, never, "/" );
    else
        foreach_node( top,"",subpath_lambda, and_lambda, never, "/" );

    if(html)
    {
        o+="</OL>";
        o+=boost::lexical_cast<val_string>(counter)+" nodes <BR>\n";
        if(counter>limit)
            o+="Use <q>limit</q> variable if not all nodes are visible! <BR>\n";
        if(counter>10)
            o+=getActionsPanel(request);
        o+="\n"+getPageClosure(_compiled);
    }
}

void processor_find::_implement_substring_find(ShmString& o,const pt::ptree& top,URLparser& request)
//Called in _implement_write
{
    unsigned counter=0,limit=1024;
    if(request.find("limit")!=request.end())
        limit=boost::lexical_cast<unsigned>(request["limit"]);

    std::string fullpath=request.getFullPath();
    if( *(--fullpath.end())!='/' )
        fullpath+="/";

    bool defret=(request["return"]!="false");
    bool   html=request.asHTML();

    std::string  subpath=request["subpath"];
    std::string    field=request["field"];
    std::string    value=request["value"];

    bool all_spaths=(subpath=="*");
    bool all_fields=(field=="*");
    bool all_values=(value=="*");

    if(html)//TYPE HEADER AND HTML HEADER
    {
        o+=ipc::string(EXT_PRE)+"htm\n";
        o+=getPageHeader(request["&path"])+"\n";
        o+=getActionsPanel(request);
        o+="\n<OL>\n";
    }
    else
        o+=ipc::string(EXT_PRE)+"txt\n";//TYPE HEADER

    auto subpath_lambda= [subpath](const ptree& t,std::string k)
                                {
                                    auto found=k.find(subpath,0);
                                    return ( found != k.npos );
                                };

    auto field_lambda= [field](const ptree& t,std::string k)
                                {
                                    auto lslas=k.rfind('/');
                                    if(lslas==k.npos)
                                        return false;
                                    auto found=k.find(field,lslas);
                                    //std::cerr<<k<<" "<<lslas<<" "<<found<<std::endl;
                                    return ( found != k.npos );
                                };

    auto value_lambda= [value](const ptree& t,std::string k)
                                {
                                    auto found=t.data().find(value,0);
                                    return ( found != std::string::npos );
                                };

    auto print_lambda=[&o,defret,html,fullpath,&request,&counter,limit,this](const ptree& t,std::string k)
                                {
                                    counter++;
                                    if(counter<limit)
                                    {
                                        o+=(html?"<LI>":"* ");
                                        std::string pathk=k;
                                        if(html) o+="<B class=fasada_path><A HREF=\""
                                                +fullpath+pathk+"?ls&html&long\">";
                                        o+=pathk;
                                        if(html)
                                            o+="</A></B> : <I class=\"fasada_val\"> <A HREF=\""
                                             +fullpath+pathk+"?get&html&long\">'";
                                            else o+="' : '";
                                        o+=t.data();
                                        o+="'";
                                        if(html)
                                        {
                                            o+="</A></I>&nbsp; "+getNodePanel(t,fullpath+pathk,request);
                                            if(t.data()=="")
                                            {
                                                auto pos=k.rfind('/');
                                                _implement_attributes(o,t,request,k.substr(pos+1));
                                            }
                                            o+="\n";
                                        }
                                        else o+="\n";
                                    } else { o+='.';if(counter%100==0) o+="<BR>\n";}
                                    return defret;
                                };

    auto and_lambda=[all_fields,field_lambda,all_values,value_lambda,print_lambda](const ptree& t,std::string k)
    {
        return (all_fields || field_lambda(t,k))
                &&
               (all_values || value_lambda(t,k))
                &&
                print_lambda(t,k);
    };

//  It would be easier, but compiler don't cooperate :-/ ;-)
//      "error: operands to ?: have different types" ?????????
//  auto real_lambda=( all_spaths ? always_lambda : subpath_lambda );

    if(all_spaths)
        foreach_node( top,"",always, and_lambda, never, "/" );
    else
        foreach_node( top,"",subpath_lambda, and_lambda, never, "/" );

    if(html)
    {
        o+="</OL>";
        o+=boost::lexical_cast<val_string>(counter)+" nodes <BR>\n";
        if(counter>limit)
            o+="Use <q>limit</q> variable if not all nodes are visible! <BR>\n";
        if(counter>10)
            o+=getActionsPanel(request);
        o+="\n"+getPageClosure(_compiled);
    }
}

void processor_find::_implement_write(ShmString& o,pt::ptree& top,URLparser& request)
{
    if(request["ready"]!="true")//<FORM> is NOT filled completelly
    {
        _implement_read(o,top,request);
    }
    else
    {
        if(request["regex"]=="true")
            _implement_regex_find(o,top,request);
        else
            _implement_substring_find(o,top,request);
    }
}

}//namespace "fasada"
