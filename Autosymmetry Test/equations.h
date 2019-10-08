#include <stdlib.h>
#include <stdio.h>

/**
 *  @brief type of equation
 */
typedef struct equation {   
    int *eq;                 /**< equation */
    int n;                   /**< equation size */
    struct equation * next;  /**< pointer to the next element */
    struct equation * prev;  /**< pointer to the previous element */
} Eqn_t;

typedef Eqn_t* list_t;       /**< type of equation list */

/**
 * @brief type of equation set 
 */
typedef struct{
    list_t head;             /**< pointer to the head of the list */
    list_t tail;             /**< pointer to the tail of the list */
} Eqns_t;

/**
 * @brief type of equation set manager
 */
typedef struct{
    Eqns_t **eqns_set;      /**< equation set list */
    int size;               /**< number of equation set in eqns_set */
} EQ_manager;

/**
 * @function EQ_manager_init
 * @brief Initializes the equation set manager
 * @return a pointer to the menager if successful; NULL otherwise
 */ 
EQ_manager* EQ_manager_init();

/**
 * @function EQ_manager_quit
 * @brief Deletes resources associated with a manager
 * @param menager equation set manager
 */ 
void EQ_manager_quit(EQ_manager *manager);

/**
 * @function new_eqns
 * @brief Creates a new equation set
 * @param menager equation set manager
 * @return a pointer to the equation set if successful; NULL otherwise
 */ 
Eqns_t* new_eqns(EQ_manager* manager);

/**
 * @function new_eqn
 * @brief Creates a new equation
 * @param eq_size equation size
 * @return a pointer to the new equation if successful; NULL otherwise
 */
Eqn_t* new_eqn(int eq_size); 

/**
 * @function add_eqn
 * @brief Adds an equation to an equation set
 * @param eqns equation set
 * @param eq equation
 */
void add_eqn(Eqns_t *eqns, Eqn_t *eq);

/**
 * @function add_var
 * @brief Adds the ith variable to the equation.
 * @param eqn equation
 * @param i index of variable
 * @param isComplemented denotes whether variable must be complemented
 */
void add_var(Eqn_t *eqn, int i, int isComplemented);

//void eqn_xorVar(Eqn_t *eqn, int i);

/**
 * @function isInEqns
 * @brief Detects whether the equation eqn is a member of equation set eqns
 * @param eqns equation set
 * @param eqn equation
 * @return 1 if eqn is member of eqns; 0 otherwise
 */
int isInEqns(Eqns_t *eqns, Eqn_t *eqn);

/**
 * @function print_equations
 * @brief Prints the equation set eqns on file f
 * @param f pointer of the output file
 * @param eqns equation set
 */
void print_equations(FILE* f, Eqns_t *eqns);

/**
 * @function all_positive
 * @brief Complements all equations in equation set
 * @param eqns equation set
 */
void complements_all(Eqns_t *eqns);

/**
 * @function delete_eqns
 * @brief Deletes all equaitons from the equation set eqns
 * @param eqns equation set
 */
void delete_eqns(Eqns_t *eqns);