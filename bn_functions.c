#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/bn_functions.h"
// check if x is NULL or not
#define CHECK_NULL(x) {if (!x) printf("Not logical value at line number %d in file %s\n", __LINE__, __FILE__);}


// A function to get ontology information from "caresses.py"
void InitializePyobject(void){
   /* import the python module  */
   p_module = PyImport_ImportModule("caresses");

   // Check if caresses.py is imported successfully
   CHECK_NULL(p_module);

   /* retrieve attribute from p_module*/
   p_dict = PyModule_GetDict(p_module);
   // check if the dictionary object of caresses.py is returned
   CHECK_NULL(p_dict);

   /* Get the object "Class Protege_Extraction" from caresses module */
   p_class = PyDict_GetItemString(p_dict, "Protege_Extraction");
   // check if the object is null
   CHECK_NULL(p_class);

   /* 1. build arguments and pass the arguments (SJP,SIN,SEN) */
   /* pSJP_Args has to be a tuple */
   /* Instantiate objects of class with one argument: gen */
   /* Equal to python code: Protege_Extraction("SJP") */

   // build a new tuple object of size 1
   p_args = PyTuple_New(1);
   PyTuple_SetItem(p_args, 0, Py_BuildValue("s", "SJP"));
   p_SJP_instance_class = PyObject_CallObject(p_class, p_args);
   CHECK_NULL(p_SJP_instance_class)

   PyTuple_SetItem(p_args, 0, Py_BuildValue("s", "SIN"));
   p_SIN_instance_class = PyObject_CallObject(p_class, p_args);
   CHECK_NULL(p_SIN_instance_class)

   PyTuple_SetItem(p_args, 0, Py_BuildValue("s", "SEN"));
   p_SEN_instance_class = PyObject_CallObject(p_class, p_args);
   CHECK_NULL(p_SEN_instance_class)

   /* call SIN Methods with the instantiated objects of Class (p_SIN_instance_class)
     ,so the instance variables we need later can be defined */
   p_time_state = PyObject_CallMethod(p_SIN_instance_class, "Get_Time_State", NULL);
   CHECK_NULL(p_time_state)

   p_period_state = PyObject_CallMethod(p_SIN_instance_class, "Get_Period_State", NULL);
   CHECK_NULL(p_period_state)

   p_room_state = PyObject_CallMethod(p_SIN_instance_class, "Get_Room_State", NULL);
   CHECK_NULL(p_room_state)

   p_object_list = PyObject_CallMethod(p_SIN_instance_class, "Get_Object_List", NULL);
   CHECK_NULL(p_object_list)

   p_activity_state = PyObject_CallMethod(p_SIN_instance_class, "Get_Activity_State", NULL);
   CHECK_NULL(p_activity_state)

   // only call, without assigning, since we've already had all the state names (p_time_state, p_period_state...)
   /* call SJP Methods with the instantiated objects of Class (p_SJP_instance_class)
     ,so the instance variables we need later can be defined */
   PyObject_CallMethod(p_SJP_instance_class, "Call_All_States_Methods", NULL);

   /* call SEN Methods with the instantiated objects of Class (p_SEN_instance_class)
      ,so the instance variables we need later can be defined */
   PyObject_CallMethod(p_SEN_instance_class, "Call_All_States_Methods", NULL);

   // get the size of states for each node
   p_time_state_size = PyList_Size(p_time_state);
   p_period_state_size = PyList_Size(p_period_state);
   p_room_state_size = PyList_Size(p_room_state);
   p_object_list_size = PyList_Size(p_object_list);
   p_activity_state_size = PyList_Size(p_activity_state);

   // dynamically allocate an array of strings(object_name)
   object_name = malloc(p_object_list_size * sizeof (char*));
   object_name[p_object_list_size];

   for (x = 0; x < p_object_list_size; x++) {
       p_uni_item_from_object_list = PyList_GetItem(p_object_list, x);
       CHECK_NULL(p_uni_item_from_object_list);
       // Encode a Unicode object (p_uni_item_from_object_list) and return the result as Python bytes object
       p_byte_item_from_object_list = PyUnicode_AsEncodedString(p_uni_item_from_object_list, "utf-8", "Error!!!");
       CHECK_NULL(p_byte_item_from_object_list);
       // return a char*
       object_name[x] = PyBytes_AS_STRING(p_byte_item_from_object_list);
       CHECK_NULL(object_name[x]);
   }

   /*
      Get the information of CPT of Period node.
      p_SXX_period_cpt is an object of a list of dictionaries with {period: probability} as key-value pair
      ex: [{'EVENING': Decimal('0.435'), 'DINNERTIME': Decimal('0.565')},....]
      Each dict belongs to an hour (time).
      The order of hour corresponds to the list in Protege ['EIGHTAM', 'EIGHTPM', 'ELEVENAM'.....].
      time_dict_list is an instance variable in python.
   */
   p_SIN_period_cpt = PyObject_GetAttrString(p_SIN_instance_class, "time_dict_list");
   CHECK_NULL(p_SIN_period_cpt)

   p_SJP_period_cpt = PyObject_GetAttrString(p_SJP_instance_class, "time_dict_list");
   CHECK_NULL(p_SJP_period_cpt)

   p_SEN_period_cpt = PyObject_GetAttrString(p_SEN_instance_class, "time_dict_list");
   CHECK_NULL(p_SEN_period_cpt)

   /*
      Get the information of CPT of Room node.
      p_SXX_room_cpt is an object of a dictionary with room state as key and propobility as value
      i.e. {'BATHROOM': Decimal('0.204'),'LIVINGROOM': Decimal('0.194')....}
      room_dict is an instance variable in python.
   */
   p_SIN_room_cpt = PyObject_GetAttrString(p_SIN_instance_class, "room_dict");
   CHECK_NULL(p_SIN_room_cpt)

   p_SJP_room_cpt = PyObject_GetAttrString(p_SJP_instance_class, "room_dict");
   CHECK_NULL(p_SJP_room_cpt)

   p_SEN_room_cpt = PyObject_GetAttrString(p_SEN_instance_class, "room_dict");
   CHECK_NULL(p_SEN_room_cpt)

   /*
      Get the information of CPT of Object nodes.
      p_SXX_objects_cpt is an object of a list of dictionaries with {room: probability} as key-value pair,
      ex: [ {'BEDROOM': 0.0001, 'LIVINGROOM': 0.0001, 'KITCHEN': 0.0001, 'DININGROOM': 0.0001, 'PUJAROOM': 0.0001, 'BATHROOM': 0.3},,....]
      Each dict belongs to an object and prob = 0.0001 (can not be 0 = deterministic)
      which means that object does not exist in that room (for that particular culture) after Google API training
      The order of objects in the list corresponds to the list in Protege ['ALTAR', 'BATHROOMACCESSORY', 'BATHROOMCABINET',...]
      object_dict_list is an instance variable in python.
   */

   p_SIN_objects_cpt = PyObject_GetAttrString(p_SIN_instance_class, "object_dict_list");
   CHECK_NULL(p_SIN_objects_cpt)

   p_SJP_objects_cpt = PyObject_GetAttrString(p_SJP_instance_class, "object_dict_list");
   CHECK_NULL(p_SJP_objects_cpt)

   p_SEN_objects_cpt = PyObject_GetAttrString(p_SEN_instance_class, "object_dict_list");
   CHECK_NULL(p_SEN_objects_cpt)

   /*
      Get the information of CPT of Activity node.
      p_SXX_activity_cpt is an object of a dictionary with room_period as key and a list of probabilities as value
      {room_period: [p_cooking , p_eating , p_others , p_praying , p_reading , p_showering, p_sleeping]}
      ex: {'BEDROOM_EVENING': [Decimal('0'), Decimal('0'), Decimal('0'), Decimal('0'), Decimal('1.000'), Decimal('0'), Decimal('0')],...}
      Each key-value pair encodes the probabilities of activites happended given a location and a period.
      The room_period pairs do not cover all the possible pairs which means if there are missing room_period pairs(not encoded in Protege),
      the p_others will be set to approxiamte 1.0 in Netica setting(should not be deterministic).
      activity_dict is an instance variable in python.
   */

   p_SIN_activity_cpt = PyObject_GetAttrString(p_SIN_instance_class, "activity_dict");
   CHECK_NULL(p_SIN_activity_cpt)

   p_SJP_activity_cpt = PyObject_GetAttrString(p_SJP_instance_class, "activity_dict");
   CHECK_NULL(p_SJP_activity_cpt)

   p_SEN_activity_cpt = PyObject_GetAttrString(p_SEN_instance_class, "activity_dict");
   CHECK_NULL(p_SEN_activity_cpt)

   // Py_DECREF(p_module);
}

