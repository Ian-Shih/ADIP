import cv2
import numpy as np
import PIL.ImageDraw as ImageDraw 
import PIL.Image as Image 
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

def mouse_point(event, x, y, flags, param):
    global point
    global LDp
    global point_break
    # 按下左鍵
    if event == 1:
        point[0] = x
        point[1] = y
        LDp = True
    if event == 3:
        point_break=True
    
def mouse_arrow(event, x, y, flags, param):
    global arrow
    global LDa
    global LUa
    global arrow_break
    # 按下左鍵
    if event == 1:
        arrow[0] = x
        arrow[1] = y
        arrow[2] = x
        arrow[3] = y
        LDa = True
        LUa = False
    if event == 0:
        if LDa:
            arrow[2] = x
            arrow[3] = y

    if event == 3:
        arrow_break=True
        
    if event == 4:
        LDa = False
        LUa = True
        
def poly_fbg(img,dot_pos):
    # ------------框選出指定範圍------------#
    bg = Image.fromarray(img)#np2PIL
    draw = ImageDraw.Draw(bg) 
    draw.polygon(tuple(dot_pos), fill=0) 
    bg = np.array(bg)#PIL2np
    gray = cv2.cvtColor(bg, cv2.COLOR_BGR2GRAY)
    mask = np.where(gray==0,255,0).astype(np.uint8)
    fg = cv2.bitwise_and(img, img, mask=mask)
    return fg,bg

def horizontal_warping(img,h_start,x_scale,i):
    rows,cols,_=img.shape
    halfa_img = np.zeros((rows,cols,3),np.uint8)
    scale = int(h_start+x_scale*i)
    if x_scale>=0:
        halfa_img[:,h_start:,:] = img[:,h_start:,:]
        bpts1 = np.float32([[0,0],[h_start,0],[0,rows],[h_start,rows]])
        bpts2 = np.float32([[0,0],[scale,0],[0,rows],[scale,rows]])
        apts1 = np.float32([[h_start,0],[cols,0],[h_start,rows],[cols,rows]])
        apts2 = np.float32([[scale,0],[cols,0],[scale,rows],[cols,rows]])
    else:
        halfa_img[:,:h_start,:] = img[:,:h_start,:]
        apts1 = np.float32([[0,0],[h_start,0],[0,rows],[h_start,rows]])
        apts2 = np.float32([[0,0],[scale,0],[0,rows],[scale,rows]])
        bpts1 = np.float32([[h_start,0],[cols,0],[h_start,rows],[cols,rows]])
        bpts2 = np.float32([[scale,0],[cols,0],[scale,rows],[cols,rows]])
    #---切分點 rows/2 以左---#
    aM=cv2.getPerspectiveTransform(apts1,apts2)
    adst = cv2.warpPerspective(halfa_img,aM,(cols,rows))
    #切出adst遮罩
    adst_gray = cv2.cvtColor(adst, cv2.COLOR_BGR2GRAY)
    mask_a = np.where(adst_gray!=0,0,255).astype(np.uint8)
    #---切分點 rows/2 以右---#
    bM=cv2.getPerspectiveTransform(bpts1,bpts2)
    bdstM = cv2.warpPerspective(img,bM,(cols,rows))
    #---左右合併---#
    #以bdst為背景，貼上adst
    bdst = cv2.bitwise_and(bdstM, bdstM, mask=mask_a)
    h_result = bdst + adst
    return h_result

