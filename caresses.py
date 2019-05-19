#!/usr/bin/env python3
from owlready2 import *
import math
from decimal import *

# onto = get_ontology("file:///home/karen/Protege-5.2.0/Caresses.owl").load()
onto = get_ontology("file://../ontology/Caresses.owl").load()

class Protege_Extraction:
    def __init__(self, gen):

        self.GEN = gen
        self.period_state = []
        self.period_string = ""

        self.time_state = []
        self.time_string = ""
        self.time_dict_list = []

        self.room_state = []
        self.room_string = ""
        self.room_dict = {}

        self.object_list = []
        self.object_dict_list = []

        self.activity_state = []
        self.activity_string = ""
        self.activity_dict = {}


    """
    Get the states of Period node
    """
    def Get_Period_State(self):
        # exclude three periods which are not used in this work
        excluded_period = ["BeforeLunch", "EarlyAfternoon", "LateAfternoon"]
        for period in onto.PeriodOfTheDay.subclasses():
            if period.name not in excluded_period:
                # store a list of states of period and convert all the letters to uppercase
                self.period_state.append(period.name.upper())
                if self.period_string == "":
                    self.period_string = period.name.upper()
                else:
                    self.period_string = self.period_string + "," + period.name.upper()

        return self.period_state

    """
    Get the states of Time node and the information of CPT of Period node
    time_dict_list is a list of dictionaries with {period: probability} as key-value pair
    ex: [{'EVENING': Decimal('0.435'), 'DINNERTIME': Decimal('0.565')},....]
    Each dict belongs to an hour (time).
    The order of hour corresponds to the list in Protege ['EIGHTAM', 'EIGHTPM', 'ELEVENAM'.....].
    """
    def Get_Time_State(self):
        for hour in onto.Hour.subclasses():
            # store a list of states of time and convert all the letters to uppercase
            hour_name = hour.name.upper()
            self.time_state.append(hour_name)
            # store a string of time state
            if self.time_string == "":
                self.time_string = hour_name
            else:
                self.time_string = self.time_string + ","+ hour_name
            # create a dictionary for this specific hour
            hour_name ={}
            # append the created dictionary to a list
            self.time_dict_list.append(hour_name)

            count = 0
            likeliness_list = []
            instance_list = []
            for individual in hour.instances():
                if individual.name.split("_")[-2] == self.GEN:
                    for prop in individual.get_properties():
                        if prop == onto.hasPeriod:
                            for instance in prop[individual]:
                                time_like_num = Decimal((onto.hasLikeliness[instance])[0])
                                period_name = instance.is_a[0].name.upper()
                                count = count + 1
                                if count == 1:
                                    # hour_name[instance.is_a[0].name]=(onto.hasLikeliness[instance])[0]/(onto.hasLikeliness[instance])[0]
                                    hour_name[period_name]= round(Decimal(1.000),3)
                                    likeliness_list.append(time_like_num)
                                    instance_list.append(period_name)
                                elif count == 2:
                                    # sum of first two likeliness
                                    sum = Decimal(time_like_num+likeliness_list[0])
                                    # save the first period-likeliness pair to this hour
                                    hour_name[instance_list[0]]=round(likeliness_list[0]/sum,3)
                                    # save the second(current loop) period-likeliness pair to this hour
                                    hour_name[period_name]=round(time_like_num/sum,3)

                                    likeliness_list.append(time_like_num)
                                    instance_list.append(period_name)
                                elif count == 3:
                                    sum=(time_like_num+likeliness_list[0]+likeliness_list[1])
                                    hour_name[period_name]= round(time_like_num/sum,3)
                                    hour_name[instance_list[0]]=round(likeliness_list[0]/sum,3)
                                    hour_name[instance_list[1]]=round(likeliness_list[1]/sum,3)
                                    difference = Decimal(1.000)- (hour_name[period_name]+hour_name[instance_list[0]]+hour_name[instance_list[1]])
                                    hour_name[instance_list[1]] = hour_name[instance_list[1]] + difference
        return self.time_state



    """
    Get the states of Room node and CPT of Room node.
    room_dict is a dictionary with room state as key and propobility as value
    i.e. {'BATHROOM': Decimal('0.204'),'LIVINGROOM': Decimal('0.194')....}
    """
    def Get_Room_State(self):
        room_like_num = Decimal(0)
        excluded_room = ["SharedBedroom", "ShareDiningRoom", "Toilet"]
        included_room_instance = ["BEDROOM","BATHROOM","DININGROOM","LIVINGROOM","KITCHEN","PUJAROOM"]
        for room in onto.Room.subclasses():
            for individual in room.instances():
                if  individual.name.split("_")[-1] in included_room_instance and individual.name.split("_")[-2] == self.GEN:
                    for prop in individual.get_properties():
                        if prop == onto.hasLikeliness:
                            for instance in prop[individual]:
                                if self.room_string == "":
                                    self.room_string = room.name.upper()
                                else:
                                    self.room_string = self.room_string + ","+ room.name.upper()
                                self.room_state.append(room.name.upper())
                                self.room_dict[room.name.upper()] = round(Decimal(instance),3)
                                room_like_num  = self.room_dict[room.name.upper()] + room_like_num

        count_2 = 0
        new_likeliness = Decimal(0)
        for key in self.room_dict.keys():

            self.room_dict[key]=  round((self.room_dict[key]/room_like_num),3)
            new_likeliness =  new_likeliness + self.room_dict[key]
            count_2 = count_2 + 1
            if count_2 == len(self.room_dict):
                difference = Decimal(1.000) - new_likeliness
                self.room_dict[key]=  self.room_dict[key] + difference

        return self.room_state


    """
    Get the states and the information of CPT of Object nodes
    object_dict_list is an object of a list of dictionaries with {room: probability} as key-value pair,
    ex: [ {'BEDROOM': 0.0001, 'LIVINGROOM': 0.0001, 'KITCHEN': 0.0001, 'DININGROOM': 0.0001, 'PUJAROOM': 0.0001, 'BATHROOM': 0.3},,....]
    Each dict belongs to an object and prob = 0.0001 (can not be 0 = deterministic)
    which means that object does not exist in that room (for that particular culture) after Google API training
    The order of objects in the list corresponds to the list in Protege ['ALTAR', 'BATHROOMACCESSORY', 'BATHROOMCABINET',...]
    """
    def Get_Object_List(self):
        for object in onto.P_Object.subclasses():
            object_name = (object.name.split("_")[-1]).upper()
            self.object_list.append(object_name)
            object_name ={}

            for individual in object.instances():
                if individual.name.split("_")[-3] == self.GEN:
                    for prop in individual.get_properties():
                        if prop == onto.hasLikeliness:
                            for instance in prop[individual]:
                                object_name[individual.name.split("_")[-1]] = instance
                                # print(individual.name.split("_")[-1])
                                # print(instance)
            self.object_dict_list.append(object_name)
        for room in self.room_state:
            for object_dict in self.object_dict_list:
                if room not in list(object_dict.keys()):
                    object_dict[room] = 0.0001
        return self.object_list


    """
    Get the states and the information of CPT of Activity node.
    activity_dict is an object of a dictionary with room_period as key and a list of probabilities as value
    {room_period: [p_cooking , p_eating , p_others , p_praying , p_reading , p_showering, p_sleeping]}
    ex: {'BEDROOM_EVENING': [Decimal('0'), Decimal('0'), Decimal('0'), Decimal('0'), Decimal('1.000'), Decimal('0'), Decimal('0')],...}
    Each key-value pair encodes the probabilities of activites happended given a location and a period.
    The room_period pairs do not cover all the possible pairs which means if there are missing room_period pairs(not encoded in Protege),
    the p_others will be set to approxiamte 1.0 in Netica setting(should not be deterministic).
    """
    def Get_Activity_State(self):

        for activity in onto.Activity.subclasses():
            self.activity_state.append(activity.name.upper())
            if self.activity_string =="":
                self.activity_string = activity.name.upper()
            else:
                self.activity_string = self.activity_string + ","+ activity.name.upper()
            for individual in activity.instances():
                if individual.name.split("_")[-5] == self.GEN:
                    for prop in individual.get_properties():
                        if prop == onto.hasLikeliness:
                            cooking = eating = others = praying = reading = showering = sleeping = Decimal(0.000)
                            self.room_time_string = (individual.name.split("_")[-2])+"_"+(individual.name.split("_")[-1])


                            self.activity_dict[self.room_time_string]=[cooking , eating , others , praying , reading , showering, sleeping]
        for activity in onto.Activity.subclasses():

            for individual in activity.instances():
                if individual.name.split("_")[-5] == self.GEN:
                    for prop in individual.get_properties():
                        if prop == onto.hasLikeliness:
                            self.room_time_string = (individual.name.split("_")[-2])+"_"+(individual.name.split("_")[-1])
                            act_name =activity.name.upper().split("_")[-1]
                            act_likeliness =round(Decimal(onto.hasLikeliness[individual][0]),3)
                            if act_name == "COOKING":
                                self.activity_dict[self.room_time_string][0]= act_likeliness
                            elif act_name == "EATING":
                                self.activity_dict[self.room_time_string][1]= act_likeliness
                            elif act_name == "OTHERS":
                                self.activity_dict[self.room_time_string][2]= act_likeliness
                            elif act_name == "PRAYING":
                                self.activity_dict[self.room_time_string][3]= act_likeliness
                            elif act_name == "READING":
                                self.activity_dict[self.room_time_string][4]= act_likeliness
                            elif act_name == "SHOWERING":
                                self.activity_dict[self.room_time_string][5]= act_likeliness
                            elif act_name== "SLEEPING":
                                self.activity_dict[self.room_time_string][6]= act_likeliness
        for key in self.activity_dict.keys():
            sum = Decimal(0.000)
            for value in self.activity_dict[key]:
                sum = sum + value
            for i in range (len(self.activity_dict[key])):
                if self.activity_dict[key][i] !=  Decimal(0.000):
                    self.activity_dict[key][i]= round(self.activity_dict[key][i]/sum,3)
            after_sum = Decimal(0.000)
            for value in self.activity_dict[key]:
                after_sum = after_sum + value

            # the part makes the probability = 1.0
            difference = Decimal(1.000) - after_sum
            count = 0
            for i in range (len(self.activity_dict[key])):
                if self.activity_dict[key][i] !=  Decimal(0.000) and count == 0:
                    # print(difference)
                    self.activity_dict[key][i] = self.activity_dict[key][i] +difference
                    count = count +1
            after_sum = Decimal(0.000)
            for value in self.activity_dict[key]:
                after_sum = after_sum + value
        # print(self.activity_dict)
        return self.activity_state

    # a method to call all the Get_XXXXX_State for initialization
    def Call_All_States_Methods(self):
        self.Get_Period_State()
        self.Get_Time_State()
        self.Get_Room_State()
        self.Get_Object_List()
        self.Get_Activity_State()


