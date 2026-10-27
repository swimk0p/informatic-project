#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stdlib.h>

class User{
    int USER_id;
    std::string username;
    std::string password;
    int user_role;

    //контроль над базой данных
    public:
		int create_BTS(User *user_n){//BTS - behind the scene

			user_n[0].USER_id=1;
			user_n[0].username="ADMIN";
			user_n[0].password="15378900";
			user_n[0].user_role=1;

		}
        int create(User *p,int n){
	        std::cout<<"Enter Name: ";
	        std::cin >> p[n].username;

            std::cout<<"Enter Password: ";
	        std::cin >> p[n].password;
	        p[n].USER_id=1+n+1;
            p[n].user_role=0;
            
        	std::cout<<"\nRECORD INSERTED...\n";
	        n++;
	    return n;
	    };

        void Search(User *p,std::string us_name,int n){
	        int i=0;
	        for( i=0;i<n;i++){
	            if(p[i].username==us_name){
		            std::cout<<"USER_id\tusername\tpassword\tuser_role\n";
	    	        std::cout<<USER_id<<"\t"<<username<<"\t"<<password<<"\t"<<user_role<<"\n";
		            break;
    		        }
	        }
	        if(p[i].username!=us_name){
	            std::cout<<"\nRECORD NOT FOUND.\n";
	            }
        }

        int Del(User *p,int n,std::string us_name){
		int j=0,k,flag=0;
		for(j=0;j<n;j++)
		{
			if(p[j].username==us_name){
                flag=1;
                break;
            }
		}
        if(flag==1){
            for(k=j;k<n;k++){
            p[k]=p[k+1];
            }
            std::cout<<"\nRECORD DELETED.\n";
            return n-1;
        }else{
				std::cout<<"\nRecord Not Found\n";
				return n;
		    }
	    };
		int Update(User *p, int id_n,int n){
			int i,ch1;
			for(i=0;i<n;i++){
			if(p[i].USER_id==id_n);
			while(1){
    			std::cout<<"\nOPTIONS IN UPDATE\n";
    			std::cout<<"\n 1. Update NAME";
    			std::cout<<"\n 2. Update ROLE";
   				std::cout<<"\n\n Enter Your Choice:";
   				std::cin>>ch1;    				
				switch(ch1){
    				case 1: std::cout<<"NAME:";
    						std::cin>>p[i].username;
    						std::cout<<"Record Updated...\n";
    						break;
   					case 2: std::cout<<"ROLE:\t";
   							std::cin>>p[i].user_role;
   							std::cout<<"Record Updated...\n";
   							break; 
				}
			break;
			}
			if(p[i].USER_id==id_n){
    			std::cout<<"\nRecord Not Found\n\n";
			}
		}
}
		void Display(){
			std::cout<<USER_id<<"\t"<<username<<"\t"<<password<<"\t"<<user_role<<"\n";
		}
};

class User_role{
    public:
        int USER_role;
        std::string role_name;
};

/*
class DatabaseManager  {
    public:
        int create(User *p,int n){
	        std::cout<<"Enter Name: ";
	        std::cin >> p[n].username;
            std::cout<<"Enter Password: ";
	        std::cin >> p[n].password;
	        p[n].USER_id=1+n+1;

        	std::cout<<"\nRECORD INSERTED...\n";
	        n++;
	return n;
	}
};
*/
int main(){
std::cout <<"hello world"<<"\n"<<"welcome to hot_coffe! \n";

int i=1;
User u_f[10];
/*
u_f[0].USER_id=1;
u_f[0].username="ADMIN";
u_f[0].password="15378900";
u_f[0].user_role=1;
*/
u_f[i].create_BTS(u_f);

//u_f.create();
u_f[0].Display();
/*
User user_n;
user_n.USER_id=1;
user_n.username="ADMIN";
user_n.password="15378900";
user_n.user_role=1;

*/


return 0;
}