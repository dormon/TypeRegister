#include<iostream>

#include<memory>

#include"data.h"

class Convertor{
  protected:
    lang::TypeManager::TypeID _convFunc ;
    lang::Accessor            _from     ;
    lang::Accessor            _to       ;
    const char*               _name     ;
    bool                      _shareData;
  public:
    inline lang::Accessor&           getFrom     (){return this->_from     ;}
    inline lang::Accessor&           getTo       (){return this->_to       ;}
    inline lang::TypeManager::TypeID getConvFunc (){return this->_convFunc ;}
    inline const char*               getName     (){return this->_name     ;}
    inline bool                      getShareData(){return this->_shareData;}
};

class ConvertorRegister{
  protected:
    std::map<const char*,std::shared_ptr<Convertor>>_nameToConvertor;
  public:
};

struct Shader{
  unsigned id;
  unsigned type;
  std::string name;
};

int main(){
  lang::TypeManager*manager=new lang::TypeManager();
  manager->addType("float4"  ,lang::TypeManager::ARRAY,4,"float" );
  manager->addType("float4x4",lang::TypeManager::ARRAY,4,"float4");
  manager->addType("mat4"    ,lang::TypeManager::ARRAY,4,"float4");
  manager->addType("shader"  ,lang::TypeManager::OBJ,(unsigned)sizeof(Shader));
  manager->addType("shader*" ,lang::TypeManager::OBJ,(unsigned)sizeof(Shader*));
  manager->addType("DIFFUSE" ,lang::TypeManager::OBJ,0);
  std::cerr<<manager->toStr()<<std::endl;


  lang::Accessor ac=manager->allocAccessor("float4x4");
  ac[0][0] = 32.321f;
  ac[1][0] = 31231.f;
  std::cout<<(float)(ac[0][0]) <<std::endl;
  std::cout<<(float)(ac[1][0]) <<std::endl;
  ac.free();

  lang::Accessor ic=manager->allocAccessor("int32");
  ic=12345;
  std::cout<<(int)ic <<std::endl;
  ic.free();

  lang::Accessor sc=manager->allocAccessor("shader");
  ((Shader&)sc).id=100001;
  std::cout<<((Shader*)sc.getData())->id<<std::endl;
  sc.free();

  lang::Accessor psc=manager->allocAccessor("shader*");
  Shader shad;
  psc=&shad;
  shad.id=17;
  std::cout<<((Shader*)psc)->id<<std::endl;
  psc.free();

  lang::Accessor ec=manager->allocAccessor("DIFFUSE");
  ec.free();

  delete manager;

  return 0;
}
