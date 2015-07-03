#include<iostream>
#include"data.h"

int main(){
  lang::TypeManager*manager=new lang::TypeManager();
  manager->addType("float4",lang::TypeManager::ARRAY,4,"float");
  manager->addType("float4x4",lang::TypeManager::ARRAY,4,"float4");

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
