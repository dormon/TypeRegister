#include<iostream>
#include"data.h"

int main(){
  lang::TypeManager*manager=new lang::TypeManager();
  std::cerr<<manager->toStr()<<std::endl;
  void*vec3=manager->alloc(manager->getTypeId("float3"));
  lang::Accessor ac(manager,vec3,manager->getTypeId("float3"));
  ac[0].getF32() = 32.321f;
  std::cout<<ac[0].getF32() <<std::endl;

  delete[](char*)vec3;
  delete manager;
  return 0;
}