// A function to convert all PyObjects to strings for setting state names for all nodes
void GetNodeStateNames(void){
   /* for time node */
   // get an object of time string
   p_uni_time_string = PyObject_GetAttrString(p_SIN_instance_class, "time_string");
   CHECK_NULL(p_uni_time_string);
   // Encode a Unicode object (p_uni_time_string) and return the result as Python bytes object (p_byte_time_string)
   p_byte_time_string = PyUnicode_AsEncodedString(p_uni_time_string, "utf-8","Error!!!");
   CHECK_NULL(p_byte_time_string);
   // return a char*
   time_string = PyBytes_AS_STRING(p_byte_time_string);
   CHECK_NULL(time_string);

   /* for period node */
   // get an object of period string
   p_uni_period_string = PyObject_GetAttrString(p_SIN_instance_class, "period_string");
   CHECK_NULL(p_uni_period_string);
   p_byte_period_string = PyUnicode_AsEncodedString(p_uni_period_string, "utf-8","Error!!!");
   CHECK_NULL(p_byte_period_string);
   period_string = PyBytes_AS_STRING(p_byte_period_string);
   CHECK_NULL(period_string);

   /* for room node */
   // get an object of room string
   p_uni_room_string = PyObject_GetAttrString(p_SIN_instance_class, "room_string");
   CHECK_NULL(p_uni_room_string);
   p_byte_room_string = PyUnicode_AsEncodedString(p_uni_room_string, "utf-8","Error!!!");
   CHECK_NULL(p_byte_room_string);
   room_string = PyBytes_AS_STRING(p_byte_room_string);
   CHECK_NULL(room_string);

   /* for activity node */
   // get an object of activity string
   p_uni_activity_string = PyObject_GetAttrString(p_SIN_instance_class, "activity_string");
   CHECK_NULL(p_uni_activity_string);
   p_byte_activity_string = PyUnicode_AsEncodedString(p_uni_activity_string, "utf-8","Error!!!");
   CHECK_NULL(p_byte_activity_string);
   activity_string = PyBytes_AS_STRING(p_byte_activity_string);
   CHECK_NULL(activity_string);

}

