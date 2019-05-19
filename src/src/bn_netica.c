#define NO_DEPRECATED_NETICA_FUNCS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Netica.h"
#include "NeticaEx.h"
#include <Python.h>
#include <time.h>
#include "../include/bn_functions.h"

#define CHKERR  {if (GetError_ns (env, ERROR_ERR, NULL))  goto error;}
#define LEN 150
#define CHECK_NULL(x) {if (!x) printf("Not logical value at line number %d in file %s\n", __LINE__, __FILE__);}

extern environ_ns *env;



int main (void)
{

	int i, j;
	// Python interpreter is initialized
	Py_Initialize();
	// A function declared in BN_functions.h to get ontology information from "caresses.py".
	InitializePyobject();

	net_bn *net;

	// here the Gen is the culture node
	node_bn *Gen, *Time, *Period, *Room, *Object[p_object_list_size], *Activity,*ActivityRecognition;

	double belief;
	char mesg[MESG_LEN_ns];
	int res;

	// env = NewNeticaEnviron_ns (NULL, NULL, NULL);
	env = NewNeticaEnviron_ns ("+MastrogiF/UGenova/310-6-A/7762",NULL,NULL);
	res = InitNetica2_bn (env, mesg);

	printf ("%s\n", mesg);

	if (res < 0)  return -1;

	// Create a BN
	net = NewNet_bn ("Caresses", env);
	CHKERR

	// Create nodes for net
	Gen     =  NewNode_bn ("Gen",      3, net);
	Time    =  NewNode_bn ("Time",     p_time_state_size, net);
	Period  =  NewNode_bn ("Period",   p_period_state_size, net);
	Room    =  NewNode_bn ("Room",     p_room_state_size, net);

	for (i=0; i<p_object_list_size;i++) {
		Object[i]   =  NewNode_bn (object_name[i],  2, net);
	}

	Activity = NewNode_bn ("Activity", p_activity_state_size, net);
	ActivityRecognition = NewNode_bn ("ActivityRecognition", p_activity_state_size, net);


	CHKERR

	// Set state names for Culture node
	SetNodeStateNames_bn (Gen, "Indian, Japanese, English");

	GetNodeStateNames();

	// Set state names for Time node
	SetNodeStateNames_bn (Time, time_string);

	// Set state names for Period node
	SetNodeStateNames_bn (Period, period_string);

	// Set state names for Room node
	SetNodeStateNames_bn (Room, room_string);

	// Set state names for Object nodes
	for (i=0; i<p_object_list_size; i++) {
		SetNodeStateNames_bn (Object[i], "True, False");
	}

	// Set state names for Activity node
	SetNodeStateNames_bn (Activity, activity_string);

	// Set state names for ActivityRecognition node
	SetNodeStateNames_bn (ActivityRecognition, "Cooking, Eating, Others, Puja_Praying, Reading, Showering, Sleeping");



	CHKERR

	// The order MATTERS!!!!!!!!!!!!!
	AddLink_bn (Gen, Room);

	for ( i = 0; i < p_object_list_size; i++ ) {
	    AddLink_bn (Gen ,  Object[i] );
	    AddLink_bn (Room , Object[i] );
	}

	AddLink_bn (Room, Activity);


	AddLink_bn (Gen, Activity);
	AddLink_bn (Gen, Period);
	AddLink_bn (Time, Period);
	AddLink_bn (Period, Activity);
	AddLink_bn (Activity, ActivityRecognition);




	CHKERR

	/*Set CPT for Cultute node
	                   Indian    Japanese    English
	*/
	SetNodeProbs(Gen, 1.0/3.0,   1.0/3.0,    1.0/3.0);


	/*Set CPT for Time node
	  you must be very careful to pass doubles for the probabilities (e.g. passing 0 instead of 0.0 will get you in trouble).
	                    |                            Hour
	              Time  |  EIGHTAM,EIGHTPM,ELEVENAM,ELEVENPM,FIVEAM,FIVEPM,FOURAM.........
	*/
	SetNodeProbs(Time, 1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0, 1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0,1.0/24.0);

	/*Get and set CPT for Period node*/
	for (i = 0; i < p_time_state_size; i++) {
    	// A function to get the probabilities of period states (given time, culture)
		GetPeriodCPT(i, p_SIN_period_cpt);
		//                                              |                                 Period
		//                       Gen            Time    |    AFTERNOON  ,    BREAKFASTTIME     DINNERTIME,       EVENING,      LUNCHTIME,      MORNING,      NIGHT
		SetNodeProbs(Period, "Indian",   str_time_item, afternoon_prob ,breakfasttime_prob, dinnertime_prob, evening_prob, lunchtime_prob, morning_prob, night_prob);

		GetPeriodCPT(i, p_SJP_period_cpt);
		SetNodeProbs(Period, "Japanese", str_time_item, afternoon_prob ,breakfasttime_prob, dinnertime_prob, evening_prob, lunchtime_prob, morning_prob, night_prob);

		GetPeriodCPT(i, p_SEN_period_cpt);
		SetNodeProbs(Period, "English",  str_time_item, afternoon_prob ,breakfasttime_prob, dinnertime_prob, evening_prob, lunchtime_prob, morning_prob, night_prob);

	}

	/*Get and set CPT for Room node*/
	// A function to get the probabilities of room states (given culture)
	GetRoomCPT(p_SIN_room_cpt);
	//                           |                                      Room
	//                     Gen   |   BATHROOM,      BEDROOM,     DININGROOM,     KITCHEN,   LIVINGROOM,      PUJAROOM
	SetNodeProbs (Room, "Indian",   room_prob[0], room_prob[1], room_prob[2], room_prob[3], room_prob[4], room_prob[5]);

    GetRoomCPT(p_SJP_room_cpt);
	SetNodeProbs (Room, "Japanese", room_prob[0], room_prob[1], room_prob[2], room_prob[3], room_prob[4], room_prob[5]);

	GetRoomCPT(p_SEN_room_cpt);
	SetNodeProbs (Room, "English",  room_prob[0], room_prob[1], room_prob[2], room_prob[3], room_prob[4], room_prob[5]);


	/*Get and set CPT for Object nodes*/
	//  outer loop  : iterate over 48 objects with iterator i
	//  innter loop : iterate over 6 rooms with iterator j
	for (i = 0; i < p_object_list_size; i++) {
		for (j = 0; j < p_room_state_size; j++) {
			   GetObjectsCPT(i,j, p_SIN_objects_cpt);
			   //                                                          State
			   //                          Gen           Room     |   True       False
			   SetNodeProbs (Object[i], "Indian",   str_room_item, obj_prob, 1.0 - obj_prob);

			   GetObjectsCPT(i,j, p_SJP_objects_cpt);
			   SetNodeProbs (Object[i], "Japanese", str_room_item, obj_prob, 1.0 - obj_prob);

			   GetObjectsCPT(i,j, p_SEN_objects_cpt);
			   SetNodeProbs (Object[i], "English",  str_room_item, obj_prob, 1.0 - obj_prob);
        }
	}


	/*Get and set CPT for Activity node */
	for ( i = 0; i < p_room_state_size; i++) {
		for (j = 0; j< p_period_state_size; j++) {
			// Indian
			GetActivityCPT(i,j, p_SIN_activity_cpt);
			//                                                                 |                                 Activity
			//                           Room          Gen          Period     |     P_COOKING,        P_EATING,        P_OTHERS,         P_PRAYING,        P_READING,        P_SHOWERING,         P_SLEEPING
			SetNodeProbs (Activity, str_room_item, "Indian",   str_period_item, activity_prob[0], activity_prob[1], activity_prob[2], activity_prob[3], activity_prob[4], activity_prob[5], activity_prob[6]);
			// Japanese
			GetActivityCPT(i,j, p_SJP_activity_cpt);
            SetNodeProbs (Activity, str_room_item, "Japanese", str_period_item, activity_prob[0], activity_prob[1], activity_prob[2], activity_prob[3], activity_prob[4], activity_prob[5], activity_prob[6]);
			// English
			GetActivityCPT(i,j, p_SEN_activity_cpt);
			SetNodeProbs (Activity, str_room_item, "English",  str_period_item, activity_prob[0], activity_prob[1], activity_prob[2], activity_prob[3], activity_prob[4], activity_prob[5], activity_prob[6]);
		}
	}


	/*Set CPT for Activity recognition node*/
	//                                              |                                 ActivityRecognition
	//                                              | Cooking,    Eating,     Others,     Praying,    Reading,    Showering,  Sleeping
	SetNodeProbs (ActivityRecognition, "P_COOKING",   0.994,      0.001,      0.001,      0.001,      0.001,      0.001,      0.001);
	SetNodeProbs (ActivityRecognition, "P_EATING",    0.001,      0.994,      0.001,      0.001,      0.001,      0.001,      0.001);
	SetNodeProbs (ActivityRecognition, "P_OTHERS",    0.001,      0.001,      0.994,      0.001,      0.001,      0.001,      0.001);
	SetNodeProbs (ActivityRecognition, "P_PRAYING",   0.001,      0.001,      0.001,      0.994,      0.001,      0.001,      0.001);
	SetNodeProbs (ActivityRecognition, "P_READING",   0.001,      0.001,      0.001,      0.001,      0.994,      0.001,      0.001);
	SetNodeProbs (ActivityRecognition, "P_SHOWERING", 0.001,      0.001,      0.001,      0.001,      0.001,      0.994,      0.001);
	SetNodeProbs (ActivityRecognition, "P_SLEEPING",  0.001,      0.001,      0.001,      0.001,      0.001,      0.001,      0.994);



	CHKERR

	CompileNet_bn (net);


	CHKERR




	FILE *fp_csv, *fp_txt;
	const char *file_name_csv ="../netica_output/Netica_output.csv";
	const char *file_name_txt ="../netica_output/Netica_output.txt";

	// vision API result path (should be the same path where output of pepper motion module is saved)
	char *file_path = "../api_result/E_Images_Sleeping.py";
	char *actual_activity = "sleeping";
	char *default_culture = "English";
	// the hour amongst 0-23
	int default_time = 0;
	printf("Default culture is %s, default hour is %s\n\n", default_culture, hour_name[default_time]);

	// char used for printing message
	char str[100];

	// variables for object nodes
	PyObject *google_labels_list_item, *p_one_list_item, *puni_one_list_item;
	Py_ssize_t google_labels_list_item_size;
	char *one_list_item_string;

	// variables for activity nodes
	PyObject *p_uni_activity_item, *p_byte_activity_item;
	char *str_activity_item;


	// variables for enterfinding of microsoft activity recognition
	PyObject *p_micro_score_one_image, *p_micro_score_each_act;
	double one_act_score;
	PyObject *p_micro_result, *p_uni_micro_result;
	char *micro_result_string;

	// initial values of score for each activity
	double cook_score = 0;
	double eat_score = 0;
	double others_score = 0;
	double pray_score = 0;
	double read_score = 0;
	double shower_score = 0;
	double sleep_score = 0;

	// create csv and txt files where the result of BN will be printed
	CreateNewFile(fp_csv, file_name_csv, "Image,Actual Activity,Culture,Time,Room Result(only based on init setting),Activity Result (only based on init setting),Activity Result (Microsoft alone),Final Room Result,Final Activity Result\n");
	CreateNewFile(fp_txt, file_name_txt, "");
	// get the python Methods for enterfinding
	EnterFindingSetting(file_path);
	// setting for culture enterfinding
	CultureStateSetting(default_culture);
	// setting for time enterfinding
	TimeStateSetting(default_time);


	for (i = 0; i < image_number; i++) {

		double belief;
		double highest_belief_room_network = 0;
		double highest_belief_act_network = 0;
		char* room_result_network;
		char* act_result_network;


		double highest_belief_room_micro = 0;
		double highest_belief_act_micro = 0;
		char* room_result_micro;
		char* act_result_micro;

		// write the index of image
        sprintf(str, "Image%d", i);
		WriteTextToCsv_Comma(fp_csv, file_name_csv, str);
		sprintf(str, "\n------------Image %d------------ \n", i);
		WriteTextToTxt(fp_txt, file_name_txt, str);

		// write the actual activity performed
	    WriteTextToCsv_Comma(fp_csv, file_name_csv, actual_activity);

		/* Enter finiding for culture node */
		EnterFinding ("Gen", gen_name_ptr, net);
		WriteTextToCsv_Comma(fp_csv, file_name_csv, gen_name_ptr);
		sprintf(str, "Culture node info:\n%s\n", gen_name_ptr);
		WriteTextToTxt(fp_txt, file_name_txt, str);

		/* Enter finiding for time node */
		EnterFinding ("Time", hour_name[time_index], net);
		WriteTextToCsv_Comma(fp_csv, file_name_csv, hour_name[time_index]);
	    sprintf(str, "\nTime node info:\n%s\n", hour_name[time_index]);
		WriteTextToTxt(fp_txt, file_name_txt, str);

		/* Enter finidings for objects nodes */
		// get the object list for this image
		google_labels_list_item = PyList_GetItem(p_google_labels_list, i);
		CHECK_NULL(google_labels_list_item)
		google_labels_list_item_size = PyList_Size(google_labels_list_item);

    	WriteTextToTxt(fp_txt, file_name_txt, "\nObject Nodes Info:\n");
		// google_labels_list_item_size depends the number of labels dectected in this image
		if (google_labels_list_item_size == 0)
		WriteTextToTxt(fp_txt, file_name_txt, "None\n");

	    for (j = 0; j < google_labels_list_item_size; j++) {
			p_one_list_item =  PyList_GetItem(google_labels_list_item, j);
	        CHECK_NULL(p_one_list_item)
	        puni_one_list_item =  PyUnicode_AsEncodedString(p_one_list_item, "utf-8","Error!!!");
	        CHECK_NULL(puni_one_list_item)
			// get the objects returned for this image
	        one_list_item_string = PyBytes_AS_STRING(puni_one_list_item);
		    EnterFinding (one_list_item_string, "True", net);
			sprintf(str, "%s\n", one_list_item_string);
			WriteTextToTxt(fp_txt, file_name_txt, str);
	    }

        // get the belief for room node (find which room is the most possible one)
		WriteTextToTxt(fp_txt, file_name_txt, "\n<<Before Microsoft Finding>>\nRoom Node Info:\n");
	    for (j = 0; j < p_room_state_size; j++) {
			p_uni_room_item = PyList_GetItem(p_room_state, j);
			p_byte_room_item = PyUnicode_AsEncodedString(p_uni_room_item, "utf-8","Error!!!");
			str_room_item = PyBytes_AS_STRING(p_byte_room_item);

			belief = GetNodeBelief ("Room", str_room_item, net);

			if (belief > highest_belief_room_network) {
				highest_belief_room_network = belief;
				room_result_network = str_room_item;
	        }

			sprintf(str, "(only based on init setting) The probability that now he or she is in %s is  %g\n",str_room_item, belief);
			WriteTextToTxt(fp_txt, file_name_txt, str);
			// printf("(only based on init setting) The probability that now he or she is in %s is  %g\n",str_room_item, belief);
		}
		WriteTextToCsv_Comma(fp_csv, file_name_csv, room_result_network);


		// get the belief for activity node (find which activity is the most possible one)
		WriteTextToTxt(fp_txt, file_name_txt, "\nActivity Node Info:\n");
	    for (j = 0; j < p_activity_state_size; j++) {
			p_uni_activity_item = PyList_GetItem(p_activity_state, j);
			p_byte_activity_item = PyUnicode_AsEncodedString(p_uni_activity_item, "utf-8","Error!!!");
			str_activity_item = PyBytes_AS_STRING(p_byte_activity_item);

			belief = GetNodeBelief ("Activity", str_activity_item, net);

			if (belief > highest_belief_act_network) {
				highest_belief_act_network = belief;
				act_result_network = str_activity_item;
	        }
			sprintf(str, "(only based on init setting) The probability that now he or she is  %s is  %g\n",str_activity_item, belief);
			WriteTextToTxt(fp_txt, file_name_txt, str);
		}
		WriteTextToCsv_Comma(fp_csv, file_name_csv, act_result_network);




		//  Microsoft finding part - Enter likelihood findings for Activity recognition node

		// ex: [[0.0, 0.51, 4.97, 3.08, 0.45, 56.3], [0.2, 1.67, 3.09, 3.01, 0.52, 71.2],...]
		// each list has the activitiy scores of one image
		// the order of activites is  [Cooking,Eating,Puja_Praying,Reading,Showering,Sleeping]
		p_micro_score_one_image = PyList_GetItem(p_micro_act_rec_score, i);
		CHECK_NULL(p_micro_score_one_image);
		//  Cooking,   Eating,   Others,   Praying,      Reading,     Showering,   Sleeping
		prob_bn like[7];
		like[0] = 0;
		like[1] = 0;
		like[2] = 0;
		like[3] = 0;
		like[4] = 0;
		like[5] = 0;
		like[6] = 0;
		// printf("Microsoft recognition result:\n");

		for (j = 0; j < 6; j++) {
			p_micro_score_each_act = PyList_GetItem(p_micro_score_one_image, j);
			CHECK_NULL(p_micro_score_each_act);

			one_act_score = PyFloat_AsDouble(p_micro_score_each_act);
			// printf("score is %f\n", one_act_score);
			if (j > 1) {
				like[j+1] = one_act_score/100;
			} else {
				like[j] = one_act_score/100;
			}
		}
		EnterNodeLikelihood_bn (ActivityRecognition, like);


		// get the activity with highest microsoft score in this image
		p_micro_result = PyList_GetItem(p_micro_act_rec_result, i);
		CHECK_NULL(p_micro_result);
		p_uni_micro_result = PyUnicode_AsEncodedString(p_micro_result, "utf-8","Error!!!");
		CHECK_NULL(p_uni_micro_result);
		micro_result_string =  PyBytes_AS_STRING(p_uni_micro_result);

		WriteTextToCsv_Comma(fp_csv, file_name_csv, micro_result_string);


		// get the belief for room node (after microsoft likelihood findings)
		WriteTextToTxt(fp_txt, file_name_txt, "\n<<After Microsoft Finding>>\nRoom Node Info:\n");
	    for (j = 0; j < p_room_state_size; j++) {
			p_uni_room_item = PyList_GetItem(p_room_state, j);
			p_byte_room_item = PyUnicode_AsEncodedString(p_uni_room_item, "utf-8","Error!!!");
			str_room_item = PyBytes_AS_STRING(p_byte_room_item);

			belief = GetNodeBelief ("Room", str_room_item, net);

			if (belief > highest_belief_room_micro) {
				highest_belief_room_micro = belief;
				room_result_micro = str_room_item;
	        }
			sprintf(str, "(After Microsoft) The probability that now he or she is in %s is  %g\n",str_room_item, belief);
			WriteTextToTxt(fp_txt, file_name_txt, str);
		}
		WriteTextToCsv_Comma(fp_csv, file_name_csv, room_result_micro);


		// get the belief for activity node  (after microsoft likelihood findings)
		WriteTextToTxt(fp_txt, file_name_txt, "\nActivity Node Info:\n");
		for (j = 0; j < p_activity_state_size; j++) {
			p_uni_activity_item = PyList_GetItem(p_activity_state, j);
			p_byte_activity_item = PyUnicode_AsEncodedString(p_uni_activity_item, "utf-8","Error!!!");
			str_activity_item = PyBytes_AS_STRING(p_byte_activity_item);

			belief = GetNodeBelief ("Activity", str_activity_item, net);

			if (belief > highest_belief_act_micro) {
				highest_belief_act_micro = belief;
				act_result_micro = str_activity_item;
			}
			sprintf(str, "(After Microsoft) The probability that now he or she is  %s is  %g\n",str_activity_item, belief);
			WriteTextToTxt(fp_txt, file_name_txt, str);

			// sum up the scores for each activity (only needed when there are series images taken for activity recognition , not for one image)
			if (strcmp(str_activity_item, "P_COOKING") == 0) {
				cook_score = cook_score + belief;
			} else if (strcmp(str_activity_item, "P_EATING") == 0) {
				eat_score = eat_score + belief;
			} else if (strcmp(str_activity_item, "P_OTHERS") == 0) {
				others_score = others_score + belief;
			} else if (strcmp(str_activity_item, "P_PRAYING") == 0) {
		    	pray_score = pray_score + belief;
			} else if (strcmp(str_activity_item, "P_READING") == 0) {
				read_score = read_score + belief;
			} else if (strcmp(str_activity_item, "P_SHOWERING") == 0) {
				shower_score = shower_score + belief;
			} else if (strcmp(str_activity_item, "P_SLEEPING") == 0) {
				sleep_score = sleep_score + belief;}
		}
		WriteTextToCsv_Nextline(fp_csv, file_name_csv, act_result_micro);
		// delete all the findings
		RetractNetFindings_bn(net);
	}

	// print out the activity scores in txt file
	WriteTextToTxt(fp_txt, file_name_txt, "\n");
	sprintf(str, "cook_score:%f\n", cook_score);
	WriteTextToTxt(fp_txt, file_name_txt, str);
	sprintf(str, "eat_score:%f\n", eat_score);
	WriteTextToTxt(fp_txt, file_name_txt, str);
	sprintf(str, "others_score:%f\n", others_score);
	WriteTextToTxt(fp_txt, file_name_txt, str);
	sprintf(str, "pray_score:%f\n", pray_score);
	WriteTextToTxt(fp_txt, file_name_txt, str);
	sprintf(str, "read_score:%f\n", read_score);
	WriteTextToTxt(fp_txt, file_name_txt, str);
	sprintf(str, "shower_score:%f\n", shower_score);
	WriteTextToTxt(fp_txt, file_name_txt, str);
	sprintf(str, "sleep_score:%f\n", sleep_score);
	WriteTextToTxt(fp_txt, file_name_txt, str);








 CHKERR


 Py_Finalize();
end:
	DeleteNet_bn (net);
	res = CloseNetica_bn (env, mesg);
	printf ("%s\n", mesg);
	printf ("Press <enter> key to quit %s\n", mesg);
	printf("!!!! The results have been printed in Netica_output.csv and Netica_output.txt (in src directory ) !!!!!\n\n");
	// getchar();
	return (res < 0) ? -1 : 0;

error:
	fprintf (stderr, "NeticaDemo: Error in %s\n",
	         ErrorMessage_ns (GetError_ns (env, ERROR_ERR, NULL)));
	goto end;
}
