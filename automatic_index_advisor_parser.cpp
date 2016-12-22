#include "automatic_index_advisor_header.h"


pair<int,int> parse_till_arithmetic(char *sql,int st,int end)
{
    int idx,i,j,k;
    pair< int,int >tmp;
    for(idx=st;idx<end;)
    {
         j=get_next_word(sql,idx,end);
         if(j-idx==3)
         {
              if(isequal(sql,idx,str_and))
                  return make_pair(idx-1,j);
         }
         idx=j+1;
    }                                        
    tmp.first=end,tmp.second=end;                                 
    return tmp;
}

   
int parse_till_stops(char *sql,int st,int end)
{
    for(;st<end;st++)
      if(isstop_char(sql[st]))
        break;   
    return st;
}


int parse_till_this_string(char *sql,int st,int end,char *str)
{
    int idx,j,i;
    int len=get_length(str);
    for(idx=st;idx<end;)
    {
        j=get_next_word(sql,idx,end);
        for(i=0;i<len && j-idx==len;i++)
          if(sql[idx+i]!=str[i])
            break;
        if(i==len)
          return idx;
        else
          idx=j+1;
    }
    return idx-1-get_length(str);
}   


int parse_till_this_char(const char *sql,int st,int end,char ch)
{
    int idx,i;
    for(idx=st;idx<end;idx++)
      if(sql[idx]==ch)
        break;
    return idx;
}


bool is_suffix_matching(string a,string b)
{
     int i,j,len=a.length();
     i=0;
     while(i<len)
     {
           j=get_next_word(a.c_str(),i,len);
           if(j-i==b.length() && isequal(a.c_str(),i,b.c_str()))
              return 1; 
           i=j+1;
     }
     return 0;
}
       

void get_table_information()
{
    int i,n; 
    string table_name;
    cin>>n;
    for(i=0;i<n;i++)
        table_perm_info[i].order.clear(),table_perm_info[i].cost=0;
    /*    
    cout<<"enter table information"<<endl;
    for(i=0;i<n;i++)
    {
         cin>>table_name;
         initial_table_ids[table_name]=i;
         cin>>table_perm_info[i].rows>>table_perm_info[i].row_size;
    }
    */
}



int* get_idx(char *sql,int len)
{
     int i,j;
     static int idx[4];
     for(i=1,j=0;i<len;i++)
     {
         if(sql[i]==' ' && sql[j]==' ')
           ;
         else
           sql[++j]=sql[i];
     }
     len=j+1;
     sql[len]='\0';                          
     idx[0]=parse_till_this_string(sql,0,len,str_from)+5;
     idx[1]=parse_till_this_string(sql,idx[0],len,str_where)+6;
     idx[2]=parse_till_this_string(sql,idx[1],len,str_groupby)+9;
     idx[3]=parse_till_this_string(sql,idx[2],len,str_having)+7;
     return idx;   
}


void parse_from_statement(char *sql,int st,int end,int query_no)
{
     int i,j,k;
     string a,b;
     mapper.clear();
     while(st<end)
     {
          j=parse_till_stops(sql,st,end);
          if(j==end)
             return;
          for(k=st,a="";k<j;k++)
                a+=sql[k];   
          if(sql[j]!=',')
          {
              i=parse_till_stops(sql,j+1,end);
              b="";
              for(k=j+1;k<i;k++)
                b+=sql[k];  
              mapper[b]=a;
              mapper[a]=a;
              j=i;
          }  
          else
              mapper[a]=a;
          st=j+1;
     }
}