class Vision_API_Result_Extraction:
    def __init__(self, file_arg):
        # vision API result path (should be the same path where output of pepper motion module is saved)
        self.path = file_arg

    """
    Return a list of highest-score acitivities
    ex: ['Sleeping', 'Sleeping', 'Sleeping', 'Sleeping', 'Sleeping']
    each element corresponds to the result of one image
    """
    def Get_Micro_Activity_Recognition_Result(self):
        "read the result of Activity_Recognition from text file"
        activity_result = []
        file = open(self.path)

        for line in file:
            if line.startswith("!") :
                activity_result.append(line.split("\n")[-2].split('!')[-1])
        return activity_result

    """
    Return a list of activity scores
    ex: [[0.0, 0.51, 4.97, 3.08, 0.45, 56.3], [0.2, 1.67, 3.09, 3.01, 0.52, 71.2],...]
    each list has the activitiy scores of one image
    the order of activites is  [Cooking,Eating,Puja_Praying,Reading,Showering,Sleeping]
    """
    def Get_Micro_Activity_Recognition_Score(self):
        "read the result of Activity_Recognition from text file"
        file = open(self.path)
        score_final_list =[]

        for line in file:
            score_list = [0,0,0,0,0,0]
            high_score = 0
            others_score = 0

            if line.startswith("-"):
                # print(line.split("\n")[-2].split('---'))
                for j in line.split("\n")[-2].split('---'):
                    # print(j.split(":")[0])
                    if j.split(":")[0] =='Cooking':
                        score_list[0] = j.split(":")[-1].split("%")[0].split(" ")[-1]
                    elif j.split(":")[0] == 'Eating':
                        score_list[1] = j.split(":")[-1].split("%")[0].split(" ")[-1]
                    elif j.split(":")[0] == 'Puja_Praying':
                        score_list[2] = j.split(":")[-1].split("%")[0].split(" ")[-1]
                    elif j.split(":")[0] == 'Reading':
                        score_list[3] = j.split(":")[-1].split("%")[0].split(" ")[-1]
                    elif j.split(":")[0] == 'Showering':
                        score_list[4] = j.split(":")[-1].split("%")[0].split(" ")[-1]
                    elif j.split(":")[0] == 'Sleeping':
                        score_list[5] = j.split(":")[-1].split("%")[0].split(" ")[-1]

                score_list = [float(score) for score in score_list]
                score_final_list.append(score_list)
        return score_final_list

    """
    Return a list of object lists
    each object list corresponds to one image result
    ex: [[], ['BED', 'TEXTILE', 'FURNITURE'], []....]
    could be empty list which means there is no any returned object matches the protege labels
    """
    def Get_Google_Labels_List(self):
        act_object = Protege_Extraction("SIN")
        act_object.Get_Object_List()
        total_list=[]
        # label_list=[None]*10
        name_list=[]
        label_dict={}
        store_list=[]
        count = 0

        """read labels from txt file and save it to dict["file_name"]=[label1, label2, label3]"""
        file = open(self.path)
        for line in file:
            # if line != "\n" and not line.startswith("-") :
            if line.startswith("#"):
                total_list.append(line.split("\n")[0].split("#")[-1].upper())
                # total_list.append(line.split("")[-2].replace(" ","").upper())
            elif line.startswith("."):
                total_list.append(line.split("\n")[0])



        label_list =[]
        for element in total_list:
            if not element.startswith(".") :
                if element.replace(" ", "") in act_object.object_list:
                    label_list.append(element.replace(" ", ""))
                    # print(element)
                count = count + 1
            elif element.startswith(".") and count != 0:
                count = count + 1
                store_list.append(label_list.copy())
                label_list = []
            else:
                count = count + 1

        store_list.append(label_list)
        # print(store_list)
        return store_list



"""Print out stuff"""
# # call Protege_Extraction class
# act_object = Protege_Extraction("SJP")
#
# # call Get_Period_State() method
# print(act_object.Get_Period_State())
# print(act_object.period_string)
#
# # call Vision_API_Result_Extraction class
# file_path = "../api_result/E_Images_Sleeping.py"
# api_object = Vision_API_Result_Extraction(file_path)
# api_object.Get_Google_Labels_List()
# print(api_object.Get_Micro_Activity_Recognition_Result())
# print(api_object.Get_Micro_Activity_Recognition_Score())
