#include"data.h"

#include<sstream>

using namespace lang;

template<typename T>
void printVec(std::vector<T>&data,unsigned from=0){
  for(unsigned i=from;i<data.size();++i)
    std::cerr<<data[i]<<" ";
  std::cerr<<std::endl;
}

template<typename T>
std::string vec2str(std::vector<T>&data,unsigned from=0){
  std::stringstream ss;
  ss<<"[";
  if(from<data.size())ss<<data[from];
  for(unsigned i=from+1;i<data.size();++i)
    ss<<","<<data[i];
  ss<<"]";
  return ss.str();
}

TypeManager::TypeManager(){
  this->addType("void"  ,TypeManager::VOID  );
  this->addType("int8"  ,TypeManager::I8    );
  this->addType("int16" ,TypeManager::I16   );
  this->addType("int32" ,TypeManager::I32   );
  this->addType("int64" ,TypeManager::I64   );
  this->addType("uint8" ,TypeManager::U8    );
  this->addType("uint16",TypeManager::U16   );
  this->addType("uint32",TypeManager::U32   );
  this->addType("uint64",TypeManager::U64   );
  this->addType("float" ,TypeManager::F32   );
  this->addType("double",TypeManager::F64   );
  this->addType("string",TypeManager::STRING);
}

TypeManager::TypeID TypeManager::getTypeId(const char*name){
  if(!this->_name2Id.count(name))std::cerr<<"name: "<<name<<"does not have TypeId"<<std::endl;
  return this->_name2Id[name];
}

std::string TypeManager::toStr(TypeID id){
  TypeManager::Type type=this->getTypeIdType(id);
  std::stringstream ss;
  switch(type){
    case TypeManager::VOID  :
    case TypeManager::I8    :
    case TypeManager::I16   :
    case TypeManager::I32   :
    case TypeManager::I64   :
    case TypeManager::U8    :
    case TypeManager::U16   :
    case TypeManager::U32   :
    case TypeManager::U64   :
    case TypeManager::F32   :
    case TypeManager::F64   :
    case TypeManager::STRING:
      return this->getTypeIdName(id);
    case TypeManager::ARRAY:
      ss<<"array[";
      ss<<this->getArraySize(id);
      ss<<",";
      ss<<this->toStr(this->getArrayInnerTypeId(id));
      ss<<"]";
      return ss.str();
    case TypeManager::STRUCT:
      ss<<"struct{";
      for(unsigned e=0;e<this->getNofStructElements(id);++e){
        ss<<this->toStr(this->getStructElementTypeId(id,e));
        if(e+1<this->getNofStructElements(id))ss<<",";
      }
      ss<<"}";
      return ss.str();
    case TypeManager::PTR:
      ss<<this->getTypeIdName(this->getPtrInnerTypeId(id))<<"*";
      return ss.str();
    case TypeManager::FCE:
      ss<<"(";
      for(unsigned e=0;e<this->getNofFceArgs(id);++e){
        ss<<this->toStr(this->getFceArgTypeId(id,e));
        if(e+1<this->getNofFceArgs(id))ss<<",";
      }
      ss<<")->"<<this->getTypeIdName(this->getFceReturnTypeId(id));
      return ss.str();
    case TypeManager::TYPEID:
      //ss<<"asdasd: "<<id<<"#"<<this->getTypeIdName(id)<<"#";
      return ss.str();
    default:
      return "unknown";
  }
}

std::string TypeManager::toStr(){
  printVec(this->_types);

  std::stringstream ss;
  for(unsigned t=0;t<this->getNofTypes();++t)
    ss<<this->toStr(this->getTypeId(t))<<std::endl;
  return ss.str();
}


TypeManager::~TypeManager(){

}

unsigned TypeManager::getNofTypes(){
  return this->_typeStart.size();
}

TypeManager::TypeID TypeManager::getTypeId(unsigned index){
  return index+TypeManager::TYPEID;
}

unsigned TypeManager::getIndex(TypeID id){
  return id-TypeManager::TYPEID;
}

unsigned TypeManager::getTypeDescriptionLength(TypeID id){
  unsigned index=this->getIndex(id);
  if(this->_typeStart.size()-1==index)return this->_types.size()-this->_typeStart[index];
  return this->_typeStart[index+1]-this->_typeStart[index];
}

unsigned TypeManager::getTypeDescriptionElem  (TypeID id,unsigned i){
  return this->_types[this->_typeStart[this->getIndex(id)]+i];
}

