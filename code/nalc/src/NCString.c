//
//#include "NCString.h"
//
//struct NCString{
//  const NAUTF8Char* str;
//  size_t len;
//};
//
//
//
//NCString* ncAllocString(const NAUTF8Char* str, size_t len){
//  NCString* string = naAlloc(NCString);
//  string->str = str;
//  string->len = len;
//  return string;
//}
//  
//void ncDeallocString(NCString* string){
//  naFree(string);
//}
//
//const NAUTF8Char* ncGetStringPtr(const NCString* string){
//  return string->str;
//}
//
//size_t ncGetStringLen(const NCString* string){
//  return string->len;
//}