int requires_join(int idx,int query_no)
{
     int i,j,fg=1,k;
     string tmp,res="",temp[5];
     for(i=0;i<5;i++)
       temp[i]="";
     for(i=0;i<conditions[query_no][idx][0].length() && conditions[query_no][idx][0][i]!='.';i++) ;
     tmp=conditions[query_no][idx][0].substr(0,i);
     temp[0]=mapper[tmp];
     res+=mapper[tmp]+conditions[query_no][idx][0][i];
     for(j=i+1;j<conditions[query_no][idx][0].size() && !isarith(conditions[query_no][idx][0][j]) ;j++);
     temp[1]=conditions[query_no][idx][0].substr(i+1,j-(i+1));
     if(table_id[query_no].find(temp[0])==table_id[query_no].end())
     {
          table_id[query_no][temp[0]]=no_of_tables[query_no]++;
          id_table[query_no][no_of_tables[query_no]-1]=temp[0];
     }
     index_cands[initial_table_ids[ temp[0]] ].push_back(temp[1]);
     temp[2]+=conditions[query_no][idx][0][j];
     if(isarith(conditions[query_no][idx][0][j+1]))
     {
           j++;
           temp[2]+=conditions[query_no][idx][0].substr(j,1);
     }
         
     res+=conditions[query_no][idx][0].substr(i+1,j-(i+1)+1);  
     for(i=j+1;i<conditions[query_no][idx][0].size() && conditions[query_no][idx][0][i]!='.';i++);
     tmp=conditions[query_no][idx][0].substr(j+1,i-j-1);
     if(isinteger(tmp) || i==conditions[query_no][idx][0].size())
     {
        res+=tmp;
        fg=0;
     }
     else
     {  
         res+=mapper[tmp];
         temp[3]=mapper[tmp];
         temp[4]=conditions[query_no][idx][0].substr(i+1);
     }
     res+=conditions[query_no][idx][0].substr(i);
     conditions[query_no][idx][0]=res;
     if(fg)
     {   
           if(table_id[query_no].find(temp[3])==table_id[query_no].end())
           {
                 table_id[query_no][temp[3]]=no_of_tables[query_no]++;
                 id_table[query_no][no_of_tables[query_no]-1]=temp[3];
           }                                                 
           index_cands[ initial_table_ids[ temp[3]] ].push_back(temp[4]);
           for(k=0;k<2;k++)
           {
                j=table_id[query_no][temp[0]];
                i=table_id[query_no][temp[3]];
                table_cond[query_no][j][i][0].push_back(temp[1]);
                table_cond[query_no][j][i][1].push_back(temp[2]);
                table_cond[query_no][j][i][2].push_back(temp[4]);
                swap(temp[0],temp[3]);
                swap(temp[1],temp[4]);
           }
     }
     return fg;
}
      

void display_tableconds(int query_no)
{
     int i,j,k,p;
     for(i=0;i<no_of_tables[query_no];i++)
     {
          cout<<i<<endl;                      
          for(j=0;j<no_of_tables[query_no];j++)
          {
              for(k=0;k<table_cond[query_no][i][j][0].size();k++)
                 for(p=0;p<3;p++)  
                   cout<<table_cond[query_no][i][j][p][k]<<" ";  
              cout<<"    ";
          }                                                                         
          cout<<endl;          
     }
}


void parse_while_statement(char *sql,int st,int end,int query_no)
{
     total_conditions[query_no]=0;
     no_of_tables[query_no]=0;
     int idx=st,i,j,k;
     pair<int,int> tmp;
     while(idx<end)
     {
          tmp=parse_till_arithmetic(sql,idx,end);
          for(i=0;i<2;i++)
             conditions[query_no][total_conditions[query_no]][i]="";
          for(i=idx;i<tmp.first;i++)
             if(sql[i]!=' ')
               conditions[query_no][total_conditions[query_no]][0]+=sql[i];     
          if( requires_join(total_conditions[query_no],query_no) )
                conditions[query_no][total_conditions[query_no]][1]="1";
          else
                conditions[query_no][total_conditions[query_no]][1]="0";
          total_conditions[query_no]++;            
          idx=tmp.second+1;   
     }          
}

                                         
void parse_query(const char *tmp,int query_no)
{
     int i,j,len;
     int *idx=new int[4];
     len=get_length(tmp);
     char *sql=new char[len+1];
     for(i=0;i<len;i++) sql[i]=tmp[i];
     idx=get_idx(sql,len);
     len=get_length(sql);
     if(idx[1]==len)
         return;
     parse_from_statement(sql,idx[0],idx[1],query_no);
     parse_while_statement(sql,idx[1],idx[2],query_no);
     delete(sql);
     delete(idx);        
}
        

void display_joins(int query_no)
{
     int i;
     for(i=0;i<total_conditions[query_no];i++)
       cout<<conditions[query_no][i][0]<<" "<<conditions[query_no][i][1]<<endl;
}



void display_current_indexes(int query_no)
{
     int i,j;
     vector<string>tmp[50];
     for(i=0;i<no_of_tables[query_no];i++,cout<<endl)
     {
       for(j=0;j<current_indexes[i].size();i++)
        cout<<initial_table_ids[id_table[query_no][i]]<<" "<<current_indexes[i][j]<<" ";       
      cout<<endl;
     }
}