// A function to get the probabilities of period states (given time, culture)
// this function will be called inside for (x=0; x < p_time_state_size; x++) in bn_netica.c
void GetPeriodCPT(int x, PyObject *p_SXX_period_cpt){
   // get the time state in string type
   p_uni_time_item = PyList_GetItem(p_time_state,x);
   CHECK_NULL(p_uni_time_item);
   p_byte_time_item = PyUnicode_AsEncodedString(p_uni_time_item, "utf-8","Error!!!");
   CHECK_NULL(p_byte_time_item);
   str_time_item = PyBytes_AS_STRING(p_byte_time_item);
   CHECK_NULL(str_time_item);

   // get the {period: probability} pair
   p_period_prob_dict = PyList_GetItem(p_SXX_period_cpt, x);
   CHECK_NULL(p_period_prob_dict);

   // set the initial probabilities of period states = 0
   afternoon_prob = breakfasttime_prob = dinnertime_prob = evening_prob = lunchtime_prob = morning_prob = night_prob = 0;

   // iterate over 7 periods to fill the  probabilities of period states at the current time state
   for (y = 0; y < p_period_state_size; y++) {
      // get the period state in string type
      p_uni_period_item = PyList_GetItem(p_period_state, y);
      CHECK_NULL(p_uni_period_item);
      p_byte_period_item = PyUnicode_AsEncodedString(p_uni_period_item, "utf-8","Error!!!");
      CHECK_NULL(p_byte_period_item);
      // str_period_item is the key for the p_period_prob_dict
      str_period_item = PyBytes_AS_STRING(p_byte_period_item);
      CHECK_NULL(str_period_item)

      p_period_prob = PyDict_GetItemString(p_period_prob_dict, str_period_item);


      // if there exist a prob for this period, update the prob to p_period_prob
      // otherwise the prob is 0 (initialization)
      if (p_period_prob) {
          switch (str_period_item[0]) {

             case 'A' :
                afternoon_prob = PyFloat_AsDouble(p_period_prob);
                // printf("it is A");
                break;
             case 'B' :
                breakfasttime_prob = PyFloat_AsDouble(p_period_prob);
                break;
             case 'D' :
                dinnertime_prob = PyFloat_AsDouble(p_period_prob);
                break;
             case 'E' :
                evening_prob = PyFloat_AsDouble(p_period_prob);
                break;
             case 'L' :
                lunchtime_prob = PyFloat_AsDouble(p_period_prob);
                break;
             case 'M' :
                morning_prob = PyFloat_AsDouble(p_period_prob);
                break;
             case 'N' :
                night_prob = PyFloat_AsDouble(p_period_prob);
                break;
             default :
                printf("sth wrong\n" );
          }
      }
   }
}


