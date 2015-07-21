#include<iostream>

#include<memory>

#include"data.h"

class ConvertorType{
  protected:
    lang::TypeManager::TypeID _convFunc;
    lang::Accessor _from               ;
    lang::Accessor _to                 ;
  public:
};

class Convertor: public ConvertorType{
  protected:
    const char* _name     ;
    bool        _shareData;
  public:
    lang::Accessor&getFrom();
    lang::Accessor&getTo  ();
};

class ConvertorRegister{
  protected:
    std::map<const char*,std::shared_ptr<Convertor>>_nameToConvertor;
  public:
};

int main(){
  lang::TypeManager*manager=new lang::TypeManager();
  manager->addType("float4"  ,lang::TypeManager::ARRAY,4,"float" );
  manager->addType("float4x4",lang::TypeManager::ARRAY,4,"float4");
  manager->addType("mat4",lang::TypeManager::ARRAY,4,"float4");


  //std::cerr<<manager->toStr()<<std::endl;
  //delete manager;
  //return 0;
  std::cerr<<manager->toStr()<<std::endl;
  lang::Accessor ac=manager->allocAccessor("float4x4");
  ac[0][0] = 32.321f;
  ac[1][0] = 31231.f;
  std::cout<<(float)(ac[0][0]) <<std::endl;
  std::cout<<(float)(ac[1][0]) <<std::endl;
  ac.free();
  delete manager;
  return 0;
}
