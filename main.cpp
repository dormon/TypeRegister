#include<iostream>
#include"data.h"

int main(){
  lang::TypeManager*manager=new lang::TypeManager();
  std::cerr<<manager->toStr()<<std::endl;
  delete manager;
  return 0;
}
