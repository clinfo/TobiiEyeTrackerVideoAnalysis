#!/usr/bin/env python
# coding: utf-8

import cv2
import os
from os.path import expanduser

home = expanduser("~")
OriginalVideoFilepath = home + '/Documents/EyeTrackerAnalysis/originalMaterial/20170421ME0021782_1.avi'
OutputVideoFilepathMo = home + '/Documents/EyeTrackerAnalysis/saliency_map/saliencyMo.mp4'
OutputImageDirMo = home + '/Documents/EyeTrackerAnalysis/saliency_map/Mo/'
OutputVideoFilepathFG = home + '/Documents/EyeTrackerAnalysis/saliency_map/saliencyFG.mp4'
OutputImageDirFG = home + '/Documents/EyeTrackerAnalysis/saliency_map/FG/'
OutputVideoFilepathSR = home + '/Documents/EyeTrackerAnalysis/saliency_map/saliencySR.mp4'
OutputImageDirSR = home + '/Documents/EyeTrackerAnalysis/saliency_map/SR/'

def SaliencyMaps(OriginalVideoFilepath, OutputVideoFilepath, SaliencyMap):

    cap = cv2.VideoCapture(OriginalVideoFilepath)
    width = int(cap.get(3))
    height = int(cap.get(4))
    if not cap.isOpened():
        exit()

    saliency = None
    fourcc = cv2.VideoWriter_fourcc('m', 'p', '4', 'v')
    writer = cv2.VideoWriter(OutputVideoFilepath,
                                           fourcc, cap.get(cv2.CAP_PROP_FPS), (720, 480), False)

    while(cap.isOpened()):
        ret, img = cap.read()
        if not ret:
            break
        frame_dst = cv2.resize(img, dsize=(720, 480))

        if saliency is None:
            saliency = SaliencyMap
            saliency.setImagesize(frame_dst.shape[1], frame_dst.shape[0])
            saliency.init()

        gray = cv2.cvtColor(frame_dst, cv2.COLOR_BGR2GRAY)

        (success, saliencyMap) = saliency.computeSaliency(gray)
        saliencyMap = (saliencyMap*255).astype('uint8')

        writer.write(saliencyMap)


    writer.release()
    cap.release()

def ImageConversion(video_file, image_dir, image_file):

    i = 0
    cap = cv2.VideoCapture(video_file)
    print('{}{}'.format('fps=',cap.get(cv2.CAP_PROP_FPS)))
    print('{}{}'.format('width=', cap.get(cv2.CAP_PROP_FRAME_WIDTH)))
    print('{}{}'.format('width=', cap.get(cv2.CAP_PROP_FRAME_HEIGHT)))
    while(cap.isOpened()):
        flag, frame = cap.read()
        if flag == False:
            break
        cv2.imwrite(image_dir+image_file % str(i).zfill(6), frame)
        print('Save', image_dir+image_file % str(i).zfill(6))
        i += 1

    cap.release()

SaliencyMaps(OriginalVideoFilepath, OutputVideoFilepathMo, cv2.saliency.MotionSaliencyBinWangApr2014_create())
SaliencyMaps(OriginalVideoFilepath, OutputVideoFilepathFG, cv2.saliency.StaticSaliencyFineGrained_create())
SaliencyMaps(OriginalVideoFilepath, OutputVideoFilepathSR, cv2.saliency.StaticSaliencySpectralResidual_create())

ImageConversion(OutputVideoFilepathMo, OutputImageDirMo, 'Mo_img_%s.png')
ImageConversion(OutputVideoFilepathFG, OutputImageDirFG, 'FG_img_%s.png')
ImageConversion(OutputVideoFilepathSR, OutputImageDirSR, 'SR_img_%s.png')
