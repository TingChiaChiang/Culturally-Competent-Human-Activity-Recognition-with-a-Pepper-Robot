You should have a folder with the following subfolders in it:

api_result -------------------------------- Results of google and microsoft vision api
doc --------------------------------------- Documentation might be useful for users
image ------------------------------------- Images taken from Pepper robot or any image that users want it to be recognized
include ----------------------------------- Header file
netica_output ----------------------------- Results of HAR; there are two files: Netica_output.csv and Netica_output.txt
ontology ---------------------------------- Contains Ontology file encoded representation of activities-related knowledge
src --------------------------------------- Contains all the source code of the culturally competent HAR system.


Some important files are:

(1). Ontology-based framework for the representation of activities-related knowledge + Bayesian Network for reasoning

src/bn_netica.c ----------------------------Source code file of the proposed Bayesian Network in Netica-C
src/bn_functions.c ------------------------ Functions used in bn_netica.c
include/bn_functions.h -------------------- Header file for bn_functions.c
src/caresses.py --------------------------- Ontology information loaded from Caresses.owl. 
                                            This file is imported in bn_functions.c for the integration of Bayesian Network 
                                            and Ontology in culturally competent HAR system


(2). Cloud vision services + Robot control

src/motion_module.py ---------------------- Robot motion algorithm: Finding a person, approaching him/her, 
                                            taking pictures, uploading images to cloud vision services, and saving the responses in PC.
src/speak_module.py------------------------ Robot speak algorithm: Interacting with a user to query whether he/she is doing the 
                                            activity which has the highest score based on the previously designed Bayesian network.
src/google_micro_api.py ------------------- Cloud vision module for sending images to cloud services and getting the responses of them




(3). Compile above files together

src/compile.sh----------------------------- Script to compile and link everything and run the executable file "my_program"
                                  

