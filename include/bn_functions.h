#ifndef __BAYESIAN_H
#define __BAYESIAN_H
#include <Python.h>

// All these variables are global variables used in bn_functions.c(mainly) and bn_netica.c

int x, y, k;
/* InitializePyobject */
// a function to get ontology information from "Caresses.py"
void InitializePyobject(void);

// Coommon global python object declarations and definitions for all the Culture in  function "InitializePyobject()"
// (if there is extern prepended, then it is only declaration. Definition needs to be done in .c file)
PyObject *p_module, *p_dict, *p_class, *p_args;
PyObject *p_time_state, *p_period_state, *p_room_state, *p_object_list, *p_activity_state;
Py_ssize_t p_time_state_size   ;
Py_ssize_t p_period_state_size ;
Py_ssize_t p_room_state_size   ;
Py_ssize_t p_object_list_size ;
Py_ssize_t p_activity_state_size ;
// char *object_name[50];
char **object_name;
PyObject *p_uni_item_from_object_list, *p_byte_item_from_object_list;


// SIN object declarations and definitions in "InitializePyobject()"
PyObject *p_SIN_instance_class;
PyObject *p_SIN_period_cpt, *p_SIN_room_cpt, *p_SIN_objects_cpt, *p_SIN_activity_cpt;

// SJP object declarations and definitions in "InitializePyobject()"
PyObject *p_SJP_instance_class;
PyObject *p_SJP_period_cpt, *p_SJP_room_cpt, *p_SJP_objects_cpt, *p_SJP_activity_cpt;

// SEN object declarations and definitions in "InitializePyobject()"
PyObject *p_SEN_instance_class;
PyObject *p_SEN_period_cpt, *p_SEN_room_cpt, *p_SEN_objects_cpt, *p_SEN_activity_cpt;



/* SetNodeStateNames */
// A function to convert all PyObjects to strings for setting state names for all nodes
void GetNodeStateNames(void);
// object declarations and definitions in "SetNodeStateNames()"
// for time node
PyObject *p_uni_time_string, *p_byte_time_string;
char *time_string;
// for period node
PyObject *p_uni_period_string, *p_byte_period_string;
char *period_string;
// for room node
PyObject *p_uni_room_string, *p_byte_room_string;
char *room_string;
// for activity node
PyObject *p_uni_activity_string, *p_byte_activity_string;
char *activity_string;



/* GetPeriodCPT */
// A function to get the probabilities of period states (given time, culture)
// this function will be called inside for(x=0; x < p_time_state_size; x++)
void GetPeriodCPT(int x, PyObject *p_SXX_period_cpt);
// get the time state in string type
PyObject *p_uni_time_item, *p_byte_time_item;
char *str_time_item;

// the {period: probability} pair
PyObject *p_period_prob_dict;

// initial probabilities of period states
double afternoon_prob,breakfasttime_prob, dinnertime_prob,evening_prob, lunchtime_prob, morning_prob, night_prob;

// variables used in the iteration of  7 periods
PyObject *p_uni_period_item, *p_byte_period_item;
char *str_period_item;
PyObject *p_period_prob;


/* GetRoomCPT */
// A function to get the probabilities of room states (given culture)
void GetRoomCPT(PyObject *p_SXX_room_cpt);
// get the time state in string type
PyObject *p_uni_room_item, *p_byte_room_item;
char *str_room_item;
PyObject *p_room_prob;
double *room_prob;

/* GetObjectsCPT */
// A function to get the probabilities of objects states (given culture and room)
void GetObjectsCPT(int x, int y, PyObject *p_SXX_objects_cpt);
PyObject *p_obj_prob_dict;
PyObject *p_obj_prob;
double obj_prob;


/* GetActivityCPT */
// A function to get the probabilities of activity states (given room, culture and period)
void GetActivityCPT(int x, int y, PyObject *p_SXX_activity_cpt);
char *underscore, *concatenate_str_1, *concatenate_str_2;
PyObject *p_activity_prob_list;
double *activity_prob;
PyObject *p_activity_prob;




/*Below are all the variable declarations and definitions for enterfinding part*/
// The function to create a new file (used for saving data to .csv file)
void CreateNewFile(FILE *fp, const char *file_name, const char *file_content);
// A function to write file_content and add a "," to a csv file
void WriteTextToCsv_Comma(FILE *fp, const char *file_name, char *file_content);
// A function to write file_content and add a "\n" to a csv file
void WriteTextToCsv_Nextline(FILE *fp, const char *file_name, char *file_content);
char *dest;
// A function to write file_content to a text file
void WriteTextToTxt(FILE *fp, const char *file_name, char *file_content);

/* EnterFindingSetting*/
// a function to get the python Methods for enterfinding
void EnterFindingSetting(char *file_api_path);
PyObject *p_class_2, *p_args_2, *p_api_instance_class;
PyObject *p_micro_act_rec_result, *p_micro_act_rec_score;
PyObject *p_google_labels_list;
Py_ssize_t image_number;

/* CultureStateSetting */
// setting for culture enterfinding
void CultureStateSetting(char *default_culture_arg);
int zero_o_one_1;
char gen_name[10];
char *gen_name_ptr;

/* TimeStateSetting */
// setting for time enterfinding
void TimeStateSetting(int default_time_arg);
char *hour_name[24];
int zero_o_one_2;
double time_number;
int time_index;


#endif  /* __BAYESIAN_H */
