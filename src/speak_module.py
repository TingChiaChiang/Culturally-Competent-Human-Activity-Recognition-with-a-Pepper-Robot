#! /usr/bin/env python

from naoqi import ALProxy
import qi
import argparse
import sys
import time


IP = "192.168.1.5"
PORT=  9559
tts = ALProxy("ALTextToSpeech", IP, PORT)
memoryProxy = ALProxy("ALMemory", IP, PORT)
autolifeProxy = ALProxy("ALAutonomousLife", IP, PORT)
autolifeProxy.setAutonomousAbilityEnabled("BackgroundMovement",True)
autolifeProxy.setAutonomousAbilityEnabled("SpeakingMovement",True)
animatedProxy = ALProxy("ALTextToSpeech", IP, PORT)
# animatedProxy = ALProxy("ALAnimatedSpeech", IP, PORT)
# speakProxy = ALProxy("ALSpeakingMovement", IP, PORT)
# print(speakProxy.getMode())
autolifeProxy.setAutonomousAbilityEnabled("BasicAwareness",False)
memVoiceValue ="WordRecognized"

file = open("../netica_output/Netica_output.txt", 'r')

# initialization of acitivities scorses
cook_score = eat_score = others_score = pray_score = read_score = shower_score = sleep_score = 0
highest_score = 0
score_list = [cook_score, eat_score, others_score, pray_score, read_score, shower_score, sleep_score]


for line in file:
    if line.startswith("cook_score"):
        cook_score  = float(line.split(":")[-1].split("\n")[0])
    elif line.startswith("eat_score"):
        eat_score = float(line.split(":")[-1].split("\n")[0])
    elif line.startswith("others_score"):
        others_score = float(line.split(":")[-1].split("\n")[0])
    elif line.startswith("pray_score"):
        pray_score = float(line.split(":")[-1].split("\n")[0])
    elif line.startswith("read_score"):
        read_score = float(line.split(":")[-1].split("\n")[0])
    elif line.startswith("shower_score"):
        shower_score = float(line.split(":")[-1].split("\n")[0])
    elif line.startswith("sleep_score"):
        sleep_score = float(line.split(":")[-1].split("\n")[0])


score_dict = {"cooking":cook_score, "eating": eat_score, "doing something interesting": others_score, "praying": pray_score,
 "reading": read_score, "showering":shower_score,"sleeping":sleep_score }

# find the activity with the highest score
for item in score_dict.items():
    if item[1] > highest_score:
        highest_score = item[1]
        highest_act = item[0]
animatedProxy.say("Hey! Are you "+ highest_act+ "?")
del score_dict[highest_act]

# use ALSpeechRecognition module
sr = ALProxy("ALSpeechRecognition", IP, 9559)

try:
    # set the language of the speech recognition engine to English:
    sr.setLanguage("English")
    # add "yes", "no" and "please" to the vocabulary (without wordspotting)
    vocabulary = ["yes", "no", "please"]
    sr.setVocabulary(vocabulary,True)
    WordReco = memoryProxy.getData(memVoiceValue,0)
    # memoryProxy.subscribeToEvent("WordRecognized")

    # Start the speech recognition engine with user Test_ASR
    print 'Speech recognition engine started'
    sr.subscribe("Test_ASR")

    # continue the loop until pepper heard the word
    while (WordReco[0] == ''):
        WordReco = memoryProxy.getData(memVoiceValue,0)
        # print(WordReco)
        if 'yes' in WordReco[0]:
            print("the human said %s" %WordReco)
            time.sleep(0.1)
            animatedProxy.say("okay, good to know, please continue !!!")
        elif 'no' in WordReco[0]:
            print("the human said %s" %WordReco)
            time.sleep(0.1)
            highest_score =0
            for item in score_dict.items():
                if item[1] > highest_score:
                    highest_score = item[1]
                    highest_act = item[0]
            animatedProxy.say("then, are you " + highest_act + "?")
            del score_dict[highest_act]
            memoryProxy.removeData(memVoiceValue)
            time.sleep(1.0)
            sr.unsubscribe("Test_ASR")

            # if the answer is wrong, continue asking the next activity with second highest score, third, fourt...
            WordReco = memoryProxy.getData(memVoiceValue,0)
            sr.subscribe("Test_ASR")
            # print(WordReco[0])
            continue


    sr.unsubscribe("Test_ASR")
    print 'Speech recognition engine finished'
    sr.setVocabulary(vocabulary, True)
except KeyboardInterrupt:
    sr.unsubscribe("Test_ASR")
    sr.setVocabulary(vocabulary, True)
    print 'Speech recognition engine finished'
