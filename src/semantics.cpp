#include "semantics.hpp"
#include <iostream>

void write(LangObj* obj){
 switch(obj->type){
    case ObjType::FUNC:{
       Func* func_obj = static_cast<Func*>(obj);  			       
       const std::string func_name = func_obj->name;
       const bool should_return = func_obj->has_return;
       const  bool is_exported = obj->is_exported;

       std::string output_string = "( func";
       if (is_exported){
	  output_string += " ( export $" + func_name + ")";  
       } else {
	  output_string += " $" + func_name;     
       }

       if (func_obj->args.size() > 0){
         std::string params = "";
	 int count = 0;
	 for(auto param : func_obj->args){
            params += "( params $" + std::to_string(count) + " " + param + ")";	 
	 }
         output_string += params;
       }
       
       if(should_return){
          output_string += "( result " + func_obj->func_return_type + ")";	       
       }
       std::cout << output_string;
       break;
      }
      default: break;		       
  }	
}