// A function to get the probabilities of room states (given culture)
// this function will be called inside in bn_netica.c at line #
void GetRoomCPT(PyObject *p_SXX_room_cpt){
    // dynamically allocate an array of doubles
    room_prob = malloc(p_room_state_size * sizeof (double));


    for (x = 0; x < p_room_state_size; x++) {
        // get the room state in string type
        p_uni_room_item = PyList_GetItem(p_room_state,x);
        p_byte_room_item = PyUnicode_AsEncodedString(p_uni_room_item, "utf-8","Error!!!");
        str_room_item = PyBytes_AS_STRING(p_byte_room_item);

        // remember p_SXX_room_cpt is an object of a dictionary with room state as key and propobility as value.
        // so now a value (p_room_prob = a propobility of room state) can be returned by giving a key (str_room_item = a certain room state).
        p_room_prob = PyDict_GetItemString(p_SXX_room_cpt, str_room_item);
        CHECK_NULL(p_room_prob);

        // return a C double representation of the contents of pyfloat (room_prob).
        room_prob[x] = PyFloat_AsDouble(p_room_prob);

    }
}

// A function to get the probabilities of objects states (given culture and room)
void GetObjectsCPT(int x, int y, PyObject *p_SXX_objects_cpt){
    // remember p_SXX_objects_cpt is a list of dictionaries with {room: probability} as key-value pair,
    // so now we can get every value ( = p_obj_prob_dict = {room: probability} ) of a list given an index (x)
    p_obj_prob_dict = PyList_GetItem(p_SXX_objects_cpt, x);

    // generate keys (room) to get the propobility later (p_obj_prob)
    p_uni_room_item = PyList_GetItem(p_room_state, y);
    p_byte_room_item = PyUnicode_AsEncodedString(p_uni_room_item, "utf-8","Error!!!");
    str_room_item = PyBytes_AS_STRING(p_byte_room_item);

    // p_obj_prob is the probability that an object[x] exists in str_room_item
    p_obj_prob = PyDict_GetItemString(p_obj_prob_dict, str_room_item);

    // return a C double representation of the contents of pyfloat (p_obj_prob).
    obj_prob = PyFloat_AsDouble(p_obj_prob);

}


