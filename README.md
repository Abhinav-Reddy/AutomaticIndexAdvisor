Automatic index advisor is library which can be used for getting index set suggestions for any given multiple queries. system R-optimizer algorithm was used for efficient join ordering and cost estimation.
Both B.S.T and Hash indexes are considered.

HEURISTIC:
1)   indexable columns for a given query:
     -> indexable columns for given query are those columns that appear in the while statement of 
        the  query.           

2)   generating multi-column indexes for a single query:
               -> let S be initialized with all indexable columns and the costs.
               -> for i in 0 to maximum_no_of_columns_in_an_index
                       generate all possible indexes and their costs using any two indexes in S
                       insert the newly generated indexes into S
                       apart from the  K lowest cost indexes, rest are deleted.
3)   generating index sets for a single query:
               ->let S be initialized with multi-column indexes generated using above heuristic
               ->for i in 0 to maximum_no_of_indexes_in_an_suggestion
                       generate all possible index sets and their costs using any two index sets in S
                       insert the newly generated index sets into S
                       apart from the  K lowest cost index sets, rest are deleted.
4)   indexable columns for work load:
      -> indexable columns for given work load consists of all the columns that appear in the
         suggested index sets for each query in work load.
5)   multi-column indexes are generated for work load in similar to that of single query except the cost
     of the index is calculated over entire work load.
6)   index sets are generated for work load in similar to that of single query except the cost
     of the index set is calculated over entire work load. 
                        
 

HOW TO USE:
1) download the files
2) get_table_information() function in "automatic_index_advisor_parser.cpp" file must be edited as follow
   for each i between 0 to total_number_of_tables-1
     -> table_perm_info[i] consists of two attributes rows-no of rows in the table,
        row_size-size of any row or average of all row sizes if rows have unequal sizes.
     -> initial_table_ids[table_name]=i where table_name represents table name of ith table.
3) get_table_information() function wasn't completed to facilitate the user to either use the library without actually creating the database, or since this is the only part which depends on the underlying database used, it is left for user to edit the function to suit his needs.  
4) include "automatic_index_advisor_heuristic.cpp" in files where you want to use the index advisor.        
5) pass to suggest_indexes_for_this_queries() function the following two arguments
   -> first argument must be a pointer to array of strings representing the multiple queries.
   -> second argument must be the number of queries.
6) suggestions are displayed in the following format
   for i in 0 to K
     for j in 0 to number of indexes in ith index set
       display table name,cost,type(BST or hash index),index 
   here K represents maximum number of suggestions that can be made.


LIMITATIONS:

1) evaluation is done offline.
2) library support's only simple queries and doesn't support nested queries. 
3) only conjunctive queries can be evaluated.
4) The syntax for queries must be as follow
   lets take an example of two tables sailor and boat
   select ----
   from  sailor s,boat b
   where s.id=b.id and -------
5) supports only while statement apart from select and from statements
6) doesn't consider costs incurred for maintenance of tables i.e. insertion and deletion costs
7) doesn't consider costs incurred for maintenance of indexes i.e. insertion and deletion costs    
8) only number of disk reads are used to measure cost.
9) only clustered indexes are considered. 

 