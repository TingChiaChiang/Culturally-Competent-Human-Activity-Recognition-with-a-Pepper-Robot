#!/usr/bin/env python
import io
import os
import json

# Imports the Google Cloud client library
from google.cloud import vision
from google.cloud.vision import types

# Imports the Microsoft custom vision library
from azure.cognitiveservices.vision.customvision.prediction import prediction_endpoint
from azure.cognitiveservices.vision.customvision.prediction.prediction_endpoint import models

def Google_Microsoft_Fun(image_path, file_name_created):


    # Google: Instantiates a google client
    client = vision.ImageAnnotatorClient()

    # Microsoft: a trained endpoint that can be used to make a prediction
    prediction_key = "82399d964ace4ef8a3b8a8b5a2540540"
    predictor = prediction_endpoint.PredictionEndpoint(prediction_key)
    # iteration = trainer.train_project(3c2e78ba-4367-4941-8cf8-b668deb28726)

    # Activity_Recognition_New
    project_id = "d620a7d4-2aec-42a1-8ea3-810077cdcbc2"

    """create a list of file paths of images"""
    path = image_path
    dirs = os.listdir(path)
    image_list = []
    activity_highest = []
    # print(dirs)

    for f in dirs:
        if f.endswith('jpg'):
                image_list.append(os.path.join(path,f))

    # sort the images with the number of image (P.S the convention of image name has to be whatever_1.jpg, whatever_2.jpg)
    image_list = sorted(image_list,key = lambda x: int(x.split('.')[-2].split('_')[-1]))

    """write the returned labels  """
    python_name =  file_name_created

    file = open(python_name, "w")
    for f in image_list:
        with open(python_name, "a")as file:
            file.write(f)
            file.write("\n")
        with io.open(f, 'rb') as image_file:
            # google_labels
            content = image_file.read()
            image = types.Image(content=content)
            response = client.label_detection(image=image)
            labels = response.label_annotations
            for i in labels:
                with open(python_name,"a")as file:
                     file.write("#")
                     file.write(i.description.encode("utf-8"))
                     file.write("\n")

        # microsoft
        with io.open(f, 'rb') as image_file:
            results = predictor.predict_image(project_id, image_file)
            count = 0
            with open(python_name, "a") as file:
                for prediction in results.predictions:
                    if count == 0:

                        str = prediction.tag_name.encode("utf-8")
                        # print("highest score is "+str)

                        file.write("!")
                        file.write(str)
                        file.write("\n")
                    # print ("\t" + prediction.tag_name + ": {0:.2f}%".format(prediction.probability * 100))
                    file.write("---" + prediction.tag_name + ": {0:.2f}%".format(prediction.probability * 100))
                    count = count + 1
                file.write("\n")

    file.close()


# run directly from the existing images
# image_path = "../image/E_Images_Sleeping"
# api_path = "../api_result/E_Images_Sleeping.py"
# Google_Microsoft_Fun(image_path,api_path)
