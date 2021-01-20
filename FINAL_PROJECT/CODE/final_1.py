# -*- coding:utf-8 -*-
import cv2
import numpy as np
import math
import time
# 滑鼠事件的回撥函式
def mouse_rect(event, x, y, flags, param):
    global rect
    global LDr
    global LUr

    # 按下左鍵
    if event == 1:
        rect[0] = x
        rect[1] = y
        rect[2] = x
        rect[3] = y
        LDr = True
        LUr = False

    # 移動滑鼠事件
    if event == 0:
        if LDr:
            rect[2] = x
            rect[3] = y

    #放開左鍵
    if event == 4:
        x_min = min(rect[0], rect[2])
        y_min = min(rect[1], rect[3])
        x_max = max(rect[0], rect[2])
        y_max = max(rect[1], rect[3])
        rect[0] = x_min
        rect[1] = y_min
        rect[2] = x_max
        rect[3] = y_max
        LDr = False
        LUr = True

def mouse_dir(event, x, y, flags, param):
    global direction
    global LDd
    global LUd

    # 按下左鍵
    if event == 1:
        direction[2] = x
        direction[3] = y
        LDd = True
        LUd = False

    # 移動滑鼠事件
    if event == 0:
        if LDd:
            direction[2] = x
            direction[3] = y

    #放開左鍵
    if event == 4:
        LDd = False
        LUd = True

# 讀入圖片
def main1(img,V):
    #------------畫框------------#
    global rect
    rect = [0, 0, 0, 0]
    
    global LDr
    LDr = False
    global LUr
    LUr= True
    cv2.namedWindow('img')
    cv2.setMouseCallback('img', mouse_rect)
    cv2.imshow('img', img)
    
    while not(LUr and rect[2] - rect[0] != 0 and rect[3] - rect[1] != 0):
        if LDr:
            #需複製一張影像，否則會有許多矩陣
            img_copy = img.copy()
            cv2.rectangle(img_copy, (rect[0], rect[1]), (rect[2], rect[3]), (0, 255, 0), 2)
            cv2.imshow('img', img_copy)
        cv2.waitKey(2)
        
    rect_img = img_copy.copy()
    
    #---------------畫線----------------#
    # 轉換為寬度高度
    rect[2] = rect[2] - rect[0]
    rect[3] = rect[3] - rect[1]
    
    global direction
    direction = [0, 0, 0, 0]
    direction[0] = abs(int(rect[0] + rect[2]/2))
    direction[1] = abs(int(rect[1] + rect[3]/2))
    global LDd
    LDd = False
    global LUd
    LUd = True
    cv2.setMouseCallback('img', mouse_dir)
    while not(LUd and direction[2] != 0 and direction[3] != 0):
        if LDd:
            #需複製一張影像，否則會有許多矩陣
            img_copy = rect_img.copy()
            #cv2.line(img_copy, (direction[0], direction[1]), (direction[2], direction[3]), (0, 255, 255), 2)
            cv2.arrowedLine(img_copy, (direction[0], direction[1]), (direction[2], direction[3]), (0, 255, 255), 2);
            cv2.imshow('img', img_copy)
        cv2.waitKey(2)
    #------------   自動區塊   -------------#
    
      
    
    time_start = time.time()
    print("%d,%d,%d,%d"%(rect[0],rect[1],rect[2],rect[3]))
    
    mask = np.zeros(img.shape[:2], np.uint8)
    bgdModel = np.zeros((1, 65), np.float64)
    fgdModel = np.zeros((1, 65), np.float64)
    cv2.grabCut(img, mask, rect, bgdModel, fgdModel, 5, cv2.GC_INIT_WITH_RECT)
    
    
    ###########   前景分離   ###########
    #0(GC_BGD):確認bg，1(GC_FGD):確認fg，2(GC_PR_BGD):可能bg，3(GC_PR_FGD):可能fg
    #mask裡的值是0或2的都變為0，其他為1
    fgmask = np.where((mask == 2) | (mask == 0), 0, 1).astype(np.uint8)
    #np.newaxis增加維度，然後藉由pixel相乘，得到mask中不為0的部分
    fgout = img * fgmask[:, :, np.newaxis]
    
    ###########   背景分離   ###########
    bgmask = np.where((mask == 1) | (mask == 3),255,0).astype(np.uint8)
    #膨脹背景區域
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (10, 10))
    dilated = cv2.dilate(bgmask, kernel)
    cv2.bitwise_not(dilated,bgmask)
    bgout = cv2.bitwise_and(img, img, mask=bgmask)
    
    ###########   背景修補   ###########
    dst = cv2.inpaint(bgout,dilated,3,cv2.INPAINT_TELEA)
    ##########    影片存取 與 速度參數  ###########
    
    fps = 15.0
    dt = int((1/fps)*1000)
    s_scale = V/15
    X = direction[2]-direction[0]
    Y = direction[3]-direction[1]
    S = (X**2+Y**2)**0.5
    #無論是atan的輸出，還是cos、sin的輸入都是弪度
    theta = math.atan2(Y,X)
    x_scale = s_scale*math.cos(theta)
    y_scale = s_scale*math.sin(theta)
    scale = int(S/s_scale)
    
    
    Irows,Icols,_ = img.shape
    # fourcc = cv2.VideoWriter_fourcc(*'XVID')
    # video = cv2.VideoWriter('output.avi', fourcc, fps, (Icols, Irows))
    
    
    ##########    物件平移   ###########
    # 平移矩阵M：[[1,0,x],[0,1,y]]
    time_end = time.time()
    time_c= time_end - time_start   #執行所花時間
    print('   time cost', round(time_c*100)/100, 's')
    for i in range(scale):
        M = np.float32([[1,0,int(i*x_scale)],[0,1,int(i*y_scale)]])
        shift_fg = cv2.warpAffine(fgout,M,(Icols,Irows))
        # 貼回原圖
        bfg = cv2.cvtColor(shift_fg, cv2.COLOR_BGR2GRAY)
        bfg = np.where(bfg>0 , 0, 255).astype(np.uint8)
        bfg = cv2.bitwise_and(dst,dst,mask=bfg)
        result = bfg+shift_fg
        # video.write(result)
        cv2.imshow('img', result)
        cv2.waitKey(dt)
    
    cv2.imshow('img', result)
    cv2.waitKey(0)
    
    # video.release()
    cv2.destroyAllWindows()
    
if __name__ == '__main__': 
    
    fname = './PART1/6.jpg'
    img = cv2.imread(fname)
    V = 100
    main1(img,V)
    
