#pragma once

#include<geCore/dtemplates.h>
#include<iostream>
#include<vector>
#include<map>
#include<typeinfo>

namespace lang{
  class Accessor;
  class TypeManager{
    public:
      DEF_ENUM(Type,VOID,I8,I16,I32,I64,U8,U16,U32,U64,F32,F64,STRING,ARRAY,STRUCT,PTR,FCE,TYPEID);
      typedef unsigned TypeID;
    protected:
      std::vector<unsigned   >_typeStart;
      std::vector<unsigned   >_types    ;
      std::map<const char*,TypeID>_name2Id;
      std::map<TypeID,const char*>_id2name;
      bool   _typeExists(TypeID et,std::vector<unsigned>&type,unsigned*start);
      bool   _typeExists(TypeID*et,std::vector<unsigned>&type,unsigned*start);
      TypeID _typeAdd(std::vector<unsigned>&type,unsigned*start);
      bool   _incrCheck(unsigned size,unsigned*start);
      void   _bindTypeIdName(TypeID id,const char*name);
    public:
      TypeManager();
      ~TypeManager();
      unsigned getNofTypes();
      TypeID   getTypeId(unsigned index);
      unsigned getIndex(TypeID id);
      unsigned getTypeDescriptionLength(TypeID id);
      unsigned getTypeDescriptionElem  (TypeID id,unsigned i);
      Type     getTypeIdType           (TypeID id);
      Type     getElementType          (unsigned element);
      unsigned getNofStructElements    (TypeID id);
      TypeID   getStructElementTypeId  (TypeID id,unsigned element);
      unsigned getArraySize            (TypeID id);
      TypeID   getArrayInnerTypeId     (TypeID id);
      TypeID   getPtrInnerTypeId       (TypeID id);
      TypeID   getFceReturnTypeId      (TypeID id);
      unsigned getNofFceArgs           (TypeID id);
      TypeID   getFceArgTypeId         (TypeID id,unsigned element);
      TypeID   getTypeId               (const char*name);
      const char*getTypeIdName         (TypeID id);
      unsigned computeTypeIdSize       (TypeID id);

      void*alloc(TypeID id);
      Accessor allocAccessor(TypeID id);
      Accessor allocAccessor(const char*name);

      void argsToVector(std::vector<unsigned>&typeConfig,unsigned element){
        typeConfig.push_back(element);
      }
      void argsToVector(std::vector<unsigned>&typeConfig,int element){
        typeConfig.push_back((unsigned)element);
      }
      void argsToVector(std::vector<unsigned>&typeConfig,TypeManager::Type type){
        argsToVector(typeConfig,(unsigned)type);
      }
      void argsToVector(std::vector<unsigned>&typeConfig,const char*typeName){
        typeConfig.push_back(this->getTypeId(typeName));
      }
      template<typename T,typename... Args>
      void argsToVector(std::vector<unsigned>&typeConfig,T t,Args... args){
        argsToVector(typeConfig,t);
        argsToVector(typeConfig,args...);
      }

      template<typename... Args>
        unsigned addType(const char* name,Args... args){
          std::vector<unsigned>typeConfig;
          argsToVector(typeConfig,args...);
          return this->addType(name,typeConfig);
        }
      TypeID addType(const char*name,std::vector<unsigned>&type);

      //unsigned getType(std::string name);
      //
      std::string toStr(TypeID id);
      std::string toStr();
  };

  class Accessor{
    protected:
      TypeManager*  _manager;
      const void*         _data   ;
      TypeManager::TypeID _id     ;
    public:
      Accessor(TypeManager*manager,const void*data,TypeManager::TypeID id);
      TypeManager*  getManager();
      const void*         getData   ();
      TypeManager::TypeID getId     ();
      Accessor operator[](unsigned elem);
      unsigned getNofElements();
      void     free();
      template<typename T>
        T&get(){
          return *((T*)this->getData());
        }
      const void*             getPointer();
      template<typename T>
        Accessor& operator=(const T&data){
          this->get<T>()=data;
          return *this;
        }
      
      template<typename T>
        operator T()const{
          return ((Accessor*)this)->get<T>();
        }
  };
}
