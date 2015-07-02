#include<iostream>
#include"data.h"

int main(){
  lang::TypeManager*manager=new lang::TypeManager();
  std::cerr<<manager->toStr()<<std::endl;
  lang::Accessor ac=manager->allocAccessor("float3");
  ac[0] = 32.321f;
  ac[1] = 31231.f;
  std::cout<<(float)(ac[0]) <<std::endl;
  std::cout<<(float)(ac[1]) <<std::endl;
  ac.free();
  delete manager;
  return 0;
}