TypeManager::Type TypeManager::getElementType(unsigned element){
  if(element>=TYPEID)return TYPEID;
  return (TypeManager::Type)element;
}

TypeManager::Type TypeManager::getTypeIdType(TypeID id){
  unsigned element=this->getTypeDescriptionElem(id,this->POSITION.TYPE);
  return this->getElementType(element);
}

unsigned TypeManager::getNofStructElements(TypeID id){
  return this->getTypeDescriptionElem(id,this->POSITION.STRUCT.NOF_ELEMENTS);
}

TypeManager::TypeID TypeManager::getStructElementTypeId(TypeID id,unsigned element){
  return this->getTypeDescriptionElem(id,this->POSITION.STRUCT.INNER_TYPES_START+element);
}

unsigned TypeManager::getArraySize(TypeID id){
  return this->getTypeDescriptionElem(id,this->POSITION.ARRAY.SIZE);
}

TypeManager::TypeID TypeManager::getArrayInnerTypeId(TypeID id){
  return this->getTypeDescriptionElem(id,this->POSITION.ARRAY.INNER_TYPE);
}

TypeManager::TypeID TypeManager::getPtrInnerTypeId(TypeID id){
  return this->getTypeDescriptionElem(id,this->POSITION.PTR.INNER_TYPE);
}

TypeManager::TypeID TypeManager::getFceReturnTypeId(TypeID id){
  return this->getTypeDescriptionElem(id,this->POSITION.FCE.RETURN_TYPE);
}

unsigned TypeManager::getNofFceArgs(TypeID id){
  return this->getTypeDescriptionElem(id,this->POSITION.FCE.NOF_ARGUMENTS);
}

TypeManager::TypeID TypeManager::getFceArgTypeId(TypeID id,unsigned element){
  return this->getTypeDescriptionElem(id,this->POSITION.FCE.ARGUMENTS_START+element);
}

bool TypeManager::_incrCheck(unsigned size,unsigned*start){
  (*start)++;
  return *start<size;
}

bool TypeManager::_typeExists(TypeID et,std::vector<unsigned>&type,unsigned*start){
  //std::cout<<"TypeManager::_typeExists("<<et<<","<<vec2str(type)<<","<<*start<<")"<<std::endl;
  unsigned lastStart=*start;
  auto falseBranch=[&start,&lastStart](){*start=lastStart;return false;};
  if(*start>=type.size())return falseBranch();
  if(this->getElementType(type[*start])==TypeManager::TYPEID){
    if(et==type[*start]){
      (*start)++;
      return true;
    }
  }
  if(this->getTypeIdType(et)!=this->getElementType(type[*start]))return falseBranch();
  switch(this->getTypeIdType(et)){
    case TypeManager::VOID  :
    case TypeManager::I8    :
    case TypeManager::I16   :
    case TypeManager::I32   :
    case TypeManager::I64   :
    case TypeManager::U8    :
    case TypeManager::U16   :
    case TypeManager::U32   :
    case TypeManager::U64   :
    case TypeManager::F32   :
    case TypeManager::F64   :
    case TypeManager::STRING:
      (*start)++;
      return true;
    case TypeManager::TYPEID:
      //std::cerr<<"et: "<<et<<", type[(*start)++]: "<<type[(*start)++]<<std::endl;
      if(et!=type[(*start)++])return falseBranch();
      return true;
    case TypeManager::ARRAY:
      if(!this->_incrCheck(type.size(),start))                        return falseBranch();
      if(this->getArraySize(et)!=type[*start])                        return falseBranch();
      if(!this->_incrCheck(type.size(),start))                        return falseBranch();
      if(!this->_typeExists(this->getArrayInnerTypeId(et),type,start))return falseBranch();
      return true;
    case TypeManager::STRUCT:
      if(!this->_incrCheck(type.size(),start))        return falseBranch();
      if(this->getNofStructElements(et)!=type[*start])return falseBranch();
      if(!this->_incrCheck(type.size(),start))        return falseBranch();
      for(unsigned e=0;e<this->getNofStructElements(et);++e)
        if(!this->_typeExists(this->getStructElementTypeId(et,e),type,start))return falseBranch();
      return true;
    case TypeManager::PTR:
      if(!this->_incrCheck(type.size(),start))                      return falseBranch();
      if(!this->_typeExists(this->getPtrInnerTypeId(et),type,start))return falseBranch();
      return true;
    case TypeManager::FCE:
      if(!this->_incrCheck(type.size(),start))                       return falseBranch();
      if(!this->_typeExists(this->getFceReturnTypeId(et),type,start))return falseBranch();
      if(this->getNofFceArgs(et)!=type[(*start)++])                  return falseBranch();
      if(!this->_incrCheck(type.size(),start))                       return falseBranch();
      for(unsigned e=0;e<this->getNofFceArgs(et);++e)
        if(!this->_typeExists(this->getFceArgTypeId(et,e),type,start))return falseBranch();
      return true;
    default:
      return falseBranch();
  }
}

