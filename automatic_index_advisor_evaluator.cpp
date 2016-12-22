#include "automatic_index_advisor_parser.cpp"


void execute_single_cond(char str,int idx,string col)
{
       int i;
       double d=get_reduction_factor(str); 
       table_info[idx].rows*=d;
       if(table_info[idx].order.size()==0)
       {
              for(i=0;i<current_indexes[idx].size();i++)
              {
                  if(is_suffix_matching(current_indexes[idx][i].substr(2),col))
                     break;
              }
              if(i==current_indexes[idx].size())
                i=-1;
              else
                table_info[idx].order.push_back(i);  
       }
       else
           i=table_info[idx].order[0];
       if(i>=0)
       {
            if(current_indexes[idx][i][0]=='1')
                 table_info[idx].cost+=(1.2*ceil(table_info[idx].rows*table_info[idx].row_size/PAGE_SIZE));
            else
                 table_info[idx].cost+=(3+ceil(table_info[idx].rows*table_info[idx].row_size/PAGE_SIZE));      
       }
       else
             table_info[idx].cost+=ceil((table_info[idx].rows/d*table_info[idx].row_size)/PAGE_SIZE);
}    
         


void get_table_info(int query_no)
{
     int i,j;
    for(i=0;i<no_of_tables[query_no];i++)
        table_info[i].order.clear(),table_info[i].cost=0;
    for(i=0;i<no_of_tables[query_no];i++)
    {
         j=initial_table_ids[id_table[query_no][i]];                      
         table_info[i].rows=table_perm_info[j].rows;
         table_info[i].row_size=table_perm_info[j].row_size;
    }  
}


int evaluate_single_conditions(int query_no)
{
     get_table_info(query_no);
     int idx,i,j,k;
     string tmp;
     for(i=0;i<total_conditions[query_no];i++)
     {
            if(conditions[query_no][i][1]=="0")
            {
                  j=parse_till_this_char(conditions[query_no][i][0].c_str(),0,conditions[query_no][i][0].length(),char_fullstop);              
                  for(k=j+1;k<conditions[query_no][i][0].length() && !isarith(conditions[query_no][i][0][k]);k++);
                  tmp=conditions[query_no][i][0].substr(j+1,k-(j+1));
                  j=table_id[query_no][conditions[query_no][i][0].substr(0,j)];
                  execute_single_cond(conditions[query_no][i][0][k],j,tmp);                  
            }  
     }
     for(i=0;i<(1<<no_of_tables[query_no]);i++)
     {
           join_info[i].cost=0;
           join_info[i].rows=0;
           for(j=0;j<no_of_tables[query_no];j++)
           {
                join_info[i].order[j].clear();
                join_info[i].order[j].push_back(-1);
           }
     }                           
     for(i=0;i<no_of_tables[query_no];i++)
     {
            join_info[1<<i].cost=table_info[i].cost;
            join_info[1<<i].rows=table_info[i].rows;
            if(table_info[i].order.size()>0)
            {
              join_info[1<<i].order[i].clear();                              
              for(j=0;j<table_info[i].order.size() && j<4;j++)
                 join_info[1<<i].order[i].push_back(table_info[i].order[j]);
            }
     }                          
}


join_information temp_info[2];


void display_result(int query_no)
{
     int i,j,k;
     for(i=0;i<(1<<no_of_tables[query_no]);i++)
         join_info[i].display(no_of_tables[query_no]);
           
}

 
void get_indexes_to_evaluate(int query_no)
{
     int i,j,k;
     vector<string> tmp[MAX_NO_OF_TABLES];
     string str;
     for(i=0;i<no_of_tables[query_no];i++)
     {
          str=id_table[query_no][i];                                
          j=initial_table_ids[str];                                
          for(k=0;k<current_indexes[j].size();k++)
            tmp[i].push_back(current_indexes[j][k]);                                   
     }
     for(i=0;i<MAX_NO_OF_TABLES;i++)
        current_indexes[i].clear();
     for(i=0;i<no_of_tables[query_no];i++)
       for(j=0;j<tmp[i].size();j++)
         current_indexes[i].push_back(tmp[i][j]);
}
 
  
void copy_into_temp(int idx,int i,int query_no)
{
     int j,k,m;
     temp_info[i].cost=join_info[idx].cost;
     temp_info[i].rows=join_info[idx].rows;
     for(j=0;j<no_of_tables[query_no];j++)
     {
           if(temp_info[i].order[j].size()>0)                     
              temp_info[i].order[j][0]=join_info[idx].order[j][0];
           else
              temp_info[i].order[j].push_back(join_info[idx].order[j][0]);
     }    
}


void copy_from_temp(int idx,int i,int query_no)
{
     int j,k,m;
     join_info[idx].cost=temp_info[i].cost;
     join_info[idx].rows=temp_info[i].rows;
     for(j=0;j<no_of_tables[query_no];j++)
         join_info[idx].order[j][0]=temp_info[i].order[j][0];                          
}