def vertical_warping(img,v_start,y_scale,i):
    rows,cols,_=img.shape
    halfa_img = np.zeros((rows,cols,3),np.uint8)
    scale = int(v_start+y_scale*i)
    if y_scale>=0:
        halfa_img[v_start:,:,:] = img[v_start:,:,:]
        bpts1 = np.float32([[0,0],[0,v_start],[cols,0],[cols,v_start]])
        bpts2 = np.float32([[0,0],[0,scale],[cols,0],[cols,scale]])
        apts1 = np.float32([[0,v_start],[0,rows],[cols,v_start],[cols,rows]])
        apts2 = np.float32([[0,scale],[0,rows],[cols,scale],[cols,rows]])
    else:
        halfa_img[:v_start,:,:] = img[:v_start,:,:]
        apts1 = np.float32([[0,0],[0,v_start],[cols,0],[cols,v_start]])
        apts2 = np.float32([[0,0],[0,scale],[cols,0],[cols,scale]])
        bpts1 = np.float32([[0,v_start],[0,rows],[cols,v_start],[cols,rows]])
        bpts2 = np.float32([[0,scale],[0,rows],[cols,scale],[cols,rows]])
    #---切分點 rows/2 以下---#
    aM=cv2.getPerspectiveTransform(apts1,apts2)
    adst = cv2.warpPerspective(halfa_img,aM,(cols,rows))
    #切出adst遮罩
    adst_gray = cv2.cvtColor(adst, cv2.COLOR_BGR2GRAY)
    mask_a = np.where(adst_gray!=0,0,255).astype(np.uint8)
    #---切分點 rows/2 以右---#
    bM=cv2.getPerspectiveTransform(bpts1,bpts2)
    bdstM = cv2.warpPerspective(img,bM,(cols,rows))
    #---上下合併---#
    #以bdst為背景，貼上adst
    bdst = cv2.bitwise_and(bdstM, bdstM, mask=mask_a)
    hv_result = bdst + adst
    return hv_result


def main1(img):
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
    # print("%d,%d,%d,%d"%(rect[0],rect[1],rect[2],rect[3]))
    global time_start
    time_start = time.time()
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
    
    
    X = direction[2]-direction[0]
    Y = direction[3]-direction[1]
    global time_end
    time_end = time.time()
    global time_c
    time_c= time_end - time_start   #執行所花時間
    return X,Y,dst,fgout