// A function to get the probabilities of activity states (given room, culture and period)
void GetActivityCPT(int x, int y, PyObject *p_SXX_activity_cpt){
    p_uni_room_item = PyList_GetItem(p_room_state, x);
	p_byte_room_item = PyUnicode_AsEncodedString(p_uni_room_item, "utf-8","Error!!!");
	str_room_item = PyBytes_AS_STRING(p_byte_room_item);

    p_uni_period_item = PyList_GetItem(p_period_state,y);
    p_byte_period_item = PyUnicode_AsEncodedString(p_uni_period_item, "utf-8","Error!!!");
    str_period_item = PyBytes_AS_STRING(p_byte_period_item);

    underscore = "_";
    concatenate_str_1 = (char *) malloc(1 + strlen(str_room_item) + strlen(underscore));

    strcpy(concatenate_str_1, str_room_item);
    strcat(concatenate_str_1, underscore);

    concatenate_str_2 = (char *) malloc(1 + strlen(str_room_item) + strlen(underscore) + strlen(str_period_item));
    strcpy(concatenate_str_2, concatenate_str_1);
    strcat(concatenate_str_2, str_period_item);

    p_activity_prob_list = PyDict_GetItemString(p_SXX_activity_cpt, concatenate_str_2);

    activity_prob = malloc(p_activity_state_size * sizeof (double));

    if (p_activity_prob_list) {
        for (k = 0; k < p_activity_state_size; k++) {
            p_activity_prob = PyList_GetItem(p_activity_prob_list, k);
            activity_prob[k] = PyFloat_AsDouble(p_activity_prob);
        }
    } else {
        for (k = 0; k < p_activity_state_size; k++) {
            activity_prob[k] = 0.0;
        }
        activity_prob[2] = 1.0;
    }
}


char *hour_name[24] = {"TWELVEAM", "ONEAM", "TWOAM", "THREEAM", "FOURAM", "FIVEAM",
    "SIXAM", "SEVENAM", "EIGHTAM", "NINEAM", "TENAM", "ELEVENAM", "TWELVEPM",
    "ONEPM", "TWOPM", "THREEPM", "FOURPM", "FIVEPM", "SIXPM", "SEVENPM", "EIGHTPM", "NINEPM", "TENPM", "ELEVENPM" };

// get the python Methods for enterfinding
void EnterFindingSetting(char *file_api_path){
    /* Get the object "Class Vision_API_Result_Extraction" from caresses module */
    p_class_2 = PyDict_GetItemString(p_dict, "Vision_API_Result_Extraction");
    // check if the object is null
    CHECK_NULL(p_class_2);

    // build a new tuple object of size 1
    p_args_2 = PyTuple_New(1);
    PyTuple_SetItem(p_args_2, 0, Py_BuildValue("s", file_api_path));
    p_api_instance_class = PyObject_CallObject(p_class_2, p_args_2);
    CHECK_NULL(p_api_instance_class)

    /* call Get_Micro_Activity_Recognition_Result Method with the instantiated objects of Class (p_api_instance_class) */
    // get a list of highest-score acitivities
    p_micro_act_rec_result = PyObject_CallMethod(p_api_instance_class, "Get_Micro_Activity_Recognition_Result", NULL);
    CHECK_NULL(p_micro_act_rec_result)
    // get a list of activity scores
    p_micro_act_rec_score = PyObject_CallMethod(p_api_instance_class, "Get_Micro_Activity_Recognition_Score", NULL);
    CHECK_NULL(p_micro_act_rec_score)
    // get a list of object lists
    p_google_labels_list = PyObject_CallMethod(p_api_instance_class, "Get_Google_Labels_List", NULL);
    CHECK_NULL(p_google_labels_list)
    // get a number of images
    image_number = PyList_Size(p_micro_act_rec_result);

}