bool TypeManager::_typeExists(TypeManager::TypeID*id,std::vector<unsigned>&type,unsigned*start){
  for(unsigned t=0;t<this->getNofTypes();++t)
    if(this->_typeExists(this->getTypeId(t),type,start)){
      *id=this->getTypeId(t);
      return true;
    }
  return false;
}

const char*TypeManager::getTypeIdName(TypeID id){
  if(!this->_id2name.count(id))return"";
  return this->_id2name[id];
}
std::set<const char*>&TypeManager::getTypeIdSynonyms(TypeID id){
  return this->_id2Synonyms[id];
}
bool                  TypeManager::hasSynonyms      (TypeID id){
  return this->_id2Synonyms[id].size()>1;
}
bool                  TypeManager::areSynonyms      (const char*name0,const char*name1){
  return this->getTypeId(name0)==this->getTypeId(name1);
}

TypeManager::TypeID TypeManager::_typeAdd(std::vector<unsigned>&type,unsigned*start){
  //std::cout<<"TypeManager::_typeAdd("<<vec2str(type)<<","<<*start<<")"<<std::endl;
  TypeManager::TypeID id;
  if(this->_typeExists(&id,type,start))return id;
  unsigned size=0;
  TypeManager::Type newType = this->getElementType(type[*start]);
  this->_typeStart.push_back(this->_types.size());
  this->_types.push_back(newType);//write type
  (*start)++;

  switch(newType){
    case TypeManager::VOID  :
    case TypeManager::I8    :
    case TypeManager::I16   :
    case TypeManager::I32   :
    case TypeManager::I64   :
    case TypeManager::U8    :
    case TypeManager::U16   :
    case TypeManager::U32   :
    case TypeManager::U64   :
    case TypeManager::F32   :
    case TypeManager::F64   :
    case TypeManager::STRING:
      return this->getTypeId(this->getNofTypes()-1);
    case TypeManager::TYPEID:
      std::cerr<<"new type is typeid = "<<type[(*start)-1]<<" that does not exists"<<std::endl;
      return type[(*start)-1];
    case TypeManager::ARRAY:
      this->_types.push_back(type[*start]);//write length
      (*start)++;
      this->_types.push_back(this->_typeAdd(type,start));//write type
      return this->getTypeId(this->getNofTypes()-1);
    case TypeManager::STRUCT:
      size=type[*start];
      this->_types.push_back(size);//write length
      (*start)++;
      for(unsigned e=0;e<size;++e)
        this->_types.push_back(this->_typeAdd(type,start));
      return this->getTypeId(this->getNofTypes()-1);
    case TypeManager::PTR:
      this->_types.push_back(this->_typeAdd(type,start));
      return this->getTypeId(this->getNofTypes()-1);
    case TypeManager::FCE:
      this->_types.push_back(this->_typeAdd(type,start));//write return type
      size=type[*start];
      this->_types.push_back(size);//write length
      (*start)++;
      for(unsigned e=0;e<size;++e)//write args
        this->_types.push_back(this->_typeAdd(type,start));
      return this->getTypeId(this->getNofTypes()-1);
    default:
      std::cerr<<"what a stupid day..."<<std::endl;
      return TypeManager::VOID;
  }
}

void TypeManager::_bindTypeIdName(TypeID id,const char*name){
  //std::cout<<"TypeManager::_bindTypeIdName("<<id<<","<<name<<")"<<std::endl;
  this->_name2Id[name] = id  ;
  if(!this->_id2name.count(id))
    this->_id2name[id  ] = name;
  this->_id2Synonyms[id].insert(name);
}