def main2(img):
    rows,cols,_ = img.shape
    
    #------------畫範圍點------------#
    global point
    global LDp
    global point_break
    point = [0, 0]
    LDp = False
    point_break = False
    img_copy = img.copy()
    cv2.namedWindow('img')
    cv2.setMouseCallback('img', mouse_point)
    cv2.imshow('img', img_copy)
    dot_pos = []
    while not point_break:
        if tuple(point) not in dot_pos and LDp:
            cv2.circle(img_copy, (point[0],point[1]), 5, (0, 0, 255), -1)
            dot_pos.append(tuple(point))
            cv2.imshow('img', img_copy)
        cv2.waitKey(2)
        
    point_img = img_copy.copy()
    #---------------畫箭頭----------------#
    global arrow
    global LDa
    global LUa
    global arrow_break
    arrow = [0, 0, 0, 0]
    LDa = False
    LUa = True
    arrow_break= False
    cv2.setMouseCallback('img', mouse_arrow)
    start_pos = []
    end_pos = []
    while not arrow_break:
        arrow = [0, 0, 0, 0]
        LDa = False
        LUa = True
        while not(LUa and (arrow[2] - arrow[0] != 0 or arrow[3] - arrow[1] != 0)) and not arrow_break:
            if LDa and arrow[2]>0 and arrow[3]>0 and arrow[2]<img.shape[1] and arrow[3]<img.shape[0]:
                img_copy = point_img.copy()
                cv2.arrowedLine(img_copy, (arrow[0], arrow[1]), (arrow[2], arrow[3]), (0, 0, 0), 2)
                cv2.imshow('img', img_copy)
                if tuple(arrow[:2]) not in start_pos:
                    start_pos.append(tuple(arrow[:2]))
            cv2.waitKey(2)
        if tuple(arrow[2:]) not in end_pos and arrow[2] and arrow[3]:
            end_pos.append(tuple(arrow[2:]))
        point_img=img_copy.copy()
    global time_start
    time_start = time.time()
    # print(start_pos)
    # print(end_pos)
    # ------------固定區域分割------------#
    _,img_bg = poly_fbg(img,dot_pos)
    rows,cols,_ = img.shape
    black=np.zeros((rows,cols),np.int32) #分水嶺的marker
    white=np.zeros((rows,cols,3),np.uint8) #分水嶺的輸入
    scale = 10 #分為幾段變化

    pimage_set = []
    nimage_set = []

    img_copy = img.copy()
    for label in range(len(start_pos)):
        cv2.line(black,(start_pos[label][0], start_pos[label][1]), (end_pos[label][0], end_pos[label][1]), label+1, 1)
    result=cv2.watershed(white, black).astype(np.uint8)
    
    result_m = np.where(result==255,255,0).astype(np.uint8)
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(15, 15)) 
    result_m = cv2.dilate(result_m,kernel)######################################
    
    for i in range(rows):
        for j in range(cols):
            if result[i,j] == 255:
                if i+1<rows:
                    if result[i+1,j]!=255:
                        result[i,j]=result[i+1,j]
                        continue
                if i-1>=0:
                    if result[i-1,j]!=255:
                        result[i,j]=result[i-1,j]
                        continue
                if j+1<cols:
                    if result[i,j+1]!=255:
                        result[i,j]=result[i,j+1]
                        continue
                if j-1>=0:
                    if result[i,j-1]!=255:
                        result[i,j]=result[i,j-1]
                        continue
                    
    label_3ch=np.zeros((rows,cols,3),np.uint8)
    label_3ch[:,:,0]=result[:,:]
    label_3ch[:,:,1]=result[:,:]
    label_3ch[:,:,2]=result[:,:]
    #展示分水嶺效果
    show=label_3ch.copy()
    show*=10
    for label in range(len(start_pos)):
        cv2.arrowedLine(show,(start_pos[label][0], start_pos[label][1]), (end_pos[label][0], end_pos[label][1]), (0,0,255), 1)
    show,_ = poly_fbg(show,dot_pos)
    
    #套外框到分水嶺結果
    fg,_ = poly_fbg(label_3ch,dot_pos)
    for i in range(scale):
        init_p_list = []  
        init_n_list = []  
        for j in range(len(start_pos)):
            label_i = np.where(fg!=j+1,0,255).astype(np.uint8)

            xmax=0
            xmin=9999
            ymax=0
            ymin=9999
            for x in range(rows):
                for y in range(cols):
                    if label_i[x][y][0]:
                        if xmax<x:
                            xmax=x
                        if xmin>x:
                            xmin=x
                        if ymax<y:
                            ymax=y
                        if ymin>y:
                            ymin=y
            
            #開始變化的分界
            x_start=int((ymax-ymin)*0.5)
            y_start=int((xmax-xmin)*0.5) 
            
            #每次變化的尺度
            x_scale = (end_pos[j][0]-start_pos[j][0])*0.05 
            y_scale = (end_pos[j][1]-start_pos[j][1])*0.05

            region_rows=xmax-xmin
            region_cols=ymax-ymin

            arrow_img = np.zeros((region_rows,region_cols,3),np.uint8)
            arrow_img[:,:]=img_copy[xmin:xmax,ymin:ymax]

            #--------------------------左右形變--------------------------#     
            ph_img = horizontal_warping(arrow_img,x_start,x_scale,i)
            nh_img = horizontal_warping(arrow_img,x_start,-x_scale,i+1) #否則會拿到兩張原圖
            #--------------------------上下形變--------------------------#
            phv_img = vertical_warping(ph_img,y_start,y_scale,i)
            nhv_img = vertical_warping(nh_img,y_start,-y_scale,i+1)         
            
            mask=label_i[xmin:xmax,ymin:ymax,0]

            phv_img = cv2.bitwise_and(phv_img, phv_img, mask=mask)
            nhv_img = cv2.bitwise_and(nhv_img, nhv_img, mask=mask)
            all_p=np.zeros((rows,cols,3),np.uint8)
            all_n=np.zeros((rows,cols,3),np.uint8)
            
            #儲存每個拼圖
            init_p=np.zeros((rows,cols,3),np.uint8)
            init_n=np.zeros((rows,cols,3),np.uint8)
            init_p[xmin:xmax,ymin:ymax]=phv_img[:,:]
            init_n[xmin:xmax,ymin:ymax]=nhv_img[:,:]
            init_p_list.append(init_p)
            init_n_list.append(init_n)

        for j in range(len(start_pos)):
            all_p+=init_p_list[j]
            all_n+=init_n_list[j]
        
        
        all_p_gray = cv2.cvtColor(all_p, cv2.COLOR_BGR2GRAY)
        mask = np.where(all_p_gray==0,255,0).astype(np.uint8)
        all_bg = cv2.bitwise_and(img_copy, img_copy, mask=mask)
        
        mix_p=all_bg+all_p
        mix_n=all_bg+all_n
        
        blur_p = cv2.GaussianBlur(mix_p, ksize=(5, 5), sigmaX=0, sigmaY=0)###########################
        blur_n = cv2.GaussianBlur(mix_n, ksize=(5, 5), sigmaX=0, sigmaY=0)
        
        mask_p = cv2.bitwise_and(blur_p,blur_p,mask=result_m)
        mask_n = cv2.bitwise_and(blur_n,blur_n,mask=result_m)
        
        fg_p,_ = poly_fbg(mask_p,dot_pos)
        fg_n,_ = poly_fbg(mask_n,dot_pos)
        
        result_m_not = np.where(fg_p>0,0,255).astype(np.uint8)
        
        Rmask = result_m_not[:,:,0]
        
        maskbg_p = cv2.bitwise_and(mix_p,mix_p,mask=Rmask)
        maskbg_n = cv2.bitwise_and(mix_n,mix_n,mask=Rmask)

        pimage_set.append(fg_p+maskbg_p)
        nimage_set.append(fg_n+maskbg_n)
        
    return pimage_set,nimage_set     

