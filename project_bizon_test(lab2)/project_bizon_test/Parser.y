%{
#include "ast.h"
#include <stdio.h>
%}

%error-verbose

%union {
    int ival;
    double fval;
    char *sval;
    struct ast *ast;
    int subtok;
}

%token <sval> VARNAME STRINGVAL
%token <ival> INTVAL
%token <ival> BOOLVAL
%token <fval> DOUBLEVAL

%left AND
%left OR


/*signs*/
%token COMA
%token DOT
%token DOUBLE_POINT

/*DDL*/
%token DROP
%token CREATE
%token FILTER
%token FOR
%token RETURN
%token REMOVE
%token UPDATE


/*utility*/
%token OPENBR_CIR CLOSEBR_CIR
%token OPENBR_FIG CLOSEBR_FIG
%token TABLENUM RAWSIZE
%token INTO
%token INSERT 
%token TABLE
%token IN
%token WITH
%token EQ NEQ GREATER LESSER GREATER_EQ LESSER_EQ


%type<ast> terminal
%type<ast> drop_table create_table
%type<ast> insert_to_table doc pairs pair
%type<ast> for_part_list for_part
%type<ast> filter_pair table_raw_id final_filter_node filter_sequence
%type<ast> query for_query
%type<ast> constant
%type<ast> CMP_SIGN
%type<ast> conclusion return_conclusion table_raw_id_list
%type<ast> remove_conclusion remove_varname_list update_conclusion

%locations
%parse-param { struct ast **root }
%start query

%%
query: terminal { $$ = $1; print_ast(stdout, *root, 0); $$ = NULL;};
terminal: drop_table | create_table | insert_to_table | for_query;

constant: INTVAL                         { $$ = newint($1); *root= $$; }
        | DOUBLEVAL                       { $$ = newdouble($1); *root= $$;}
        | BOOLVAL                        { $$ = newbool($1); *root= $$;}
        | STRINGVAL                      { $$ = newstring($1); *root= $$;};

CMP_SIGN: EQ                         { $$ = newsign("=="); *root= $$; }
        | NEQ                       { $$ = newsign("!="); *root= $$;}
        | GREATER                        { $$ = newsign(">"); *root= $$;}
        | LESSER                      { $$ = newsign("<"); *root= $$;}
        | GREATER_EQ                        { $$ = newsign(">="); *root= $$;}
        | LESSER_EQ                      { $$ = newsign("<="); *root= $$;} ;
 

//drop
drop_table: DROP TABLE TABLENUM OPENBR_CIR INTVAL CLOSEBR_CIR { $$ = newdrop($5); *root= $$; };

//create
create_table: CREATE TABLE TABLENUM OPENBR_CIR INTVAL CLOSEBR_CIR RAWSIZE OPENBR_CIR INTVAL CLOSEBR_CIR { $$ = create_tableAst($5, $9); *root= $$; };

//insert 

insert_to_table: INSERT OPENBR_FIG pairs CLOSEBR_FIG INTO TABLENUM OPENBR_CIR INTVAL CLOSEBR_CIR {$$=newinsert($8, $3); *root = $$;};

doc: pairs {$$ = $1;}

pairs: pair { $$ = newlist($1, NULL);*root= $$;}
     | pairs COMA pair { $$ = newlist($3, $1);*root= $$;};

pair: INTVAL DOUBLE_POINT constant {$$ = newpair($1, $3);*root= $$;};         


// for queries
for_query: for_part_list final_filter_node conclusion {$$ = newforquery($1, $2, $3);*root= $$;}
           | for_part_list conclusion {$$ = newforquery($1, NULL, $2);*root= $$;};

// for part
for_part_list: for_part {$$ = newlist($1, NULL);*root= $$;}
             | for_part_list for_part_list {$$ = newlist($1, $2);*root = $$;};

for_part: FOR VARNAME IN TABLENUM OPENBR_CIR INTVAL CLOSEBR_CIR {$$ = newforast($2, $6); *root= $$;};

// filter part
final_filter_node: FILTER filter_sequence {$$ = $2};

filter_sequence: filter_pair {$$ = newfilterlist($1, NULL, NULL);*root= $$;}
                 | filter_sequence AND filter_sequence {$$ = newfilterlist($1, 1, $3);*root= $$;}
                 | filter_sequence OR filter_sequence {$$ = newfilterlist($1, 2, $3);*root= $$;};

filter_pair: table_raw_id CMP_SIGN constant {$$ = newfilterpair($1, $2, $3); *root= $$;}
            | table_raw_id CMP_SIGN table_raw_id {$$ = newfilterpair($1, $2, $3); *root= $$;};;

table_raw_id: VARNAME DOT INTVAL {$$ = newtablefraw($1, $3); *root = $$;};


// conclusion part
conclusion: return_conclusion {$$ = $1}
           | remove_conclusion{$$ = $1}
           | update_conclusion{$$ = $1};

return_conclusion: RETURN OPENBR_FIG table_raw_id_list CLOSEBR_FIG {$$ = newreturn($3);*root = $$;}

remove_conclusion: REMOVE remove_varname_list {$$ = newremove($2);*root = $$;};

update_conclusion: UPDATE VARNAME WITH OPENBR_FIG pairs CLOSEBR_FIG {$$ = newupdate($2, $5);*root = $$;};

remove_varname_list: VARNAME {$$ = newlist(newstring($1), NULL);*root = $$;}
            | remove_varname_list remove_varname_list {$$ = newlist($1, $2);*root = $$;};

table_raw_id_list: table_raw_id {$$ = newlist($1, NULL);*root = $$;}
                 | table_raw_id_list COMA table_raw_id_list {$$ = newlist($1, $3);*root = $$;};

%%

int yyerror(char *s) {
    fprintf(stderr, "error: %s\n", s);
}

struct ast * parse_ast(){
    struct ast *root = NULL;
    yyparse(&root);
    return root;
}