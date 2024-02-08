#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#define Max_vars 10 /* maximum amount of global variables/macros */
#define Short_str 10 /* maximum size of string for names of variables/macros */
#define Long_str 50 /* maximum size of string for stored text */
#define Max_lists 10 /* maximum amount of lists */
#define List_size 10 /* maximum amount of elements per list */ 
#define Flt_limit 0.01 /* proportion threshold for equal floating points */

typedef struct {
    char* name;
    char* string;
    char type;
    /* types:
        00 - null/error
        01 - int
        02 - flt
        03 - str
        11 - macro to int
        12 - macro to flt
        13 - macro to str
    */
    int64_t integer;
    long double flt;
} Variable;

typedef struct {
    unsigned len;
    Variable elements[List_size];
} List;

List* AllLists = NULL;

Variable* Vars = NULL;

Variable solve_macro(Variable *var){
    Variable result;
    clear_var(&result);
    if ((10<=var->type) && (var->type<20)){
        int index= find_var_by_name(var->string);
        if (index!=-1){
            result=Vars[index];
        }
        if (result.type==((var->type)-10)){
            return result;
        } else if (result.type==var->type){
            return solve_macro(&result);
        }
    }
    return result;
}

void clear_var(Variable *var) {
    free(var->name);
    free(var->string);
    var->name = (char*)calloc(Short_str+1,sizeof(char));
    var->string = (char*)calloc(Long_str+1,sizeof(char));
    var->type = 0;
    var->integer = 0;
    var->flt = 0.0;
}

int find_var_by_name(char* name){
    for (unsigned i=0;i<Max_vars;i++){
        if (!strcmp(name,Vars[i].name)){
            return i;
        }
    }
    /* if no result is found, -1 is returned; */
    return -1;
}

int find_free_space(){
    for (unsigned i=0;i<Max_vars;i++){
        if (Vars[i].type==0){
            return i;
        }
    }
    /* if no result is found, -1 is returned; */
    return -1;
}

int init_var(char* name, char* string, char type, int64_t integer, long double flt){
    int index = find_free_space();
    if (index != -1){
        Variable *var = &Vars[index];
        strncpy(var->name, name, Short_str);
        var->name[Short_str] = '\0';
        strncpy(var->string, string, Long_str);
        var->string[Long_str] = '\0';
        var->type = type;
        var->integer = integer;
        var->flt = flt;
        return 0; /* success */
    }
    return 1; /* error */
}

void clear_list(List* L){
    L->len=0;
    for (unsigned i=0;i<List_size;i++){
        clear_var(&(L->elements[i]));
    }
}

int insert_on_list(List* L, Variable x){
    for (unsigned i=0; i<List_size;i++){
        if (L->elements[i].type==0){
            L->elements[i]=x;
            L->len+=1;
            return 0; /* sucess */
        }
    }
    return 1; /* error */
}

void move_zeros_to_end(List* L) {
    int nonZeroIndex = 0;

    /* Traverse the list and move non-zero elements to the front */
    for (unsigned i = 0; i < List_size; i++) {
        if (L->elements[i].type != 0) {
            L->elements[nonZeroIndex++] = L->elements[i];
        }
    }

    /* Fill the remaining space with elements of type=0 */
    while (nonZeroIndex < List_size) {
        clear_var(&(L->elements[nonZeroIndex++]));
    }
}

int is_equal(Variable x, Variable y){
    if (x.type==y.type){
        if (x.type%10==1){
            if (x.integer==y.integer){return 1;}
        } else if (x.type%10==2){
            if ((x.flt*(1+Flt_limit)>y.flt) &&(x.flt*(1-Flt_limit)<y.flt)){return 1;}
        } else if (x.type%10==3){
            if (!strcmp(x.string,y.string)){return 1;}
        }
    }
    return 0;
}

int remove_from_list_by_value(List* L, Variable x){
    if (x.type==0){return -1;} /* empty variable */
    for (unsigned i = 0; i < List_size; i++){
        if (is_equal(x,L->elements[i])){
            clear_var(&(L->elements[i]));
            L->len-=1;
            move_zeros_to_end(L);
            return 1; /* sucess */
        }
    }
    return 0; /* error */
}

int remove_from_list_by_name(List* L, Variable x){
    if (x.name[0]=='\0'){return -1;} /* nameless variable */
    for (unsigned i = 0; i < List_size; i++){
        if (!strcmp(L->elements[i].name, x.name)){
            clear_var(&(L->elements[i]));
            L->len-=1;
            move_zeros_to_end(L);
            return 1; /* sucess */
        }
    }
    return 0; /* error */
}

void scanning(Variable* x){
    if (x->type==1){
        scanf("%ld",&(x->integer));
    } else if (x->type==2){
        scanf("%Lf",&(x->flt));
    } else if (x->type==3){
        fgets(x->string, Long_str, stdin);
    }
}

void printing(Variable* x){
    if (x->type==1){
        printf("%ld",(x->integer));
    } else if (x->type==2){
        printf("%Lf",(x->flt));
    } else if (x->type==3){
        printf("%s",x->string);
    }
}

int main(){
    /* initialization */
    Vars=(Variable*)malloc(sizeof(Variable)*Max_vars);
    for (unsigned i=0;i<Max_vars;i++){
        clear_var(&Vars[i]);
    }
    
    AllLists=(List*)malloc(sizeof(List)*Max_lists);
    for (unsigned i=0;i<Max_lists;i++){
        clear_list(&AllLists[i]);
    }

    /* code will be transpiled here */

    /* cleaning */
    for (unsigned i=0;i<Max_vars;i++){
        free(Vars[i].name);
        free(Vars[i].string);
    }
    free(Vars);

    for (unsigned i = 0; i < Max_lists; i++) {
        free(AllLists[i].elements);
    }
free(AllLists);
    return 0;
}