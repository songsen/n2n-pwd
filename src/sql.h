
/*
 * return 0 success 
 * else return NOT NULL
*/
#ifndef SQL_H_
#define SQL_H_
#include <stdint.h>

int Connection(const char* host, const char* user, const char* password, const char* database);


int Create(const char* table,const char* rel);


int Insert(const char* table,const char* rel,const char* values); 

int Find( const char* table,char* field,const char* str );

int Addup(const char* table,const char* filed,const char* ip) ;

int Exist(const char* table_name);

int Query(const char* table,const char* filed,const char* ip);

void delete() ;

void CloseSql();

int itoa_my(uint32_t value,char *string,int radix) ;

void Update(const char* table ,const char* account ,const char * ip);

int NumRow(const char* table,const char* field,const char* account);

#endif
//int SqlExist( const char* from,const char* field,const char* str2 );

//char *itoa_my(int value,char *string,int radix)  ;
