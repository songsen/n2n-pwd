#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <mysql/errmsg.h>  
#include <mysql/mysqld_error.h> 
#include "sql.h"
#include <mysql/mysql.h>
#include <time.h>  

MYSQL *conn; 
time_t timep;  
//char strDdate[30];   

// const char* host;
// const char* user;
// const char* password;
// const char* database;

static int MysqlPing(const char* str)
{
    time(&timep);  

    int i=mysql_ping(conn);
    if(i == 0){
        fprintf(stderr, "%s: %s error %d: %s\n",ctime(&timep),str, mysql_errno(conn), mysql_error(conn));
    }
    else if(i == CR_SERVER_GONE_ERROR){
        mysql_close(conn); 
        fprintf(stderr, "%s: disconnect with server %d: %s\n",ctime(&timep),mysql_errno(conn), mysql_error(conn));  
    }
    else if(CR_UNKNOWN_ERROR){
        fprintf(stderr, "%s: unknow error occur error %d: %s\n",ctime(&timep),mysql_errno(conn), mysql_error(conn));
    }
    return i;
}

int Connection(const char* host, const char* user, const char* password, const char* database) 
{  
  conn = mysql_init(NULL);

  char value = 1;

  mysql_options(conn, MYSQL_OPT_RECONNECT, (char *)&value);

  if (mysql_real_connect(conn, host, user, password, database, 0, NULL, 0)) {  
      printf("Connection success!\n");
      return 0;//  
  } else {  
      if (mysql_errno(conn)) {  
          fprintf(stderr, "Connection error %d: %s\n", mysql_errno(conn), mysql_error(conn));  
      }  
      fprintf(stderr, "Connection failed and program will exit!\n");
      exit(EXIT_FAILURE);  
  }  
}  

int Create(const char* table,const char* format)
{
    char* str = (char*)calloc(1024,sizeof(char) );
    sprintf(str,"create table %s(%s)",table,format);
    //printf("create %s\n",str);
    int res = mysql_query(conn,str); free(str);
    if(res == 0){
        printf("create table %s scuccess!\n",str);
        return 0;
    }else {  
        return MysqlPing("create error");
        //fprintf(stderr, "create error %d: %s\n", mysql_errno(conn), mysql_error(conn));  
    }  
}

int Insert(const char* table,const char* format,const char* values) {  
  char* str = (char*)calloc(1024,sizeof(char) );
  sprintf(str,"INSERT INTO %s(%s) VALUES(%s)",table,format,values);
  //printf("Insert %s\n",str);
  //"INSERT INTO student(student_no,student_name) VALUES('123465', 'Ann')"
  int res = mysql_query(conn, str); free(str);
  if (!res) {  
      printf("Inserted %lu rows\n", (unsigned long)mysql_affected_rows(conn));  
  } else {  
        MysqlPing(" insert error ");
      //fprintf(stderr, "Insert error %d: %s\n", mysql_errno(conn), mysql_error(conn));  
  }  
  return res;
}  
void Update(const char* table ,const char* account ,const char * ip)
{
    char* str = (char*)calloc(1024,sizeof(char) );
    sprintf(str,"UPDATE %s \
                SET ID=%s\
                WHERE IP=%s",\
            table,account,ip);

    int res = mysql_query(conn,str );  
    if (!res) {  
        printf("Update %lu rows is OK!\n", (unsigned long)mysql_affected_rows(conn));  
    } else { 
        MysqlPing(" Update error");
        //fprintf(stderr, "Update error %d: %s\n", mysql_errno(conn), mysql_error(conn)); 
    } 
    free(str);
}
int NumRow(const char* table,const char* field,const char* account)
{
    char ret = 0;
    MYSQL_RES *res;

    char* buf = (char*)calloc(1024,sizeof(char) );
    if(account == NULL)
        sprintf(buf,"SELECT %s FROM %s",field,table);
    else
        sprintf(buf,"SELECT %s FROM %s WHERE ID=%s",field,table,account);

    if(mysql_query(conn,buf)!=0){
        MysqlPing(" get row error!");
        //fprintf(stderr, "NumRow error %d: %s\n", mysql_errno(conn), mysql_error(conn));  
    }else{
        res = mysql_store_result(conn);
        ret = mysql_num_rows(res);
        mysql_free_result(res);
    }
    free(buf);
    return ret;
}
/*
 *  password 错误一次相应的IP地址错误累计;
 */