// setting for culture enterfinding
void CultureStateSetting(char *default_culture_arg){
    printf("If you prefer to use the default state, please enter 0 \n\
If you prefer to input state for Culture node manually, please enter 1 \n");

    scanf("%d", &zero_o_one_1);
    while ((zero_o_one_1!= 0) && (zero_o_one_1!= 1)){
        printf("Invalid number ! Try again ! If you prefer to use the default state, please enter 0 \n\
If you prefer to input state for Culture node manually, please enter 1 \n");
        scanf("%d", &zero_o_one_1);
    }
    if (zero_o_one_1 == 0) {
        // default culture
        gen_name_ptr = default_culture_arg;

    } else {
        printf("Enter the culture amongst Indian, Japanese, English: ");
        scanf("%s", gen_name);
        while ((strcmp(gen_name, "Indian")!= 0) && (strcmp(gen_name, "Japanese")!= 0) && (strcmp(gen_name, "English")!= 0)){
            printf("Wrong culture name!! Please enter again the culture amongst Indian, Japanese, English: ");
            scanf("%s", gen_name);
        }
        gen_name_ptr = gen_name;
    }
}


// setting for time enterfinding
void TimeStateSetting(int default_time_arg){
    printf("If you prefer to use the default state, please enter 0 \n\
If you prefer to input state for Time node manually, please enter 1 \n\
If you prefer to use the current time, please enter 2\n");

    scanf("%d", &zero_o_one_2);
    while ((zero_o_one_2!= 0) && (zero_o_one_2!= 1) && (zero_o_one_2!= 2)){
        printf("Invalid number ! Try again ! If you prefer to use the default state, please enter 0 \n\
If you prefer to input state for Time node manually, please enter 1 \n\
If you prefer to use the current time, please enter 2\n");
        scanf("%d", &zero_o_one_2);
    }
    if (zero_o_one_2 == 0) {
        // default value (can be set beforehand to prevent setting from console)
        time_number = default_time_arg;
    } else if (zero_o_one_2 == 1) {
        printf("Enter the hour amongst 0-23: ");
        scanf("%lf", &time_number);
        while ((int)time_number > 24 || (int)time_number < 1){
            printf("Wrong time number!! Enter the time amongst 0-23: ");
            scanf("%lf", &time_number);
        }
    } else {
        time_t curtime;
	    struct tm *loc_time;

	    //Getting current time of system
	    curtime = time(NULL);
	    if (curtime == -1) {
	       puts("The time() function failed");
	    }

	    // Converting current time to local time
	    loc_time = localtime(&curtime);
	    if (loc_time == NULL) {
	         puts("The localtime() function failed");
	     }

	    time_number = loc_time->tm_hour;
    }

    time_index = (int) time_number;
}


// The function to create a new file (used for saving data to .csv file)
void CreateNewFile(FILE *fp, const char *file_name, const char *file_content){
    fp = fopen(file_name, "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    if (file_content) {
        fputs(file_content, fp);
    }
    fclose(fp);
}

// A function to write file_content and add a "," to a csv file
void WriteTextToCsv_Comma(FILE *fp, const char *file_name, char *file_content){
    // to append contents in the file
    fp = fopen(file_name, "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    dest = malloc(sizeof(char) * (strlen(file_content) + 1));
    strcpy(dest, file_content);
    strcat(dest,",");

    fputs(dest, fp);
    // close the file
    fclose(fp);
}


// A function to write file_content and add a "\n" to a csv file
void WriteTextToCsv_Nextline(FILE *fp, const char *file_name, char *file_content){
    // to append contents in the file
    fp = fopen(file_name, "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    dest = malloc(sizeof(char) * (strlen(file_content) + 2));
    strcpy(dest, file_content);
    strcat(dest,"\n");

    fputs(dest, fp);
    // close the file
    fclose(fp);
}

// A function to write file_content to a text file
void WriteTextToTxt(FILE *fp, const char *file_name, char *file_content){
    // to append contents in the file
    fp = fopen(file_name, "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    fputs(file_content, fp);
    // close the file
    fclose(fp);
}
