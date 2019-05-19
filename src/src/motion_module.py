#! /usr/bin/env python
# -*- encoding: UTF-8 -*-
import qi
import argparse
import sys
import math
import almath

# Perception part
import time
from datetime import datetime
from naoqi import ALProxy

# Taking image part
import vision_definitions
import Image

# cloud API part
from google_micro_api import Google_Microsoft_Fun


class Pepper:
    def __init__(self, arg_session):

        """Initialization"""
        self.session = arg_session
        # the modules commonly need to be used
        self.motion_service = self.session.service("ALMotion")
        self.posture_service = self.session.service("ALRobotPosture")
        self.AutonomousLife_service = self.session.service("ALAutonomousLife")
        self.BasicAwareness_service = self.session.service("ALBasicAwareness")
        # the distance that a robot needs to move toward a human (along x-axis)
        self.Xlast = 0



    # wake up the robot and enable some autonomous ablities
    def SetAutonomousLifeParam(self):
        # Wake up robot
        self.motion_service.wakeUp()

        # Send robot to Stand Init
        self.posture_service.goToPosture("StandInit", 0.5)

        # Enable arms control by move algorithm
        self.motion_service.setMoveArmsEnabled(True, True)

        # Food contact protection
        self.motion_service.setMotionConfig([["ENABLE_FOOT_CONTACT_PROTECTION", True]])

        # Disable the robot to make its eye LEDs blink when it sees someone and when it is interacting.
        self.AutonomousLife_service.setAutonomousAbilityEnabled("AutonomousBlinking", False)
        # Enable slight movements the robot does autonomously when its limbs are not moving.
        self.AutonomousLife_service.setAutonomousAbilityEnabled("BackgroundMovement", True)
        # Allows the robot to react to the environment to establish and keep eye contact with people.
        self.AutonomousLife_service.setAutonomousAbilityEnabled("BasicAwareness", True)
        # Enables to start autonomously movements during the speech of the robot.
        self.AutonomousLife_service.setAutonomousAbilityEnabled("SpeakingMovement", True)

        # set the the tracking mode: "BodyRotation" which uses the head and the rotation of the body
        self.BasicAwareness_service.setTrackingMode("BodyRotation")
        # self.BasicAwareness_service.setTrackingMode("WholeBody")

        # check whether an autonomous ability is enabled.
        # print(AutonomousLife_service.getAutonomousAbilityEnabled("BasicAwareness"))
        # print(AutonomousLife_service.getAutonomousAbilityEnabled("SpeakingMovement"))


    # Disable the previous autonomous ablities
    def StopAutonomousLifeParam(self):
        # Wake up robot
        self.motion_service.wakeUp()

        # Send robot to Stand Init
        self.posture_service.goToPosture("StandInit", 0.5)

        # Enable arms control by move algorithm
        self.motion_service.setMoveArmsEnabled(True, True)

        # Food contact protection
        self.motion_service.setMotionConfig([["ENABLE_FOOT_CONTACT_PROTECTION", True]])

        self.AutonomousLife_service.setAutonomousAbilityEnabled("AutonomousBlinking", False)
        self.AutonomousLife_service.setAutonomousAbilityEnabled("BackgroundMovement", False)
        self.AutonomousLife_service.setAutonomousAbilityEnabled("BasicAwareness", False)
        self.AutonomousLife_service.setAutonomousAbilityEnabled("SpeakingMovement", False)


    # The robot goes to a relaxed and safe position and sets Motor off.
    def Rest(self):
        self.motion_service.rest()
        # print motion state
        # print(self.motion_service.getSummary())

    def FaceDectection(self):
        face_service = self.session.service("ALFaceDetection")
        # Sets the image resolution for the extractor
        face_service.setResolution(2)
        face_service.setTrackingEnabled(True)
        period = 100
        face_service.subscribe("Test_Face", period, 0.0)
        memory_service = self.session.service("ALMemory")
        memFaceValue ="FaceDetected"
        val_face = memory_service.getData(memFaceValue, 0)

        # A while loop that reads the val_face and checks whether faces are detected.
        while (val_face == [] or val_face == None):

            val_face = memory_service.getData(memFaceValue, 0)
            # print("can't see face")

        # once the robot detects a face
        else:
            print("found face")
            IP = "192.168.1.5"
            tts = ALProxy("ALTextToSpeech", IP, 9559)
            # the robot will say "I saw a face!"
            tts.say("I saw a face!")

            # get the angle of headyaw
            names  = ["HeadYaw"]
            useSensors  = True
            angle = self.motion_service.getAngles(names, useSensors)
            print("HeadYaw angle is:", angle[0])
            face_service.unsubscribe("Test_Face")

            # disable the the robot to react to the environment to establish and keep eye contact with people.
            self.AutonomousLife_service.setAutonomousAbilityEnabled("BasicAwareness", False)
            self.motion_service.setStiffnesses("Head", 1.0)

            # move the wheel toward the direction where the robot detected the face
            self.motion_service.moveTo(0, 0, angle[0], _async=True)
            self.motion_service.waitUntilMoveIsFinished()
            # make pepper move his head back to the center
            angles = [0]
            fractionMaxSpeed  = 0.2
            self.motion_service.setAngles(names, angles, fractionMaxSpeed)


    def GetDistance(self):
        # use ALPeoplePerception module to get the distance between a robot and a human
        people_service = self.session.service("ALPeoplePerception")

        period = 100
        people_service.subscribe("Test_People", period, 0.0)
        memory_service = self.session.service("ALMemory")
        # the value history of event "PeoplePerception/PeopleDetected" is stored into ALMemory
        memPeopleValue = "PeoplePerception/PeopleDetected"
        # to access the event value
        val_people = memory_service.getData(memPeopleValue, 0)
        time.sleep(1)
        # A while loop that reads the val_people and checks whether a person is detected.
        while (val_people == [] or val_people == None):
            val_people = memory_service.getData(memPeopleValue, 0)
            print("can't find person")
        else:
            # print(val_people)
            print("find person")
            timeStamp = val_people[0]
            personDataArray = val_people[1]
            for personData in personDataArray:
                personID = personData[0]
                DistanceToCamera = personData[1]
                PitchAngleInImage = personData[2]
                YawAngleInImage = personData[3]
                print("person ID %f" %(personID))
                print("DistanceToCamera %f" %(DistanceToCamera))
                print("PitchAngleInImage %f" %(PitchAngleInImage))
                # robot will move to the place where is 1.5 m away from the user
                self.Xlast =  DistanceToCamera -1.5
                # self.Xlast =  DistanceToCamera -2.5


    def NaviageToTarget(self):

        # navigation_service = self.session.service("ALNavigation")
        try:
            Xmove = self.Xlast
            # if the distance (along x-axis) that the robot needs to move not equal to 0
            if (Xmove!= None):
                initRobotPosition = almath.Pose2D(self.motion_service.getRobotPosition(False))
                # navigation_service.navigateTo(Xmove,0)
                print("Distance (along x-axis) that the robot needs to move \n" +
                "(in meter) to approach a human:", Xmove)
                self.motion_service.moveTo(Xmove, 0, 0, _async = True)
                self.motion_service.waitUntilMoveIsFinished()
                time.sleep(0.5)
                endRobotPosition = almath.Pose2D(self.motion_service.getRobotPosition(False))
                robotMove = almath.pose2DInverse(initRobotPosition)*endRobotPosition
                print("Distance change and angle change after moveTo() function:",robotMove)


        except KeyboardInterrupt:
            print "Interrupted by user, stopping moving"
            #stop
            self.motion_service.stopMove()
            sys.exit(0)


    # Get images from Pepper’s cameras and store images
    def GetImage(self, i):

        # Get the service ALVideoDevice.
        video_service = self.session.service("ALVideoDevice")

        # Register a Generic Video Module
        resolution = 2    # VGA 640*480px
        colorSpace = 11   # RGB
        fps = 15
        # use 3D camera in the eyes
        # video_service.setActiveCamera(2)

        # get the default camera is
        # activeCam =video_service.getActiveCamera()
        # print("Active camera %d\n" %(activeCam))

        nameId = video_service.subscribe("python_client", resolution, colorSpace, fps)
        time.sleep(1)
        print "getting image " + str(i)

        # Get image remotely and save it as a JPG using ImageDraw package
        naoImage = video_service.getImageRemote(nameId)
        video_service.unsubscribe(nameId)

        # Get the image size and pixel array.
        imageWidth = naoImage[0]
        imageHeight = naoImage[1]
        array = naoImage[6]
        image_string = str(bytearray(array))

        # Create a PIL Image from the pixel array.
        im = Image.fromstring("RGB", (imageWidth, imageHeight), image_string)

        # Save the image in the following directory
        im.save("../image/" + save_path_name + "/" + save_path_name + "_" + str(i) + ".jpg", "JPEG")

        im.show()


    def TakeClosePic(self):
        # x – normalized, unitless, velocity along X-axis.
        # +1 and -1 correspond to the maximum velocity in the forward and backward directions, respectively.
        x = 1.0
        y = 0.0
        theta = 0.0
        # index of image
        image_i = 0
        frequency = 0.1
        # get current time
        t0 = time.time()
        initRobotPosition = almath.Pose2D(self.motion_service.getRobotPosition(False))


        #  since now the robot is 1.5 away from the human
        #  let the robot move 0.7 meters forward while taking photos(now the velocity is 0.1 meter per second)
        #  in order to keep 0.8 meter distance away from human

        while ((time.time() - t0) <= 7.0):
            # once 7s has been passed, break the loop
            if (time.time() - t0) > 7.0:
                break
            # print "time difference :" + str((time.time() - t0))
            self.motion_service.moveToward(x, y, theta, [["MaxVelXY", frequency]])
            image_i += 1
            self.GetImage(image_i)

        # stop the robot
        self.motion_service.stopMove()
        endRobotPosition = almath.Pose2D(self.motion_service.getRobotPosition(False))
        robotMove = almath.pose2DInverse(initRobotPosition)*endRobotPosition
        robotMove.theta = almath.modulo2PI(robotMove.theta)
        print "Distance change and angle change after approaching the human:" ,robotMove



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    # IP = "192.168.1.5"
    parser.add_argument("--ip", type=str, default="192.168.1.5",
                        help="Robot IP address. On robot or Local Naoqi: use '127.0.0.1'.")
    parser.add_argument("--port", type=int, default=9559,
                        help="Naoqi port number")
    args = parser.parse_args()
    session = qi.Session()
    try:
        session.connect("tcp://" + args.ip + ":" + str(args.port))
    except RuntimeError or KeyboardInterrupt:
        print ("Can't connect to Naoqi at ip \"" + args.ip + "\" on port " + str(args.port) +".\n"
               "Please check your script arguments. Run with -h option for help.")
        motion_service = session.service("ALMotion")
        print("stop")
        motion_service.stopMove()
        sys.exit(1)

    # create an object of Pepper class
    pepper_object = Pepper(session)

    # pepper_object.StopAutonomousLifeParam
    # pepper_object.Rest()

    pepper_object.SetAutonomousLifeParam()
    pepper_object.FaceDectection()
    time.sleep(2)
    pepper_object.GetDistance()
    pepper_object.NaviageToTarget()
    pepper_object.TakeClosePic()
    print "calling google microsoft API........"
    time.sleep(0.5)

    save_path_name = "E_Images_Sleeping"
    image_path = "../image/" + save_path_name
    api_path = "../api_result/" + save_path_name +".py"

    Google_Microsoft_Fun(image_path,api_path)
