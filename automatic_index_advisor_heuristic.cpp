#include "automatic_index_advisor_evaluator.cpp"

multiset< index_info > initial_seed;


void eliminate_duplicates()
{
     int i,j,k;
     for(i=0;i<MAX_NO_OF_TABLES;i++)
     {
            sort(current_indexes[i].begin(),current_indexes[i].end());                        
            for(j=1,k=0;j<current_indexes[i].size();j++)
              if(current_indexes[i][k]!=current_indexes[i][j])
                current_indexes[i][++k]=current_indexes[i][j];
            current_indexes[i].erase(current_indexes[i].begin()+k+1,current_indexes[i].end());
     }
}

                            
void display_initial_seed()
{
    int i; 
    index_info tp; 
    multiset< index_info > ::iterator it;
    cout<<endl;
    for(it=initial_seed.begin();it!=initial_seed.end();it++)
    {
          tp=*it;
          tp.display();
          cout<<endl;
    }       
}

    
void generate_query_final_indexes(int flg,int query_no)
{
    int i,j,k;
    index_info tp;
    string tmp;
     multiset< index_info   > tep,tep1;
    multiset< index_info > ::iterator it,it1,it3;                                                   
    set<int>masker;
    masker.clear();
    for(it=initial_seed.begin(),i=0;it!=initial_seed.end();it++,i++)
    {
          tp=(*it);
          tp.msk=(1<<i);
          tep.insert(tp);
    }               
    initial_seed.clear();
    for(it=tep.begin();it!=tep.end();it++)
      initial_seed.insert(*it);
    tep.clear();                                     
    for(i=0;i<MAX_INDEXES_IN_RESULT;i++)
    {
        for(it=initial_seed.begin();it!=initial_seed.end();it++)
        {
                 it1=it;                                               
                 for(it1++;it1!=initial_seed.end();it1++)
                 {
                       if(it!=it1 && (it->msk&it1->msk)==0)
                       { 
                           tp.indx.clear();
                           for(k=0;k<it->indx.size();k++)
                                tp.indx.push_back(it->indx[k]);
                           for(k=0;k<it1->indx.size();k++)
                                 tp.indx.push_back(it1->indx[k]);
                           for(k=0;k<no_of_tables[query_no];k++)
                               current_indexes[k].clear();
                           for(k=0;k<tp.indx.size();k++)
                                 current_indexes[tp.indx[k].second].push_back(tp.indx[k].first);
                           tp.msk=it->msk|it1->msk;
                           if(flg==0)
                           {
                             evaluate_query(query_no,flg);
                             tp.cost=join_info[(1<<no_of_tables[query_no])-1].cost;
                           }
                           else
                             tp.cost=evaluate_multiple_queries();  
                           tep.insert(tp);      
                       }
                  }                  
         }
         j=initial_seed.size()+tep.size();
         for(k=min(j,MAX_INITIAL_SEED_SZ)-1,it1=tep.begin(),it=initial_seed.begin();k>=0;k--)
         {
                if(it1!=tep.end() && it!=initial_seed.end())
                {                                                                     
                   if((*it).cost<=(*it1).cost)
                   {
                      tp=*it;
                      it++;
                   }
                   else
                   {
                      tp=*it1;    
                      it1++;
                   }
                }
                else
                {
                    if(it!=initial_seed.end())
                    {
                        tp=*it;
                        it++;
                    }
                    else
                    {
                        tp=*it1;
                        it1++;
                    }                        
                }
                tep1.insert(tp);
          }
          
          initial_seed.clear();
          for(it=tep1.begin(),initial_seed.clear();it!=tep1.end();it++)
             initial_seed.insert(*it);
          tep1.clear();
          tep.clear();                        
    }
    if(flg==0)
    {
       for(it=initial_seed.begin();it!=initial_seed.end();it++)
       {
           for(i=0;i<it->indx.size();i++)
           {
               tmp=it->indx[i].first.substr(2);
               for(j=0;j<tmp.length();)
               {
                    k=get_next_word(tmp.c_str(),j,tmp.length());                      
                    final_index_cands[it->indx[i].second].insert(tmp.substr(j,k));                      
                    j=k+1;                                                                
               }
           }
       }
    }    
    //display_initial_seed();                                                      
}


string get_random_string()
{
       string str="a";
       str[0]=char(rand()%2+48);
       return str;
}