void get_indexes(int idx2,int idx,string col)
{
     int i,j;
     if(join_info[idx2].order[idx][0]==-1)
          join_info[idx2].order[idx].clear();
     for(i=0;i<current_indexes[idx].size() ;i++)
     {
           if(is_suffix_matching(current_indexes[idx][i].substr(2),col))
             join_info[idx2].order[idx].push_back(i);
     }
     if(join_info[idx2].order[idx].size()==0)
        join_info[idx2].order[idx].push_back(-1);
}
                                   

double get_cost_estimate(int idx1,int idx,char ch,string col,int i)
{
     int j,k,m;
     double d=get_reduction_factor(ch);
     k=join_info[idx1].order[idx][i];
     if(k>=0 && is_suffix_matching(current_indexes[idx][k].substr(2),col))
     {
           if(current_indexes[idx][k][0]=='1')
                return (1.2*ceil(table_info[idx].rows*table_info[idx].row_size/PAGE_SIZE));
           else
               return (3+ceil(table_info[idx].rows*table_info[idx].row_size/PAGE_SIZE));
     }
     else
           return ceil((table_info[idx].rows*table_info[idx].row_size)/(d*PAGE_SIZE));    
}


void copy_to_info(int idx1,int idx2,int p,double d,int idx,int query_no)
{
     int q;
     if( join_info[idx1|idx2].cost==0 || join_info[idx1|idx2].cost >d)
     {
           join_info[idx1|idx2].cost=d;           
           for(q=0;q<no_of_tables[query_no];q++)
              join_info[idx1|idx2].order[q][0]=join_info[idx1].order[q][0];
           join_info[idx1|idx2].order[idx][0]=join_info[idx2].order[idx][p];
     }
}


void join_these(int idx1,int idx2,int query_no)
{
     int i,j,k,p,idx,fg=0,q,r;
     double d;
     if(idx1==0)
        return;
     for(idx=0;(1<<idx)!=idx2;idx++);
     copy_into_temp(idx1,0,query_no);
     copy_into_temp(idx2,1,query_no);
     d=join_info[idx1].cost+join_info[idx2].cost;
     d+=join_info[idx1].rows*ceil(join_info[idx2].rows*table_info[idx].row_size/PAGE_SIZE);
     copy_to_info(idx1,idx2,0,d,idx,query_no);
     j=join_info[idx2].order[idx][0];
     for(k=0;k<no_of_tables[query_no];k++)
     {
         if((1<<k)&idx1)
         { 
          for(i=0;i<table_cond[query_no][idx][k][0].size();i++)                                       
              if(j==-1)
                 get_indexes(idx2,idx,table_cond[query_no][idx][k][0][i]);
         }
      }
       double db[MAX_NO_OF_TABLES],red=1;
       for(k=0;k<no_of_tables[query_no];k++)
       {
         if((1<<k)&idx1)
         { 
          for(i=0;i<table_cond[query_no][idx][k][0].size();i++)
          { 
              d=get_reduction_factor(table_cond[query_no][idx][k][1][i][0]);
              table_info[idx].rows*=d;
              red*=d;
              for(p=0;p<MAX_NO_OF_TABLES;p++)
                  db[p]=0;
              for(p=0; p<join_info[idx2].order[idx].size();p++)
              {
                  d=join_info[idx1].rows*get_cost_estimate(idx2,idx,table_cond[query_no][idx][k][1][i][0],table_cond[query_no][idx][k][0][i],p);
                  d+= join_info[idx1].cost + join_info[idx2].cost;
                  db[p]+=d;
              }
              for(p=0;p<join_info[idx2].order[idx].size();p++)
                  copy_to_info(idx1,idx2,p,db[p],idx,query_no); 
              table_info[idx].rows/=get_reduction_factor(table_cond[query_no][idx][k][1][i][0]);                                                                                   
          }
         }
       } 
      join_info[idx1|idx2].rows=join_info[idx1].rows*join_info[idx2].rows*red;
     copy_from_temp(idx1,0,query_no);
     copy_from_temp(idx2,1,query_no);                                   
}




void evaluate_query(int query_no,int flg)
{
    get_indexes_to_evaluate(query_no);
    evaluate_single_conditions(query_no);        
    int i,j,idx;
    for(i=1;i<(1<<no_of_tables[query_no]);i++)
    {
        for(j=1;j<i;j=(j<<1))
          if(i&j)        
             join_these(i^j,j,query_no);
    } 
    for(i=0;i<MAX_NO_OF_TABLES;i++)
       current_indexes[i].clear();                                       
}



double evaluate_multiple_queries()
{
     int i,j,k;
     double d=0;
     int query_no;
     string str;
     vector<string>tmp[MAX_NO_OF_TABLES];
     for(i=0;i<MAX_NO_OF_TABLES;i++)
     {
          for(k=0;k<current_indexes[i].size();k++)
            tmp[i].push_back(current_indexes[i][k]);
          current_indexes[i].clear();
     }                                     
     for(query_no=0;query_no<total_queries;query_no++)
     {
           for(i=0;i<MAX_NO_OF_TABLES;i++)
              for(k=0;k<tmp[i].size();k++)
                current_indexes[i].push_back(tmp[i][k]);                                           
           evaluate_query(query_no,1);                             
           d+=join_info[(1<<no_of_tables[query_no])-1].cost; 
     }
     for(i=0;i<MAX_NO_OF_TABLES;i++)
        current_indexes[i].clear();
     return d;
}