TypeManager::TypeID TypeManager::addType(const char*name,std::vector<unsigned>&type){
  //std::cout<<"TypeManager::addType("<<name<<","<<vec2str(type)<<")"<<std::endl;
  for(unsigned t=0;t<this->getNofTypes();++t){
    unsigned start=0;
    if(this->_typeExists(this->getTypeId(t),type,&start)){
      if(this->_name2Id.count(name)&&this->_name2Id[name]!=this->getTypeId(t)){
        std::cerr<<"typeName: "<<name<<" clashes with TypeIds: "
          <<this->_name2Id[name]<<" and "<<this->getTypeId(t)<<std::endl;
        return this->getTypeId(t);
      }
      if(std::strcmp(name,""))this->_bindTypeIdName(this->getTypeId(t),name);
      return this->getTypeId(t);
    }
  }
  unsigned start=0;
  TypeID newTypeId=this->_typeAdd(type,&start);
  if(std::strcmp(name,"")){
    this->_bindTypeIdName(newTypeId,name);
    return newTypeId;
  }
  return 0;
}

unsigned TypeManager::computeTypeIdSize(TypeID id){
  TypeManager::Type type=this->getTypeIdType(id);
  unsigned size=0;
  switch(type){
    case TypeManager::VOID  :return 0;
    case TypeManager::I8    :return sizeof(char              );
    case TypeManager::I16   :return sizeof(short             );
    case TypeManager::I32   :return sizeof(int               );
    case TypeManager::I64   :return sizeof(long long int     );
    case TypeManager::U8    :return sizeof(unsigned char     );
    case TypeManager::U16   :return sizeof(unsigned short    );
    case TypeManager::U32   :return sizeof(unsigned          );
    case TypeManager::U64   :return sizeof(unsigned long long);
    case TypeManager::F32   :return sizeof(float             );
    case TypeManager::F64   :return sizeof(double            );
    case TypeManager::STRING:return sizeof(std::string       );
    case TypeManager::PTR   :return sizeof(void*             );
    case TypeManager::ARRAY:
                             return this->getArraySize(id)*this->computeTypeIdSize(this->getArrayInnerTypeId(id));
    case TypeManager::STRUCT:
                             for(unsigned e=0;e<this->getNofStructElements(id);++e)
                               size+=this->computeTypeIdSize(this->getStructElementTypeId(id,e));
                             return size;
    case TypeManager::FCE:
                             size+=this->computeTypeIdSize(this->getFceReturnTypeId(id));
                             for(unsigned e=0;e<this->getNofFceArgs(id);++e)
                               size+=this->computeTypeIdSize(this->getFceArgTypeId(id,e));
                             return size;
    default:
                             std::cerr<<"uta aus aus gerichtic himla!"<<std::endl;
                             return 0;
  }
}

void* TypeManager::alloc(TypeID id){
  unsigned size=this->computeTypeIdSize(id);
  return (void*)(new char[size]);
}

Accessor TypeManager::allocAccessor(TypeID id){
  return Accessor(this,this->alloc(id),id);
}

Accessor TypeManager::allocAccessor(const char*name){
  return this->allocAccessor(this->getTypeId(name));
}

Accessor::Accessor(TypeManager*manager,const void*data,TypeManager::TypeID id){
  this->_manager = manager;
  this->_data    = data   ;
  this->_id      = id     ;
}
TypeManager*        Accessor::getManager(){return this->_manager;}
const void*         Accessor::getData   (){return this->_data   ;}
TypeManager::TypeID Accessor::getId     (){return this->_id     ;}

Accessor Accessor::operator[](unsigned elem){
  TypeManager::TypeID innerType = 0;
  unsigned            offset    = 0;
  switch(this->getManager()->getTypeIdType(this->_id)){
    case TypeManager::ARRAY :
      innerType = this->getManager()->getArrayInnerTypeId(this->getId());
      offset    = this->getManager()->computeTypeIdSize(innerType)*elem;
      return Accessor(this->getManager(),((char*)this->getData())+offset,innerType);
    case TypeManager::STRUCT:
      innerType = this->getManager()->getStructElementTypeId(this->getId(),elem);
      for(unsigned i=0;i<elem;++i)
        offset += this->getManager()->computeTypeIdSize(this->getManager()->getStructElementTypeId(this->getId(),i));
      return Accessor(this->getManager(),((char*)this->getData())+offset,innerType);
    default:
      return Accessor(this->getManager(),this->getData(),this->getId());
  }
}

unsigned Accessor::getNofElements(){
  switch(this->_id){
    case TypeManager::ARRAY :
      return this->getManager()->getArraySize(this->getId());
    case TypeManager::STRUCT:
      return this->getManager()->getNofStructElements(this->getId());
    default                 :
      return 0;
  }
}

void Accessor::free(){
  delete[](char*)this->_data;
  this->_data = NULL;
  this->_id   = 0;
}

const void*Accessor::getPointer(){return (void*)this->getData();}


