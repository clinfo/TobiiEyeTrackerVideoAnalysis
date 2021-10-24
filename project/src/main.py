#!/usr/bin/env python
# coding: utf-8

import glob
import re
import os
import time
import pandas as pd
import cv2
import math
import numpy as np
import scene
from os.path import expanduser

#Get scenes
scene, start = scene.makeScene()

#Get homefilepath
home = expanduser("~")
#Set using filepaths
path1 = home + '/Documents/EyeTrackerAnalysis/'
path2 = home + '/Downloads/data_video/use/'
path3 = path0 + 'output_csv/'


# # Original Distance score

#moving objects' data
OriginalOutputFilepath = home + '/Documents/EyeTrackerAnalysis/output.csv'
OriginalOutput = pd.read_csv(OriginalOutputFilepath)
x = []
y = []
for z in range(int(start[-1]*20.02002)):
    c = z//20.02002
    c = int(c)
    x.append(output.loc[c, 'X(pixel)'])
    y.append(output.loc[c, 'Y(pixel)'])

path0 = home + '/Documents/EyeTrackerAnalysis/'
df_output = pd.DataFrame({'output_x': x, 'output_y': y})
df_output.to_csv(path0 + 'Output/MovingObjects.csv', index=False)

def AddDistancescore(pathA, pathB):

    #video_Recording*.tsv: Eye-gaze position data for each individual obtained from the eye-tracker
    for filepath in glob.glob(pathA + 'video_Recording*.tsv'):
        if __name__ == '__main__':
            starttime = time.time()
        df = pd.read_csv(filepath, sep='\t')
        df = df.fillna('NaN')
        df = df.rename(columns={'Recording timestamp':'Recording_timestamp'})
        df_s = df.copy().loc[:int(start[-1]*20.02002)-1, :]
        df_s[['Gaze point X', 'Gaze point Y']] = df_s[['Gaze point X', 'Gaze point Y']].astype(float)
        x = re.sub("\\D", "", filepath)

        df_gaze_x = np.array(df_s[('Gaze point X')])
        df_gaze_y = np.array(df_s[('Gaze point Y')])
        output_x = np.array(df_output['output_x'])
        output_y = np.array(df_output['output_y'])
        df_s['timestamp_0'] = np.array(df_s['Recording_timestamp'])-df_s.loc[0, 'Recording_timestamp']
        df_s['x'] = (df_gaze_x-150)/2.25
        df_s['y'] = df_gaze_y/2.25
        df_s['distance'] = np.sqrt((output_x-(df_gaze_x-150)/2.25)**2+(output_y-df_gaze_y/2.25)**2)

        os.makedirs(pathB, exist_ok=True)
        df_s.to_csv(pathB + 'm_video_Recording{}.csv'.format(x))

        elapsed_time = time.time() - starttime
        print(x, 'Done', "elapsed_time:{0}".format(elapsed_time) + "[sec]")

AddDistancescore(path2+'use/', path3+'personal_data/')


# # Original Saliency scores
def AddSaliencyscores(gfilepath, filepathFG, filepathSR, filepathMS):
    for filepath in glob.glob(gfilepath +'m_video_Recording*.csv'):
        if __name__ == '__main__':
            starttime = time.time()

            df = pd.read_csv(filepath, sep=',', index_col=0)
            df = df.fillna('NaN')
            x = re.sub("\\D", "", filepath)
            df['FG_{}'.format(x)] = ''
            df['SR_{}'.format(x)] = ''
            df['Mo_{}'.format(x)] = ''

            for a in range(int(start[-1]*20.02002)):
                b = a//20.02002
                image_fileFG = filepathFG +'FG_img_{}.png'.format(str(int(b)).zfill(6))
                image_fileSR = filepathSR + 'SR_img_{}.png'.format(str(int(b)).zfill(6))
                image_fileMS = filepathMo + 'Mo_img_{}.png'.format(str(int(b)).zfill(6))

                imgFG = cv2.imread(image_fileFG)
                imgSR = cv2.imread(image_fileSR)
                imgMS = cv2.imread(image_fileMS)

                if not((df.loc[a, ('x')]=='NaN') or (df.loc[a, ('y')]=='NaN')):
                    if 0<=(int(df.loc[a, ('x')]))<=719 and 0<=(int(df.loc[a, ('y')]))<=479:
                        df.loc[a, ('FG_{}'.format(x))] = (imgFG[int(df.loc[a, ('y')]),int(df.loc[a, ('x')])])[0]/255
                        df.loc[a, ('SR_{}'.format(x))] = (imgSR[int(df.loc[a, ('y')]),int(df.loc[a, ('x')])])[0]/255
                        df.loc[a, ('Mo_{}'.format(x))] = (imgMS[int(df.loc[a, ('y')]),int(df.loc[a, ('x')])])[0]/255
                    else:
                        df.loc[a, ('FG_{}'.format(x))] = 0
                        df.loc[a, ('SR_{}'.format(x))] = 0
                        df.loc[a, ('Mo_{}'.format(x))] = 0
                else:
                    df.loc[a, ('FG_{}'.format(x))] = 'NaN'
                    df.loc[a, ('SR_{}'.format(x))] = 'NaN'
                    df.loc[a, ('Mo_{}'.format(x))] = 'NaN'

            df.to_csv(gfilepath + 'm_video_Recording{}.csv'.format(x))
            elapsed_time = time.time() - starttime
            print(x, 'Done', "elapsed_time:{0}".format(elapsed_time) + "[sec]")


AddSaliencyscores(
path3 + 'personal_data/',
path1 + 'saliency_map/FG/',
path1 + 'saliency_map/SR/',
path1 + 'saliency_map/Mo/')


# # Scores for the statistical analysis
ID = []
distanceos_med = []
FGos_med = []
SRos_med = []
MSos_med = []

for filepath in glob.glob(output_dir + '/output_csv/personal_data/m_video_Recording*.csv'):
    df = pd.read_csv(filepath, sep=',', index_col=0)
    df_gaze = df.query('x <= 720 & 0 <= x & y <= 480 & 0 <=y' )
    df_gazedes = df_gaze.describe()

    num = re.sub("\\D", "", filepath)
    ID.append(num)

    distanceos_med.append(df_gazedes.loc['50%', 'distance'])
    FGos_med.append(df_gazedes.loc['50%', 'FG_{}'.format(num)])
    SRos_med.append(df_gazedes.loc['50%', 'SR_{}'.format(num)])
    MSos_med.append(df_gazedes.loc['50%', 'MS_{}'.format(num)])

df_scores　= pd.DataFrame({ 'ID' : ID,
                            'distanceos_median': distanceos_med,
                            'FGos_median': FGos_med,
                            'SRos_median': SRos_med,
                            'MSos_median': MSos_med
                          })
df_scores.to_csv(home + '/output_csv/output_scores.csv', index=False)
