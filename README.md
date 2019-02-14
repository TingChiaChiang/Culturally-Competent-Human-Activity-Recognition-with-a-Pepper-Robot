# Project Title (Under Construction)
Culturally Competent Human Activity Recognition with a Pepper Robot

# Contents
 - [:blue_book: The general idea](#blue_book-the-general-idea)
 - [:file_folder: Folders](#file_folder-folders)
 - [:information_source: Further information](#information_source-further-information)
 - [:page_facing_up: References](#page_facing_up-references)

# :blue_book: The general idea
This culturally-competent HAR module is designed, developed and tested on a robot Pepper. It relies on an ontology-based framework (with Protégé) for the representation of activities-related knowledge, a Bayesian Network for reasoning (with Netica), and online version services (Google cloud vision dervice and Microsoft custom vision service) for the semantic labeling of images and scene classification.



# :file_folder: Folders
You should have a folder with the following subfolders in it:


* api_result - Results of google and microsoft vision api
* dataset - Contains training dataset and testing dataset
* doc - Documentation might be useful for users
* image - Images taken from Pepper robot or any image that users want it to be recognized
* include - Header file
* netica_output - Results of HAR; there are two files: Netica_output.csv and Netica_output.txt
* ontology - Contains Ontology file encoded representation of activities-related knowledge
* pepper_package - Contains a package which allows users to call the cloud vision services on Pepper
* src - Contains all the source code of the culturally competent HAR system
                                  
# :information_source: Further information

Some important files are:

(1). Ontology-based framework for the representation of activities-related knowledge + Bayesian Network for reasoning

* src/bn_netica.c ----------------------------Source code file of the proposed Bayesian Network in Netica-C
* src/bn_functions.c ------------------------ Functions used in bn_netica.c
* include/bn_functions.h -------------------- Header file for bn_functions.c
* src/caresses.py --------------------------- Ontology information loaded from Caresses.owl. 
                                            This file is imported in bn_functions.c for the integration of Bayesian Network 
                                            and ontology in culturally competent HAR system


(2). Cloud vision services + Robot control

* src/motion_module.py ---------------------- Robot motion algorithm: Finding a person, approaching him/her, 
                                            taking pictures, uploading images to cloud vision services, and saving the responses in PC.
* src/speak_module.py------------------------ Robot speak algorithm: Interacting with a user to query whether he/she is doing the activity which has the highest score based on the previously designed Bayesian network.
* src/google_micro_api.py ------------------- Cloud vision module for sending images to cloud services and getting the responses of them




(3). Compile above files together

* src/compile.sh ----------------------------- Script to compile and link everything and run the executable file "my_program"


# :page_facing_up: References
