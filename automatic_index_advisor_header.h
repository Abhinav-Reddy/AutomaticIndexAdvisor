#include<map>
#include<string.h>
#include<vector>
#include<iostream>
#include<math.h>
#include<set>
#include<algorithm>
using namespace std;
#define MAX_NO_OF_TABLES  3
#define PAGE_SIZE  4128   // page size in bytes
#define MAX_NO_OF_COLUMNS_IN_INDEX  3
#define MAX_INITIAL_SEED_SZ 8
#define MAX_INDEXES_IN_RESULT 5
#define MAX_NO_OF_QUERIES  50   
char char_space,char_comma,char_semicolon;
char char_openbracket,char_closedbracket,char_fullstop;
char *str_and,*str_or;
char *str_from,*str_where,*str_groupby,*str_having;
int no_of_tables[MAX_NO_OF_TABLES],total_queries;
string *input_queries;
double table_size[MAX_NO_OF_TABLES];
map<string,string> mapper;
map<string,int>table_id[MAX_NO_OF_QUERIES],initial_table_ids;
string id_table[MAX_NO_OF_QUERIES][MAX_NO_OF_TABLES];
int total_conditions[MAX_NO_OF_QUERIES],no_of_indexes;
string conditions[MAX_NO_OF_QUERIES][50][2];
vector<string>current_indexes[MAX_NO_OF_TABLES];
vector<string> table_cond[MAX_NO_OF_QUERIES][MAX_NO_OF_TABLES][MAX_NO_OF_TABLES][3];   
vector<string>index_cands[MAX_NO_OF_TABLES];
set<string>final_index_cands[MAX_NO_OF_TABLES];


void initialise()
{
     char_space=' ';
     char_comma=',';
     char_semicolon=';';
     char_openbracket='(';
     char_closedbracket=')';
     char_fullstop='.';
     str_from=new char[5];
     str_where=new char[6];
     str_groupby=new char[8];
     str_having=new char[7];
     str_and=new char[4];
     str_or=new char[3];
     str_from="from";
     str_where="where";
     str_groupby="group by";
     str_having="having";
     str_and="and";
     str_or="or";
}
 

struct join_information{
      double cost;
      double rows;
      vector<int>order[MAX_NO_OF_TABLES];
      void display(int n)
      {
           int i;
           cout<<cost<<" "<<rows<<endl;
           for(i=0;i<n;i++)
             cout<<order[i][0]<<" ";
           cout<<endl;  
      }
};

struct table_information{
      double cost;
      double rows;
      double row_size;
      vector<int>order;
};       



join_information join_info[40];
table_information table_info[MAX_NO_OF_TABLES],table_perm_info[MAX_NO_OF_TABLES];

struct index_info{
       long long msk;
       vector < pair<string,int> > indx;
       double cost;
       bool operator< (const index_info& rhs) const 
       {
           return cost<rhs.cost;  
       }
       void display()
       {
            cout<<cost<<" "<<msk<<" "<<endl;
            int i,j;
            for(i=0;i<indx.size();i++)
              cout<<indx[i].first<<" "<<indx[i].second<<endl;
              
       }
       };


int get_length(const char *sql)
{
    int idx=0;
    for(;sql[idx]!='\0';idx++);
    return idx;
}

                 
string to_string(char *str,int st,int end)
{
       string tmp="";
       for(;st<end;st++)
         tmp+=str[st];
       return tmp;
}  
         

inline int isstop_char(char ch)
{
    return (ch==' '  || ch==',' || ch==';' || ch=='\n');
}


int get_next_word(const char *sql,int st,int end)
{
    while(st<end && !isstop_char(sql[st]))
      st++;
    return st;
}  


inline int isequal(const char *a,int idx,const char *b)
{
       int len=get_length(b),i;
       for(i=0;i<len;i++)
          if(a[idx+i]!=b[i])
            return 0;
       return 1;
}

     
inline int isarith(char ch)
{
    return (ch=='>' || ch=='<' || ch=='!' || ch=='=');
}


int isinteger(string s)
{
    int i;
    for(i=s.length()-1;i>=0;i--)
      if(s[i]<'0' || s[i]>'9')
        return 0;
    return 1;
}


double get_reduction_factor(char str)
{
       if(str=='!')
          return 0.9;
       else
       {
           if(str=='=')
             return 0.1;
           else
             return 0.5;
       }  
}