void generate_query_initial_multicol_indexes(int flg,int query_no)
{
     int i,j,k;
     double d;
     string tmp;
     index_info tp;
     multiset< index_info   > tep,tep1;
     multiset<  index_info   > ::iterator it,it1;
     set<int>masker;
     for(i=0;i<MAX_NO_OF_COLUMNS_IN_INDEX;i++)
     {
          for(it=initial_seed.begin();it!=initial_seed.end();it++)
          {
                 for(it1=initial_seed.begin();it1!=initial_seed.end();it1++)
                 {
                       if(it!=it1 && it->indx[0].second==it1->indx[0].second && (it->msk&it1->msk)==0 )//&& masker.insert((it->msk|it1->msk)).second==true)
                       {
                          tmp=get_random_string()+" "+it->indx[0].first.substr(2)+" "+(*it1).indx[0].first.substr(2);
                          current_indexes[(*it).indx[0].second].push_back(tmp);
                          if(flg==0)
                          {
                             evaluate_query(query_no,flg);
                             tp.cost=join_info[(1<<no_of_tables[query_no])-1].cost;
                          }
                           else
                             tp.cost=evaluate_multiple_queries();
                          tp.msk=it->msk|it1->msk;
                          tp.indx.clear();
                          tp.indx.push_back( make_pair(tmp,it->indx[0].second) );    
                          tep.insert(tp); 
                       }
                 }
          }
          j=initial_seed.size()+tep.size();
         for(k=min(j,MAX_INITIAL_SEED_SZ)-1,it1=tep.begin(),it=initial_seed.begin();k>=0;k--)
         {
                if(it1!=tep.end() && it!=initial_seed.end())
                {                                                                     
                   if((*it).cost<=(*it1).cost)
                   {
                      tp=*it;
                      it++;
                   }
                   else
                   {
                      tp=*it1;    
                      it1++;
                   }
                }
                else
                {
                    if(it!=initial_seed.end())
                    {
                        tp=*it;
                        it++;
                    }
                    else
                    {
                        tp=*it1;
                        it1++;
                    }                        
                }
                tep1.insert(tp);
          }
          initial_seed.clear();          
          for(it=tep1.begin(),initial_seed.clear();it!=tep1.end();it++)
             initial_seed.insert(*it);
          tep1.clear();
          tep.clear();                                   
     }
     //display_initial_seed();                                                                                                                   
}


void generate_query_intial_indexes(int flg,int query_no)
{
     int i,j,k,cntr,n;
     initial_seed.clear();     
     index_info tmp;
     string s,chr;
     cntr=0;
     multiset< index_info   >:: iterator it;
     for(j=0;j<MAX_NO_OF_TABLES;j++)
     {
         for(i=0;i<index_cands[j].size();i++)
         {                         
              s=get_random_string()+" "+index_cands[j][i];
              for(k=0;k<MAX_NO_OF_TABLES;k++)
                  current_indexes[k].clear();                              
              current_indexes[j].push_back(s);
              if(flg==0)
              {
                  evaluate_query(query_no,flg);
                  tmp.cost=join_info[(1<<no_of_tables[query_no])-1].cost;
              }
              else
                  tmp.cost=evaluate_multiple_queries();
              tmp.indx.clear();
              tmp.indx.push_back(make_pair(s,j));
              tmp.msk=(1<<i);
              initial_seed.insert(tmp);
         }
         index_cands[j].clear();
     }
     while(initial_seed.size()>MAX_INITIAL_SEED_SZ)
     {
          it=initial_seed.end();
          it--;
          initial_seed.erase(it);
     }                           
     //display_initial_seed();           
}


void display_final_result()
{
     int i,j;
     index_info tp;
     multiset< index_info   >:: iterator it;
     map<string,int>:: iterator itr;
     string str[MAX_NO_OF_TABLES],s;
     for(itr=initial_table_ids.begin();itr!=initial_table_ids.end();itr++)
         str[itr->second]=itr->first;                                                                          
     for(it=initial_seed.begin();it!=initial_seed.end();it++)
     {
            tp=(*it);                                                 
            cout<<tp.cost<<endl;
            for(i=0;i<tp.indx.size();i++)
            {
               s=(tp.indx[i].first[0]=='1')?"B.S.T INDEX":"HASH INDEX";                          
               cout<<str[tp.indx[i].second]<<"      "<<s<<"       "<<tp.indx[i].first.substr(2)<<endl;
            }
            cout<<endl;
     }
     initial_seed.clear();
}

                                                      
void suggest_indexes_for_this_queries(string *str,int len)
{
     int i,j,k;
     total_queries=len;
     input_queries=new string[len];
     get_table_information();
     for(i=0;i<MAX_NO_OF_TABLES;i++)
         final_index_cands[i].clear();    
     for(i=0;i<len;i++)
         parse_query(str[i].c_str(),i);
     for(i=0;i<len;i++)
     {    
         generate_query_intial_indexes(0,i);
         generate_query_initial_multicol_indexes(0,i);
         generate_query_final_indexes(0,i);       
     }
     set<string>::iterator it;
     for(i=0;i<MAX_NO_OF_TABLES;i++)
        for(it=final_index_cands[i].begin();it!=final_index_cands[i].end();it++)
          index_cands[i].push_back(*it);
     generate_query_intial_indexes(1,1);
     generate_query_initial_multicol_indexes(1,1);
     generate_query_final_indexes(1,1);
     display_final_result();
     delete(input_queries);
}
