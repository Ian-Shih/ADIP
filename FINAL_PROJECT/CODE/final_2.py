import cv2
import numpy as np
import PIL.ImageDraw as ImageDraw 
import PIL.Image as Image 
import time
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
        
    # print(start_pos)
    # print(end_pos)
    # ------------固定區域分割------------#
    time_start = time.time()
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
    
    time_end = time.time()
    time_c= time_end - time_start   #執行所花時間
    print('   time cost', round(time_c*100)/100, 's')
    for i in range(scale):
        high_img = pimage_set[i]
        low_img = nimage_set[scale-i-1]
        alpha = (i/scale)**2
        dst = cv2.addWeighted(high_img,1-alpha,low_img,alpha,0)
        cv2.imshow('img',dst)

        cv2.waitKey(200)     
    
    cv2.destroyAllWindows()       

if __name__ == '__main__': 
    fname = 'PART2/6.jpg'
    img = cv2.imread(fname)
    main2(img)     
            
     