#include <stdlib.h>
#include <stdio.h>
#include <equations.h>

/**
 * @function EQ_manager_init
 * @brief Initializes the equation set manager
 * @return a pointer to the menager if successful; NULL otherwise
 */ 
EQ_manager* EQ_manager_init(){
    EQ_manager *manager = malloc(sizeof(EQ_manager));
    if(manager == NULL){
        fprintf(stderr, "ERROR: malloc\n");
        return NULL;
    }
    manager->size = 0;
    manager->eqns_set = malloc(sizeof(Eqns_t*));
    if(manager->eqns_set == NULL){
        fprintf(stderr, "ERROR: malloc\n");
        return NULL;
    }
    return manager;
}

/**
 * @function EQ_manager_quit
 * @brief Deletes resources associated with a manager
 * @param menager equation set manager
 */ 
void EQ_manager_quit(EQ_manager *manager){
    for(int i=0; i<manager->size; i++){
        delete_eqns(manager->eqns_set[i]);
    }
    free(manager->eqns_set);
    free(manager);
}

/**
 * @function new_eqns
 * @brief Creates a new equation set
 * @param menager equation set manager
 * @return a pointer to the equation set if successful; NULL otherwise
 */ 
Eqns_t* new_eqns(EQ_manager *manager){
    Eqns_t* eqns = malloc(sizeof(Eqns_t));
    if(eqns == NULL){
        fprintf(stderr, "ERROR: malloc\n");
        return NULL;
    }
    eqns->head = NULL;
    eqns->tail = NULL;
    manager->size++;
    manager->eqns_set = realloc(manager->eqns_set, manager->size*sizeof(Eqns_t*));
    if(manager->eqns_set == NULL){
        fprintf(stderr, "ERROR: malloc\n");
        return NULL;
    }
    manager->eqns_set[manager->size-1] = eqns;
    return eqns;
}

/**
 * @function new_eqn
 * @brief Creates a new equation
 * @param eq_size equation size
 * @return a pointer to the new equation if successful; NULL otherwise
 */
Eqn_t* new_eqn(int eq_size){
    Eqn_t *equation = malloc(sizeof(Eqn_t));
    if(equation == NULL){
        fprintf(stderr, "ERROR: malloc\n");
        return NULL;
    }
    equation->eq = (int*)malloc(eq_size*sizeof(int));
    if(equation->eq == NULL){
        fprintf(stderr, "ERROR: malloc\n");
        return NULL;
    }
    equation->n = eq_size;
    for(int i=0; i<eq_size; i++)
        equation->eq[i] = 0;
    return equation;
}

/**
 * @function add_eqn
 * @brief Adds an equation to an equation set
 * @param eqns equation set
 * @param eq equation
 */
void add_eqn(Eqns_t *eqns, Eqn_t *eq){
    list_t *l = &eqns->head;
    list_t corr = *l;
	eq -> next = NULL;
	if(*l == NULL) *l = eq;
	else {
	    while(corr->next!=NULL)
		    corr = corr->next;
	
		corr->next = eq;
	}
    eq->prev = corr;
    eqns->tail = eq;
}

/**
 * @function add_var
 * @brief Adds the ith variable to the equation.
 * @param eqn equation
 * @param i index of variable
 * @param isComplemented denotes whether variable must be complemented
 */
void add_var(Eqn_t *eqn, int i, int isComplemented){
    if(isComplemented)
        eqn->eq[i] = -1;
    else eqn->eq[i] = 1; 
}

/**
 * @function isInEqns
 * @brief Detects whether the equation eqn is a member of equation set eqns
 * @param eqns equation set
 * @param eqn equation
 * @return 1 if eqn is member of eqns; 0 otherwise
 */
int isInEqns(Eqns_t *eqns, Eqn_t *eqn){
    int k;
    list_t l = eqns->head;
    while(l != NULL){
        k = 0;
        if(l->n == eqn->n){
            for(int i=0; i<l->n; i++){
                if(eqn->eq[i] == l->eq[i])
                    k++;
            }
            if(k == eqn->n)
                return 1;
        }
        l = l->next;
    }
    return 0;   
}

/**
 * @function print_equations
 * @brief Prints the equation set eqns on file f
 * @param f pointer of the output file
 * @param eqns equation set
 */
void print_equations(FILE* f, Eqns_t* eqns){
    list_t l = eqns->tail;
    while(l!=NULL){
        for(int i=0; i<l->n; i++){
            fprintf(f, "%d", l->eq[i]);
        }
        fprintf(f, "\n");
        l = l->prev;
    }
}

/**
 * @function all_positive
 * @brief Complements all equations in equation set
 * @param eqns equation set
 */
void complements_all(Eqns_t *eqns){
    list_t l = eqns->head;
    while(l != NULL){
        for(int i=0; i<l->n; i++)
            if(l->eq[i] == -1){
                l->eq[i] = 1;
                break; // jumps to the next equation
            }
        l = l->next;
    }
}

/**
 * @function delete_eqns
 * @brief Deletes all equaitons from the equation set eqns
 * @param eqns equation set
 */
void delete_eqns(Eqns_t *eqns){
    list_t tmp;
    while(eqns->head != NULL){
        tmp = eqns->head;
        eqns->head = eqns->head->next;
        free(tmp);
    }
    free(eqns);
}

