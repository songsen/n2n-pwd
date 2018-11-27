#include "sql.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "n2n_wire.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>   
#include <stdio.h>   
#include <time.h>   
#include <string.h>   

char table_ip[] = "n2n_register_ip";
char table_user[] = "n2n_register_user";
//id = userid
char table_ip_descr[] = "ID BIGINT ,  \
                     IP BIGINT NOT NULL PRIMARY KEY ,\
                     ERROR BIGINT DEFAULT 0,\
                     MAC BIGINT, \
                     DEP INT,\
                     update_time timestamp default current_timestamp on update current_timestamp" ;

char table_user_descr[] = " USERID BIGINT NOT NULL PRIMARY KEY ,\
                           PASSWD VARCHAR(20),\
                           IP BIGINT ,\
                           DEP INT,\
                           update_time timestamp default current_timestamp on update current_timestamp";

int Rand(int fanwei)
{
    srand((unsigned)time(NULL)); //用于保证是随机数
    return rand()%fanwei;  //用rand产生随机数并设定范围
}

n2n_REGISTER_SUPER_t            *regs;
time_t timep;  
char s[30];  

int main (int argc, char *argv[]) 
{  
   // MYSQL_RES *res;
   // MYSQL_ROW row;

   //char custm_ip[64]={0};
 
    Connection("localhost", "root", "yss", "n2n"); 
    

    regs = malloc(sizeof(n2n_REGISTER_SUPER_t));
    //sender_sock = malloc(sizeof(struct sockaddr_in));
    
    if(Exist(table_ip) == 0)
        Create( table_ip,table_ip_descr );
    if(Exist(table_user)==0){
        Create(table_user,table_user_descr);
        Insert(table_user,"USERID","10086");
    }
    memcpy(&(regs->account),"10086",sizeof("10086") );

    //memcpy(sender_sock.sin_addr , inet_aton(127.0.0.1))

    struct sockaddr_in              sender_sock;
    sender_sock.sin_addr.s_addr = inet_addr("127.0.0.1");

    int i=2;
    while(i--)
    {
        //n2n_account_t
        //
        itoa_my(Rand(0xffff),(char*)&(regs->account),10) ;

        printf("check the ----------------------------------------------------------%d\n",i);

        if(NumRow(table_ip,"ID",(char *)regs->account)>50)//一个ID可以带50台设备
            printf("too many device \n");//return 0;
        else
            printf("number of device is ok!\n");

        char* pstr ;
        char* custm_ip ;
        custm_ip = inet_ntoa( sender_sock.sin_addr);//网络地址转换

        //memcpy(custm_ip,pstr,strlen(pstr));
        
        pstr = custm_ip;    
        for( ;*pstr != 0 ;pstr++ )
        {
            if(*pstr == '.')
                *pstr = '0';
        }

        if(Find(table_ip,"IP",custm_ip) == 0){//没有记录当前IP，则记录当前IP
            Insert(table_ip,"IP",custm_ip);
        }else{
            printf("find the same IP=%s \n",custm_ip);
        }

        if(Query(table_ip,"ERROR",custm_ip)>12)//查询错误次数，错误大于n，则返回
            printf("too many ERROR log\n");//return 0;
        else{
            printf("ERROR turn is ok! \n");
        }

        if(Find(table_user,"USERID",(char*)&(regs->account ))==0){//账号错误，则返回
            Addup(table_ip,"ERROR",custm_ip);
            printf("ERROR addup\n");//return 0;
        }else{
            printf("there is USERID =%s \n",(char*)&(regs->account ));
        }

        if(Query(table_ip,"ID",custm_ip) != atoi((char*)&(regs->account)))//查询当前IP的账号，如果
            Update(table_ip,(char*)&(regs->account) ,custm_ip);
        else{
            printf("there is current acount =%d ok! \n",atoi((char*)&(regs->account)));
        }

    }
    free(regs);
    //free(sender_sock);
    CloseSql();

          
        time(&timep);  
      
        printf("%s\n", ctime(&timep));  


    exit(EXIT_SUCCESS);  
  }  