int Addup(const char* table,const char* field,const char* ip) {  

    char* str = (char*)calloc(1024,sizeof(char) );

    sprintf(str,"UPDATE %s a,\
                    (SELECT %s \
                        FROM %s \
                        WHERE IP=%s)b\
                 SET a.%s =b.%s+1\
                 WHERE a.IP=%s",\
            table,field,table,ip,field,field,ip);

  int res = mysql_query(conn,str );  free(str);
  if (!res) {  
      printf("addup %lu rows\n", (unsigned long)mysql_affected_rows(conn));  
  } else {  
    MysqlPing(" add up  error");
     // fprintf(stderr, "addup error %d: %s\n", mysql_errno(conn), mysql_error(conn));  
  } 
  return res;
}  

int Query(const char* table,const char* field,const char* ip)
{
    int ret = 0;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* cmd = (char*)calloc(1024,sizeof(char) );

    sprintf(cmd,"SELECT %s\
                 FROM %s\
                 WHERE IP=%s",field,table,ip); 

    if(mysql_query(conn,cmd)!=0){
        MysqlPing(" queery error");
        //fprintf(stderr, "query error %d: %s\n", mysql_errno(conn), mysql_error(conn));  
    }else{           
        res = mysql_store_result(conn);
        if(mysql_num_rows(res) == 0)
            ret = -1;
        else{
            row = mysql_fetch_row(res);
            if(row[0] == NULL)
                ret = -1;
            else
                ret = atoi(row[0]);
        }
        mysql_free_result(res);
    }
    free(cmd);
    return ret;
}

void delete() {  
  int res = mysql_query(conn, "DELETE table student WHERE student_no='123465'");  
  if (!res) {  
      printf("Delete %lu rows\n", (unsigned long)mysql_affected_rows(conn));  
  } else {  
    MysqlPing(" delete error");
     // fprintf(stderr, "Delete error %d: %s\n", mysql_errno(conn), mysql_error(conn));  
  }  
}  

int Find( const char* table,char* field,const char* str )
{
    char ret = 0;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* buf = (char*)calloc(1024,sizeof(char) );

    sprintf(buf,"SELECT %s FROM %s",field,table);
    if(mysql_query(conn,buf)!=0){
        MysqlPing(" find error");
        //fprintf(stderr, "Find error %d: %s\n", mysql_errno(conn), mysql_error(conn));  
    }else{
        res = mysql_store_result(conn);
        if(mysql_num_rows(res)!=0){//行数==0； 
            while ((row = mysql_fetch_row(res)) != NULL){
                if(row[0]!= NULL)
                {
                    if(0 != strcmp(row[0],str)){  
                        continue;
                    }else{
                        ret = 1;
                        printf("find the same ID of %s\n",str);
                        break;
                    }  
                }
            }
        }
        mysql_free_result(res);
    }
    free(buf);
    return ret;
}

int Exist(const char* table_name)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    int ret = 0;
    //查询表
    if(mysql_query(conn, "show tables")) 
    {
        MysqlPing(" show error");
        //fprintf(stderr, "show tables %s\n", mysql_error(conn));
        ret = -1;
    }
    
    res = mysql_store_result(conn);
    if(mysql_num_rows(res) != 0){
        //printf("MySQL Tables in mysql database:\n");      
        while ((row = mysql_fetch_row(res)) != NULL)
        {
            //printf("%s \n", row[0]);
            if(0 == strcmp(row[0],table_name)){  
                ret = 1;
                //printf("find the same table of table of n2n_user\n");
                break;
            }  
        }
    }
    mysql_free_result(res);
    return ret;
}

void CloseSql()
{
    mysql_close(conn); 
}

int itoa_my(uint32_t value,char *string,int radix)  
{  
    char zm[37]="0123456789abcdefghijklmnopqrstuvwxyz";  
    char aa[100]={0};  
  
    uint32_t sum=value;  
    char *cp=string;  
    int i=0;  
      
    if(radix<2||radix>36)//增加了对错误的检测  
    {  
        return 1;  
    }  
  
    // if(value<0)  
    // {  
    //     return 2; 
    // }  
      
  
    while(sum>0)  
    {  
        aa[i++]=zm[sum%radix];  
        sum/=radix;  
    }  
  
    for(int j=i-1;j>=0;j--)  
    {  
        *cp++=aa[j];  
    }  
    *cp='\0';  
    return 0;  
}  