def main3(img,V):    
    X,Y,dst,fgout = main1(img)
    pimage_set,nimage_set = main2(dst)
    
    fps = 15.0
    dt = int((1/fps)*1000)
    s_scale = V/15
    S = (X**2+Y**2)**0.5
    #無論是atan的輸出，還是cos、sin的輸入都是弪度
    theta = math.atan2(Y,X)
    x_scale = s_scale*math.cos(theta)
    y_scale = s_scale*math.sin(theta)
    scale1 = int(S/s_scale)
    scale2 = len(pimage_set)
    
    
    Irows,Icols,_ = img.shape
    
    #########    物件平移   ###########
    #平移矩阵M：[[1,0,x],[0,1,y]]
    flag=0

    for i in range(scale1):
        if i%3==0:
            high_img = pimage_set[flag]
            low_img = nimage_set[scale2-flag-1]
            alpha = (flag/scale2)**2
            dst = cv2.addWeighted(high_img,1-alpha,low_img,alpha,0)
            flag+=1
            if flag>=10:
                flag=0
        M = np.float32([[1,0,int(i*x_scale)],[0,1,int(i*y_scale)]])
        shift_fg = cv2.warpAffine(fgout,M,(Icols,Irows))
        # 貼回原圖
        bfg = cv2.cvtColor(shift_fg, cv2.COLOR_BGR2GRAY)
        bfg = np.where(bfg>0 , 0, 255).astype(np.uint8)
        bfg = cv2.bitwise_and(dst,dst,mask=bfg)
        result = bfg+shift_fg
        cv2.imshow('img', result)
        cv2.waitKey(dt)
    global time_end
    time_end = time.time()
    global time_c
    time_c+= time_end - time_start   #執行所花時間    
    print('   time cost', round(time_c*100)/100, 's')
    cv2.imshow('img', result)
    cv2.waitKey(0)
    
    cv2.destroyAllWindows()
    
    

if __name__ == '__main__': 
    fname = './PART3/4.png'
    img = cv2.imread(fname)
    V = 100
    main3(img,V)
    
    
